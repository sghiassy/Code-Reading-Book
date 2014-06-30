/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

import java.util.Date;
import java.lang.Math;

/**
 * A validation object that remains valid for a specified amount of time (minutes).
 *
 * @author Michael Homeijer
 */

public final class DeltaTimeCacheValidity implements CacheValidity {

    private long cachedDateTime;  // Holds the store-time in miliseconds
    private long timeInCache;     // maximum time allowed in cache in minutes

    public DeltaTimeCacheValidity(long timeInCache) {
        this.cachedDateTime = System.currentTimeMillis();
        this.timeInCache = timeInCache * 60000;
    }

    public boolean isValid(CacheValidity validity) {
        if (validity instanceof DeltaTimeCacheValidity) {
            return Math.abs((((DeltaTimeCacheValidity)validity).getCachedDateTime() - this.cachedDateTime)) < this.timeInCache;
        }
        return false;
    }

    public long getCachedDateTime() {
        return this.cachedDateTime;
    }

    public String toString() {
        return "DeltaTimeCacheValidity: " + this.cachedDateTime + "(+" + this.timeInCache + "min)";
    }

}
