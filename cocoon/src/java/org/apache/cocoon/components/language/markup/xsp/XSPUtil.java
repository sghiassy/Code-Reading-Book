/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup.xsp;

import org.apache.cocoon.Constants;
import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.components.url.URLFactory;
import org.apache.cocoon.environment.Context;
import org.apache.cocoon.environment.Session;
import org.apache.cocoon.util.NetUtils;
import org.apache.cocoon.util.Tokenizer;
import org.apache.cocoon.xml.IncludeXMLConsumer;
import org.xml.sax.ContentHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import java.io.*;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Map;

/**
 * The XSP <code>Utility</code> object helper
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @author <a href="mailto:bloritsch@apache.org>Berin Loritsch</a>
 * @version $Revision: 1.1 $ $Date: 2002/01/03 12:31:10 $
 */
public class XSPUtil {
    public static String pathComponent(String filename) {
        int i = filename.lastIndexOf(File.separator);
        return (i >= 0) ? filename.substring(0, i) : filename;
    }

    public static String fileComponent(String filename) {
        int i = filename.lastIndexOf(File.separator);
        return (i >= 0) ? filename.substring(i + 1) : filename;
    }

    public static String baseName(String filename) {
        return baseName(filename, ".");
    }

    public static String baseName(String filename, String suffix) {
        int lastDot = filename.lastIndexOf(suffix);
        if (lastDot >= 0) {
            filename = filename.substring(0, lastDot);
        }
        return filename;
    }

    public static String normalizedBaseName(String filename) {
        filename = baseName(filename);
        return normalizedName(filename);
    }

    public static String normalizedName(String filename) {
        String[] path = split(filename, File.separator);
        int start = (path[0].length() == 0) ? 1 : 0;
        StringBuffer buffer = new StringBuffer();
        for (int i = start; i < path.length; i++) {
            if (i > start) {
                buffer.append(File.separator);
            }
            buffer.append('_');
            char[] chars = path[i].toCharArray();
            for (int j = 0; j < chars.length; j++) {
                if (isAlphaNumeric(chars[j])) {
                    buffer.append(chars[j]);
                } else {
                    buffer.append('_');
                }
            }
        }
        return buffer.toString();
    }

    public static String relativeFilename(String filename, Map objectModel)
        throws IOException {
            File file = new File(filename);
            if (file.isAbsolute() && file.exists()) {
                return filename;
            }
            Context context = (Context) objectModel.get(Constants.CONTEXT_OBJECT);
            return NetUtils.getPath(context.getResource(filename).toExternalForm());
    }

    public static boolean isAlphaNumeric(char c) {
        return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
    }

    public static String[] split(String line) {
        return split(line, " \t\r\f\n");
    }

    public static String[] split(String line, String delimiter) {
        Tokenizer tokenizer = new Tokenizer(line, delimiter);
        int tokenCount = tokenizer.countTokens();
        String[] result = new String[tokenCount];
        for (int i = 0; i < tokenCount; i++) {
            result[i] = tokenizer.nextToken();
        }
        return result;
    }

    public static void include(InputSource is, ContentHandler parentContentHandler, Parser parser)
        throws SAXException, IOException {
            parser.setConsumer(
                new IncludeXMLConsumer(parentContentHandler));
            parser.parse(is);
    }

    public static String encodeMarkup(String string) {
        char[] array = string.toCharArray();
        StringBuffer buffer = new StringBuffer();
        for (int i = 0; i < array.length; i++) {
            switch (array[i]) {
                case '<':
                    buffer.append("&lt;");
                    break;
                case '>':
                    buffer.append("&gt;");
                    break;
                case '&':
                    buffer.append("&amp;");
                    break;
                default:
                    buffer.append(array[i]);
                    break;
            }
        }
        return buffer.toString();
    }

    public static String formEncode(String text) throws Exception {
	return URLEncoder.encode (text);
    }

    // Shameless, ain't it?
    public static String formDecode(String s) throws Exception {
	return URLDecoder.decode (s);
    }

  /* Logicsheet Utility Methods */

    // Inclusion
    public static String getURLContents(String url, URLFactory urlFactory) throws IOException {
        return getContents(urlFactory.getURL(url).openStream());
    }

    public static String getURLContents(String url, String encoding, URLFactory urlFactory) throws IOException {
        return getContents(urlFactory.getURL(url).openStream(), encoding);
    }

    public static String getFileContents(String filename) throws IOException {
        return getContents(
            new BufferedReader(new FileReader(filename)));
    }

    public static String getFileContents(String filename, String encoding) throws IOException {
        return getContents(
            new BufferedInputStream(new FileInputStream(filename)), encoding);
    }

    public static String getContents(InputStream in, String encoding) throws IOException {
        return getContents(
            new BufferedReader(new InputStreamReader(in, encoding)));
    }

    public static String getContents(InputStream in) throws IOException {
        return getContents(
            new BufferedReader(new InputStreamReader(in)));
    }

    public static String getContents(Reader reader) throws IOException {
        int len;
        char[] chr = new char[4096];
        StringBuffer buffer = new StringBuffer();
        while ((len = reader.read(chr)) > 0) {
            buffer.append(chr, 0, len);
        }
        return buffer.toString();
    }

    // Date
    public static String formatDate(Date date, String pattern) {
        if (pattern == null || pattern.length() == 0) {
            pattern = "yyyy/MM/dd hh:mm:ss aa";
        }
        try {
            return (
                new SimpleDateFormat(pattern)).format(date);
        } catch (Exception e) {
            return date.toString();
        }
    }

    // Counters
    private static volatile int count = 0;

    public static int getCount() {
        return ++count;
    }

    public static int getSessionCount(Session session) {
        synchronized(session) {
	        Integer integer = (Integer)session.getAttribute("util.counter");
	        if (integer == null) {
	            integer = new Integer(0);
	        }
	        int cnt = integer.intValue() + 1;
	        session.setAttribute("util.counter",
	            new Integer(cnt));
	        return cnt;
        }
    }

    public static Object getContextAttribute(Map objectModel, String name)
    {
        Context context = (Context) objectModel.get(Constants.CONTEXT_OBJECT);
        return context.getAttribute(name);
    }
}
