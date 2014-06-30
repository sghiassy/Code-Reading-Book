/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.selection.Selector;

import org.apache.cocoon.treeprocessor.AbstractParentProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.LinkedProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class SelectNodeBuilder extends AbstractParentProcessingNodeBuilder implements ThreadSafe {

    private ComponentManager manager;
    private SelectNode node;

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        String type = this.treeBuilder.getTypeForStatement(config, Selector.ROLE + "Selector");
        
        this.node = new SelectNode(type);
        this.treeBuilder.setupNode(this.node, config);
        // Selector is set in linkNode() since it isn't visible now
        
        // Lists of ProcessingNode[] and test resolvers for each "when"
        List whenChildren = new ArrayList();
        List whenTests = new ArrayList();
        
        // Nodes for otherwhise (if any)
        ProcessingNode[] otherwhiseNodes = null;
        
        Configuration[] childrenConfig = config.getChildren();
        for (int i = 0; i < childrenConfig.length; i++) {
            
            Configuration childConfig = childrenConfig[i];
            String name = childConfig.getName();
            
            if ("when".equals(name)) {
                
                checkNamespace(childConfig);
                whenTests.add(MapStackResolver.getResolver(childConfig.getAttribute("test")));
                whenChildren.add(buildChildNodes(childConfig));
                
            } else if ("otherwhise".equals(name)) {
                
                checkNamespace(childConfig);
                if (otherwhiseNodes != null) {
                    String msg = "Duplicate " + name + " (only one is allowed) at " + childConfig.getLocation();
                    getLogger().error(msg);
                    throw new ConfigurationException(msg);
                }
                
                otherwhiseNodes = buildChildNodes(childConfig);
                
            } else if (isParameter(childConfig)) {
                // ignore it. It is handled automatically in setupNode()
                
            } else {
                // Unknown element
                String msg = "Unknown element '" + name + "' in select at " + childConfig.getLocation();
                getLogger().error(msg);
                throw new ConfigurationException(msg);
            }
        }
        
        this.node.setCases(
            (ProcessingNode[][])whenChildren.toArray(new ProcessingNode[0][0]),
            (MapStackResolver[])whenTests.toArray(new MapStackResolver[whenTests.size()]),
            otherwhiseNodes
        );
        
        return this.node;
    }
}
