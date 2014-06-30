/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.environment.http;

import javax.servlet.http.HttpServletRequest;
import java.io.File;
import java.util.Vector;

/**
 *
 * Implements the {@link javax.servlet.http.HttpServletRequest} interface
 * to provide request information for HTTP servlets.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */

public final class RequestWrapper {

    /**
     * Return a wrapped request object
     */
    public static HttpServletRequest getServletRequest(HttpServletRequest request,
                                                       boolean saveUploadedFilesToDisk,
                                                       File    uploadDirectory,
                                                       boolean allowOverwrite,
                                                       boolean silentlyRename,
                                                       int     maxUploadSize) {
        return request;
    }

    /**
     * Implementation of the get method
 */
    public static Object get(HttpServletRequest request, String name) {
        String[] values = request.getParameterValues(name);

        if (values == null) return null;

        if (values.length == 1) {
            return values[0];
    }

        if (values.length > 1) {
            Vector vect = new Vector(values.length);

            for (int i = 0; i < values.length; i++) {
                vect.add(values[i]);
            }

            return vect;
        }
        return null;
    }
}
