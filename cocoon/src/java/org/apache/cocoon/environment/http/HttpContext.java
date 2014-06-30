/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.environment.http;

import org.apache.cocoon.environment.Context;

import javax.servlet.ServletContext;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Enumeration;
import java.io.InputStream;

/**
 *
 * Implements the {@link org.apache.cocoon.environment.Context} interface
 */

public class HttpContext implements Context {

    /** The ServletContext */
    private ServletContext servletContext = null;

    /**
     * Constructs a HttpContext object from a ServletContext object
     */
    public HttpContext (ServletContext servletContext) {
        this.servletContext = servletContext;
    }

    public Object getAttribute(String name) {
        return servletContext.getAttribute(name);
    }

    public void setAttribute(String name, Object value) {
        servletContext.setAttribute(name, value);
    }
    
    public void removeAttribute(String name) {
        servletContext.removeAttribute(name);
    }
    
    public Enumeration getAttributeNames() {
        return servletContext.getAttributeNames();
    }

    public URL getResource(String path)
       throws MalformedURLException {
       return servletContext.getResource(path);
    }

    public InputStream getResourceAsStream(String path) { 
	return servletContext.getResourceAsStream(path);
    }

    public String getRealPath(String path) {
        if (path.equals("/")) {
            String value = servletContext.getRealPath(path);
            if (value == null) {
                // Try to figure out the path of the root from that of WEB-INF
                try {
                value = this.servletContext.getResource("/WEB-INF").toString();
                } catch (MalformedURLException mue) {
                    throw new RuntimeException("Cannot determine the base URL");
                }
                value = value.substring(0,value.length()-"WEB-INF".length());
            }
            return value;
        }
        return servletContext.getRealPath(path);
    }

    public String getMimeType(String file) {
      return servletContext.getMimeType(file);
    }

    public String getInitParameter(String name) {
        return servletContext.getInitParameter(name);
    }
}
