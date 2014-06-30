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
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.LinkedProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class CallNodeBuilder extends AbstractProcessingNodeBuilder
  implements LinkedProcessingNodeBuilder {

    private CallNode node;
    private String resourceName;
    
    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        this.resourceName = config.getAttribute("resource");
        this.node = new CallNode();
        this.treeBuilder.setupNode(this.node, config);
        
        return this.node;
    }
    
    public void linkNode() throws Exception {
        CategoryNode resources = CategoryNodeBuilder.getCategoryNode(this.treeBuilder, "resources");
        
        if (resources == null) {
            String msg = "This sitemap contains no resources. Cannot call at " + this.node.getLocation();
            getLogger().error(msg);
            throw new ConfigurationException(msg);
        }
        
        this.node.setResource(
            resources,
            this.resourceName
        );
    }
}
