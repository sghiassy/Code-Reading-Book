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
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.serialization.Serializer;

import org.apache.cocoon.treeprocessor.AbstractProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.LinkedProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class SerializeNodeBuilder extends AbstractProcessingNodeBuilder
  implements LinkedProcessingNodeBuilder {

    private SerializeNode node;
    
    private Collection views;
    
    /** This builder has no parameters -- return <code>false</code> */
    protected boolean hasParameters() {
        return false;
    }

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        String type = this.treeBuilder.getTypeForStatement(config, Serializer.ROLE + "Selector");
        
        this.views = ((SitemapLanguage)this.treeBuilder).getViewsForStatement(Serializer.ROLE, type, config);
        
        this.node = new SerializeNode(
            type,
            config.getAttribute("mime-type", null),
            config.getAttributeAsInteger("status-code", -1)
        );
        return this.treeBuilder.setupNode(node, config);
    }
    
    public void linkNode() throws Exception {
        this.node.setViews(
            ((SitemapLanguage)this.treeBuilder).getViewNodes(this.views)
        );
    }
}
