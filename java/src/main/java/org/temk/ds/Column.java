package org.temk.ds;

import java.nio.ByteOrder;

public class Column {
    private long handle;
    private String name;
    private DataStorage parent;
    
    private native Object read(long offset, Object indexes, Object data);
    
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
     * append data to column
     * @param array - data to append. Should be an array of primitives.
     * @return current column 
    **/
    public native Column append(Object array);

    
    /**
     * read column data
     * @param data - primitives array to populate, if data is null, then allocate it.
     * @return read data
    **/
    public Object read(Object data) {
        return read(0, null, data);
    }
    
    /**
     * read column from particular offset
     * @param offset - how many elements to skip
     * @param data - primitives array to populate, if data is null, then allocate it.
     * @return read data
    **/    
    public Object read(long offset, Object data) {
        return read(offset, null, data);
    }
    
    
    /**
     * read column partially, by array of indexes.
     * @param data - primitives array to populate, if data is null, then allocate it.
     * @param indexes - indexes to read. Should be of type: int[], long[], short[] or byte
     * @return read data
    **/
    public Object read(Object indexes, Object data) {
        return read(0, indexes, data);
    }    

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
}
