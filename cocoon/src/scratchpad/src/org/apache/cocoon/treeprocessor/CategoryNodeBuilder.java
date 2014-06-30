/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

import org.apache.avalon.framework.thread.ThreadSafe;

import java.util.*;

/**
 * Builds a generic container node.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public class CategoryNodeBuilder extends AbstractParentProcessingNodeBuilder
  implements Configurable, ThreadSafe {

    // Prefix used for registering as a TreeBuilder attribute
    private static String PREFIX = CategoryNodeBuilder.class.getName() + "/";
    
    protected String name;
    
    /**
     * The category name is the value of the "category-name" child, or if not
     * present, the name of the configuration element.
     */
    public void configure(Configuration config) throws ConfigurationException {
        super.configure(config);
        this.name = config.getChild("category-name").getValue(config.getAttribute("name"));
    }

    /** This builder has no parameters -- return <code>false</code> */
    protected boolean hasParameters() {
        return false;
    }

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        CategoryNode node = new CategoryNode();
        this.treeBuilder.setupNode(node, config);
        
        // Get all children and associate them to their name
        Map category = new HashMap();
        
        List children = buildChildNodesList(config);
        Iterator iter = children.iterator();
        while(iter.hasNext()) {
            NamedProcessingNode child = (NamedProcessingNode)iter.next();
            category.put(child.getName(), child);
        }
        
        node.setCategory(this.name, category);
        
        // Register node to allow lookup by other nodes
        this.treeBuilder.registerNode(PREFIX + this.name, node);
        
        return node;
    }
    
    public static CategoryNode getCategoryNode(TreeBuilder builder, String categoryName) {
        return (CategoryNode)builder.getRegisteredNode(PREFIX + categoryName);
    }

    public static ProcessingNode getNamedNode(TreeBuilder builder, String categoryName, String nodeName)
      throws Exception {
        CategoryNode category = getCategoryNode(builder, categoryName);
        
        return category.getNodeByName(nodeName);
    }
}
