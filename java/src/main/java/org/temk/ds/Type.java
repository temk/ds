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
    public static Type fromClass(Class clazz) {
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

    public Class toClass() {
        Class result = null;
        switch(this)
        {
            case INVALID: 
                break;

            case BOOL:
                result = Boolean.TYPE;
                break;
                
            case INT8:
            case UINT8:
                result = Byte.TYPE;
                break;
                
            case INT16:
            case UINT16:
                result = Short.TYPE;
                break;
                
            case INT32:
            case UINT32:
                result = Integer.TYPE;
                break;
                
            case INT64:
            case UINT64:
                result = Long.TYPE;
                break;
                
            case FLOAT32:
                result = Float.TYPE;
                break;

            case FLOAT64:
                result = Double.TYPE;
                break;

            case STRING8:
            case STRING16:
            case STRING32:
                result = String.class;
                break;
        }
        
        return result;
    }
    
    public boolean isString() {
        return this == STRING8 || this == STRING16 || this == STRING32;
    }
}
