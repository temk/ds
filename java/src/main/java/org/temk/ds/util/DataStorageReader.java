package org.temk.ds.util;

import java.lang.reflect.Field;
import java.util.Map;
import org.temk.ds.Column;
import org.temk.ds.DataStorage;
import org.temk.ds.persistent.DefaultFieldType;
import org.temk.ds.persistent.Persistent;

public class DataStorageReader<T> extends DataStorageWrapper<T> {
    
    private int bufLen = 0;
    private long totalRead = 0;
    private long available = Long.MAX_VALUE;

    public DataStorageReader(DataStorage ds, Class<T> clazz, int bufSize) {
        this(ds, clazz, bufSize, DefaultFieldType.TRANSIENT);
    }
    
    public DataStorageReader(DataStorage ds, Class<T> clazz) {
        this(ds, clazz, Buffer.DEFAULT_BUF_SIZ, DefaultFieldType.TRANSIENT);
    }
    
    public DataStorageReader(DataStorage ds, Class<T> clazz, DefaultFieldType ft) {
        this(ds, clazz, Buffer.DEFAULT_BUF_SIZ, ft);
    }
    
    public DataStorageReader(DataStorage ds, Class<T> clazz, int bufSize, DefaultFieldType ft) {
        super(ds, clazz, bufSize, ft, false);

        for (BufferedColumn bc: list) {
            available = Math.min(available, bc.column.getLength());                        
        }

        fetch();
    }

    
    public T read() {
        try {
            T obj = clazz.newInstance();
            return read(obj);
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
    }

    public T read(T obj) {
        if (counter == bufLen) {
            if (!fetch()) {
                return null;
            }
        }
        try {
            for (BufferedColumn bc: list) {
                bc.buffer.read(obj, counter);
            }
            if (metaMap.size() > 0) {
                for (Map.Entry<Field, String> e: metaMap.entrySet()) {
                    e.getKey().set(obj, e.getValue());
                }
            }
            ++ counter;
            if (postLoad != null) {
                postLoad.invoke(obj);
            }
            return obj;
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
    }
    
    private boolean fetch() {
        if (totalRead >= available)
            return false;
        
        long rd = Math.min(available - totalRead, buffSize);
        for (BufferedColumn bc: list) {
            bc.fetch(totalRead, rd);
        }
        totalRead += rd;
        counter = 0;
        bufLen  = (int )rd;
        return true;
    }
    
    public long getAvailable() {
        return available;
    }

    @Override
    BufferedColumn processField(Field field, Persistent p, int bufSize, boolean createIfNotExists) {
        String name = field.getName();
        if (p != null && p.value().length() > 0) {
            name = p.value();
        }

        Column col = ds.getColumn(name);

        return new BufferedColumn(Buffer.create(bufSize, field), col);
    }
    
    
}
