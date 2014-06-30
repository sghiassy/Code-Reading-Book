/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.store;

import org.apache.avalon.framework.activity.Startable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Iterator;

/**
 * This class is a implentation of a StoreJanitor. Store classes
 * can register to the StoreJanitor. When memory is too low,
 * the StoreJanitor frees the registered caches until memory is normal.
 *
 * @author <a href="mailto:cs@ffzj0ia9.bank.dresdner.net">Christian Schmitt</a>
 * @author <a href="mailto:g-froehlich@gmx.de">Gerhard Froehlich</a>
 * @author <a href="mailto:proyal@managingpartners.com">Peter Royal</a>
 */
public class StoreJanitorImpl 
extends AbstractLoggable 
implements StoreJanitor,
           Configurable,
           ThreadSafe,
           Runnable,
           Startable {

    private int freememory = -1;
    private int heapsize = -1;
    private int cleanupthreadinterval = -1;
    private int priority = -1;
    private Runtime jvm;
    private ArrayList storelist;
    private int index = -1;
    private static boolean doRun = false;
    private int m_percent;

    /**
     * Initialize the StoreJanitorImpl.
     * A few options can be used :
     * <UL>
     *  <LI>freememory = how many bytes shall be always free in the jvm</LI>
     *  <LI>heapsize = max. size of jvm memory consumption</LI>
     *  <LI>cleanupthreadinterval = how often (sec) shall run the cleanup thread</LI>
     *  <LI>threadpriority = priority of the thread (1-10). (Default: 10)</LI>
     * </UL>
     *
     * @param the Configuration of the application
     * @exception ConfigurationException
     */
    public void configure(Configuration conf) throws ConfigurationException {
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Configure StoreJanitorImpl");
        }
        this.setJVM(Runtime.getRuntime());

        Parameters params = Parameters.fromConfiguration(conf);
        this.setFreememory(params.getParameterAsInteger("freememory",1000000));
        this.setHeapsize(params.getParameterAsInteger("heapsize",60000000));
        this.setCleanupthreadinterval(params.getParameterAsInteger("cleanupthreadinterval",10));
        this.setPriority(params.getParameterAsInteger( "threadpriority",
                                        Thread.currentThread().getPriority()));
        this.m_percent = params.getParameterAsInteger( "percent_to_free",10);

        if ((this.getFreememory() < 1)) {
            throw new ConfigurationException("StoreJanitorImpl freememory parameter has to be greater then 1");
        }
        if ((this.getHeapsize() < 1)) {
            throw new ConfigurationException("StoreJanitorImpl heapsize parameter has to be greater then 1");
        }
        if ((this.getCleanupthreadinterval() < 1)) {
            throw new ConfigurationException("StoreJanitorImpl cleanupthreadinterval parameter has to be greater then 1");
        }
        if ((this.getPriority() < 1)) {
            throw new ConfigurationException("StoreJanitorImpl threadpriority has to be greater then 1");
        }
        if ((this.m_percent > 100 && this.m_percent < 1)) {
            throw new ConfigurationException("StoreJanitorImpl percent_to_free, has to be between 1 and 100");
        }

        this.setStoreList(new ArrayList());
    }

    public void start() {
        doRun = true;
        Thread checker = new Thread(this);
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Intializing checker thread");
        }
        checker.setPriority(this.getPriority());
        checker.setDaemon(true);
        checker.setName("checker");
        checker.start();
    }

    public void stop() {
        doRun = false;
    }

    /**
     * The "checker" thread checks if memory is running low in the jvm.
     */
    public void run() {
        while (doRun) {
            // amount of memory used is greater then heapsize
            if (this.memoryLow()) {
                if (this.getLogger().isDebugEnabled()) {
                    this.getLogger().debug("Invoking garbage collection, total memory = "
                            + this.getJVM().totalMemory() + ", free memory = "
                            + this.getJVM().freeMemory());
                }
                
                this.freePhysicalMemory();
                
                if (this.getLogger().isDebugEnabled()) {
                    this.getLogger().debug("Garbage collection complete, total memory = "
                        + this.getJVM().totalMemory() + ", free memory = "
                        + this.getJVM().freeMemory());
                }
                
                synchronized (this) {
                    if (this.memoryLow() && this.getStoreList().size() > 0) {
                        this.freeMemory();
                        this.setIndex(this.getIndex() + 1);
                    }
               }
            }
            try {
                Thread.currentThread().sleep(this.cleanupthreadinterval * 1000);
            } catch (InterruptedException ignore) {}
        }
    }

    /**
     * Method to check if memory is running low in the JVM.
     *
     * @return true if memory is low
     */
    private boolean memoryLow() {
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("JVM total Memory: " + this.getJVM().totalMemory());
            this.getLogger().debug("JVM free Memory: " + this.getJVM().freeMemory());
        }
      
        if((this.getJVM().totalMemory() > this.getHeapsize()) 
            && (this.getJVM().freeMemory() < this.getFreememory())) {
            if (this.getLogger().isDebugEnabled()) {
                this.getLogger().debug("Memory is low = true");
            }
            return true;
        } else {
            if (this.getLogger().isDebugEnabled()) {
                this.getLogger().debug("Memory is low = false");
            }
            return false;
        }
    }

    /**
     * This method register the stores
     *
     * @param the store to be registered
     */
    public void register(Store store) {
        this.getStoreList().add(store);
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Registering store instance");
            this.getLogger().debug("Size of StoreJanitor now:" 
                                   + this.getStoreList().size());
        }
    }

    /**
     * This method unregister the stores
     *
     * @param the store to be unregistered
     */
    public void unregister(Store store) {
        this.getStoreList().remove(store);
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Unregister store instance");
            this.getLogger().debug("Size of StoreJanitor now:" 
                                   + this.getStoreList().size());
        }
    }

    /**
     * This method return a java.util.Iterator of every registered stores
     * 
     * <i>The iterators returned is fail-fast: if list is structurally
     * modified at any time after the iterator is created, in any way, the
     * iterator will throw a ConcurrentModificationException.  Thus, in the
     * face of concurrent modification, the iterator fails quickly and
     * cleanly, rather than risking arbitrary, non-deterministic behavior at
     * an undetermined time in the future.</i>
     *
     * @return a java.util.Iterator
     */
    public Iterator iterator() {
        return this.getStoreList().iterator();
     }
     
    /**
     * Round Robin alghorithm for freeing the registerd caches.
     */
    private void freeMemory() {
        Store store;
      
        try {
            //Determine elements in Store:
            if (this.getLogger().isDebugEnabled()) {
                this.getLogger().debug("StoreList size=" + this.getStoreList().size());
                this.getLogger().debug("Actual Index position: " + this.getIndex());
            }
            if (this.getIndex() < this.getStoreList().size()) {
                if(this.getIndex() == -1) {
                    this.setIndex(0);
                    store = (Store)this.getStoreList().get(this.getIndex());
                    
                    if (this.getLogger().isDebugEnabled()) {
                        this.getLogger().debug("Freeing Store: " + this.getIndex());
                    }
                
                    //delete proportionate elements out of the cache as
                    //configured.
                    int limit = this.calcToFree(store);
                    for (int i=0; i < limit; i++) {
                        store.free();
                    }
                } else {
                    store = (Store)this.getStoreList().get(this.getIndex());
                    
                    if (this.getLogger().isDebugEnabled()) {
                        this.getLogger().debug("Freeing Store: " + this.getIndex());
                    }
                    
                    //delete proportionate elements out of the cache as
                    //configured.
                    int limit = this.calcToFree(store);
                    for (int i=0; i < limit; i++) {
                        store.free();
                    }
                }
            } else {
                if (this.getLogger().isDebugEnabled()) {
                    this.getLogger().debug("Starting from the beginning");
                }
                
                this.resetIndex();
                this.setIndex(0);
                store = (Store)this.getStoreList().get(this.getIndex());
                 
                //delete proportionate elements out of the cache as
                //configured.
                int limit = this.calcToFree(store);
                for (int i=0; i < limit; i++) {
                    store.free();
                }
            }
        } catch(Exception e) {
            this.getLogger().error("Error in freeMemory()",e);
        }
    }

    /**
     * This method claculates the number of Elements to be freememory
     * out of the Cache.
     *
     * @param store the Store which was selected as victim
     * @return number of elements to be removed!
     */
    private int calcToFree(Store store) {
        int cnt = 0;
        try {
            this.getLogger().debug("Calculating percentage!");
            Enumeration enum = store.keys();
            
            while(enum.hasMoreElements()) {
                cnt++; 
                enum.nextElement(); 
            }
            
            this.getLogger().debug("Counted: " + cnt);
            double fac = new Integer(this.m_percent).doubleValue() / 100;
            double dcnt = new Integer(cnt).doubleValue();
            this.getLogger().debug("Calculated: " + new Double(dcnt * fac).intValue());
            
            return new Double(dcnt * fac).intValue();
        } catch (Exception e) {
            this.getLogger().error("",e);
            return 0;
        }
    }
    
    /**
     * This method forces the garbage collector
     */
    private void freePhysicalMemory() {
        this.getJVM().runFinalization();
        this.getJVM().gc();
    }

    private int getFreememory() {
        return freememory;
    }

    private void setFreememory(int _freememory) {
        this.freememory = _freememory;
    }

    private int getHeapsize() {
        return this.heapsize;
    }

    private void setHeapsize(int _heapsize) {
        this.heapsize = _heapsize;
    }

    private int getCleanupthreadinterval() {
        return this.cleanupthreadinterval;
    }

    private void setCleanupthreadinterval(int _cleanupthreadinterval) {
        this.cleanupthreadinterval = _cleanupthreadinterval;
    }

    private int getPriority() {
        return this.priority;
    }

    private void setPriority(int _priority) {
        this.priority = _priority;
    }

    private Runtime getJVM() {
        return this.jvm;
    }

    private void setJVM(Runtime _runtime) {
        this.jvm = _runtime;
    }

    private ArrayList getStoreList() {
        return this.storelist;
    }

    private void setStoreList(ArrayList _storelist) {
        this.storelist = _storelist;
    }

    private void setIndex(int _index) {
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Setting index=" + _index);
        }
        this.index = _index;
    }

    private int getIndex() {
        return this.index;
    }

    private void resetIndex() {
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Reseting index");
        }
        this.index = -1;
    }
}
