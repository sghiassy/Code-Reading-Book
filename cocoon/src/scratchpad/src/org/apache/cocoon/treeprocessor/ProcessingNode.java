/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.environment.Environment;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public interface ProcessingNode extends ThreadSafe {
    
    /**
     * The key of the <code>SourceResolver</code> in the object model.
     */
    String OBJECT_SOURCE_RESOLVER = "source-resolver";
    
    /**
     * Process environment.
     */
    boolean invoke(Environment env, InvokeContext context) throws Exception;
    
    /**
     * Get the location of this node.
     */
    String getLocation();
}
