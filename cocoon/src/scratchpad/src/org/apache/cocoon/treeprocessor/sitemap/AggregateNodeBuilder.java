/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

import org.apache.cocoon.treeprocessor.AbstractProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import org.apache.cocoon.treeprocessor.LinkedProcessingNodeBuilder;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class AggregateNodeBuilder extends AbstractProcessingNodeBuilder
  implements LinkedProcessingNodeBuilder {
    
    /** The collection of views for each part */
    private List partViews;
    
    /** The views for the whole aggregate element */
    private Collection views;
    
    private AggregateNode node;

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        // Get root node data
        this.node = new AggregateNode(
            config.getAttribute("element"),
            config.getAttribute("ns", ""),
            config.getAttribute("prefix", "")
        );
        this.treeBuilder.setupNode(this.node, config);
        
        this.views = ((SitemapLanguage)this.treeBuilder).getViewsForStatement("", "", config);
        
        // Build parts
        List parts = new ArrayList();
        
        this.partViews = new ArrayList();
        
        Configuration[] childConfigs = config.getChildren();
        for (int i = 0; i < childConfigs.length; i++) {
            Configuration childConfig = childConfigs[i];
            
            if (!"part".equals(childConfig.getName())) {
                String msg = "Unknown element '" + childConfig.getName() + " in aggregate ' at " +
                    childConfig.getLocation();
                getLogger().error(msg);
                throw new ConfigurationException(msg);
            }
            
            checkNamespace(childConfig);
            
            parts.add(new AggregateNode.Part(
                childConfig.getAttribute("src"),
                childConfig.getAttribute("element", ""),
                childConfig.getAttribute("ns", ""),
                childConfig.getAttribute("prefix", ""),
                childConfig.getAttributeAsBoolean("strip-root", false)
            ));
            
            this.partViews.add(((SitemapLanguage)this.treeBuilder).getViewsForStatement("", "", childConfig));
        }
        
        if (parts.size() == 0) {
            String msg = "There must be at least one part in map:aggregate at " + config.getLocation();
            getLogger().error(msg);
            throw new ConfigurationException(msg);
        }
        
        AggregateNode.Part[] partArray = (AggregateNode.Part[])parts.toArray(
            new AggregateNode.Part[parts.size()]);
        
        node.setParts(partArray);
        
        return node;
        
    }

    public void linkNode() throws Exception {
        
        SitemapLanguage sitemap = (SitemapLanguage)this.treeBuilder;
        
        // Replace all contents of partViews by the map of views.
        Map[] partViewNodes = new Map[this.partViews.size()];
        
        for (int i = 0; i < partViewNodes.length; i++) {
            partViewNodes[i] = sitemap.getViewNodes((Collection)this.partViews.get(i));
        }
        
        this.node.setViews(sitemap.getViewNodes(this.views), partViewNodes);
    }
}
