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

/**
 * Builds a generic named container node.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public class NamedContainerNodeBuilder extends ContainerNodeBuilder implements Configurable {

    private String nameAttr;

    public void configure(Configuration config) throws ConfigurationException {
        super.configure(config);
        this.nameAttr = config.getChild("name-attribute").getValue("name");
    }

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        NamedContainerNode node = new NamedContainerNode(config.getAttribute(this.nameAttr));
        this.setupNode(node, config);
        return node;
    }
}
