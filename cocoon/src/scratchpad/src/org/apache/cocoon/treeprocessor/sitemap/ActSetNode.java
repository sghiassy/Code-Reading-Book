/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.acting.Action;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;

import org.apache.cocoon.sitemap.PatternException;
import org.apache.cocoon.sitemap.SitemapRedirector;

import org.apache.cocoon.treeprocessor.InvokeContext;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ParameterizableProcessingNode;
import org.apache.cocoon.treeprocessor.ProcessingNode;
import org.apache.cocoon.treeprocessor.SimpleParentProcessingNode;

import java.util.List;
import java.util.Map;

/**
 * Handles &lt;map:act type="..."&gt; (action-sets calls are handled by {@link ActSetNode}).
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class ActSetNode extends SimpleParentProcessingNode
  implements ParameterizableProcessingNode {
    
    /** The parameters of this node */
    private Map parameters;

    /** The 'src' attribute */
    protected MapStackResolver source;
    
    /** The action set to call */
    private ActionSetNode actionSet;

    public ActSetNode(String source) throws PatternException {
        this.source = MapStackResolver.getResolver(source);
    }
    
    public void setParameters(Map parameterMap) {
        this.parameters = parameterMap;
    }
    
    public void setActionSet(ActionSetNode actionSet) {
        this.actionSet = actionSet;
    }

    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        List       mapStack = context.getMapStack();
        String     resolvedSource = source.resolve(mapStack);
        Parameters resolvedParams = MapStackResolver.buildParameters(this.parameters, mapStack);

        Map result = this.actionSet.call(env, context, resolvedSource, resolvedParams);
        
        if (PipelinesNode.getRedirector(env).hasRedirected()) {
            return true;
            
        } else if (result == null) {
            return false;
            
        } else if (this.children == null) {
            return true;
            
        } else {
            return this.invokeNodes(this.children, env, context, result);
        }
    }
}
