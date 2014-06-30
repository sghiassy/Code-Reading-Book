/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.thread.ThreadSafe;

import java.util.*;

/**
 * Builds a generic container node.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public class ContainerNodeBuilder extends AbstractParentProcessingNodeBuilder implements ThreadSafe {

    /** This builder has no parameters -- return <code>false</code> */
    protected boolean hasParameters() {
        return false;
    }

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        ContainerNode node = new ContainerNode();
        setupNode(node, config);
        
        return node;
    }
    
    protected void setupNode(ContainerNode node, Configuration config)throws Exception {
        
        this.treeBuilder.setupNode(node, config);
        
        ProcessingNode[] children = buildChildNodes(config);
        if (children.length == 0) {
            String msg = "There must be at least one child at " + config.getLocation();
            getLogger().error(msg);
            throw new ConfigurationException(msg);
        }
            
        node.setChildren(children);
    }
}
