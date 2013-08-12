package org.temk.ds;

import java.nio.ByteOrder;

public class Column {
    private long handle;
    private String name;
    private DataStorage parent;
    
    private native void read(Object data, Object indexes, int idxSiz, long offset, long num);
    private native void write(Object data);
    
    public Column(String name, long handle, DataStorage parent) {
        this.name = name;
        this.handle = handle;
        this.parent = parent;
        parent.columns.put(handle, this);
    }
    
    /**
     * remove current column
    **/
    public native void remove(); 

    /**
     * truncate current column to newLength
     * @param newLength - new length of column
     * @return current column 
     */
    public native Column truncate(long newLength);
    
    /**
     * flush data belongs to current column to disk and rewrite index.
    **/
    public native void flush();    
    
    /**
     * @return index of current column
    **/
    public native long getIndex();
    
    /**
     * @return name of current column
    **/
    public String getName() {
        return name;
    }
    
    
    /**
     * @return length of current column
    **/
    public native long getLength();

    /**
     * @return byte order of current order
    **/
    public native ByteOrder getByteOrder();
    
    /**
     * @return type of current column
    **/
    public native Type getType();    
    
    /**
     * @return type of current column
    **/
    public native Type getExtType();    
    
    // __DO_NOT_EDIT_GENERATED_CODE__

    /**
     * Appends boolean array to column
     * @param data - data to append 
     * @return this column 
    **/
    public Column append(boolean [] data) {
        write(data);
        return this;
    }

    /**
     * Reads nun elements from specified offset into boolean array.
     * @param num - number of elements to read
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public boolean [] read(long offset, int num, boolean [] data) {
        if (data == null) {
            data = new boolean [num];
        }
        read(data, null, 0, offset, num);
        return data;
    }

    /**
     * Reads all elements from specified offset into boolean array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public boolean [] read(long offset, boolean [] data) {
        return read(offset, (int)(getLength() - offset + 1), data);
    }

    /**
     * Reads all elements from specified offset into boolean array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public boolean [] read(boolean [] data) {
        return read(0, (int )getLength(), data);
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public boolean [] read(int [] indexes, boolean [] data) {
        if (data == null) {
            data = new boolean [indexes.length];
        }
        read(data, indexes, Integer.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public boolean [] read(long [] indexes, boolean [] data) {
        if (data == null) {
            data = new boolean [indexes.length];
        }
        read(data, indexes, Long.SIZE/Byte.SIZE, 0, 0);

        return data;
    }


    /**
     * Appends byte array to column
     * @param data - data to append 
     * @return this column 
    **/
    public Column append(byte [] data) {
        write(data);
        return this;
    }

    /**
     * Reads nun elements from specified offset into byte array.
     * @param num - number of elements to read
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public byte [] read(long offset, int num, byte [] data) {
        if (data == null) {
            data = new byte [num];
        }
        read(data, null, 0, offset, num);
        return data;
    }

    /**
     * Reads all elements from specified offset into byte array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public byte [] read(long offset, byte [] data) {
        return read(offset, (int)(getLength() - offset + 1), data);
    }

    /**
     * Reads all elements from specified offset into byte array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public byte [] read(byte [] data) {
        return read(0, (int )getLength(), data);
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public byte [] read(int [] indexes, byte [] data) {
        if (data == null) {
            data = new byte [indexes.length];
        }
        read(data, indexes, Integer.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public byte [] read(long [] indexes, byte [] data) {
        if (data == null) {
            data = new byte [indexes.length];
        }
        read(data, indexes, Long.SIZE/Byte.SIZE, 0, 0);

        return data;
    }


    /**
     * Appends short array to column
     * @param data - data to append 
     * @return this column 
    **/
    public Column append(short [] data) {
        write(data);
        return this;
    }

    /**
     * Reads nun elements from specified offset into short array.
     * @param num - number of elements to read
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public short [] read(long offset, int num, short [] data) {
        if (data == null) {
            data = new short [num];
        }
        read(data, null, 0, offset, num);
        return data;
    }

    /**
     * Reads all elements from specified offset into short array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public short [] read(long offset, short [] data) {
        return read(offset, (int)(getLength() - offset + 1), data);
    }

    /**
     * Reads all elements from specified offset into short array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public short [] read(short [] data) {
        return read(0, (int )getLength(), data);
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public short [] read(int [] indexes, short [] data) {
        if (data == null) {
            data = new short [indexes.length];
        }
        read(data, indexes, Integer.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public short [] read(long [] indexes, short [] data) {
        if (data == null) {
            data = new short [indexes.length];
        }
        read(data, indexes, Long.SIZE/Byte.SIZE, 0, 0);

        return data;
    }


    /**
     * Appends int array to column
     * @param data - data to append 
     * @return this column 
    **/
    public Column append(int [] data) {
        write(data);
        return this;
    }

