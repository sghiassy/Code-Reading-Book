/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

import java.util.Map;

/**
 * This is an cached object as it is stored in the <code>StreamCache</code>
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/22 00:17:11 $
 */
public final class CachedStreamObject implements java.io.Serializable {

    private Map validityObjects;
    private byte[] response;

    /**
     * Create a new entry for the cache.
     *
     * @param validityObjects The CacheValidity objects hashed by their
     *        <code>ComponentCacheKey</code>
     * @param response     The cached response
     */
    public CachedStreamObject(Map validityObjects,
                              byte[] response) {
        this.validityObjects = validityObjects;
        this.response = response;
    }

    /**
     * Checks if the CacheValidity object is still valid.
     */
    public boolean isValid(ComponentCacheKey componentKey,
                           CacheValidity     componentValidity) {
        CacheValidity ownValidity = (CacheValidity)this.validityObjects.get(componentKey);
        if (ownValidity != null && ownValidity.isValid(componentValidity)) {
            return true;
        }
        return false;
    }

    /**
     * Get the validity object
     * @return The <CODE>CacheValidity</CODE> object or <CODE>null</CODE>.
     */
    public CacheValidity getCacheValidity(ComponentCacheKey componentKey) {
        return (CacheValidity)this.validityObjects.get(componentKey);
    }

    /**
     * Get the cached response.
     *
     * @return The response
     */
    public byte[] getResponse() {
        return this.response;
    }
}
