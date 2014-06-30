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

import org.apache.cocoon.acting.Action;

import org.apache.cocoon.treeprocessor.AbstractProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class ActionSetNodeBuilder extends AbstractProcessingNodeBuilder implements ThreadSafe {

    private ComponentManager manager;

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        String actionSetName = config.getAttribute("name");
        
        // Lists of action types and names for each map:act
        List actionTypes = new ArrayList();
        List actionNames = new ArrayList();
                
        Configuration[] childrenConfig = config.getChildren();
        for (int i = 0; i < childrenConfig.length; i++) {
            
            Configuration childConfig = childrenConfig[i];
            String name = childConfig.getName();
            
            if ("act".equals(name)) {

                checkNamespace(childConfig);
                String type = this.treeBuilder.getTypeForStatement(childConfig, Action.ROLE + "Selector");
                
                actionTypes.add(type);
                actionNames.add(childConfig.getAttribute("action", null));
                
            } else {
                // Unknown element
                String msg = "Unknown element " + name + " in action-set at " + childConfig.getLocation();
                getLogger().error(msg);
                throw new ConfigurationException(msg);
            }
        }
        
        String[] types   = (String[])actionTypes.toArray(new String[actionTypes.size()]);
        String[] actions = (String[])actionNames.toArray(new String[actionNames.size()]);
        
        ActionSetNode node = new ActionSetNode(actionSetName, types, actions);
        this.treeBuilder.setupNode(node, config);

        return node;
    }
}
