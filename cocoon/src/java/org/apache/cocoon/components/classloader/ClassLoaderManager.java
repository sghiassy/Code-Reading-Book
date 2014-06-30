/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.classloader;

import org.apache.avalon.framework.component.Component;

import java.io.File;
import java.io.IOException;

/**
 * A class loader manager acting as a proxy for a <b>single</b>
 * <code>RepositoryClassLoader</code>.
 * This class guarantees that a single class loader instance exists so
 * that it can be safely reinstantiated for dynamic class reloading
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:09 $
 */
public interface ClassLoaderManager extends Component {

    String ROLE = "org.apache.cocoon.components.classloader.ClassLoaderManager";

    /**
    * Add a directory to the proxied class loader
    *
    * @param directoryName The repository name
    * @exception IOException If the directory is invalid
    */
    void addDirectory(File directoryName) throws IOException;

    /**
    * Load a class through the proxied class loader
    *
    * @param className The name of the class to be loaded
    * @return The loaded class
    * @exception ClassNotFoundException If the class is not found
    */
    Class loadClass(String className) throws ClassNotFoundException;

    /**
    * Reinstantiate the proxied class loader to allow for class reloading
    *
    */
    void reinstantiate();
}
