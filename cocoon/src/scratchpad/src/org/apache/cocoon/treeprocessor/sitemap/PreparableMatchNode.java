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
import org.apache.cocoon.matching.PreparableMatcher;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.treeprocessor.ParameterizableProcessingNode;
import org.apache.cocoon.treeprocessor.InvokeContext;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class PreparableMatchNode extends SimpleSelectorProcessingNode implements ParameterizableProcessingNode, Composable {
    
    /** The 'pattern' attribute */
    private String pattern;
    
    private Object preparedPattern;
    
    private Map parameters;
    
    /** The matcher, if it's ThreadSafe */
    private PreparableMatcher threadSafeMatcher;
    
    public PreparableMatchNode(String name, String pattern) throws PatternException {
        super(name);
        this.pattern = pattern;
    }
    
    public void setParameters(Map parameterMap) {
        this.parameters = parameterMap;
    }


    public void compose(ComponentManager manager) throws ComponentException {

        setSelector((ComponentSelector)manager.lookup(Matcher.ROLE + "Selector"));
        
        // Prepare the pattern, and keep matcher if ThreadSafe
        PreparableMatcher matcher = (PreparableMatcher)selector.select(componentName);
        
        if (matcher instanceof ThreadSafe) {
            this.threadSafeMatcher = matcher;
        }
        
        try {
            this.preparedPattern = matcher.preparePattern(MapStackResolver.unescape(this.pattern));
            
        } catch(PatternException pe) {
            String msg = "Invalid pattern '" + this.pattern + "' for matcher at " + this.getLocation();
            getLogger().error(msg, pe);
            throw new ComponentException(msg, pe);

        } finally {
            if (this.threadSafeMatcher == null) {
                selector.release(matcher);
            }
        }
    }
    
    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        Map objectModel = env.getObjectModel();
        Parameters resolvedParams = MapStackResolver.buildParameters(this.parameters, context.getMapStack());
        
        Map result = null;
        
        if (this.threadSafeMatcher != null) {
            // Avoid select() and try/catch block (faster !)
            result = this.threadSafeMatcher.preparedMatch(preparedPattern, objectModel, resolvedParams);
                
        } else {
            // Get matcher from selector
            PreparableMatcher matcher = (PreparableMatcher)this.selector.select(this.componentName);
            try {
                result = matcher.preparedMatch(preparedPattern, objectModel, resolvedParams);
                    
            } finally {
                this.selector.release(matcher);
            }
        }
        
        if (result != null) {
            if (getLogger().isInfoEnabled()) {
                getLogger().info("Matcher '" + this.componentName + "' matched prepared pattern '" +
                    this.pattern + "' at " + this.getLocation());
            }
            
            // Invoke children with the matcher results
            return this.invokeNodes(children, env, context, result);
            
        } else {
            // Matcher failed
            return false;
        }
    }
}
