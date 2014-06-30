/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.framework.logger.AbstractLoggable;

import org.apache.cocoon.environment.SourceResolver;

import java.util.Map;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public abstract class AbstractProcessingNode extends AbstractLoggable implements ProcessingNode {

    protected String location = "unknown location";
    
    /**
     * Get the <code>SourceResolver</code> in an object model.
     */
    protected static final SourceResolver getSourceResolver(Map objectModel) {
        return (SourceResolver)objectModel.get(OBJECT_SOURCE_RESOLVER);
    }
    
    /**
     * Get the location of this node.
     */
    public String getLocation() {
        return this.location;
    }
    
    /**
     * Set the location of this node.
     */
    public void setLocation(String location) {
        this.location = location;
    }
}
