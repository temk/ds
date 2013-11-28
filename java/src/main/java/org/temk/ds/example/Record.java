package org.temk.ds.example;


import org.temk.ds.DataStorage;
import org.temk.ds.persistent.Persistent;
import org.temk.ds.persistent.PostLoad;
import org.temk.ds.persistent.PrePersist;
import org.temk.ds.util.DataStorageReader;
import org.temk.ds.util.DataStorageWriter;


public class Record extends Base {
    @Persistent
    private String name;

    @Persistent
    private double value;

    @Persistent("Timestamp")
    private long timestamp;

    public Record() {
        super(Number.UNKNOWN);
    }
    public Record(String name, double value, long timestamp, Number number) {
        super(number);
        this.name = name;
        this.value = value;
        this.timestamp = timestamp;
    }

    @PostLoad
    private void hello() {
        System.out.println("Hello " + name + "!");
    }
    
    @PrePersist
    private void bye() {
        System.out.println("Bye " + name + "!");
    }
    
    @Override
    public String toString() {
        return "Record{" + "name=" + name + ", value=" + value + ", timestamp=" + timestamp + ", number=" + getNumber().toString() + '}';
    }

    public static void main(String [] args) {
        DataStorage ds = new DataStorage("/tmp/temp.ds", "rwct", 3);                
        DataStorageWriter<Record> writer = new DataStorageWriter<Record>(ds, Record.class, 12, true);
        
        for (int k = 0; k < 20; ++ k) {
            writer.write(new Record("record", k * 2.0, k, Number.ONE));
        }
        
        writer.write(new Record("Ok", 0, 0, Number.ONE));
        
        writer.flush();
        writer.close();
        
        ds = new DataStorage("/tmp/temp.ds");     
        DataStorageReader<Record> reader = new DataStorageReader<Record>(ds, Record.class, 15);
        
        for (Record r = reader.read(); r!= null; r = reader.read()) {
            System.out.println(r.toString());
        }
        
        ds.close();
    }
}
