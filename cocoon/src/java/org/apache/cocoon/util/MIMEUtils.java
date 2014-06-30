/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.util;

/**
 * A collection of <code>File</code>, <code>URL</code> and filename
 * utility methods
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:21 $
 */

public class MIMEUtils {

    public static String getMIMEType(String ext) {
        // todo
        return null;
    }
        
    public static String getDefaultExtension(String type) {
        if (type == null) {
            return ".html";
        } else if ("text/html".equals(type)) {
            return ".html";
        } else if ("text/xml".equals(type)) {
            return ".xml";
        } else if ("text/css".equals(type)) {
            return ".css";
        } else if ("text/vnd.wap.wml".equals(type)) {
            return ".wml";
        } else if ("image/jpg".equals(type)) {
            return ".jpg";
        } else if ("image/jpeg".equals(type)) {
            return ".jpg";
        } else if ("image/png".equals(type)) {
            return ".png";
        } else if ("image/gif".equals(type)) {
            return ".gif";
        } else if ("image/svg-xml".equals(type)) {
            return ".svg";
        } else if ("application/pdf".equals(type)) {
            return ".pdf";
        } else if ("model/vrml".equals(type)) {
            return ".wrl";
        } else if ("text/plain".equals(type)) {
            return ".txt";
        } else if ("application/rtf".equals(type)) {
            return ".rtf";
        } else if ("text/rtf".equals(type)) {
            return ".rtf";
        } else if ("application/smil".equals(type)) {
            return ".smil";
        } else if ("application/x-javascript".equals(type)) {
            return ".js";
        } else if ("application/zip".equals(type)) {
            return ".zip";
        } else if ("video/mpeg".equals(type)) {
            return ".mpg";
        } else if ("video/quicktime".equals(type)) {
            return ".mov";
        } else if ("audio/midi".equals(type)) {
            return ".mid";
        } else if ("audio/mpeg".equals(type)) {
            return ".mp3";
        } else {
            return "";
        }
    }
}
