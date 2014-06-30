/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.treeprocessor.AbstractParentProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.ProcessingNode;

/**
 * Builds a &lt;map:handle-errors&gt;
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class HandleErrorsNodeBuilder extends AbstractParentProcessingNodeBuilder implements ThreadSafe {

    /** This builder has no parameters -- return <code>false</code> */
    protected boolean hasParameters() {
        return false;
    }
    
    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        HandleErrorsNode node = new HandleErrorsNode(config.getAttributeAsInteger("type", 500));
        this.treeBuilder.setupNode(node, config);
        
        // Get all children
        node.setChildren(buildChildNodes(config));
        
        return node;  
    }
}
