/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.sitemap.PatternException;

import java.util.*;

/**
 * Utility class for handling {...} pattern substitutions from a List of Maps.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.7 $ $Date: 2002/01/30 14:35:07 $
 */

public abstract class MapStackResolver {

    public static final Map EMPTY_MAP = Collections.unmodifiableMap(new java.util.HashMap(0));

    /**
     * Resolve all {...} patterns using the values given in the list of maps.
     */
    public abstract String resolve(List mapStack) throws PatternException;

    /**
     * Does an expression need resolving (i.e. contain {...} patterns) ?
     */
    public static boolean needsResolve(String expression) {
        if (expression == null || expression.length() == 0) {
            return false;
        }

        // Is the first char a '{' ?
        if (expression.charAt(0) == '{') {
            return true;
        }

        if (expression.length() < 2) {
            return false;
        }

        // Is there any unescaped '{' ?
        int pos = 1;
        while ( (pos = expression.indexOf('{', pos)) != -1) {
            // Found a '{' : is it escaped ?
            if (expression.charAt(pos - 1) != '\\') {
                // No : need to resolve
                return true;
            }
            pos++;
        }
        // Nothing found...
        return false;
    }

    /**
     * Unescape an expression that doesn't need to be resolved, but may contain
     * escaped '{' characters.
     *
     * @param expression the expression to unescape.
     * @return the unescaped result, or <code>expression</code> if unescaping isn't necessary.
     */
    public static String unescape(String expression) {
        // Does it need escaping ?
        if (expression == null || expression.indexOf("\\{") == -1) {
            return expression;
        }

        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < expression.length(); i++) {
            char ch = expression.charAt(i);
            if (ch != '\\' || i >= (expression.length() - 1) || expression.charAt(i+1) != '{') {
                buf.append(ch);
            }
        }

