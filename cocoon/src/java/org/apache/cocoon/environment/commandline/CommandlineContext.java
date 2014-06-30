/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.environment.commandline;

import org.apache.avalon.excalibur.collections.IteratorEnumeration;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.cocoon.environment.Context;

import java.io.File;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Enumeration;
import java.util.Map;
import java.util.HashMap;
import java.io.InputStream;

/**
 *
 * Implements the {@link org.apache.cocoon.environment.Context} interface
 */

public class CommandlineContext extends AbstractLoggable implements Context {

    /** The context directory path*/
    private String contextDir;
    
    /** The context attributes */
    private Map attributes;

    /**
     * Constructs a CommandlineContext object from a ServletContext object
     */
    public CommandlineContext (String contextDir) {
        String contextDirPath = new File(contextDir).getAbsolutePath();
        // store contextDirPath as is don't remove trailing /.
        this.contextDir = contextDirPath;
        this.attributes = new HashMap();
    }

    public Object getAttribute(String name) {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("CommandlineContext: getAttribute=" + name);
        }
        return this.attributes.get(name);
    }

    public void setAttribute(String name, Object value) {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("CommandlineContext: setAttribute=" + name);
        }
        this.attributes.put(name, value);
    }
    
    public void removeAttribute(String name) {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("CommandlineContext: removeAttribute=" + name);
        }
        this.attributes.remove(name);
    }
    
    public Enumeration getAttributeNames() {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("CommandlineContext: getAttributeNames");
        }
        return new IteratorEnumeration(this.attributes.keySet().iterator());
    }

    public URL getResource(String path) throws MalformedURLException {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("CommandlineContext: getResource=" + path);
        }
        // rely on File to build correct File and URL
        File f = new File( contextDir, path );
        return f.toURL();
    }

    public String getRealPath(String path) {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("CommandlineContext: getRealPath=" + path);
        }
        // rely on File to build correct File and URL
        File f = new File( this.contextDir, path );
        return f.getAbsolutePath();
    }

    public String getMimeType(String file) {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("CommandlineContext: getMimeType=" + file);
        }
        //return servletContext.getMimeType(file);
        return null;
    }
    
    public String getInitParameter(String name) {
        getLogger().debug("CommandlineContext: getInitParameter=" + name);
        return null;
    }

    public InputStream getResourceAsStream(String path){ 
        getLogger().debug("CommandlineContext: getResourceAsStream "+path);
	return null;
    }
}
