package org.temk.ds.util;

import java.lang.reflect.Field;
import org.temk.ds.Column;
import org.temk.ds.DataStorage;
import org.temk.ds.Type;
import org.temk.ds.persistent.DefaultFieldType;
import org.temk.ds.persistent.Persistent;

public class DataStorageWriter<T> extends DataStorageWrapper<T> {

    public DataStorageWriter(DataStorage ds, Class<T> clazz) {
        this(ds, clazz, Buffer.DEFAULT_BUF_SIZ, DefaultFieldType.TRANSIENT, false);
    }
    
    public DataStorageWriter(DataStorage ds, Class<T> clazz, DefaultFieldType ft) {
        this(ds, clazz, Buffer.DEFAULT_BUF_SIZ, ft, false);
    }
    
    public DataStorageWriter(DataStorage ds, Class<T> clazz, boolean createIfNotExists) {
        this(ds, clazz, Buffer.DEFAULT_BUF_SIZ, DefaultFieldType.TRANSIENT, createIfNotExists);
    }

    public DataStorageWriter(DataStorage ds, Class<T> clazz, int bufSize, boolean createIfNotExists) {
        this(ds, clazz, bufSize, DefaultFieldType.TRANSIENT, createIfNotExists);
    }

    public DataStorageWriter(DataStorage ds, Class<T> clazz, int bufSize, DefaultFieldType ft, boolean createIfNotExists) {
        super(ds, clazz, bufSize, ft, createIfNotExists);
    }

    public void write(T obj) {
        try {
            if (prePersist != null) {
                prePersist.invoke(obj);
            }
            for (BufferedColumn bc : list) {
                bc.buffer.write(obj, counter);
            }
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }

        if (++counter == buffSize) {
            flush();
        }
    }

    public void flush() {
        for (BufferedColumn bc : list) {
            bc.flush(counter);
        }
        counter = 0;
    }

    public void close() {
        flush();
        ds.close();
    }

    @Override
    BufferedColumn processField(Field field, Persistent p, int bufSize, boolean createIfNotExists) { 
        String name = field.getName();
        int width = 1;
        
        if (p != null) {
            if (p.value().length() > 0) {
                name = p.value();
            }
            width = p.width();
        }

        Column col = null;
        if (ds.hasColumn(name)) {
            col = ds.getColumn(name);
        } else {
            if (createIfNotExists) {
                col = ds.addColumn(field.getType().isEnum() ? Type.STRING16 : Type.fromClass(field.getType()), name, width);
            } else {
                throw new RuntimeException("Column " + name + " not exists");
            }
        }

        return new BufferedColumn(Buffer.create(bufSize, field), col);
    }
}
