/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.treeprocessor.AbstractProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.ProcessingNode;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class MountNodeBuilder extends AbstractProcessingNodeBuilder implements ThreadSafe {

    /** This builder has no parameters -- return <code>false</code> */
    protected boolean hasParameters() {
        return false;
    }

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        MountNode node = new MountNode(
            config.getAttribute("uri-prefix"),
            config.getAttribute("src"),
            config.getAttribute("language", null),
            this.treeBuilder.getProcessor()
        );
        return (this.treeBuilder.setupNode(node, config));
    }
}
