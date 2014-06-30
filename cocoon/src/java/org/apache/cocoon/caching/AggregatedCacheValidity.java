/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * A validation object using a List.
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:08 $
 */
public final class AggregatedCacheValidity
implements CacheValidity {

    private List a;

    public AggregatedCacheValidity() {
        this.a = new ArrayList();
    }

    public void add(CacheValidity validity) {
        this.a.add(validity);
    }

    public boolean isValid(CacheValidity validity) {
        if (validity instanceof AggregatedCacheValidity) {
            List b = ((AggregatedCacheValidity)validity).a;
            if(a.size() != b.size())
                return false;
            for(Iterator i = a.iterator(), j = b.iterator(); i.hasNext();) {
                if(!((CacheValidity)i.next()).isValid((CacheValidity)j.next()))
                    return false;
            }
            return true;
        }
        return false;
    }

    public String toString() {
        StringBuffer b = new StringBuffer("AggregatedCacheValidity ");
        for(Iterator i = a.iterator(); i.hasNext();) {
            b.append(i.next());
            if(i.hasNext()) b.append(':');
        }
        return b.toString();
    }
}