    /**
     * Reads nun elements from specified offset into int array.
     * @param num - number of elements to read
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public int [] read(long offset, int num, int [] data) {
        if (data == null) {
            data = new int [num];
        }
        read(data, null, 0, offset, num);
        return data;
    }

    /**
     * Reads all elements from specified offset into int array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public int [] read(long offset, int [] data) {
        return read(offset, (int)(getLength() - offset + 1), data);
    }

    /**
     * Reads all elements from specified offset into int array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public int [] read(int [] data) {
        return read(0, (int )getLength(), data);
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public int [] read(int [] indexes, int [] data) {
        if (data == null) {
            data = new int [indexes.length];
        }
        read(data, indexes, Integer.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public int [] read(long [] indexes, int [] data) {
        if (data == null) {
            data = new int [indexes.length];
        }
        read(data, indexes, Long.SIZE/Byte.SIZE, 0, 0);

        return data;
    }


    /**
     * Appends long array to column
     * @param data - data to append 
     * @return this column 
    **/
    public Column append(long [] data) {
        write(data);
        return this;
    }

    /**
     * Reads nun elements from specified offset into long array.
     * @param num - number of elements to read
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public long [] read(long offset, int num, long [] data) {
        if (data == null) {
            data = new long [num];
        }
        read(data, null, 0, offset, num);
        return data;
    }

    /**
     * Reads all elements from specified offset into long array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public long [] read(long offset, long [] data) {
        return read(offset, (int)(getLength() - offset + 1), data);
    }

    /**
     * Reads all elements from specified offset into long array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public long [] read(long [] data) {
        return read(0, (int )getLength(), data);
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public long [] read(int [] indexes, long [] data) {
        if (data == null) {
            data = new long [indexes.length];
        }
        read(data, indexes, Integer.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public long [] read(long [] indexes, long [] data) {
        if (data == null) {
            data = new long [indexes.length];
        }
        read(data, indexes, Long.SIZE/Byte.SIZE, 0, 0);

        return data;
    }


    /**
     * Appends float array to column
     * @param data - data to append 
     * @return this column 
    **/
    public Column append(float [] data) {
        write(data);
        return this;
    }

    /**
     * Reads nun elements from specified offset into float array.
     * @param num - number of elements to read
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public float [] read(long offset, int num, float [] data) {
        if (data == null) {
            data = new float [num];
        }
        read(data, null, 0, offset, num);
        return data;
    }

    /**
     * Reads all elements from specified offset into float array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public float [] read(long offset, float [] data) {
        return read(offset, (int)(getLength() - offset + 1), data);
    }

    /**
     * Reads all elements from specified offset into float array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public float [] read(float [] data) {
        return read(0, (int )getLength(), data);
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public float [] read(int [] indexes, float [] data) {
        if (data == null) {
            data = new float [indexes.length];
        }
        read(data, indexes, Integer.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public float [] read(long [] indexes, float [] data) {
        if (data == null) {
            data = new float [indexes.length];
        }
        read(data, indexes, Long.SIZE/Byte.SIZE, 0, 0);

        return data;
    }


    /**
     * Appends double array to column
     * @param data - data to append 
     * @return this column 
    **/
    public Column append(double [] data) {
        write(data);
        return this;
    }

    /**
     * Reads nun elements from specified offset into double array.
     * @param num - number of elements to read
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public double [] read(long offset, int num, double [] data) {
        if (data == null) {
            data = new double [num];
        }
        read(data, null, 0, offset, num);
        return data;
    }

    /**
     * Reads all elements from specified offset into double array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public double [] read(long offset, double [] data) {
        return read(offset, (int)(getLength() - offset + 1), data);
    }

    /**
     * Reads all elements from specified offset into double array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public double [] read(double [] data) {
        return read(0, (int )getLength(), data);
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public double [] read(int [] indexes, double [] data) {
        if (data == null) {
            data = new double [indexes.length];
        }
        read(data, indexes, Integer.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public double [] read(long [] indexes, double [] data) {
        if (data == null) {
            data = new double [indexes.length];
        }
        read(data, indexes, Long.SIZE/Byte.SIZE, 0, 0);

        return data;
    }


    /**
     * Appends String array to column
     * @param data - data to append 
     * @return this column 
    **/
    public Column append(String [] data) {
        write(data);
        return this;
    }

    /**
     * Reads nun elements from specified offset into String array.
     * @param num - number of elements to read
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public String [] read(long offset, int num, String [] data) {
        if (data == null) {
            data = new String [num];
        }
        read(data, null, 0, offset, num);
        return data;
    }

    /**
     * Reads all elements from specified offset into String array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public String [] read(long offset, String [] data) {
        return read(offset, (int)(getLength() - offset + 1), data);
    }

    /**
     * Reads all elements from specified offset into String array.
     * @param offset - number of elements to skip
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public String [] read(String [] data) {
        return read(0, (int )getLength(), data);
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public String [] read(int [] indexes, String [] data) {
        if (data == null) {
            data = new String [indexes.length];
        }
        read(data, indexes, Integer.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

    /**
     * Reads elements specified by index array.
     * @param indexes - indexes to read
     * @param data - buffer for data, will allocated if it null
     * @return data that was read
    **/
    public String [] read(long [] indexes, String [] data) {
        if (data == null) {
            data = new String [indexes.length];
        }
        read(data, indexes, Long.SIZE/Byte.SIZE, 0, 0);

        return data;
    }

    // __END_OF_GENERATED_CODE__
}


