/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentManager;

import org.apache.cocoon.components.source.SourceHandler;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.ProcessingException;

import org.xml.sax.SAXException;

import java.io.IOException;
import java.net.URL;

/**
 * A <code>SourceResolver</code> that resolves URIs relative to an <code>Environment</code>.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public class EnvironmentSourceResolver implements SourceResolver, Disposable {
    
    private ComponentManager manager;
    
    private Source contextSource;
    
    private SourceHandler sourceHandler;

    public EnvironmentSourceResolver(ComponentManager manager, Environment env) throws Exception {
        this.manager = manager;
        this.contextSource = env.resolve("");
        this.sourceHandler = (SourceHandler)manager.lookup(SourceHandler.ROLE);
    }
    
    /**
     * Resolve an entity.
     */
    public Source resolve(String systemId) throws ProcessingException, SAXException, IOException {
        if (systemId == null) throw new SAXException("Cannot resolve a null system ID !");

        URL context = new URL(this.contextSource.getSystemId());

        if (systemId.length() == 0)
            return this.sourceHandler.getSource(null, context, systemId);
        if (systemId.indexOf(":") > 1)
            return this.sourceHandler.getSource(null, systemId);
        if (systemId.charAt(0) == '/')
            return this.sourceHandler.getSource(null, context.getProtocol() + ":" + systemId);
        return this.sourceHandler.getSource(null, context, systemId);
    }

    public void dispose() {
        if (contextSource != null)
        {
            contextSource.recycle();
        }
        manager.release(sourceHandler);
    }
}
