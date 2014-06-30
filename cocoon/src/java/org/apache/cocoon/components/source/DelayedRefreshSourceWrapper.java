/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.source;

import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.ModifiableSource;
import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.xml.XMLizable;
import org.xml.sax.InputSource;
import java.io.IOException;
import java.io.InputStream;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;

/**
 * A wrapper around a <code>Source</code> that reduces the number of calls to
 * <code>Source.getLastModified()</code> which can be a costly operation.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version $Id: DelayedRefreshSourceWrapper.java,v 1.1 2002/01/03 12:31:14 giacomo Exp $
 */
public final class DelayedRefreshSourceWrapper implements Source, ModifiableSource, XMLizable {
    
    private Source source;
    
    private long delay;
    
    private long nextCheckTime = 0;
    
    private long lastModified = 0;
    
    private boolean isModifiableSource;
    
    /**
     * Creates a wrapper for a <code>Source</code> which ensures that
     * <code>Source.getLastModified()</code> won't be called more than once per
     * <code>delay</code> milliseconds period.
     * 
     * @param source the wrapped <code>Source</code>
     * @param delay  the last-modified refresh delay, in milliseconds
     */
    public DelayedRefreshSourceWrapper(Source source, long delay) {
        this.source = source;
        this.delay = delay;
        this.isModifiableSource = source instanceof ModifiableSource;
    }
    
    /**
     * Get the last modification time for the wrapped <code>Source</code>. The
     * age of the returned information is guaranteed to be lower than or equal to
     * the delay specified in the constructor.
     *
     * @return the last modification time.
     */
    public final long getLastModified() {
        
        // Do we have to refresh the source ?
        if (System.currentTimeMillis() >= nextCheckTime) {
            // Yes
            this.refresh();
        }
        
        return this.lastModified;
    }

    /**
     * Force the refresh of the wrapped <code>Source</code>, even if the refresh period
     * isn't over, and starts a new period.
     */
    public final void refresh() {
 
        this.nextCheckTime = System.currentTimeMillis() + this.delay;
        // Refresh modifiable sources
        if (this.isModifiableSource) {
            ((ModifiableSource)this.source).refresh();
        }
        
        // Keep the last modified date
        this.lastModified = source.getLastModified();
    }
    
    public final long getContentLength() {
        return this.source.getContentLength();
    }

    public final InputStream getInputStream() throws ProcessingException, IOException {
        return this.source.getInputStream();
    }

    public final InputSource getInputSource() throws ProcessingException, IOException {
        return this.source.getInputSource();
    }

    public final String getSystemId() {
        return this.source.getSystemId();
    }

    public final void recycle() {
        this.source.recycle();
    }

    public final void toSAX(ContentHandler handler) throws SAXException, ProcessingException {
        this.source.toSAX(handler);
    }
}
