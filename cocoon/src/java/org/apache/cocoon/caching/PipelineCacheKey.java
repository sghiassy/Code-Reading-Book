/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

import java.util.ArrayList;
import java.util.List;

/**
 * This is the cache key for one pipeline. It consists of one
 * or more <code>ComponentCacheKey</code> objects.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:08 $
 */
public final class PipelineCacheKey {

    private List list;
    private String toStringValue = "PCK:";

    public void addKey(ComponentCacheKey key) {
        if (this.list == null) {
            this.list = new ArrayList();
        }
        this.list.add(key);
        toStringValue += key.toString();
    }

    public void addKey(PipelineCacheKey key) {
        if (this.list == null) {
            this.list = new ArrayList();
        }
        this.list.add(key);
        toStringValue += key.toString();
    }

    public String toString() {
        return toStringValue;
    }

    public boolean equals(Object object) {
        return this.toStringValue.equals(object.toString());
    }

    public int hashCode() {
        return this.toStringValue.hashCode();
    }

}
