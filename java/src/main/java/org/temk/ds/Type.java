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
        return INVALID;
    }

    boolean isString() {
        return this == STRING8 || this == STRING16 || this == STRING32;
    }
}
