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
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.treeprocessor.ContainerNodeBuilder;
import org.apache.cocoon.treeprocessor.ProcessingNode;

/**
 * Builds a &lt;map:pipelines&gt;
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class PipelinesNodeBuilder extends ContainerNodeBuilder implements ThreadSafe {

    public ProcessingNode buildNode(Configuration config) throws Exception {
        PipelinesNode node = new PipelinesNode();
        this.treeBuilder.setupNode(node, config);
        
        ProcessingNode[] children = buildChildNodes(config);
        if (children.length == 0) {
            String msg = "There must be at least one pipeline at " + config.getLocation();
            getLogger().error(msg);
            throw new ConfigurationException(msg);
        }
            
        node.setChildren(children);
        
        return node;
    }
}
