package org.temk.ds.example;


import org.temk.ds.DataStorage;
import org.temk.ds.persistent.Meta;
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

    @Meta
    private String type;
    
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
//        System.out.println("Hello " + name + "!");
    }
    
    @PrePersist
    private void bye() {
//        System.out.println("Bye " + name + "!");
    }
    
    @Override
    public String toString() {
        return "Record{" + "name=" + name + ", value=" + value + ", timestamp=" + timestamp + ", number=" + getNumber().toString() + ", type = " + type + '}';
    }

    private static long TRIALS = 1000000;
    public static void main(String [] args) {
        DataStorage ds = new DataStorage("/tmp/temp.ds", "rwct", 0);                
        ds.addMeta("type", "any/type");
        DataStorageWriter<Record> writer = new DataStorageWriter<Record>(ds, Record.class, 1 << 20, true);
        
        Record record = new Record("record", 2.0, 10, Number.ONE);
        long t1 = System.nanoTime();
        for (int k = 0; k < TRIALS; ++ k) {
            writer.write(record);
        }
        long t2 = System.nanoTime();
        
        writer.write(new Record("Ok", 0, 0, Number.ONE));
        
        writer.flush();
        writer.close();
        
        ds = new DataStorage("/tmp/temp.ds", "r", 0);     
        DataStorageReader<Record> reader = new DataStorageReader<Record>(ds, Record.class, 1 << 20);
        
        long t3 = System.nanoTime();
        for (record = reader.read(record); record!= null; record = reader.read()) {
         //   System.out.println(r.toString());
        }
        long t4 = System.nanoTime();
        
        ds.close();
        
        System.out.println("Write: " + (t2 - t1) / 1000000000.0 + " sec, Read: " +  (t4 - t3) /1000000000.0 + " sec");
        System.out.println("Write: " + (t2 - t1) * 1.0 /TRIALS + "ns, Read: " +  (t4 - t3) * 1.0 /TRIALS + " ns");
    }
}
