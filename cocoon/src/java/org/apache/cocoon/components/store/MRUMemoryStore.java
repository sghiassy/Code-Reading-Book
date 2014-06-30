/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.store;

import org.apache.avalon.excalibur.collections.SynchronizedPriorityQueue;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.excalibur.collections.SynchronizedPriorityQueue;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.util.ClassUtils;
import org.apache.cocoon.util.IOUtils;
import org.apache.regexp.RE;
import org.apache.regexp.RESyntaxException;

import java.io.File;
import java.io.IOException;
import java.net.URLEncoder;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.LinkedList;

/**
 * This class provides a cache algorithm for the requested documents.
 * It combines a HashMap and a LinkedList to create a so called MRU
 * (Most Recently Used) cache.
 *
 * The idea was taken from the "Writing Advanced Application Tutorial" from
 * javasoft. Many thanx to the writers!
 *
 * @author <a href="mailto:g-froehlich@gmx.de">Gerhard Froehlich</a>
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 */
public final class MRUMemoryStore
extends AbstractLoggable
implements Store,
           Configurable,
           ThreadSafe,
           Composable,
           Disposable,
           Contextualizable {

    private int maxobjects;
    private boolean filesystem;
    private Hashtable cache;
    private LinkedList mrulist;
    private File cachefile;
    private Store fsstore;
    private StoreJanitor storejanitor;
    private File cachedir;
    private File workdir;
    private String cachedirstr;
    private ComponentManager manager;

    /**
     * Get components of the ComponentManager
     *
     * @param the ComponentManager
     */
    public void compose(ComponentManager manager) throws ComponentException {
        this.manager = manager;
        this.fsstore = (Store)manager.lookup(Store.ROLE + "/Filesystem");
        this.storejanitor = (StoreJanitor)manager.lookup(StoreJanitor.ROLE);
        if (this.getLogger().isDebugEnabled()) {
            getLogger().debug("Looking up " + Store.ROLE + "/Filesystem");
            getLogger().debug("Looking up " + StoreJanitor.ROLE);    
        }
    }

    /**
     * Get the context
     *
     * @param the Context of the application
     */
    public void contextualize(Context context) throws ContextException {
        this.cachedir = (File)context.get(Constants.CONTEXT_CACHE_DIR);
        this.workdir = (File)context.get(Constants.CONTEXT_WORK_DIR);
        this.cachedirstr = IOUtils.getContextFilePath(this.workdir.getPath(),
                                                          this.cachedir.getPath());
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("cachedir=" + this.cachedir);
            this.getLogger().debug("workdir=" + this.workdir);
            this.getLogger().debug("cachedirstr=" + this.cachedirstr);
            this.getLogger().debug("Context path="
                + IOUtils.getContextFilePath(this.workdir.getPath(),this.cachedir.getPath()));
        }
    }

    /**
     * Initialize the MRUMemoryStore.
     * A few options can be used :
     * <UL>
     *  <LI>maxobjects = how many objects will be stored in memory (Default: 10 objects)</LI>
     *  <LI>filesystem = use filesystem storage to keep object persistent (Default: false)</LI>
     * </UL>
     *
     * @param the Configuration of the application
     * @exception ConfigurationException
     */
    public void configure(Configuration conf) throws ConfigurationException {
        Parameters params = Parameters.fromConfiguration(conf);
        this.maxobjects            = params.getParameterAsInteger("maxobjects",100);
        this.filesystem            = params.getParameterAsBoolean("filesystem",false);
        if ((this.maxobjects < 1)) {
            throw new ConfigurationException("MRUMemoryStore maxobjects must be at least 1 milli second!");
        }

        this.cache = new Hashtable((int)(this.maxobjects * 1.2));
        this.mrulist     = new LinkedList();
        this.storejanitor.register(this);
    }

    /**
     * Dispose the component
     */
    public void dispose() {
        if (this.manager != null) {
            this.getLogger().debug("Disposing component!");          
            this.manager.release(this.storejanitor);
            this.storejanitor = null;
            this.manager.release(this.fsstore);
            
            //save all cache entries to filesystem
            this.getLogger().debug("Final cache size=" + this.cache.size());
            Enumeration enum = this.cache.keys();
            while(enum.hasMoreElements()) {
                Object tmp = enum.nextElement();
                try {
                    if(tmp != null 
                       && checkSerializable(this.cache.get(tmp))) {
                       this.fsstore.store(this.getFileName(tmp.toString()),
                                           this.cache.remove(tmp));
                    }
                } catch (IOException ioe) {
                    this.getLogger().error("Error in dispose():",ioe); 
                }
            }
            
            this.fsstore = null;
        }
    }

    /**
     * Store the given object in a persistent state. It is up to the
     * caller to ensure that the key has a persistent state across
     * different JVM executions.
     *
     * @param the key for the object to store
     * @param the object to store
     */
    public void store(Object key, Object value) {
        this.hold(key,value);
    }

    /**
     * This method holds the requested object in a HashMap combined
     * with a LinkedList to create the MRU.
     * It also stores objects onto the filesystem if configured.
     *
     * @param the key of the object to be stored
     * @param the object to be stored
     */
    public void hold(Object key, Object value) {
        if (this.getLogger().isDebugEnabled()) {
            getLogger().debug("Holding object in memory. key: " + key);
            getLogger().debug("Holding object in memory. value: " + value);
        }
        /** ...first test if the max. objects in cache is reached... */
        while (this.mrulist.size() >= this.maxobjects) {
            /** ...ok, heapsize is reached, remove the last element... */
            this.free();
        }
        /** ..put the new object in the cache, on the top of course ... */
        this.cache.put(key, value);
        this.mrulist.remove(key);
        this.mrulist.addFirst(key);
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Cache size=" + cache.size());
        }
    }

    /**
     * Get the object associated to the given unique key.
     *
     * @param the key of the requested object
     * @return the requested object
     */
    public Object get(Object key) {
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Getting object from memory. Key: " + key);
        }
        Object tmpobject = this.cache.get(key);
        if ( tmpobject != null ) {
            /** put the accessed key on top of the linked list */
            this.mrulist.remove(key);
            this.mrulist.addFirst(key);
            return tmpobject;
        }

        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Object not found in memory");
        }
        /** try to fetch from filesystem */
        if(this.filesystem) {
            tmpobject = this.fsstore.get(getFileName(key.toString()));
            if (tmpobject == null) {
                if (this.getLogger().isDebugEnabled()) {
                    this.getLogger().debug( "Object was NOT found on fs.  Looked for: "
                                    + getFileName(key.toString()));
                }
                return null;
            } else {
                if (this.getLogger().isDebugEnabled()) {
                    this.getLogger().debug("Object was found on fs");
                }
                try {
                    tmpobject = IOUtils.deserializeObject((File)tmpobject);
                    if(!this.cache.containsKey(key)) {
                        this.hold(key,tmpobject);
                    }
                    return tmpobject;
                } catch (ClassNotFoundException ce) {
                    this.getLogger().error("Error in get()!", ce);
                    return null;
                } catch (IOException ioe) {
                    this.getLogger().error("Error in get()!", ioe);
                    return null;
                }
            }
        }
        return null;
    }

    /**
     * Remove the object associated to the given key.
     *
     * @param the key of to be removed object
     */
    public void remove(Object key) {
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Removing object from store");
        }
        this.cache.remove(key);
        this.mrulist.remove(key);
        if(this.filesystem && key != null) {
            this.fsstore.remove(getFileName(key.toString()));
        }
    }

    /**
     * Indicates if the given key is associated to a contained object.
     *
     * @param the key of the object
     * @return true if the key exists
     */
    public boolean containsKey(Object key) {
        if(filesystem) {
            return (this.cache.containsKey(key) || this.fsstore.containsKey(key));
        } else {
            return this.cache.containsKey(key);
        }
    }

    /**
     * Returns the list of used keys as an Enumeration.
     *
     * @return the enumeration of the cache
     */
    public Enumeration keys() {
        return this.cache.keys();
    }

    /**
     * Frees some of the fast memory used by this store.
     * It removes the last element in the store.
     */
    public void free() {
        try {
            if(this.cache.size() > 0) {
                if (this.getLogger().isDebugEnabled()) {
                    this.getLogger().debug("Freeing cache");
                }
                // Swapping object on fs.
                if(this.filesystem) {
                    if(checkSerializable(this.cache.get(this.mrulist.getLast()))) {
                        try {
                            this.fsstore.store(this.getFileName(this.mrulist.getLast().toString()), 
                                               this.cache.get(this.mrulist.getLast()));
                        } catch(Exception e) {
                            this.getLogger().error("Error storing Object on fs",e);
                        }
                    }
                }
                this.cache.remove(this.mrulist.getLast());
                this.mrulist.removeLast();
                if (this.getLogger().isDebugEnabled()) {
                    this.getLogger().debug("Cache size=" + cache.size());
                }
            }
        } catch (Exception e) {
            this.getLogger().error("Error in free()", e);
        }
    }

    /**
     * This method checks if an object is seriazable.
     * FIXME: In the moment only CachedEventObject or
     * CachedStreamObject are stored.
     *
     * @param the object to be checked
     * @return true if the object is storeable
     */
    private boolean checkSerializable(Object object) {
        try {
            if (this.getLogger().isDebugEnabled()) {
                this.getLogger().debug("Object=" + object);
            }
            if((object.getClass().getName().equals("org.apache.cocoon.caching.CachedEventObject"))
              || (object.getClass().getName().equals("org.apache.cocoon.caching.CachedStreamObject"))
              || (ClassUtils.implementsInterface(object.getClass().getName(),"org.apache.cocoon.caching.CacheValidity"))) {
                return true;
            } else {
                return false;
            }
        } catch (Exception e) {
            this.getLogger().error("Error in checkSerializable()!", e);
            return false;
        }
    }

    /**
     * This method puts together a filename for
     * the object, which shall be stored on the
     * filesystem.
     *
     * @param the key of the object
     * @return the filename of the key
     */
    private String getFileName(String key) {
        return new StringBuffer()
          .append(this.cachedirstr)
          .append(File.separator)
          .append(URLEncoder.encode(key.toString()))
          .toString();
    }
}

