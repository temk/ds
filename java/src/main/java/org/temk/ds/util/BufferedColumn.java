package org.temk.ds.util;

import org.temk.ds.Column;

public final class BufferedColumn {
    protected final Buffer buffer;
    protected final Column column;

    protected BufferedColumn(Buffer buffer, Column column) {
        this.buffer = buffer;
        this.column = column;
    }
    
    protected void flush(long num) {
        column.write(buffer.getArray(), num);
    }
    
    protected void fetch(long offs, long num) {
        column.read(buffer.getArray(), null, 0, offs, num);
    }
}
