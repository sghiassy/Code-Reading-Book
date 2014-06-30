/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.jispstore;

import org.apache.avalon.framework.activity.Startable;
import org.apache.avalon.framework.parameters.ParameterException;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.thread.ThreadSafe;

//only for scratchpad
import org.apache.cocoon.components.store.StoreJanitor;
import org.apache.cocoon.components.store.Store;

import java.util.ArrayList;
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
public class StoreJanitorImpl extends AbstractLoggable implements StoreJanitor,
                                                                  Parameterizable,
                                                                  ThreadSafe,
                                                                  Runnable,
                                                                  Startable {

    private int mFreeMemory = -1;
    private int mHeapSize = -1;
    private int mCleanupThreadInterval = -1;
    private int mPriority = -1;
    private Runtime mJVM;
    private ArrayList mStoreList;
    private int mIndex = -1;
    private static boolean mDoRun = false;

    /**
     * Initialize the StoreJanitorImpl.<br>
     * A few options can be used :
     * <UL>
     *  <LI>freememory = how many bytes shall be always free in the jvm</LI>
     *  <LI>heapsize = max. size of jvm memory consumption</LI>
     *  <LI>cleanupthreadinterval = how often (sec) shall run the cleanup thread</LI>
     *  <LI>threadpriority = priority of the thread (1-10). (Default: 10)</LI>
     * </UL>
     *
     * @param params the Configuration of the application
     * @exception ConfigurationException
     */
    public void parameterize(Parameters params) 
        throws ParameterException {
        if (this.getLogger().isDebugEnabled() == true) {
            this.getLogger().debug("Configure StoreJanitorImpl");
        }

        this.setJVM(Runtime.getRuntime());
        this.setFreememory(params.getParameterAsInteger("freememory",1000000));
        this.setHeapsize(params.getParameterAsInteger("heapsize",60000000));
        this.setCleanupthreadinterval(params.getParameterAsInteger("cleanupthreadinterval",10));
        this.setPriority(params.getParameterAsInteger("threadpriority",Thread.currentThread().getPriority()));

        if ((this.getFreememory() < 1)) {
            throw new ParameterException("StoreJanitorImpl freememory parameter has to be greater then 1");
        }
        if ((this.getHeapsize() < 1)) {
            throw new ParameterException("StoreJanitorImpl heapsize parameter has to be greater then 1");
        }
        if ((this.getCleanupthreadinterval() < 1)) {
            throw new ParameterException("StoreJanitorImpl cleanupthreadinterval parameter has to be greater then 1");
        }
        if ((this.getPriority() < 1)) {
            throw new ParameterException("StoreJanitorImpl threadpriority has to be greater then 1");
        }

        this.setStoreList(new ArrayList());
    }

    /**
     * Starts the cleanup Thread
     */
    public void start() {
        mDoRun = true;
        Thread checker = new Thread(this);
        if (this.getLogger().isDebugEnabled() == true) {
            this.getLogger().debug("Intializing checker thread");
        }
        checker.setPriority(this.getPriority());
        checker.setDaemon(true);
        checker.setName("checker");
        checker.start();
    }

    /**
     * Stops the cleanup Thread
     */
    public void stop() {
        mDoRun = false;
    }

    /**
     * The "checker" thread checks if memory is running low in the jvm.
     */
    public void run() {
        while (mDoRun) {
            // amount of memory used is greater then heapsize
            if (this.memoryLow()) {
                if (this.getLogger().isDebugEnabled() == true) {
                    this.getLogger().debug("Invoking garbage collection, total memory = "
                            + this.getJVM().totalMemory() + ", free memory = "
                            + this.getJVM().freeMemory());
                }
                this.freePhysicalMemory();
                if (this.getLogger().isDebugEnabled() == true) {
                    this.getLogger().debug("Garbage collection complete, total memory = "
                        + this.getJVM().totalMemory() + ", free memory = "
                        + this.getJVM().freeMemory());
                }
                synchronized (this) {
                    while (this.memoryLow() && this.getStoreList().size() > 0) {
                        this.freeMemory();
                    }
                    this.resetIndex();
                }
            }
            try {
                Thread.currentThread().sleep(this.mCleanupThreadInterval * 1000);
            } catch (InterruptedException ignore) {}
        }
    }

    /**
     * Method to check if memory is running low in the JVM.
     *
     * @return true if memory is low
     */
    private boolean memoryLow() {
        if (this.getLogger().isDebugEnabled() == true) {
            this.getLogger().debug("getJVM().totalMemory()=" + this.getJVM().totalMemory());
            this.getLogger().debug("getHeapsize()=" + this.getHeapsize());
            this.getLogger().debug("getJVM().freeMemory()=" + this.getJVM().freeMemory());
            this.getLogger().debug("getFreememory()=" + this.getFreememory());
        }
        return this.getJVM().totalMemory() > this.getHeapsize() && this.getJVM().freeMemory() < this.getFreememory();
    }

    /**
     * This method register the stores
     *
     * @param store the store to be registered
     */
    public void register(Store store) {
        this.getStoreList().add(store);
        if (this.getLogger().isDebugEnabled() == true) {
            this.getLogger().debug("Registering store instance");
            this.getLogger().debug("Size of StoreJanitor now:" + this.getStoreList().size());
        }
    }

    /**
     * This method unregister the stores
     *
     * @param store the store to be unregistered
     */
    public void unregister(Store store) {
        this.getStoreList().remove(store);
        if (this.getLogger().isDebugEnabled() == true) {
            this.getLogger().debug("Unregister store instance");
            this.getLogger().debug("Size of StoreJanitor now:" + this.getStoreList().size());
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
        try {
            if (this.getLogger().isDebugEnabled() == true) {
                this.getLogger().debug("StoreJanitor freeing memory!");
                this.getLogger().debug("StoreList size=" + this.getStoreList().size());
                this.getLogger().debug("Index before=" + this.getIndex());
            }
            if (this.getIndex() < this.getStoreList().size()) {
                if(this.getIndex() == -1) {
                    if (this.getLogger().isDebugEnabled() == true) {
                        this.getLogger().debug("Freeing at index=" + this.getIndex());
                    }
                    ((Store)this.getStoreList().get(0)).free();
                    this.setIndex(0);
                } else {
                    if (this.getLogger().isDebugEnabled() == true) {
                        this.getLogger().debug("Freeing at index=" + this.getIndex());
                    }
                    ((Store)this.getStoreList().get(this.getIndex())).free();
                    this.setIndex(this.getIndex() + 1);
                }
            } else {
                if (this.getLogger().isDebugEnabled() == true) {
                    this.getLogger().debug("Starting from the beginning");
                }
                this.resetIndex();
                ((Store)this.getStoreList().get(0)).free();
                this.setIndex(0);
            }
            this.freePhysicalMemory();
            if (this.getLogger().isDebugEnabled() == true) {
                this.getLogger().debug("Index after=" + this.getIndex());
            }
        } catch(Exception e) {
            this.getLogger().error("Error in freeMemory()",e);
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
        return mFreeMemory;
    }

    private void setFreememory(int _freememory) {
        this.mFreeMemory = _freememory;
    }

    private int getHeapsize() {
        return this.mHeapSize;
    }

    private void setHeapsize(int _heapsize) {
        this.mHeapSize = _heapsize;
    }

    private int getCleanupthreadinterval() {
        return this.mCleanupThreadInterval;
    }

    private void setCleanupthreadinterval(int _cleanupthreadinterval) {
        this.mCleanupThreadInterval = _cleanupthreadinterval;
    }

    private int getPriority() {
        return this.mPriority;
    }

    private void setPriority(int _priority) {
        this.mPriority = _priority;
    }

    private Runtime getJVM() {
        return this.mJVM;
    }

    private void setJVM(Runtime _runtime) {
        this.mJVM = _runtime;
    }

    private ArrayList getStoreList() {
        return this.mStoreList;
    }

    private void setStoreList(ArrayList _storelist) {
        this.mStoreList = _storelist;
    }

    private void setIndex(int _index) {
        if (this.getLogger().isDebugEnabled() == true) {
            this.getLogger().debug("Setting index=" + _index);
        }
        this.mIndex = _index;
    }

    private int getIndex() {
        return this.mIndex;
    }

    private void resetIndex() {
        if (this.getLogger().isDebugEnabled() == true) {
            this.getLogger().debug("Reseting index");
        }
        this.mIndex = -1;
    }
}
