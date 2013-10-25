package org.temk.ds.util;

import org.temk.ds.util.Buffer;
import java.lang.annotation.Annotation;
import java.lang.reflect.Field;
import java.util.ArrayList;
import org.temk.ds.Column;
import org.temk.ds.DataStorage;
import org.temk.ds.Persistent;
import org.temk.ds.Type;

public class DataStorageWriter<T> {

    private int bufSize;
    private int counter;
    private Class<T> clazz;
    private DataStorage ds;
    private ArrayList<BufferedColumn> list = new ArrayList<BufferedColumn>();

    public DataStorageWriter(DataStorage ds, Class<T> clazz) {
        this(ds, clazz, Buffer.DEFAULT_BUF_SIZ, false);
    }

    public DataStorageWriter(DataStorage ds, Class<T> clazz, int bufSize, boolean createIfNotExists) {
        this.ds = ds;
        this.clazz = clazz;
        this.bufSize = bufSize;
        this.counter = 0;


        for (Field field : clazz.getDeclaredFields()) {
            field.setAccessible(true);

            Persistent p = field.getAnnotation(Persistent.class);
            if (p != null) {
                String name = p.value();
                if (name.length() == 0) {
                    name = field.getName();
                }

                Column col = null;
                if (ds.hasColumn(name)) {
                    col = ds.getColumn(name);
                } else {
                    if (createIfNotExists) {
                        col = ds.addColumn(Type.fromClass(field.getType()), name);
                    } else {
                        throw new RuntimeException("Column " + name + " not exists");
                    }
                }

                list.add(new BufferedColumn(Buffer.create(bufSize, field), col));
            }
        }
    }

    public void write(T obj) {
        try {
            for (BufferedColumn bc : list) {
                bc.buffer.write(obj, counter);
            }
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }

        if (++counter == bufSize) {
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
}
