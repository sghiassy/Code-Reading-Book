/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.configuration.Configuration;

import org.apache.cocoon.generation.Generator;

import org.apache.cocoon.treeprocessor.AbstractProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.LinkedProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class GenerateNodeBuilder extends AbstractProcessingNodeBuilder
  implements LinkedProcessingNodeBuilder {

    private GenerateNode node;
    
    private Collection views;
    
    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        String type = this.treeBuilder.getTypeForStatement(config, Generator.ROLE + "Selector");
        
        this.views = ((SitemapLanguage)this.treeBuilder).getViewsForStatement(Generator.ROLE, type, config);
        
        this.node = new GenerateNode(type, config.getAttribute("src", null));        
        return this.treeBuilder.setupNode(this.node, config);
    }
    
    public void linkNode() throws Exception {
        this.node.setViews(
            ((SitemapLanguage)this.treeBuilder).getViewNodes(this.views)
        );
    }
}
