/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.acting.Action;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.SourceResolver;

import org.apache.cocoon.sitemap.PatternException;
import org.apache.cocoon.sitemap.SitemapRedirector;

import org.apache.cocoon.treeprocessor.InvokeContext;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ParameterizableProcessingNode;
import org.apache.cocoon.treeprocessor.SimpleSelectorProcessingNode;

import java.util.*;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.component.ComponentManager;

/**
 * Handles &lt;map:act type="..."&gt; (action-sets calls are handled by {@link ActSetNode}).
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class ActTypeNode extends SimpleSelectorProcessingNode
  implements ParameterizableProcessingNode, Disposable, Composable {
    
    /** The parameters of this node */
    private Map parameters;

    /** The 'src' attribute */
    protected MapStackResolver source;

    /** Pre-selected action, if it's ThreadSafe */
    protected Action threadSafeAction;

    public ActTypeNode(String name, String source) throws PatternException {
        super(name);
        this.source = MapStackResolver.getResolver(source);
    }
    
    public void setParameters(Map parameterMap) {
        this.parameters = parameterMap;
    }

    public void compose(ComponentManager manager) throws ComponentException {
        
        setSelector((ComponentSelector)manager.lookup(Action.ROLE + "Selector"));

        // Get the action, if it's thread safe
        this.threadSafeAction = (Action)this.getThreadSafeComponent();
    }
    
    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        // Prepare data needed by the action
        Map               objectModel    = env.getObjectModel();
        SitemapRedirector redirector     = PipelinesNode.getRedirector(env);
        SourceResolver    resolver       = getSourceResolver(objectModel);
        List              mapStack       = context.getMapStack();
        String            resolvedSource = source.resolve(mapStack);
        Parameters        resolvedParams = MapStackResolver.buildParameters(this.parameters, mapStack);
        
        Map actionResult;
        
        // If action is ThreadSafe, avoid select() and try/catch block (faster !)
        if (this.threadSafeAction != null) {
            actionResult = this.threadSafeAction.act(
                redirector, resolver, objectModel, resolvedSource, resolvedParams );
            
        } else {
            Action action = (Action)this.selector.select(this.componentName);
            try {
                actionResult = action.act(
                redirector, resolver, objectModel, resolvedSource, resolvedParams );
                
            } finally {
                this.selector.release(action);
            }
        }
        
        if (redirector.hasRedirected()) {
            return true;
        }
        
        if (actionResult == null) {
            // Action failed
            return false;
            
        } else {
            // Action succeeded : process children if there are some, with the action result
            if (this.children != null) {
                return this.invokeNodes(this.children, env, context, actionResult);

            } else {
                // Return false to continue sitemap invocation
                return false;
            }
        }
    }
    
    public void dispose() {
        if (this.threadSafeAction != null) {
            this.selector.release(this.threadSafeAction);
        }
    }
}
