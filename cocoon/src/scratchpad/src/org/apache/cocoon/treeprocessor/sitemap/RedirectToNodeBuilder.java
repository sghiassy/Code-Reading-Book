/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

import org.apache.cocoon.treeprocessor.AbstractProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.CategoryNode;
import org.apache.cocoon.treeprocessor.CategoryNodeBuilder;
import org.apache.cocoon.treeprocessor.LinkedProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class RedirectToNodeBuilder extends AbstractProcessingNodeBuilder
  implements LinkedProcessingNodeBuilder {

    private CallNode callNode;
    private String resourceName;
    
    /** This builder has no parameters -- return <code>false</code> */
    protected boolean hasParameters() {
        return false;
    }

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        // Is it a redirect to resource ?
        String uri = config.getAttribute("uri", null);
        if (uri != null) {
            ProcessingNode URINode = new RedirectToURINode(uri, config.getAttributeAsBoolean("session", false));   
            return this.treeBuilder.setupNode(URINode, config);
            
        } else {
            this.resourceName = config.getAttribute("resource");
            this.callNode = new CallNode();
            this.treeBuilder.setupNode(this.callNode, config);
            
            String target = config.getAttribute("target", null);
            if (target != null) {
                Map params = new HashMap(1);
                params.put("target", MapStackResolver.getResolver(target));
                this.callNode.setParameters(params);
            }
            return this.callNode;
        }
    }
    
    public void linkNode() throws Exception {
        
        if (this.callNode != null) {
            CategoryNode resources = CategoryNodeBuilder.getCategoryNode(this.treeBuilder, "resources");
            
            if (resources == null) {
                String msg = "This sitemap contains no resources. Cannot redirect at " +
                    this.callNode.getLocation();
                getLogger().error(msg);
                throw new ConfigurationException(msg);
            }
            
            this.callNode.setResource(
                resources,
                this.resourceName
            );
        }
    }
}
