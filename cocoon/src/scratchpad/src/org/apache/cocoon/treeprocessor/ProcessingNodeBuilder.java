/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

import java.util.Map;

/**
 * A <code>ProcessingNode</code> builder.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public interface ProcessingNodeBuilder extends Component {

    /**
     * Set the builder for which we are building.
     */
    void setBuilder(TreeBuilder builder);
    
    /**
     * Build the {@link ProcessingNode} and its children from the given
     * <code>Configuration</code>, and optionnaly register it in the {@link Builder}
     * for lookup by other <code>LinkedProcessingNodeBuilder</code>s.
     */
    ProcessingNode buildNode(Configuration config) throws Exception;
}
