package org.temk.ds;

public enum Type {
    INVALID,
    BOOL,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
    STRING8,
    STRING16,
    STRING32;
    
    /**
     * @param clazz - type of data
     * @return mapped data type for given class
     */
    static Type fromClass(Class clazz) {
            if (clazz == Boolean.TYPE)
                return BOOL;
            else if(clazz == Byte.TYPE)
                return INT8;
            else if(clazz == Short.TYPE)
                return INT16;
            else if(clazz == Integer.TYPE)
                return INT32;
            else if(clazz == Long.TYPE)
                return INT64;
            else if(clazz == Float.TYPE)
                return FLOAT32;
            else if(clazz == Double.TYPE)
                return FLOAT64;
            else if(clazz == String.class)
                return STRING16;
            else
                return INVALID;
    }

    boolean isString() {
        return this == STRING8 || this == STRING16 || this == STRING32;
    }
}
