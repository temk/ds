package org.temk.ds.example;

import org.temk.ds.Persistent;

public class Base {
    public static enum Number {
        UNKNOWN,
        ONE, 
        TWO,
        THREE
    }
    
   @Persistent
    private Number number;
    
    public Base(Number number) {
        this.number = number;
    }
    
    public Number getNumber() {
        return number;
    }
}
