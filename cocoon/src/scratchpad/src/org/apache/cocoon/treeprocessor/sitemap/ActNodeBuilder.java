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

import org.apache.cocoon.acting.Action;

import org.apache.cocoon.treeprocessor.AbstractParentProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.AbstractProcessingNode;
import org.apache.cocoon.treeprocessor.CategoryNodeBuilder;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.LinkedProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class ActNodeBuilder extends AbstractParentProcessingNodeBuilder
  implements LinkedProcessingNodeBuilder {

    private ComponentManager manager;
    
    private ActSetNode  actSetNode;
    private String      actSetName;

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        String source = config.getAttribute("src", null);
        
        // Is it an action-set call ?
        this.actSetName = config.getAttribute("set", null);
        if (actSetName == null) {
            
            String type = this.treeBuilder.getTypeForStatement(config, Action.ROLE + "Selector");
            
            ActTypeNode actTypeNode = new ActTypeNode(type, source);
            this.treeBuilder.setupNode(actTypeNode, config);
            
            actTypeNode.setChildren(buildChildNodes(config));
            
            return actTypeNode;
            
        } else {
            
            // Action set call
            this.actSetNode = new ActSetNode(source);
            this.treeBuilder.setupNode(this.actSetNode, config);
            
            this.actSetNode.setChildren(buildChildNodes(config));
            
            return this.actSetNode;
        }
    }

    public void linkNode() throws Exception {

        if (this.actSetNode != null) {
            // Link action-set call to the action set
            this.actSetNode.setActionSet(
                (ActionSetNode)CategoryNodeBuilder.getNamedNode(
                    this.treeBuilder, "action-sets", this.actSetName)
            );
        }
    }
}
