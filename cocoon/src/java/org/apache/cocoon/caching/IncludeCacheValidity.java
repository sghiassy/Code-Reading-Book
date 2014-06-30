/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;


/**
 * A validation object used in CachingCIncludeTransformer
 *
 * @author <a href="mailto:maciejka@tiger.com.pl">Maciek Kaminski</a>
 */
public final class IncludeCacheValidity implements CacheValidity {

    private List sources;
    private List timeStamps;

    private boolean isNew;

    private SourceResolver resolver;

    public IncludeCacheValidity(SourceResolver resolver) {
        this.resolver = resolver;
        sources = new ArrayList();
        timeStamps = new ArrayList();
        isNew = true;
    }

    public boolean isNew() {
        return isNew;
    }

    public void setIsNew2False() {
        isNew = false;
        resolver = null;
    }

    public void add(String source, long timeStamp) {
        this.sources.add(source);
        this.timeStamps.add(new Long(timeStamp));
    }

    public boolean isValid(CacheValidity validity) {
        if (validity instanceof IncludeCacheValidity) {
            SourceResolver otherResolver = ((IncludeCacheValidity) validity).resolver;

            for(Iterator i = sources.iterator(), j = timeStamps.iterator(); i.hasNext();) {
                String src = ((String)i.next());
                long timeStamp = ((Long)j.next()).longValue();
                Source otherSource = null;
                try {
                    otherSource = otherResolver.resolve(src);
                    if(otherSource.getLastModified() != timeStamp ||
                        timeStamp == 0)
                        return false;
                } catch (Exception e) {
                    return false;
                } finally {
                    if (otherSource != null) otherSource.recycle();
                }
            }
            return true;
        }
        return false;
    }

    public String toString() {
        StringBuffer b = new StringBuffer("IncludeCacheValidity: " );
        for(Iterator i = sources.iterator(), j = timeStamps.iterator(); i.hasNext();) {
            b.append("[");
            b.append(i.next());
            b.append(" - ");
            b.append(j.next());
            b.append("]");
            if(i.hasNext()) b.append(", ");
        }
        return b.toString();
    }
}

