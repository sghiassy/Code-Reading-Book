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

import org.apache.cocoon.treeprocessor.AbstractParentProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.ProcessingNode;
import org.apache.cocoon.treeprocessor.ProcessingNodeBuilder;

import java.util.*;

/**
 * Builds all nodes below the top-level &lt;sitemap&gt; element, and returns the
 * &lt;pipelines&gt; node. There is no node for &gt;sitemap&lt; since no processing
 * occurs at this level.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class SitemapNodeBuilder extends AbstractParentProcessingNodeBuilder implements ThreadSafe {

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        ProcessingNode[] children = this.buildChildNodes(config);
        
        ProcessingNode pipelines = null;
        
        for (int i = 0; i < children.length; i++) {
            if (children[i] instanceof PipelinesNode) {
                if (pipelines != null) {
                    String msg = "Only one 'pipelines' is allowed, at " + config.getLocation();
                    getLogger().error(msg);
                    throw new ConfigurationException(msg);
                }
                pipelines = children[i];
            }
        }
        
        if (pipelines == null) {
            String msg = "Invalid sitemap : there must be a 'pipelines' at " + config.getLocation();
            getLogger().error(msg);
            throw new ConfigurationException(msg);
        }

        return pipelines;
    }
}
