/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import org.apache.avalon.framework.logger.Loggable;
import org.apache.cocoon.util.ClassUtils;
import org.apache.log.Logger;
import org.w3c.dom.NodeList;

import java.util.HashMap;

/**
 * This class is used as a XSLT extension class. It is used by the sitemap
 * generation stylesheet to load <code>MatcherFactory</code>s or
 * <code>SelectorFactory</code>s to get the generated source code.
 *
 * <strong>Note:</strong> This class uses a static log instance to
 * set up the instances it creates. This is suboptimal.
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/14 02:58:35 $
 */
public class XSLTFactoryLoader {
    protected static Logger log;
    private HashMap obj = new HashMap();

    public String getClassSource(String className, String prefix, String pattern, NodeList conf) throws ClassNotFoundException,
        InstantiationException, IllegalAccessException, Exception {
            
        throw new UnsupportedOperationException("CodeFactory is no longer supported.");
    }

    public static void setLogger(Logger logger) {
        if (log == null) {
            log = logger;
        }
    }

    public String getParameterSource(String className, NodeList conf) throws ClassNotFoundException, InstantiationException,
        IllegalAccessException, Exception {
            
        throw new UnsupportedOperationException("CodeFactory is no longer supported.");
    }

    public String getMethodSource(String className, NodeList conf) throws ClassNotFoundException, InstantiationException,
        IllegalAccessException, Exception {
            
        throw new UnsupportedOperationException("CodeFactory is no longer supported.");
    }

    public boolean isFactory(String className) {
        
        throw new UnsupportedOperationException("Factories are no longer supported.");
    }

    /**
     * Escapes '"' and '\' characters in a String (add a '\' before them) so that it can
     * be inserted in java source.
     */
    public String escape(String string) {
        if (string.indexOf('\\') == -1 && string.indexOf('"') == -1) {
            // Nothing to escape
            return string;
        }

        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < string.length(); i++) {
            char ch = string.charAt(i);
            if (ch == '\\' || ch == '"') {
                buf.append('\\');
            }
            buf.append(ch);
        }
        return buf.toString();
    }

    /**
     * Escapes like {@link #escape(String)} after having removed any '\' preceding a '{'.
     * This is used to insert a pattern with escaped subsitution syntax in Java source.
     */
    public String escapeBraces(String string) {
        if (string.indexOf("\\{") == -1)
        {
            return escape(string);
        }

        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < string.length(); i++) {
            char ch = string.charAt(i);
            if (ch != '\\' || i >= (string.length() - 1) || string.charAt(i+1) != '{') {
                buf.append(ch);
            }
        }
        return escape(buf.toString());
    }

    public boolean hasSubstitutions(String pattern) {
        if (pattern.length() == 0) {
            return false;
        }
        // Does it start by a substitution ?
        if (pattern.charAt(0) == '{') {
            return true;
        }

        // Search for an unescaped '{'
        int i = 1;
        while ((i = pattern.indexOf('{', i)) != -1) {
            if (pattern.charAt(i-1) != '\\') {
                return true;
            }
            i++; // Pass '{'
        }

        return false;
    }
}