        return buf.toString();
    }

    /**
     * Get a resolver for a given expression. Chooses the most efficient implementation
     * depending on <code>expression</code>.
     */
    public static MapStackResolver getResolver(String expression) throws PatternException {
        if (needsResolve(expression)) {
//            return new RealResolver(expression);
            return new CompiledResolver(expression);
        } else {
            return new NullResolver(expression);
        }
    }

    /**
     * Build a <code>Parameters</code> object from a Map of named <code>ListOfMapResolver</code>s and
     * a list of Maps used for resolution.
     *
     * @return a fully resolved <code>Parameters</code>.
     */
    public static Parameters buildParameters(Map expressions, List mapStack) throws PatternException {
        if (expressions == null || expressions.size() == 0) {
            return Parameters.EMPTY_PARAMETERS;
        }

        Parameters result = new Parameters();

        Iterator iter = expressions.entrySet().iterator();
        while (iter.hasNext()) {
            Map.Entry entry = (Map.Entry)iter.next();
            String value = ((MapStackResolver)entry.getValue()).resolve(mapStack);
            result.setParameter((String)entry.getKey(), value);
        }

        return result;
    }

    /**
     * Resolve all values of a <code>Map</code> from a Map of named <code>ListOfMapResolver</code>s and
     * a list of Maps used for resolution.
     *
     * @return a fully resolved <code>Map</code>.
     */
    public static Map resolveMap(Map expressions, List mapStack) throws PatternException {
        int size;
        if (expressions == null || (size = expressions.size()) == 0) {
            return EMPTY_MAP;
        }

        Map result = new HashMap(size);

        Iterator iter = expressions.entrySet().iterator();
        while (iter.hasNext()) {
            Map.Entry entry = (Map.Entry)iter.next();
            String value = ((MapStackResolver)entry.getValue()).resolve(mapStack);
            result.put(entry.getKey(), value);
        }

        return result;
    }

    //-------------------------------------------------------------------------
    /**
     * No-op resolver for expressions that don't need to be resolved.
     */
    private static class NullResolver extends MapStackResolver {
        private String originalExpr = null;
        private String expression = null;

        public NullResolver(String expression) {
            if (expression != null) {
                this.originalExpr = expression;
                this.expression = this.unescape(expression);
            }
        }

        public String toString() {
            return this.originalExpr;
        }

        public final String resolve(List mapStack) {
            return this.expression;
        }
    }

    //-------------------------------------------------------------------------

    /**
     * Compiled form for faster substitution
     */
    private static class CompiledResolver extends MapStackResolver {
        private String originalExpr;

        private String[] strings;
        private int[] levels;

        public CompiledResolver(String expression) throws PatternException {
            this.originalExpr = expression;
            compile(expression);
        }

        public String toString() {
            return this.originalExpr;
        }

        private void compile(String expr) throws PatternException {
            // We're sure here that expr *contains* some substitutions

            List stringList = new ArrayList();
            List levelList  = new ArrayList();

            int length = expr.length();
            int prev = 0; // position after last closing brace

            comp : while(prev < length) {
                // find next unescaped '{'
                int pos = prev;
                while(pos < length &&
                      (pos = expr.indexOf('{', pos)) != -1 &&
                      (pos != 0 && expr.charAt(pos - 1) == '\\')) {
                    pos++;
                }

                if (pos >= length || pos == -1) {
                    // no more braces
                    if (prev < length - 1) {
                        stringList.add(this.unescape(expr.substring(prev)));
                        levelList.add(new Integer(-1));
                    }
                    break comp;
                }

                // Pass closing brace
                pos++;

                // Add litteral strings between closing and next opening brace
                if (prev < pos-1) {
                    stringList.add(this.unescape(expr.substring(prev, pos - 1)));
                    levelList.add(new Integer(-1));
                }

                // Determine subst level
                int level = 1; // Start at 1 since it will be substracted from list.size()
                while(expr.startsWith("../", pos)) {
                    level++;
                    pos += "../".length();
                }

                int end = expr.indexOf('}', pos);
                if (end == -1) {
                    throw new PatternException("Unmatched '{' in " + expr);
                }

                stringList.add(expr.substring(pos, end));
                levelList.add(new Integer(level));

                prev = end + 1;
            }

            this.strings = new String[stringList.size()];
            this.levels = new int[stringList.size()];
            for (int i = 0; i < strings.length; i++) {
                this.strings[i] = (String)stringList.get(i);
                this.levels[i] = ((Integer)levelList.get(i)).intValue();
            }
        }

        public final String resolve(List mapStack) throws PatternException {
            StringBuffer result = new StringBuffer();
            int stackSize = mapStack.size();

            for (int i = 0; i < this.strings.length; i++) {
                int level = this.levels[i];
                if (level == -1) {
                    result.append(this.strings[i]);

                } else {
                    if (level > stackSize) {
                        throw new PatternException("Error while evaluating '" + this.originalExpr +
                            "' : not so many levels");
                    }

                    Object value = ((Map)mapStack.get(stackSize - level)).get(this.strings[i]);
                    if (value != null) {
                        result.append(value);
                    }
                }
            }

            return result.toString();
        }

//        public void dump() {
//            System.out.println(this.originalExpr + " compiled in :");
//            for (int i = 0; i < this.strings.length; i++) {
//                System.out.print("[" + this.levels[i] + ":'" + this.strings[i] + "'] ");
//            }
//            System.out.println();
//            System.out.println();
//        }
    }

//    public static void main(String [] args) throws Exception {
//
//        new CompiledResolver("&{../../blah}").dump();
//        new CompiledResolver("{t1}tt{t2}").dump();
//        new CompiledResolver("\\{t1}tt{t2}xx").dump();
//        new CompiledResolver("{t1}tt\\{t2}xx").dump();
//        new CompiledResolver("{t1}tt{t2}xx").dump();
//        new CompiledResolver("xx{../t1}{../../../t2}zz").dump();
//        new CompiledResolver("xx{../t1}\\{../../../t2}zz").dump();
//
//    }
}
