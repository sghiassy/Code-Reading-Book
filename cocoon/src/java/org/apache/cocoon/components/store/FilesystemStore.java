/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.store;

import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.util.IOUtils;

import java.io.File;
import java.io.IOException;
import java.util.Enumeration;

public final class FilesystemStore
extends AbstractLoggable
implements Contextualizable, Store, ThreadSafe {

    /** The directory repository */
    protected File directoryFile;
    protected volatile String directoryPath;

    /**
     * Sets the repository's location
     */
    public void setDirectory(final String directory)
    throws IOException {
        this.setDirectory(new File(directory));
    }

    public void contextualize(final Context context)
    throws ContextException {
        try {
            setDirectory((File) context.get(Constants.CONTEXT_WORK_DIR));
        } catch (Exception e) {
            // ignore
        }
    }

    /**
     * Sets the repository's location
     */
    public void setDirectory(final File directory)
    throws IOException {
        this.directoryFile = directory;

        /* Save directory path prefix */
        this.directoryPath = IOUtils.getFullFilename(this.directoryFile);
        this.directoryPath += File.separator;

        /* Does directory exist? */
        if (!this.directoryFile.exists()) {
            /* Create it anew */
            if (!this.directoryFile.mkdir()) {
                throw new IOException(
                "Error creating store directory '" + this.directoryPath + "': ");
            }
        }

        /* Is given file actually a directory? */
        if (!this.directoryFile.isDirectory()) {
            throw new IOException("'" + this.directoryPath + "' is not a directory");
        }

        /* Is directory readable and writable? */
        if (!(this.directoryFile.canRead() && this.directoryFile.canWrite())) {
            throw new IOException(
                "Directory '" + this.directoryPath + "' is not readable/writable"
            );
        }
    }

    /**
     * Returns the repository's full pathname
     */
    public String getDirectoryPath() {
        return this.directoryPath;
    }

    /**
     * Get the file associated with the given unique key name.
     */
    public Object get(final Object key) {
        if (this.getLogger().isDebugEnabled())
            getLogger().debug("FilesystemStore get(): Get file with key: " + key.toString());
        final File file = fileFromKey(key);

        if (file != null && file.exists()) {
            if (this.getLogger().isDebugEnabled())
                getLogger().debug("FilesystemStore get(): Found file with key: " + key.toString());
            return file;
        }
        return null;
    }

    /**
     * Store the given object in a persistent state.
     * 1) Null values generate empty directories.
     * 2) String values are dumped to text files
     * 3) Object values are serialized
     */
    public void store(final Object key, final Object value)
    throws IOException {
        final File file = fileFromKey(key);

        /* Create subdirectories as needed */
        final File parent = file.getParentFile();
        if (parent != null) {
            parent.mkdirs();
        }

        /* Store object as file */
        if (value == null) { /* Directory */
            if (file.exists()) {
                if (!file.delete()) { /* FAILURE */
                    getLogger().error("File cannot be deleted: " + file.toString());
                    return;
                }
            }

            file.mkdir();
        } else if (value instanceof String) { /* Text file */
            IOUtils.serializeString(file, (String) value);
        } else { /* Serialized Object */
            IOUtils.serializeObject(file, value);
        }
    }

    /**
     * Holds the given object in a volatile state.
     */
    public void hold(final Object key, final Object value)
    throws IOException {
        this.store(key, value);
        final File file = (File) this.get(key);
        if (file != null) {
          file.deleteOnExit();
        }
    }

    /**
     * Remove the object associated to the given key.
     */
    public void remove(final Object key) {
        final File file = fileFromKey(key);
        if (file != null) {
            file.delete();
        }
    }

    /**
     * Indicates if the given key is associated to a contained object.
     */
    public boolean containsKey(final Object key) {
        final File file = fileFromKey(key);
        if (file == null) {
            return false;
        }
        return file.exists();
    }

    /**
     * Returns the list of stored files as an Enumeration of Files
     */
   public Enumeration keys() {
        final FSEnumeration enum = new FSEnumeration();
        this.addKeys(enum, this.directoryFile);
        return enum;
    }

    protected void addKeys(FSEnumeration enum,
                           File  directory) {
        final int subStringBegin = this.directoryFile.getAbsolutePath().length() + 1;
        final File[] files = directory.listFiles();
        for(int i=0; i<files.length; i++) {
            if (files[i].isDirectory()) {
                this.addKeys(enum, files[i]);
            } else {
                enum.add(files[i].getAbsolutePath().substring(subStringBegin));
            }
        }
    }

    final class FSEnumeration implements Enumeration {
        private String[] array;
        private int      index;
        private int      length;

        FSEnumeration() {
            this.array = new String[16];
            this.length = 0;
            this.index = 0;
        }

        public void add(String key) {
            if (this.length == array.length) {
                String[] newarray = new String[this.length + 16];
                System.arraycopy(this.array, 0, newarray, 0, this.array.length);
                this.array = newarray;
            }
            this.array[this.length] = key;
            this.length++;
        }

        public boolean hasMoreElements() {
            return (this.index < this.length);
        }

        public Object nextElement() {
            if (this.hasMoreElements()) {
                this.index++;
                return this.array[index-1];
            }
            return null;
        }
    }

    /* Utility Methods*/
    protected File fileFromKey(final Object key) {
        return IOUtils.createFile(this.directoryFile, key.toString());
    }

    public String getString(final Object key)
    throws IOException {
        final File file = (File) this.get(key);
        if (file != null) {
            return IOUtils.deserializeString(file);
        }

        return null;
    }

    public void free() {}

    public Object getObject(final Object key)
    throws IOException, ClassNotFoundException
    {
        final File file = (File) this.get(key);
        if (file != null) {
            return IOUtils.deserializeObject(file);
        }

        return null;
    }
}
