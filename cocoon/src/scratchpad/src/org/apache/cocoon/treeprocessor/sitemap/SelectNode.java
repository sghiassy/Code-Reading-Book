/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.environment.Environment;

import org.apache.cocoon.selection.Selector;

import org.apache.cocoon.sitemap.PatternException;

import org.apache.cocoon.treeprocessor.InvokeContext;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ParameterizableProcessingNode;
import org.apache.cocoon.treeprocessor.ProcessingNode;
import org.apache.cocoon.treeprocessor.SimpleSelectorProcessingNode;

import java.util.List;
import java.util.Map;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class SelectNode extends SimpleSelectorProcessingNode
  implements ParameterizableProcessingNode, Composable, Disposable {
    
    /** The parameters of this node */
    private Map parameters;

    /** Selector where to get Actions from */
    protected ComponentSelector componentSelector;
    
    /** Pre-selected selector, if it's ThreadSafe */
    protected Selector threadSafeSelector;
    
    private ProcessingNode[][] whenNodes;
    
    private MapStackResolver[] whenTests;
    
    private ProcessingNode[] otherwhiseNodes;

    public SelectNode(String name) throws PatternException {
        super(name);
    }
    
    public void setParameters(Map parameterMap) {
        this.parameters = parameterMap;
    }

    public void setCases(ProcessingNode[][] whenNodes, MapStackResolver[] whenTests, ProcessingNode[] otherwhiseNodes) {
        this.whenNodes = whenNodes;
        this.whenTests = whenTests;
        this.otherwhiseNodes = otherwhiseNodes;
    }

    public void compose(ComponentManager manager) throws ComponentException {
        
        setSelector((ComponentSelector)manager.lookup(Selector.ROLE));
        
        // Get the selector, if it's ThreadSafe
        this.threadSafeSelector = (Selector)this.getThreadSafeComponent();
    }
    
    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        // Prepare data needed by the action
        Map objectModel = env.getObjectModel();
        List mapStack   = context.getMapStack();
        Parameters resolvedParams = MapStackResolver.buildParameters(this.parameters, mapStack);
        
        // If selector is ThreadSafe, avoid select() and try/catch block (faster !)
        if (this.threadSafeSelector != null) {
            
            for (int i = 0; i < this.whenTests.length; i++) {
                if (this.threadSafeSelector.select(
                        whenTests[i].resolve(mapStack),
                        objectModel,
                        resolvedParams)) {
                    return invokeNodes(this.whenNodes[i], env, context);
                }
            }
            
            if (this.otherwhiseNodes != null) {
                return invokeNodes(this.otherwhiseNodes, env, context);
            }
            
            return false;
            
        } else {
            Selector selector = (Selector)this.componentSelector.select(this.componentName);
            try {

                for (int i = 0; i < this.whenTests.length; i++) {
                    if (selector.select(
                            whenTests[i].resolve(mapStack),
                            objectModel,
                            resolvedParams)) {
                        return invokeNodes(this.whenNodes[i], env, context);
                    }
                }
                
                if (this.otherwhiseNodes != null) {
                    return invokeNodes(this.otherwhiseNodes, env, context);
                }
                
                return false;
            } finally {
                this.componentSelector.release(selector);
            }
        }
    }
    
    public void dispose() {
        if (this.threadSafeSelector != null) {
            this.componentSelector.release(this.threadSafeSelector);
        }
    }
}
