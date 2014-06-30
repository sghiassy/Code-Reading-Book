/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.util;

import java.io.File;
import java.io.FileFilter;

/**
 * Implements a filter for java archives.
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</A>
 */

public class JavaArchiveFilter implements FileFilter {

    public boolean accept(File file) {
        String name = file.getName().toLowerCase();
        return (name.endsWith(".jar") || name.endsWith(".zip"));
    }

}
