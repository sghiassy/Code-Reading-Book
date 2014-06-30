/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.ProcessingException;

import java.util.*;

/**
 * A generic container node that just invokes its children.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public final class CategoryNode extends AbstractParentProcessingNode {

    /** The name of this category */
    private String categoryName;

    /** The Map of named nodes in this category */
    private Map nodes;
    
    public void setCategory(String categoryName, Map nodes) {
        this.categoryName = categoryName;
        this.nodes = (nodes != null) ? nodes : new HashMap(0);
    }
    
    public final boolean invoke(Environment env, InvokeContext context) throws Exception {
        String msg = "Cannot invoke " + this.categoryName + " at " + getLocation();
        getLogger().error(msg);
        throw new ProcessingException(msg);
    }
    
    public final ProcessingNode getNodeByName(String name) throws Exception {
        ProcessingNode node = (ProcessingNode)nodes.get(name);
        if (node == null) {
            String msg = "Unknown " + this.categoryName + " named '" + name + "' at " + getLocation();
            getLogger().error(msg);
            throw new ProcessingException(msg);
        }
        
        return node;
    }
    
    public final boolean invokeByName(String name, Environment env, InvokeContext context)
      throws Exception {
        
        return getNodeByName(name).invoke(env, context);
    }
}
