package org.temk.ds;

import java.nio.ByteOrder;

public interface DataStorage {

    /**
     * Open data storage or create it
     * @param path - path to storage
     * @param mode - possible values are 
     * <ul>
     *  <li>r - read
     *  <li>w - write
     *  <li>t - truncate
     *  <li>c - create
     * </ul>
     * @param buffSiz - size of caching buffer (in number of elements)
    **/
    void open(String path, String mode, long buffSiz);
    
    /**
     * Open data storage with mode = "rwc" and buffSiz = 1024
     * @param path - path to storage
    **/
    void open(String path);
    
    /**
     * Close object and release resources
    **/
    void close();

    /**
     * check status of object
     * @return true if storage is open
    */
    boolean isOpen();
    
    /**
     * @return number of columns in data storage
    **/
    long getColumnNumber();
            
    /**
     * get column by name
     * @param name - column name
     * @return column
    **/
    Column getColumn(String name);
    
    /**
     * get column by index
     * @param index - column index
     * @return column
    **/
    Column getColumn(long index);
    
    /**
     * Create new column. 
     * @param type - type to store underlying data
     * @param name - column name
     * @param endian - byte order  
     * @param index - column index
     * @param auxSiz - number of bytes to store strings via dictionary. Valid values are 1,2,4 and 8
     * @return new column
    **/
    Column addColumn(Type type, String name, ByteOrder endian, long index, long auxSiz);
    
    /**
     * Create new column with default values. (endian = host)
     * @param type - type to store underlying data
     * @param name - column name
     * @return new column
     */
    Column addColumn(Type type, String name);
    
    /**
     * Create new column with default values. (name = "var" and endian = host).
     * @param type - type to store underlying data
     * @return new column
     */
    Column addColumn(Type type);
    
    /**
     * flush all data to disk
    **/
    void flush();
}
/*
		
		column &add(type_t type, const string &name = "", int endian = DS_ENDIAN_HOST, size_t index = size_t(-1), int siz = -1);
		
 */