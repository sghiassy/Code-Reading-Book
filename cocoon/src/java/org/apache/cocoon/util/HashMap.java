/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.util;

import java.util.Map;

/**
 * Extended Version of {@link java.util.HashMap}.
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:21 $
 */
public class HashMap extends java.util.HashMap {

    public HashMap () {
	super();
    }
    
    public HashMap ( int initialCapacity ) {
	super(initialCapacity);
    }

    public HashMap ( int initialCapacity, float loadFactor ) {
	super(initialCapacity, loadFactor);
    }

    public HashMap ( Map t) {
	super(t);
    }


    /**
     * Get method extended by default object to be returned when key
     * is not found. 
     *
     * @param key key to look up
     * @param _default default value to return if key is not found
     * @return value that is associated with key
     */
    public Object get ( Object key, Object _default ) {
	if (this.containsKey(key)) {
	    return this.get(key);
	} else {
	    return _default;
	}
    }

}
