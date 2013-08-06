package org.temk.ds;

import java.nio.ByteOrder;

public interface Column {
    /**
     * remove current column
    **/
    void remove(); 

    /**
     * truncate current column to newLength
     * @param newLength - new length of column
     * @return current column 
     */
    Column truncate(long newLength);
    
    /**
     * flush data belongs to current column to disk and rewrite index.
    **/
    void flush();    
    
    /**
     * append data to column
     * @param array - data to append. Should be an array of primitives.
     * @return current column 
    **/
    Column append(Object array);

    
    /**
     * read column data
     * @param data - primitives array to populate, if data is null, then allocate it.
     * @return read data
    **/
    Object read(Object data);
    
    /**
     * read column from particular offset
     * @param offset - how many elements to skip
     * @param data - primitives array to populate, if data is null, then allocate it.
     * @return read data
    **/
    Object read(long offset, Object data);
    
    
    /**
     * read column partially, by array of indexes.
     * @param data - primitives array to populate, if data is null, then allocate it.
     * @param indexes - indexes to read. Should be of type: int[], long[], short[] or byte
     * @return read data
    **/
    Object read(Object indexes, Object data);    

    /**
     * @return index of current column
    **/
    long getIndex();
    
    /**
     * @return name of current column
    **/
    String getName();
    
    
    /**
     * @return length of current column
    **/
    long getLength();

    /**
     * @return size of element 
    **/
    int getElementSize();
    
    /**
     * @return byte order of current order
    **/
    ByteOrder getByteOrder();
    
    /**
     * @return type of current column
    **/
    Type getType();    
}
