/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.jispstore;

import org.apache.avalon.framework.activity.Initializable;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.ParameterException;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;

//only for scratchpad
import org.apache.cocoon.components.store.Store;

import org.apache.cocoon.util.IOUtils;

import com.coyotegulch.jisp.BTreeIndex;
import com.coyotegulch.jisp.IndexedObjectDatabase;
import com.coyotegulch.jisp.KeyNotFound;
import com.coyotegulch.jisp.KeyObject;

import java.io.File;
import java.io.IOException;
import java.io.Serializable;
import java.util.Enumeration;

/**
 * This store is based on the Jisp library 
 * (http://www.coyotegulch.com/jisp/index.html). This store uses B-Tree indexes
 * to access variable-length serialized data stored in files.
 *
 * @author <a href="mailto:g-froehlich@gmx.de">Gerhard Froehlich</a>
 */
public final class JispFilesystemStore
extends AbstractLoggable
implements Store,
           Contextualizable,
           ThreadSafe,
           Initializable,
           Parameterizable {

    /**
     *  The directory repository
     */
    protected File mDirectoryFile;
    protected volatile String mDirectoryPath;

    /**
     *  The database
     */
    private String mDatabaseName;
    private String mIndexName;
    private int mOrder;
    private IndexedObjectDatabase mDatabase;
    private BTreeIndex mIndex;

    /**
     *  Sets the repository's location
     *
     * @param directory the new directory value
     * @exception  IOException
     */
    public void setDirectory(final String directory)
        throws IOException {
        this.setDirectory(new File(directory));
    }

    /**
     *  Sets the repository's location
     *
     * @param directory the new directory value
     * @exception  IOException
     */

    public void setDirectory(final File directory)
        throws IOException {
        this.mDirectoryFile = directory;

        this.mDirectoryPath = IOUtils.getFullFilename(this.mDirectoryFile);
        this.mDirectoryPath += File.separator;

        if (!this.mDirectoryFile.exists()) {
            /* Create it new */
            if (!this.mDirectoryFile.mkdir()) {
                throw new IOException(
                        "Error creating store directory '" 
                        + this.mDirectoryPath + "': ");
            }
        }

        /* Is given file actually a directory? */
        if (!this.mDirectoryFile.isDirectory()) {
            throw new IOException("'" + this.mDirectoryPath 
                                  + "' is not a directory");
        }

        /* Is directory readable and writable? */
        if (!(this.mDirectoryFile.canRead()
              && this.mDirectoryFile.canWrite())) {
            throw new IOException(
                    "Directory '" + this.mDirectoryPath 
                    + "' is not readable/writable"
                    );
        }
    }

    /**
     * Returns the repository's full pathname
     *
     * @return the directory as String
     */
    public String getDirectoryPath() {
        return this.mDirectoryPath;
    }

    /**
     * Returns a Object from the store associated with the Key Object
     *
     * @param key the Key object
     * @return the Object associated with Key Object
     */
    public Object get(Object key) {
        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug("get(): Get file with key: " + key.toString());
        }
        Object readObj = null;

        try {
            readObj = mDatabase.read(this.wrapKeyObject(key), mIndex);
            if (getLogger().isDebugEnabled()) {
                if (readObj != null) {
                    this.getLogger().debug("get(): FOUND!!= " + readObj);
                } else {
                    this.getLogger().debug("get(): NOT_FOUND!!");
                }
            }
        } catch (Exception e) {
            getLogger().error("get(..): Exception", e);
        }
        return readObj;
    }

    /**
     * Contextualize the Component
     *
     * @param  context the Context of the Application
     * @exception  ContextException
     */
    public void contextualize(final Context context)
        throws ContextException {
        try {
            setDirectory((File) context.get(Constants.CONTEXT_WORK_DIR));
        } catch (Exception ignore) {;}
    }

    /**
     *  Initialize the Component
     */
    public void initialize() {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("initialize() JispFilesystemStore");
        }

        try {
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("initialize(): Path to Datafile=" 
                + this.getDirectoryPath() + mDatabaseName);
            }
            File myFile = new File(this.getDirectoryPath() + mDatabaseName);
            if (myFile.exists()) {
                if (getLogger().isDebugEnabled()) {
                    this.getLogger().debug("initialize(): Datafile exists");
                }
                mDatabase = new IndexedObjectDatabase(getDirectoryPath() 
                                + mDatabaseName, false);
                mIndex = new BTreeIndex(this.getDirectoryPath() + mIndexName);
                mDatabase.attachIndex(mIndex);
            } else {
                if (getLogger().isDebugEnabled()) {
                    this.getLogger().debug("initialize(): Datafile not exists");
                }
                mDatabase = new IndexedObjectDatabase(getDirectoryPath() 
                            + mDatabaseName, false);
                mIndex = new BTreeIndex(this.getDirectoryPath() + mIndexName,
                        mOrder, new JispStringKey(), false);
                mDatabase.attachIndex(mIndex);
            }
        } catch (KeyNotFound ignore) {
        } catch (Exception e) {
            getLogger().error("initialize(..) Exception", e);
        }
    }

    /**
     *  Configure the Component.<br>
     *  A few options can be used 
     *  <UL>
     *    <LI> datafile = the name of the data file (Default: cocoon.dat)
     *    </LI>
     *    <LI> indexfile = the name of the index file (Default: cocoon.idx)
     *    </LI>
     *    <LI> order = The page size of the B-Tree</LI>
     *  </UL>
     *
     * @param params the configuration paramters
     * @exception  ParameterException
     */
     public void parameterize(Parameters params) {
      
        mDatabaseName = params.getParameter("datafile", "cocoon.dat");
        mIndexName = params.getParameter("indexfile", "cocoon.idx");
        mOrder = params.getParameterAsInteger("order", 301);

        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug("parameterize(..): mDatabaseName=" 
                                    + mDatabaseName);
            this.getLogger().debug("parameterize(..): mIndexName=" 
                                    + mIndexName);
            this.getLogger().debug("parameterize(..): mOrder=" + mOrder);
        }
    }

    /**
     *  Store the given object in the indexed data file.
     *
     * @param key the key object
     * @param value the value object 
     * @exception  IOException
     */
    public void store(Object key, Object value)
        throws IOException {
        
        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug("store(): Store file with key: " 
                                  + key.toString());
            this.getLogger().debug("store(): Store file with value: " 
                                  + value.toString());
        }

        if (value instanceof Serializable) {
            try {
                KeyObject[] keyArray = new KeyObject[1];
                keyArray[0] = this.wrapKeyObject(key);
                mDatabase.write(keyArray, (Serializable) value);
            } catch (Exception e) {
                this.getLogger().error("store(..): Exception", e);
            }
        } else {
            throw new IOException("Object not Serializable");
        }
    }

    /**
     *  Holds the given object in the indexed data file.
     *
     * @param key the key object
     * @param value the value object 
     * @exception IOException
     */
    public void hold(Object key, Object value)
        throws IOException {
        this.store(key, value);
    }

    /**
     * Frees some values of the data file.<br>
     * TODO: implementation
     */
    public void free() { 
       //TODO: implementation
    }

    /**
     * Removes a value from the data file with the given key.
     *
     * @param key the key object
     */
    public void remove(Object key) {
        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug("remove(..) Remove item");
        }

        try {
            KeyObject[] keyArray = new KeyObject[1];
            keyArray[0] = this.wrapKeyObject(key);
            mDatabase.remove(keyArray);
        } catch (KeyNotFound ignore) {
        } catch (Exception e) {
            this.getLogger().error("remove(..): Exception", e);
        }
    }

    /**
     *  Test if the the index file contains the given key
     *
     * @param key the key object
     * @return true if Key exists and false if not
     */
    public boolean containsKey(Object key) {
        long res = -1;

        try {
            res = mIndex.findKey(this.wrapKeyObject(key));
            if (getLogger().isDebugEnabled()) {
                this.getLogger().debug("containsKey(..): res=" + res);
            }
        } catch (KeyNotFound ignore) {
        } catch (Exception e) {
            this.getLogger().error("containsKey(..): Exception", e);
        }

        if (res > 0) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Returns a Enumeration of all Keys in the indexed file.<br>
     * TODO: Implementation
     *
     * @return  Enumeration Object with all existing keys
     */
    public Enumeration keys() {
        //TODO: Implementation
        return null;
    }

    /**
     * This method wraps around the key Object a Jisp KeyObject.
     * 
     * @param key the key object
     * @return the wrapped key object
     */
    private KeyObject wrapKeyObject(Object key) {

        if(key.toString() instanceof String) {
            if (getLogger().isDebugEnabled()) {
                this.getLogger().debug("wrapKeyObject() Key is String");
            }

            return new JispStringKey(key.toString());
        } else {
            //TODO: Implementation of Integer and Long keys
            return null;
        }
    }
}



