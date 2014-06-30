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

import org.apache.cocoon.treeprocessor.AbstractParentProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.ProcessingNode;
import org.apache.cocoon.treeprocessor.ProcessingNodeBuilder;

import java.util.*;

/**
 * Builds a &lt;map:pipeline&gt;
 
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class PipelineNodeBuilder extends AbstractParentProcessingNodeBuilder implements ThreadSafe {

    /** This builder has no parameters -- return <code>false</code> */
    protected boolean hasParameters() {
        return true;
    }

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        PipelineNode node = new PipelineNode();
        this.treeBuilder.setupNode(node, config);

        node.setInternalOnly(config.getAttributeAsBoolean("internal-only", false));

        ProcessingNode error404Handler = null;
        ProcessingNode error500Handler = null;
        
        Configuration[] childConfigs = config.getChildren();
        List children = new ArrayList();
        
        childLoop : for (int i = 0; i < childConfigs.length; i++) {
            Configuration childConfig = childConfigs[i];
            if (!isChild(childConfig)) {
                continue childLoop;
            }
                
            ProcessingNodeBuilder builder = this.treeBuilder.createNodeBuilder(childConfig);
            
            if (builder instanceof HandleErrorsNodeBuilder) {
                // Error handler : check type
                HandleErrorsNode handler = (HandleErrorsNode)builder.buildNode(childConfig);
                int type = handler.getStatusCode();
                
                if ( (type == 404 && error404Handler != null) ||
                     (type == 500 && error500Handler != null) ) {
                    String msg = "Duplicate handle-errors at " + handler.getLocation();
                    getLogger().error(msg);
                    throw new ConfigurationException(msg);
                }
                
                if (type == 404) {
                    error404Handler = handler;
                } else if (type == 500) {
                    error500Handler = handler;
                } else {
                    String msg = "Unkown handle-errors type (" + type + ") at " + handler.getLocation();
                    getLogger().error(msg);
                    throw new ConfigurationException(msg);
                }

            } else {
                // Regular builder
                children.add(builder.buildNode(childConfig));
            }
        }
         
        node.setChildren(toNodeArray(children));
        node.set404Handler(error404Handler);
        node.set500Handler(error500Handler);
        
        return node;
    }

}
