/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.classloader;

import org.apache.avalon.framework.thread.ThreadSafe;

import java.io.File;
import java.io.IOException;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.Vector;

/**
 * A singleton-like implementation of <code>ClassLoaderManager</code>
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:09 $
 */
public class ClassLoaderManagerImpl implements ClassLoaderManager, ThreadSafe {
  /**
   * The single class loader instance
   */
  protected static RepositoryClassLoader instance = null;

  protected static Set fileSet = Collections.synchronizedSet(new HashSet());

  /**
   * A constructor that ensures only a single class loader instance exists
   *
   */
  public ClassLoaderManagerImpl() {
    if (instance == null) {
      this.reinstantiate();
    }
  }

  /**
   * Add a directory to the proxied class loader
   *
   * @param directoryName The repository name
   * @exception IOException If the directory is invalid
   */
  public void addDirectory(File directoryName) throws IOException {
    if ( ! ClassLoaderManagerImpl.fileSet.contains(directoryName)) {
        ClassLoaderManagerImpl.fileSet.add(directoryName);
        ClassLoaderManagerImpl.instance.addDirectory(directoryName);
    }
  }

  /**
   * Load a class through the proxied class loader
   *
   * @param className The name of the class to be loaded
   * @return The loaded class
   * @exception ClassNotFoundException If the class is not found
   */
  public Class loadClass(String className) throws ClassNotFoundException {
    return ClassLoaderManagerImpl.instance.loadClass(className);
  }

  /**
   * Reinstantiate the proxied class loader to allow for class reloading
   *
   */
  public void reinstantiate() {
    if ( ClassLoaderManagerImpl.fileSet.isEmpty()) {
      ClassLoaderManagerImpl.instance = new RepositoryClassLoader();
    } else {
      ClassLoaderManagerImpl.instance = new RepositoryClassLoader(new Vector(ClassLoaderManagerImpl.fileSet));
    }
  }
}
