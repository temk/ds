package org.temk.ds.util;

import java.lang.reflect.Field;

public abstract class Buffer<TArray> {
    public static final int DEFAULT_BUF_SIZ = 1024;
    
    protected int size;
    protected Field field;
    protected TArray array;

    public Buffer(int size, Field field, TArray array) {
        this.size = size;
        this.field = field;
        this.array = array;
    }

    public TArray getArray() {
        return array;
    }

    
    public abstract void read(Object obj, int index) throws Exception;

    public abstract void write(Object obj, int index) throws Exception;

    public static Buffer create(int size, Field field) {
        Class clazz = field.getType();

        if (clazz == Boolean.TYPE) {
            return new Buffer<boolean[]>(size, field, new boolean[size]) {
                @Override
                public void read(Object obj, int index) throws Exception {
                    field.setBoolean(obj, array[index]);
                }

                @Override
                public void write(Object obj, int index) throws Exception {
                    array[index] = field.getBoolean(obj);
                }
            };
        } else if (clazz == Byte.TYPE) {
            return new Buffer<byte[]>(size, field, new byte[size]) {
                @Override
                public void read(Object obj, int index) throws Exception {
                    field.setByte(obj, array[index]);
                }

                @Override
                public void write(Object obj, int index) throws Exception {
                    array[index] = field.getByte(obj);
                }
            };
        } else if (clazz == Short.TYPE) {
            return new Buffer<short[]>(size, field, new short[size]) {
                @Override
                public void read(Object obj, int index) throws Exception {
                    field.setShort(obj, array[index]);
                }

                @Override
                public void write(Object obj, int index) throws Exception {
                    array[index] = field.getShort(obj);
                }
            };
        } else if (clazz == Integer.TYPE) {
            return new Buffer<int[]>(size, field, new int[size]) {
                @Override
                public void read(Object obj, int index) throws Exception {
                    field.setInt(obj, array[index]);
                }

                @Override
                public void write(Object obj, int index) throws Exception {
                    array[index] = field.getInt(obj);
                }
            };
        } else if (clazz == Long.TYPE) {
            return new Buffer<long[]>(size, field, new long[size]) {
                @Override
                public void read(Object obj, int index) throws Exception {
                    field.setLong(obj, array[index]);
                }

                @Override
                public void write(Object obj, int index) throws Exception {
                    array[index] = field.getLong(obj);
                }
            };
        } else if (clazz == Float.TYPE) {
            return new Buffer<float[]>(size, field, new float[size]) {
                @Override
                public void read(Object obj, int index) throws Exception {
                    field.setFloat(obj, array[index]);
                }

                @Override
                public void write(Object obj, int index) throws Exception {
                    array[index] = field.getFloat(obj);
                }
            };
        } else if (clazz == Double.TYPE) {
            return new Buffer<double[]>(size, field, new double[size]) {
                @Override
                public void read(Object obj, int index) throws Exception {
                    field.setDouble(obj, array[index]);
                }

                @Override
                public void write(Object obj, int index) throws Exception {
                    array[index] = field.getDouble(obj);
                }
            };
        } else if (clazz == String.class) {
            return new Buffer<String[]>(size, field, new String[size]) {
                @Override
                public void read(Object obj, int index) throws Exception {
                    field.set(obj, array[index]);
                }

                @Override
                public void write(Object obj, int index) throws Exception {
                    array[index] = (String) field.get(obj);
                }
            };
        }
        return null;
    }
}
