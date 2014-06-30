/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.environment.http;

import uk.co.weft.maybeupload.MaybeUploadRequestWrapper;

import javax.servlet.http.HttpServletRequest;
import java.io.File;
import java.util.Map;
import java.util.Vector;

/**
 *
 * Implements the {@link javax.servlet.http.HttpServletRequest} interface
 * to provide request information for HTTP servlets.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */

public final class RequestWrapper extends MaybeUploadRequestWrapper implements HttpServletRequest {

    /**
     * Return a wrapped request object
     */
    public static HttpServletRequest getServletRequest(HttpServletRequest request,
                                                       boolean saveUploadedFilesToDisk,
                                                       File    uploadDirectory,
                                                       boolean allowOverwrite,
                                                       boolean silentlyRename,
                                                       int     maxUploadSize) {
        HttpServletRequest req = request;
        String contentType = req.getContentType();

        if (contentType == null) {
            contentType = "application/x-www-form-urlencoded";
        }

        if (contentType.startsWith("multipart/form-data")) {
            try {
                req = new RequestWrapper(request,
                                         saveUploadedFilesToDisk,
                                         uploadDirectory,
                                         allowOverwrite,
                                         silentlyRename,
                                         maxUploadSize);
            } catch (Exception e) {
                req = request;
            }
        }

        return req;
    }

    /**
     * Implementation of the get method
     */
    public static Object get(HttpServletRequest request, String name) {
        // FIXME We should get rid of this instanceof test
        if (request instanceof RequestWrapper) {
            return ((RequestWrapper) request).get(name);
        } else {
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
        }
        return null;
    }

    public RequestWrapper(HttpServletRequest httpservletrequest, boolean flag1, File file, boolean flag2, boolean flag3, int size)
        throws Exception {
        super(httpservletrequest, flag1, file, flag2, flag3, size);
    }

    public Map getParameterMap() {
        // FIXME:
        return null;
    }


    public void setCharacterEncoding(String s) {
        // FIXME:
    }


    public StringBuffer getRequestURL() {
        // FIXME:
        return null;
    }
}
