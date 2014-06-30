/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import java.util.Map;

/**
 * A <code>ProcessingNode</code> that has parameters.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:36 $
 */

public interface ParameterizableProcessingNode extends ProcessingNode {

    /**
     * Set the parameters of this node as a <code>Map</code> of <code>MapStackResolver</code>s
     * that will be resolved at process-time.
     */
    void setParameters(Map parameterMap);    
}
