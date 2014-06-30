/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

import java.util.HashMap;

/**
 * A validation object using a Hashmap.
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:08 $
 */
public final class ParametersCacheValidity
implements CacheValidity {

    private HashMap map;

    public ParametersCacheValidity(HashMap map) {
        this.map = map;
    }

    public boolean isValid(CacheValidity validity) {
        if (validity instanceof ParametersCacheValidity) {
            return this.map.toString().equals(((ParametersCacheValidity)validity).getParameters().toString());
        }
        return false;
    }

    public HashMap getParameters() {
        return this.map;
    }

    public String toString() {
        return "ParametersCacheValidity: " + this.map;
    }

}
