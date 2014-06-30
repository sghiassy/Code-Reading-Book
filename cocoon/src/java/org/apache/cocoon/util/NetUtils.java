/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;

import java.util.BitSet;
import java.util.Iterator;
import java.util.Map;

/**
 * A collection of <code>File</code>, <code>URL</code> and filename
 * utility methods
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/04 09:45:36 $
 */

public class NetUtils {

    /**
     * Array containing the safe characters set as defined by RFC 1738
     */
    private static BitSet safeCharacters;


    private static final char[] hexadecimal =
    {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
     'A', 'B', 'C', 'D', 'E', 'F'};

    static {
        safeCharacters = new BitSet(256);
        int i;
        // 'lowalpha' rule
        for (i = 'a'; i <= 'z'; i++) {
            safeCharacters.set(i);
        }
        // 'hialpha' rule
        for (i = 'A'; i <= 'Z'; i++) {
            safeCharacters.set(i);
        }
        // 'digit' rule
        for (i = '0'; i <= '9'; i++) {
            safeCharacters.set(i);
        }

        // 'safe' rule
        safeCharacters.set('$');
        safeCharacters.set('-');
        safeCharacters.set('_');
        safeCharacters.set('.');
        safeCharacters.set('+');

        // 'extra' rule
        safeCharacters.set('!');
        safeCharacters.set('*');
        safeCharacters.set('\'');
        safeCharacters.set('(');
        safeCharacters.set(')');
        safeCharacters.set(',');

        // special characters common to http: file: and ftp: URLs ('fsegment' and 'hsegment' rules)
        safeCharacters.set('/');
        safeCharacters.set(':');
        safeCharacters.set('@');
        safeCharacters.set('&');
        safeCharacters.set('=');
    }

    /**
     * Decode a path
     *
     * @param path the path to decode
     * @return the decoded path
     */
    public static String decodePath(String path) {
        return java.net.URLDecoder.decode( path );
    }

    /**
     * Encode a path as required by the URL specificatin (<a href="http://www.ietf.org/rfc/rfc1738.txt">
     * RFC 1738</a>). This differs from <code>java.net.URLEncoder.encode()</code> which encodes according
     * to the <code>x-www-form-urlencoded</code> MIME format.
     *
     * @param path the path to encode
     * @return the encoded path
     */
    public static String encodePath(String path) {
       // stolen from org.apache.catalina.servlets.DefaultServlet ;)

        /**
         * Note: This code portion is very similar to URLEncoder.encode.
         * Unfortunately, there is no way to specify to the URLEncoder which
         * characters should be encoded. Here, ' ' should be encoded as "%20"
         * and '/' shouldn't be encoded.
         */

        int maxBytesPerChar = 10;
        StringBuffer rewrittenPath = new StringBuffer(path.length());
        ByteArrayOutputStream buf = new ByteArrayOutputStream(maxBytesPerChar);
        OutputStreamWriter writer = null;
        try {
            writer = new OutputStreamWriter(buf, "UTF8");
        } catch (Exception e) {
            e.printStackTrace();
            writer = new OutputStreamWriter(buf);
        }

        for (int i = 0; i < path.length(); i++) {
            int c = (int) path.charAt(i);
            if (safeCharacters.get(c)) {
                rewrittenPath.append((char)c);
            } else {
                // convert to external encoding before hex conversion
                try {
                    writer.write(c);
                    writer.flush();
                } catch(IOException e) {
                    buf.reset();
                    continue;
                }
                byte[] ba = buf.toByteArray();
                for (int j = 0; j < ba.length; j++) {
                    // Converting each byte in the buffer
                    byte toEncode = ba[j];
                    rewrittenPath.append('%');
                    int low = (int) (toEncode & 0x0f);
                    int high = (int) ((toEncode & 0xf0) >> 4);
                    rewrittenPath.append(hexadecimal[high]);
                    rewrittenPath.append(hexadecimal[low]);
                }
                buf.reset();
            }
        }

        return rewrittenPath.toString();
    }

    /**
     * Returns the path of the given resource.
     *
     * @path the resource
     * @return the resource path
     */
    public static String getPath(String uri) {
        int i = uri.lastIndexOf('/');
        if(i > -1)
            return uri.substring(0, i);
        i = uri.indexOf(':');
        return (i > -1) ? uri.substring(i+1,uri.length()) : "";
    }

