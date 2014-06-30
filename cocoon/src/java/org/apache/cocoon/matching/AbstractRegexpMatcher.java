/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.matching;

import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.environment.ObjectModelHelper;
import org.apache.cocoon.sitemap.PatternException;

import org.apache.regexp.RE;
import org.apache.regexp.RECompiler;
import org.apache.regexp.REProgram;
import org.apache.regexp.RESyntaxException;

import java.util.HashMap;
import java.util.Map;

/**
 * Base class for all matchers using a regular expression pattern.
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @authos <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/11 16:27:53 $
 */

public abstract class AbstractRegexpMatcher extends AbstractPreparableMatcher implements ThreadSafe {

    /**
     * Compile the pattern in a <code>org.apache.regexp.REProgram</code>.
     */
    public Object preparePattern(String pattern) throws PatternException {
        if (pattern == null)
        {
            throw new PatternException("null passed as a pattern", null);
        }

        if (pattern.length() == 0)
        {
            pattern = "^$";
            if (getLogger().isWarnEnabled()) {
                getLogger().warn("The empty pattern string was rewritten to '^$'"
                                 + " to match for empty strings.  If you intended"
                                 + " to match all strings, please change your"
                                 + " pattern to '.*'");
            }
        }

        try {
            RECompiler compiler = new RECompiler();
            REProgram program = compiler.compile(pattern);
            return program;

        } catch (RESyntaxException rse) {
            getLogger().debug("Failed to compile the pattern '" + pattern + "'", rse);
            throw new PatternException(rse.getMessage(), rse);
        }
    }

    /**
     * Match the prepared pattern against the value returned by {@link #getMatchString(Map, Parameters)}.
     */
    public Map preparedMatch(Object preparedPattern, Map objectModel, Parameters parameters) {

        RE re = new RE((REProgram)preparedPattern);
        String match = getMatchString(objectModel, parameters);

        if (match == null)
            return null;

        if(re.match(match)) {
            /* Handle parenthesised subexpressions. XXX: could be faster if we count
             * parens *outside* the generated code.
             * Note: *ONE* based, not zero.
             */
            int parenCount = re.getParenCount();
            Map map = new HashMap();
            for (int paren = 1; paren <= parenCount; paren++) {
                map.put(Integer.toString(paren), re.getParen(paren));
            }

            return map;
        }

        return null;
    }

    /**
     * Get the string to test against the regular expression. To be defined
     * by concrete subclasses.
     */
    protected abstract String getMatchString(Map objectModel, Parameters parameters);
}
