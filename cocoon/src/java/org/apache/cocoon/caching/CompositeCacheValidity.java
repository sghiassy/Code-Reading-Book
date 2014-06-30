/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;



/**
 * A validation object using a Hashmap.
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:08 $
 */
public final class CompositeCacheValidity
implements CacheValidity {

    private CacheValidity v1;
    private CacheValidity v2;

    public CompositeCacheValidity(CacheValidity v1, CacheValidity v2) {
        this.v1 = v1;
        this.v2 = v2;
    }

    public boolean isValid(CacheValidity validity) {
        if (validity instanceof CompositeCacheValidity) {
            return (v1.isValid(((CompositeCacheValidity)validity).getValidity1()) &&
                    v2.isValid(((CompositeCacheValidity)validity).getValidity2()));
        }
        return false;
    }

    public CacheValidity getValidity1() {
        return this.v1;
    }

    public CacheValidity getValidity2() {
        return this.v2;
    }

    public String toString() {
        return new StringBuffer("CompositeCacheValidity: ").append(v1).append(":").append(v2).toString();
    }

}
