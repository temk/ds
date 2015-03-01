package org.temk.ds;

import java.nio.ByteOrder;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

public class DataStorage {
    static {
        load_native();
    }
    /**
     * pointer to C++ DataStorage object
    **/
    protected long handle = 0;
    
    
    public static final long DEFAULT_BUFF_SIZ = 1024;
    
    protected Map<Long, Column> columns = new HashMap<Long, Column>();
    
    protected native long getColumnByIndex(long index);
    protected native long getColumnByName(String name);
    public static native String getVersion();
        
    /**
     * Constructor.
    **/
    public DataStorage() {
        
    }
    
    /**
     * Constructor. Opens storage. see @open
    **/
    public DataStorage(String path, String mode, long buffSiz) {
        open(path, mode, buffSiz);
        
//        long columns [] = getColumns();
    }

    /**
     * Constructor. Opens storage. see @open
    **/
    public DataStorage(String path) {
        open(path, "rwc", DEFAULT_BUFF_SIZ);
    }
    
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
    public native void open(String path, String mode, long buffSiz);
    
    /**
     * Open data storage with mode = "rwc" and buffSiz = 1024
     * @param path - path to storage
    **/
    public  void open(String path) {
        open(path, "rwc", DEFAULT_BUFF_SIZ);
    }
    
    /**
     * Close object and release resources
    **/
    public native void close();

    /**
     * check status of object
     * @return true if storage is open
    */
    public boolean isOpen() {
        return handle != 0;
    }
    
    /**
     * @return list of all columns
    **/
    public Collection<Column> getColumns() {
        return columns.values();
    }
    
    /**
     * @return number of columns in data storage
    **/
    public native long getColumnNumber();
            
    /**
     * get column by name
     * @param name - column name
     * @return column
    **/
    public Column getColumn(String name) {
        return columns.get(getColumnByName(name));
    }
    
    /**
     * get column by index
     * @param index - column index
     * @return column
    **/
    public Column getColumn(long index) {
        return columns.get(getColumnByIndex(index));
    }
    
    /**
     * Create new column. 
     * @param type - type to store underlying data
     * @param name - column name
     * @param endian - byte order  
     * @param index - column index
     * @param extType - integral type to store strings via dictionary. use Type.INVALID when it not relevant
     * @return new column
    **/
    public native Column addColumn(Type type, Type extType, String name, int width, ByteOrder endian, long index);

    /**
     * Create new column. 
     * @param type - type to store underlying data
     * @param name - column name
     * @param endian - byte order  
     * @param index - column index
     * @param extType - integral type to store strings via dictionary. use Type.INVALID when it not relevant
     * @return new column
    **/
    public Column addColumn(Type type, Type extType, String name, ByteOrder endian, long index) {
        return addColumn(type, extType, name, 1, endian, index);
    }
    
    /**
     * Create new column with default values. (endian = host)
     * @param type - type to store underlying data
     * @param name - column name
     * @return new column
     */
    public Column addColumn(Type type, String name) {
        Type extType = type.isString() ? Type.UINT32 :  Type.INVALID;
        return addColumn(type, extType, name, ByteOrder.nativeOrder(), -1);
    }
    
    /**
     * Create new column with default values. (endian = host)
     * @param type - type to store underlying data
     * @param name - column name
     * @return new column
     */
    public Column addColumn(Type type, String name, int width) {
        Type extType = type.isString() ? Type.UINT32 :  Type.INVALID;
        return addColumn(type, extType, name, width, ByteOrder.nativeOrder(), -1);
    }
    
    /**
     * Create new column with default values. (name = "var" and endian = host).
     * @param type - type to store underlying data
     * @return new column
     */
    public Column addColumn(Type type) {
        return addColumn(type, "var");
    }
    
    /**
     * flush all data to disk
    **/
    public native void flush();
    
    
    public native void addMeta(String key, String value);
    public native Map<String, String> getMeta();

    public boolean hasColumn(String name) {
        for (Column c: columns.values())  {
            if (c.getName().equals(name)) {
                return true;
            }
        }
        return false;
    }
    
    private static void load_native() {
//        try {
//            File file = File.createTempFile("libdsjni", ".so");
//            file.setExecutable(true);
//            file.deleteOnExit();
//            
//            InputStream is = DataStorage.class.getResourceAsStream("/libdsjni.so");
//            FileOutputStream os = new FileOutputStream(file);
//            
//            byte [] buffer = new byte[1 << 20];
//            while(is.available() > 0) {
//                int rd = is.read(buffer);
//                os.write(buffer, 0, rd);
//            }
//            is.close();
//            os.close();
//
//            System.load(file.getAbsolutePath());
//        } catch (IOException t) {
//            throw new RuntimeException("Fail to load libdsjni.so: ", t);
//        }
        System.loadLibrary("dsjni");
    }
    
}
