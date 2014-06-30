/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.activity.Initializable;

import org.apache.cocoon.environment.Environment;

import org.apache.cocoon.sitemap.PatternException;

import org.apache.cocoon.treeprocessor.AbstractProcessingNode;
import org.apache.cocoon.treeprocessor.CategoryNode;
import org.apache.cocoon.treeprocessor.InvokeContext;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ParameterizableProcessingNode;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.List;
import java.util.Map;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class CallNode extends AbstractProcessingNode
    implements Initializable, ParameterizableProcessingNode {
    
    /** The parameters of this node */
    private Map parameters;

    /** The 'resource' attribute */
    private String resourceName;
    
    private MapStackResolver resourceResolver;
    
    /** The category node */
    private CategoryNode resources;
    
    private ProcessingNode resourceNode;
    
    public void setParameters(Map parameterMap) {
        this.parameters = parameterMap;
    }

    public void setResource(CategoryNode resources, String resourceName) throws Exception {
        this.resourceName = resourceName;
        this.resources = resources;
    }

    public void initialize() throws Exception {
        if (MapStackResolver.needsResolve(this.resourceName)) {
            // Will always be resolved at invoke time
            this.resourceResolver = MapStackResolver.getResolver(this.resourceName);
        } else {
            // Static name : get it now
            this.resourceNode = this.resources.getNodeByName(MapStackResolver.unescape(this.resourceName));
        }
    }

    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        // Resolve parameters
        if (this.parameters != null) {
            // In redirect-to a level is added to the map stack *only* if the
            // 'target' attribute is present.
            Map params = MapStackResolver.resolveMap(this.parameters, context.getMapStack());
            context.pushMap(params);
        }
        
        boolean result;

        if (this.resourceNode != null) {
            // Static resource name
                result = this.resourceNode.invoke(env, context);
            
        } else {
            // Resolved resource name
            String name = this.resourceResolver.resolve(context.getMapStack());
            
            if (getLogger().isInfoEnabled()) {
                getLogger().info("Calling resource " + name);
            }
            
            result = this.resources.invokeByName(name, env, context);
        }
        
        if (this.parameters != null) {
            context.popMap();
        }
        return result;
    }
}
