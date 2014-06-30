/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.component.ComponentManager;
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
import org.apache.cocoon.treeprocessor.NamedProcessingNode;
import org.apache.cocoon.treeprocessor.SimpleSelectorProcessingNode;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class ActionSetNode extends SimpleSelectorProcessingNode
  implements Disposable, NamedProcessingNode, Composable {
    
    /** The action types */
    private String[] types;
    
    /** The 'action' attribute for each action */
    private String[] actionNames;
    
    /** The actions that are ThreadSafe, to avoid lookups */
    private Action[] threadSafeActions;
    
    public ActionSetNode(String name, String[] types, String[] actionNames) {
        super(name);
        this.types = types;
        this.actionNames = actionNames;
    }

    public void compose(ComponentManager manager) throws ComponentException {
        
        setSelector((ComponentSelector)manager.lookup(Action.ROLE + "Selector"));

        // Get all actions that are thread safe
        this.threadSafeActions = new Action[types.length];
        
        for (int i = 0; i < this.types.length; i++) {
            this.threadSafeActions[i] = (Action)this.getThreadSafeComponent(this.types[i]);
        }
    }
    
    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        String msg = "An action-set cannot be invoked, at " + this.getLocation();
        getLogger().error(msg);
        throw new UnsupportedOperationException(msg);
    }
    
    /**
     * Call the actions composing the action-set and return the combined result of
     * these actions.
     */
    public final Map call(Environment env, InvokeContext context, String source, Parameters params) throws Exception {
        
        // Prepare data needed by the actions
        Map               objectModel    = env.getObjectModel();
        SitemapRedirector redirector     = PipelinesNode.getRedirector(env);
        SourceResolver    resolver       = getSourceResolver(objectModel);
        
        String cocoonAction = env.getAction();
        
        Map result = null;
        
        // Call each action that either has no cocoonAction, or whose cocoonAction equals
        // the one from the environment.
        
        for (int i = 0; i < types.length; i++) {
            
            Map actionResult;
            Action action;
            
            String actionName = actionNames[i];
            if (actionName == null || actionName.equals(cocoonAction)) {
                
                // If action is ThreadSafe, avoid select() and try/catch block (faster !)
                if ((action = this.threadSafeActions[i]) != null) {
                    
                    actionResult = action.act(
                        redirector, resolver, objectModel, source, params);

                } else {
                    
                    action = (Action)this.selector.select(this.componentName);
                    try {
                        actionResult = action.act(
                            redirector, resolver, objectModel, source, params);
                    } finally {
                        this.selector.release(action);
                    }
                }
                
                if (actionResult != null) {
                    // Merge the result in the global result, creating it if necessary.
                    if (result == null) {
                        result = new HashMap(actionResult);
                    } else {
                        result.putAll(actionResult);
                    }
                }
            } // if (actionName...
        } // for (int i...
        
        return result;
    }
    
    public void dispose() {
        // Dispose all ThreadSafe actions
        for (int i = 0; i < this.threadSafeActions.length; i++) {
            this.selector.release(this.threadSafeActions[i]);
        }
    }

    /**
     * Implementation of <code>NamedProcessingNode</code>.
     */

    public String getName() {
        return this.componentName;
    }
}
