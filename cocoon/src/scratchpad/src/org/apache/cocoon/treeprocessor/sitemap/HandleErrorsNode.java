/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.ResourceNotFoundException;

import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.treeprocessor.AbstractParentProcessingNode;
import org.apache.cocoon.treeprocessor.InvokeContext;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.*;

/**
 * Handles &lt;map:handle-errors&gt;
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public final class HandleErrorsNode extends AbstractParentProcessingNode {

    private ProcessingNode[] children;
    private int statusCode;
    
    public HandleErrorsNode(int statusCode) {
        this.statusCode = statusCode;
    }
    
    public int getStatusCode() {
        return this.statusCode;
    }
    
    public void setChildren(ProcessingNode[] nodes)
    {
        this.children = nodes;
    }
    
    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        if (getLogger().isInfoEnabled()) {
            getLogger().info("Processing handle-errors at " + getLocation());
        }
        
        context.getEventPipeline().setGenerator("!notifying-generator!", "", Parameters.EMPTY_PARAMETERS);
            
        return invokeNodes(this.children, env, context);
    }
}
