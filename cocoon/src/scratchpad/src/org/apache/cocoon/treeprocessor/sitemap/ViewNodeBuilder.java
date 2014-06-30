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

import org.apache.cocoon.treeprocessor.NamedContainerNodeBuilder;
import org.apache.cocoon.treeprocessor.NamedProcessingNode;
import org.apache.cocoon.treeprocessor.ProcessingNode;
import org.apache.cocoon.treeprocessor.TreeBuilder;

import org.apache.cocoon.util.StringUtils;

import java.util.*;

/**
 * Builds a &lt;map:view&gt;
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/15 11:10:54 $
 */

public class ViewNodeBuilder extends NamedContainerNodeBuilder implements ThreadSafe {
    
    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        // Get the label or position (pseudo-label) of this view.
        String label = config.getAttribute("from-label", null);
        
        if (label == null) {
            String position = config.getAttribute("from-position");
            if ("first".equals(position)) {
                label = SitemapLanguage.FIRST_POS_LABEL;
            } else if ("last".equals(position)) {
                label = SitemapLanguage.LAST_POS_LABEL;
            } else {
                String msg = "Bad value for 'from-position' at " + config.getLocation();
                getLogger().error(msg);
                throw new ConfigurationException(msg);
            }
        }
        
        SitemapLanguage sitemapBuilder = (SitemapLanguage)this.treeBuilder;

        // Indicate to child builders that we're in a view (they won't perform view branching)        
        sitemapBuilder.setBuildingView(true);
        
        // Build children
        NamedProcessingNode result = (NamedProcessingNode)super.buildNode(config);
        
        sitemapBuilder.addViewForLabel(label, result.getName());

        // Clear the flag
        sitemapBuilder.setBuildingView(false);
        
        return result;
    }
}
