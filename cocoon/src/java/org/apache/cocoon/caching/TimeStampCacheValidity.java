/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

/**
 * A validation object for time-stamps.
 * This is might be the most used CacheValidity object.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:08 $
 */
public final class TimeStampCacheValidity
implements CacheValidity {

    private long timeStamp;

    public TimeStampCacheValidity(long timeStamp) {
        this.timeStamp = timeStamp;
    }

    public boolean isValid(CacheValidity validity) {
        if (validity instanceof TimeStampCacheValidity) {
            return this.timeStamp == ((TimeStampCacheValidity)validity).getTimeStamp();
        }
        return false;
    }

    public long getTimeStamp() {
        return this.timeStamp;
    }

    public String toString() {
        return "TimeStampCacheValidity: " + this.timeStamp;
    }

}
