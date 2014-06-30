/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.cocoon.environment.Environment;

/**
 * A no-op node to stub not yet implemented features.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public class NullNode extends AbstractProcessingNode {
    
    public final boolean invoke(Environment env, InvokeContext context) throws Exception {
        
        getLogger().debug("Invoke on NullNode at " + getLocation());
        return false;
        
    }  
}
