/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.Enumeration;
import java.io.InputStream;

/**
 * Defines an interface to provide client context information .
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/29 20:05:54 $
 *
 */

public interface Context {

    Object getAttribute(String name);
    
    void setAttribute(String name, Object value);
    
    void removeAttribute(String name);
    
    Enumeration getAttributeNames();

    URL getResource(String path) throws MalformedURLException;

    String getRealPath(String path);

    String getMimeType(String file);

    String getInitParameter(String name);

    InputStream getResourceAsStream(String path);
}