   /**
    * Remove path and file information from a filename returning only its
    * extension  component
    *
    * @param filename The filename
    * @return The filename extension (with starting dot!)
    */
    public static String getExtension(String uri) {
        int dot = uri.lastIndexOf('.');
        if (dot > -1) {
            uri = uri.substring(dot);
            int slash = uri.lastIndexOf('/');
            if (slash > -1) {
                return null;
            } else {
                int sharp = uri.lastIndexOf('#');
                if (sharp > -1) {
                    // uri starts with dot already
                    return uri.substring(0, sharp);
                } else {
                    int mark = uri.lastIndexOf('?');
                    if (mark > -1) {
                        // uri starts with dot already
                        return uri.substring(0, mark);
                    } else {
                        return uri;
                    }
                }
            }
        } else {
            return null;
        }
    }

    /**
     * Absolutize a relative resource on the given absolute path.
     *
     * @path the absolute path
     * @relativeResource the relative resource
     * @return the absolutized resource
     */
    public static String absolutize(String path, String relativeResource) {
        if (("".equals(path)) || (path == null)) return relativeResource;
        if (relativeResource.charAt(0) != '/') {
            int length = path.length() - 1;
            boolean slashPresent = (path.charAt(length) == '/');
            StringBuffer b = new StringBuffer();
            b.append(path);
            if (!slashPresent) b.append('/');
            b.append(relativeResource);
            return b.toString();
        } else {
            // resource is already absolute
            return relativeResource;
        }
    }

    /**
     * Relativize an absolute resource on a given absolute path.
     *
     * @path the absolute path
     * @relativeResource the absolute resource
     * @return the resource relative to the given path
     */
    public static String relativize(String path, String absoluteResource) {
        if (("".equals(path)) || (path == null)) return absoluteResource;
        int length = path.length() - 1;
        boolean slashPresent = path.charAt(length) == '/';
        if (absoluteResource.startsWith(path)) {
            // resource is direct descentant
            return absoluteResource.substring(length + (slashPresent ? 1 : 2));
        } else {
            // resource is not direct descendant
            if (!slashPresent) path += "/";
            int index = StringUtils.matchStrings(path, absoluteResource);
            if (index > 0 && path.charAt(index-1) != '/') {
                index = path.substring(0, index).lastIndexOf('/');
                index++;
            }
            String pathDiff = path.substring(index);
            String resource = absoluteResource.substring(index);
            int levels = StringUtils.count(pathDiff, '/');
            StringBuffer b = new StringBuffer();
            for (int i = 0; i < levels; i++) {
                b.append("../");
            }
            b.append(resource);
            return b.toString();
        }
    }

    /**
     * Normalize a uri containing ../ and ./ paths.
     *
     * @param uri The uri path to normalize
     * @return The normalized uri
     */
    public static String normalize(String uri) {
        String[] dirty = StringUtils.split(uri, "/");
        int length = dirty.length;
        String[] clean = new String[length];

        boolean path;
        boolean finished;
        while (true) {
            path = false;
            finished = true;
            for (int i = 0, j = 0; (i < length) && (dirty[i] != null); i++) {
                if (".".equals(dirty[i])) {
                    // ignore
                } else if ("..".equals(dirty[i])) {
                    clean[j++] = dirty[i];
                    if (path) finished = false;
                } else {
                    if ((i+1 < length) && ("..".equals(dirty[i+1]))) {
                        i++;
                    } else {
                        clean[j++] = dirty[i];
                        path = true;
                    }
                }
            }
            if (finished) {
                break;
            } else {
                dirty = clean;
                clean = new String[length];
            }
        }

        StringBuffer b = new StringBuffer(uri.length());

        for (int i = 0; (i < length) && (clean[i] != null); i++) {
            b.append(clean[i]);
            if ((i+1 < length) && (clean[i+1] != null)) b.append("/");
        }

        return b.toString();
    }

    /**
     * Remove parameters from a uri.
     *
     * @param uri The uri path to deparameterize.
     * @param parameters The map that collects parameters.
     * @return The cleaned uri
     */
    public static String deparameterize(String uri, Map parameters) {
        int i = uri.lastIndexOf('?');
        if (i == -1) return uri;
        String[] params = StringUtils.split(uri.substring(i+1), "&");
        for (int j = 0; j < params.length; j++) {
            String p = params[j];
            int k = p.indexOf('=');
            if (k == -1) break;
            String name = p.substring(0, k);
            String value = p.substring(k+1);
            parameters.put(name, value);
        }
        return uri.substring(0, i);
    }

    public static String parameterize(String uri, Map parameters) {
        if (parameters.size() == 0) {
            return uri;
        }
        StringBuffer buffer = new StringBuffer(uri);
        buffer.append('?');
        for (Iterator i = parameters.entrySet().iterator(); i.hasNext();) {
            Map.Entry entry = (Map.Entry)i.next();
            buffer.append(entry.getKey());
            buffer.append('=');
            buffer.append(entry.getValue());
            if (i.hasNext()) {
                buffer.append('&');
            }
        }
        return buffer.toString();
    }
}
