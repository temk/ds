package org.temk.ds.util;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.Stack;
import org.temk.ds.DataStorage;
import org.temk.ds.persistent.DefaultFieldType;
import org.temk.ds.persistent.Persistent;
import org.temk.ds.persistent.PostLoad;
import org.temk.ds.persistent.PrePersist;
import org.temk.ds.persistent.Transient;

abstract class DataStorageWrapper<T> {
    int counter;
    int buffSize;
    Class<T> clazz;
    DataStorage ds;

    Method postLoad;
    Method prePersist;
    ArrayList<BufferedColumn> list;    
    
    DataStorageWrapper(DataStorage ds, Class<T> clazz, int buffSize, DefaultFieldType ft, boolean createIfNotExists) {
        this.ds = ds;
        this.clazz = clazz;
        this.buffSize = buffSize;
        this.counter = 0;
        this.list = new ArrayList<BufferedColumn>();
        
        for (Class c: getHierarchy(clazz)) {
            for (Field f: c.getDeclaredFields()) {
                Persistent p = f.getAnnotation(Persistent.class);
                Transient t = f.getAnnotation(Transient.class);
                
                if ((ft == DefaultFieldType.PERSISTENT && t == null) ||  
                    (ft == DefaultFieldType.TRANSIENT && p != null)) 
                {
                    f.setAccessible(true);                    
                    list.add(processField(f, p, buffSize, createIfNotExists));
                }
            }
            
            for (Method m: c.getDeclaredMethods()) {                
                if (m.isAnnotationPresent(PostLoad.class)) {
                    m.setAccessible(true);                    
                    postLoad = m;
                }

                if (m.isAnnotationPresent(PrePersist.class)) {
                    m.setAccessible(true);                    
                    prePersist = m;
                }
            }
        }
    }

    abstract BufferedColumn processField(Field field, Persistent p, int bufSize, boolean createIfNotExists);
    
    private static final List<Class> getHierarchy(Class clazz) {
        Stack<Class> stack = new Stack<Class>();
        while(clazz != null) {
            stack.push(clazz);
            clazz = clazz.getSuperclass();
        }
        
        return stack;
    }
}
