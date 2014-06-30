/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.cocoon.treeprocessor.AbstractParentProcessingNode;
import org.apache.cocoon.treeprocessor.ProcessingNode;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public abstract class SimpleParentProcessingNode extends AbstractParentProcessingNode {

    /** The childrens of this matcher */
    protected ProcessingNode[] children;

    public void setChildren(ProcessingNode[] children) {
        this.children = children;
    }
}
