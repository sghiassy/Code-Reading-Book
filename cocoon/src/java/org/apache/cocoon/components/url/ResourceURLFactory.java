/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.url;

import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.cocoon.util.ClassUtils;

import java.net.MalformedURLException;
import java.net.URL;

/**
 * @author <a href="mailto:giacomo@apache.org">Giacomo Pati</a>
 * @version $Id: ResourceURLFactory.java,v 1.1 2002/01/03 12:31:15 giacomo Exp $
 */
public class ResourceURLFactory extends AbstractLoggable implements URLFactory, Contextualizable {

    /**
     * The context
     */
    protected Context context;

    /**
     * Create a URL from a location. This method supports the
     * <i>resource://</i> pseudo-protocol for loading resources
     * accessible to this same class' <code>ClassLoader</code>
     *
     * @param location The location
     * @return The URL pointed to by the location
     * @exception MalformedURLException If the location is malformed
     */
    public URL getURL(String location) throws MalformedURLException {
        URL u = ClassUtils.getResource(location);
        if (u != null)
            return u;
        else {
            getLogger().error(location + " could not be found. (possible classloader problem)");
            throw new RuntimeException(location + " could not be found. (possible classloader problem)");
        }
    }

    public URL getURL(URL base, String location) throws MalformedURLException {
        return getURL (base.toExternalForm() + location);
    }

    /**
     * Get the context
     */
    public void contextualize(Context context) throws ContextException {
        if (this.context == null) {
            this.context = context;
        }
    }
}
