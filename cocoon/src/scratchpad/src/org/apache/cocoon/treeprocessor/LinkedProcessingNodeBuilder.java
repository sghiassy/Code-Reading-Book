/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

/**
 * A <code>ProcessingNode</code> builder that links its node to
 * other nodes in the hierarchy. This allows to turn the node tree
 * into a directed graph.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public interface LinkedProcessingNodeBuilder extends ProcessingNodeBuilder {

    /**
     * Resolve the links needed by the node built by this builder.
     */
    void linkNode() throws Exception;
}
