/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.jispstore;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.ParameterException;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;

//only for scratchpad
import org.apache.cocoon.components.store.Store;
import org.apache.cocoon.components.store.StoreJanitor;

import org.apache.cocoon.util.ClassUtils;

import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.LinkedList;

/**
 *  This class provides a cache algorithm for the requested documents. It
 *  combines a HashMap and a LinkedList to create a so called MRU (Most Recently
 *  Used) cache.
 *
 * @author     <a href="mailto:g-froehlich@gmx.de">Gerhard Froehlich</a>
 * @author     <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 */
public final class MRUMemoryStore
extends AbstractLoggable
implements Store,
           Parameterizable,
           ThreadSafe,
           Composable,
           Disposable {

    private int mMaxobjects;
    private Hashtable mCache;
    private LinkedList mMRUList;
    private Store mFsstore;
    private StoreJanitor mStorejanitor;
    private ComponentManager mComponetManager;

    /**
     *  Get the object associated to the given unique key.
     *
     * @param key the Key Object
     * @return the Object associated with Key Object
     */
    public Object get(Object key) {
        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug("Getting object from memory. Key: " + key);
        }
        Object tmpobject = this.mCache.get(key);
        if (tmpobject != null) {
            this.mMRUList.remove(key);
            this.mMRUList.addFirst(key);
            return tmpobject;
        }

        this.getLogger().debug("Object not found in memory");
        
        tmpobject = this.mFsstore.get(key);
        if (tmpobject == null) {
            if (getLogger().isDebugEnabled()) {
                this.getLogger().debug("Object was NOT found on fs. "
                                       + "Looked for: " + key);
            }
            return null;
        } else {
            if (getLogger().isDebugEnabled()) {
                this.getLogger().debug("Object was found on fs");
            }
            if (!this.mCache.containsKey(key)) {
                this.hold(key, tmpobject);
            }
            return tmpobject;
        }
    }

    /**
     *  Get components of the ComponentManager
     *
     * @param manager the Component Manager
     * @exception  ComponentException
     */
    public void compose(ComponentManager manager)
        throws ComponentException {
        this.mComponetManager = manager;
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Looking up " 
                               + Store.ROLE + "/JispFilesystemStore");
        }

        this.mFsstore = (Store) manager.lookup(Store.ROLE 
                                               + "/JispFilesystemStore");
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Looking up " + StoreJanitor.ROLE);
        }
        this.mStorejanitor = (StoreJanitor) manager.lookup(StoreJanitor.ROLE);
    }

    /**
     *  Configure the MRUMemoryStore. This options can be used :
     *  <UL>
     *    <LI> maxobjects = how many objects will be stored in memory (Default:
     *    10 objects)</LI>
     *    (Default: false)</LI>
     *  </UL>
     *
     * @param params the configuration parameters
     * @exception  ParameterException
     */

    public void parameterize(Parameters params)
        throws ParameterException {
        this.mMaxobjects = params.getParameterAsInteger("maxobjects", 100);
        if ((this.mMaxobjects < 1)) {
            throw new ParameterException("MRUMemoryStore maxobjects must be "
                                         + "at least 1 milli second!");
        }
        this.mCache = new Hashtable((int) (this.mMaxobjects * 1.2));
        this.mMRUList = new LinkedList();
        this.mStorejanitor.register(this);
    }

    /**
     *  Dispose the component
     */
    public void dispose() {
        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug("dispose()");
        }

        if (this.mComponetManager != null) {
            this.mComponetManager.release(this.mStorejanitor);
            this.mStorejanitor = null;
            this.mComponetManager.release(this.mFsstore);
            this.mFsstore = null;
        }
    }

    /**
     *  Store the given object in a persistent state. It is up to the caller to
     *  ensure that the key has a persistent state across different JVM
     *  executions.
     *
     * @param key the key object
     * @param value the value object
     */
    public void store(Object key, Object value) {
        if (this.checkSerializable(value)) {
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("Holding object on FS. key: " + key);
                getLogger().debug("Holding object on FS. value: " + value);
            }
            try {
                this.mFsstore.store(key,value);
            } catch (IOException ioe) {
                this.getLogger().error("Error storing Object on Filesystem",ioe);
            }
        }
    }

    /**
     *  This method holds the requested object in a HashMap combined with a
     *  LinkedList to create the MRU. It also stores objects onto the filesystem
     *  if configured.
     *
     * @param key the key object
     * @param value the value object
     */
    public void hold(Object key, Object value) {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Holding object in memory. key: " + key);
            getLogger().debug("Holding object in memory. value: " + value);
        }

        while (this.mMRUList.size() >= this.mMaxobjects) {
            this.free();
        }
      
        this.mCache.put(key, value);
        this.mMRUList.remove(key);
        this.mMRUList.addFirst(key);
        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug("Cache size=" + mCache.size());
        }
    }

    /**
     * Remove the object associated to the given key.
     *
     * @param key the key object
     */
    public void remove(Object key) {
        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug("Removing object from store");
        }
        this.mCache.remove(key);
        this.mMRUList.remove(key);
        if (key != null) {
            this.mFsstore.remove(key);
        }
    }

    /**
     *  Indicates if the given key is associated to a contained object.
     *
     * @param key the key object
     * @return true if key exists and false if not
     */
    public boolean containsKey(Object key) {
        return (this.mCache.containsKey(key));
    }

    /**
     *  Returns the list of used keys as an Enumeration.
     *
     * @return the enumeration of the cache
     */
    public Enumeration keys() {
        return this.mCache.keys();
    }

    /**
     *  Frees some of the fast memory used by this store. It removes the last
     *  element in the store.
     */
    public void free() {
        try {
            if (this.mCache.size() > 0) {
                this.getLogger().debug("Freeing cache");
                
                if(checkSerializable(this.mCache.get(this.mMRUList.getLast()))) {
                    this.mFsstore.store(this.mMRUList.getLast(),
                                        this.mCache.get(this.mMRUList.getLast()));     
                }

                this.mCache.remove(this.mMRUList.getLast());
                this.mMRUList.removeLast();

                if (getLogger().isDebugEnabled()) {
                    this.getLogger().debug("Cache size=" + mCache.size());
                }
            }
        } catch (Exception e) {
            this.getLogger().error("Error in free()", e);
        }
    }

    /**
     *  This method checks if an object is seriazable. FIXME: In the moment only
     *  CachedEventObject or CachedStreamObject are stored.
     *
     * @param object the object to be tested
     * @return true if the object is storeable
     */
    private boolean checkSerializable(Object object) {
        try {
            if (getLogger().isDebugEnabled()) {
                this.getLogger().debug("Object=" + object);
            }
            if ((object.getClass().getName().equals(
                                "org.apache.cocoon.caching.CachedEventObject"))
                     || (object.getClass().getName().equals(
                                "org.apache.cocoon.caching.CachedStreamObject"))
                     || (ClassUtils.implementsInterface(object.getClass().getName(),
                                "org.apache.cocoon.caching.CacheValidity"))) {
                return true;
            } else {
                return false;
            }
        } catch (Exception e) {
            this.getLogger().error("Error in checkSerializable()!", e);
            return false;
        }
    }
}
