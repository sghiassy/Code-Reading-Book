/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Redirector;

import org.apache.cocoon.sitemap.PatternException;

import org.apache.cocoon.treeprocessor.AbstractProcessingNode;
import org.apache.cocoon.treeprocessor.InvokeContext;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.List;
import java.util.Map;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class RedirectToURINode extends AbstractProcessingNode {
        
    /** The 'uri' attribute */
    private MapStackResolver uri;
    
    private boolean keepSession;

    public RedirectToURINode(String uri, boolean keepSession) throws PatternException {
        this.uri = MapStackResolver.getResolver(uri);
    }
    
    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        String resolvedURI = uri.resolve(context.getMapStack());
        
        if (getLogger().isInfoEnabled()) {
            getLogger().info("Redirecting to '" + resolvedURI + "' at " + this.getLocation());
        }

        env.redirect (this.keepSession, resolvedURI);

        return true;
    }
}
