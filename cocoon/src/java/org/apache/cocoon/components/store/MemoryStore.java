/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.store;

import org.apache.avalon.framework.thread.ThreadSafe;

import java.util.Enumeration;
import java.util.Hashtable;

/**
 *
 * @author <a href="mailto:scoobie@betaversion.org">Federico Barbieri</a>
 *         (Betaversion Productions)
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 *         (Apache Software Foundation)
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:14 $
 */
public class MemoryStore implements Store, ThreadSafe {
    /* WARNING: Hashtable is threadsafe, whereas HashMap is not.
     * Should we move this class over to the Collections API,
     * use Collections.synchronizedMap(Map map) to ensure
     * accesses are synchronized.
     */

    /** The shared store */
    private static Hashtable table=null;

    public MemoryStore() {
        if (table==null) table=new Hashtable();
    }

    /**
     * Get the object associated to the given unique key.
     */
    public Object get(Object key) {
        return(table.get(key));
    }

    /**
     * Store the given object in a persistent state. It is up to the
     * caller to ensure that the key has a persistent state across
     * different JVM executions.
     */
    public void store(Object key, Object value) {
        this.hold(key,value);
    }

    /**
     * Holds the given object in a volatile state. This means
     * the object store will discard held objects if the
     * virtual machine is restarted or some error happens.
     */
    public void hold(Object key, Object value) {
        table.put(key,value);
    }

    /**
     * Remove the object associated to the given key.
     */
    public void remove(Object key) {
        table.remove(key);
    }

    public void free() {}

    /**
     * Indicates if the given key is associated to a contained object.
     */
    public boolean containsKey(Object key) {
        return(table.containsKey(key));
    }

    /**
     * Returns the list of used keys as an Enumeration of Objects.
     */
    public Enumeration keys() {
        return(table.keys());
    }
}
