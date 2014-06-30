/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

import java.io.IOException;
import java.io.OutputStream;

/**
 * This is an OutputStream which forwards all received bytes to another
 * output stream and in addition caches all bytes, thus acting like a
 * TeeOutputStream.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:08 $
 */

public final class CachingOutputStream
extends OutputStream {

    private OutputStream receiver;

    /** The buffer for the compile xml byte stream. */
    private byte buf[];

    /** The number of valid bytes in the buffer. */
    private int bufCount;

    public CachingOutputStream(OutputStream os) {
        this.receiver = os;
        this.buf = new byte[1024];
        this.bufCount = 0;
    }

    public byte[] getContent() {
        byte newbuf[] = new byte[this.bufCount];
        System.arraycopy(this.buf, 0, newbuf, 0, this.bufCount);
        return newbuf;
    }

    public void write(int b) throws IOException {
        this.receiver.write(b);
        int newcount = this.bufCount + 1;
        if (newcount > this.buf.length) {
            byte newbuf[] = new byte[Math.max(this.buf.length << 1, newcount)];
            System.arraycopy(this.buf, 0, newbuf, 0, this.bufCount);
            this.buf = newbuf;
        }
        this.buf[this.bufCount] = (byte)b;
        this.bufCount = newcount;
    }

    public void write( byte b[] ) throws IOException {
        this.write(b, 0, b.length);
    }

    public void write(byte b[], int off, int len) throws IOException {
        this.receiver.write(b, off, len);
        if (len == 0) return;
        int newcount = this.bufCount + (len-off);
        if (newcount > this.buf.length) {
            byte newbuf[] = new byte[Math.max(this.buf.length << 1, newcount)];
            System.arraycopy(this.buf, 0, newbuf, 0, this.bufCount);
            this.buf = newbuf;
        }
        System.arraycopy(b, off, this.buf, this.bufCount, len);
        this.bufCount = newcount;
    }

    public void flush() throws IOException {
        this.receiver.flush();
    }

    public void close() throws IOException {
        this.receiver.close();
    }


}
