/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.sitemap.PatternException;
import org.apache.cocoon.treeprocessor.SimpleSelectorProcessingNode;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.matching.Matcher;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.treeprocessor.InvokeContext;

import java.util.*;
import org.apache.cocoon.treeprocessor.ParameterizableProcessingNode;
import java.util.Map;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.component.ComponentManager;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class MatchNode extends SimpleSelectorProcessingNode implements ParameterizableProcessingNode, Composable {
    
    /** The 'pattern' attribute */
    private MapStackResolver pattern;
    
    /** The matcher, if it's ThreadSafe */
    private Matcher threadSafeMatcher;
    
    private Map parameters;
    
    public MatchNode(String name, String pattern) throws PatternException {
        super(name);
        this.pattern = MapStackResolver.getResolver(pattern);
    }
     
    public void setParameters(Map parameterMap) {
        this.parameters = parameterMap;
    }

    public void compose(ComponentManager manager) throws ComponentException {
        
        super.setSelector((ComponentSelector)manager.lookup(Matcher.ROLE + "Selector"));
        
        // Get matcher if it's ThreadSafe
        this.threadSafeMatcher = (Matcher)this.getThreadSafeComponent();
    }
    
    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        List mapStack = context.getMapStack();
        
        String resolvedPattern = pattern.resolve(mapStack);
        Map objectModel = env.getObjectModel();
        Parameters resolvedParams = MapStackResolver.buildParameters(this.parameters, mapStack);
        
        Map result = null;
        
        if (this.threadSafeMatcher != null) {
            // Avoid select() and try/catch block (faster !)
            result = this.threadSafeMatcher.match(resolvedPattern, objectModel, resolvedParams);
                
        } else {
            // Get matcher from selector
            Matcher matcher = (Matcher)this.selector.select(this.componentName);
            try {
                result = matcher.match(resolvedPattern, objectModel, resolvedParams);
                    
            } finally {
                this.selector.release(matcher);
            }
        }
        
        if (result != null) {
            if (getLogger().isInfoEnabled()) {
                getLogger().info("Matcher '" + this.componentName + "' matched pattern '" + this.pattern +
                    "' at " + this.getLocation());
            }
            
            // Invoke children with the matcher results
            return this.invokeNodes(children, env, context, result);
            
        } else {
            // Matcher failed
            return false;
        }
    }
}
