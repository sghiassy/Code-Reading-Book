/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

/**
 * A validation object which is always valid.
 * This might be the most used CacheValidity object for serializers.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/08 10:47:45 $
 */
public final class NOPCacheValidity
implements CacheValidity {

    public static final CacheValidity CACHE_VALIDITY = new NOPCacheValidity();

    public boolean isValid(CacheValidity validity) {
        return validity instanceof NOPCacheValidity;
    }

    public String toString() {
        return "NOPCacheValidity";
    }

}
