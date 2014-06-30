/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;

import java.util.List;
import java.util.Map;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/30 14:35:07 $
 */

public abstract class AbstractParentProcessingNode extends AbstractProcessingNode {

    /**
     * Invoke all nodes of a node array in order, until one succeeds.
     *
     * @parameter currentMap the <code>Map<code> of parameters produced by this node,
     *            which is added to <code>listOfMap</code>.
     */
    protected final boolean invokeNodes(
        ProcessingNode[] nodes,
        Environment env,
        InvokeContext context,
        Map currentMap)
      throws Exception {
        
        context.pushMap(currentMap);

        for (int i = 0; i < nodes.length; i++) {
            if (nodes[i].invoke(env, context)) {
                // Success
                context.popMap();
                return true;
            }
        }
        
        // No success
        context.popMap();
        return false;
    }
    
    /**
     * Invoke all nodes of a node array in order, until one succeeds.
     */
    protected final boolean invokeNodes (
        ProcessingNode[] nodes,
        Environment env,
        InvokeContext context)
      throws Exception {
        
        for (int i = 0; i < nodes.length; i++) {
            if (nodes[i].invoke(env, context)) {
                return true;
            }
        }
        
        return false;
    }
}
