/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment;

import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.source.URLSource;
import org.apache.cocoon.components.url.URLFactory;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.net.URL;

/**
 * A <code>SourceResolver</code> based on a <code>URLFactory</code>.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:15 $
 */

public class URLFactorySourceResolver implements SourceResolver {

    /** The component manager */
    protected ComponentManager manager;
    
    /** The URL factory */
    protected URLFactory urlFactory;
    
    /**
     * Creates an <code>URLFactorySourceResolver</code> with a component manager.
     * The <code>URLFactory</code> is looked up in the component manager.
     */
    public URLFactorySourceResolver(ComponentManager manager)
      throws ComponentException {
        this.manager = manager;
        this.urlFactory = (URLFactory)manager.lookup(URLFactory.ROLE);
    }
    
    /**
     * Creates an <code>URLFactorySourceResolver</code> with a component manager and
     * a <code>URLFactory</code> that will be used to resolve URLs.
     */
    public URLFactorySourceResolver(URLFactory factory, ComponentManager manager) {
        this.urlFactory = factory;
        this.manager = manager;
    }
    
    /**
     * Resolve the source.
     *
     * @param systemID This is either a system identifier
     *        (<code>java.net.URL</code>) or a local file.
     */
    public Source resolve(String systemID)
      throws ProcessingException, SAXException, IOException {
        
        URL url = this.urlFactory.getURL(systemID);
        return new URLSource(url, this.manager);
    }
}
