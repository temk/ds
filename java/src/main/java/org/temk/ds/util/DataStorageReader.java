package org.temk.ds.util;

import java.lang.reflect.Field;
import java.util.ArrayList;
import org.temk.ds.Column;
import org.temk.ds.DataStorage;
import org.temk.ds.Persistent;

public class DataStorageReader<T> {
    private int bufSize;
    private int counter;
    private Class<T> clazz;
    private DataStorage ds;
    private ArrayList<BufferedColumn> list = new ArrayList<BufferedColumn>();

    private long totalRead = 0;
    private long available = Long.MAX_VALUE;
    
    public DataStorageReader(DataStorage ds, Class<T> clazz, int bufSize) {
        this.ds = ds;
        this.clazz = clazz;
        this.bufSize = bufSize;
        this.counter = 0;
        
        
        for (Field field: clazz.getDeclaredFields()) {
            field.setAccessible(true);
            
            Persistent p = field.getAnnotation(Persistent.class);
            if (p != null) {
                String name = p.value();
                if (name.length() == 0) {
                    name = field.getName();
                }
                
                Column col = ds.getColumn(name);
                
                available = Math.min(available, col.getLength());                        
                list.add(new BufferedColumn(Buffer.create(bufSize, field), col));
            }
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
        try {
            for (BufferedColumn bc: list) {
                bc.buffer.read(obj, counter);
            }
            if (++ counter == bufSize) {
                if (!fetch()) {
                    return null;
                }
            }
            return obj;
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
    }
    
    private boolean fetch() {
        if (totalRead >= available)
            return false;
        
        long rd = Math.min(available - totalRead, bufSize);
        for (BufferedColumn bc: list) {
            bc.fetch(totalRead, rd);
        }
        totalRead += rd;
        counter = 0;
        return true;
    }
    
    public long getAvailable() {
        return available;
    }
    
    
}
