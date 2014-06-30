/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.matching;

import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.sitemap.PatternException;

import java.util.Map;

/**
 * A matcher that can prepare patterns during sitemap setup for faster match at request time.
 * This is also a regular matcher, meaning the sitemap can decide either to prepare the pattern
 * or to match with a request-time evaluated pattern (for {..} substitution).
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/07 12:39:14 $
 */
public interface PreparableMatcher extends Matcher {

    /**
     * Prepares a pattern in a form that allows faster match. For example, a regular
     * expression matcher can precompile the expression and return the corresponding
     * object. This method is called once for each pattern used with a particular matcher
     * class. The returned value is then passed back as the <code>preparedPattern</code>
     * parameter of {@link #preparedMatch(Object, Map, Parameters)}.
     *
     * @param pattern The pattern to prepare. Depending on the implementation the pattern
     *                can contain wildcards or regular expressions.
     * @return an optimized representation of the pattern.
     * @throws a <code>PatternException</code> if the pattern couldn't be prepared.
     */
    Object preparePattern(String pattern) throws PatternException;

    /**
     * Matches the prepared pattern against some values in the object model (most often the
     * <code>Request</code>) and returns a <code>Map</code> object with replacements
     * for wildcards contained in the pattern.
     *
     * @param preparedPattern The preparedPattern to match against, as returned by {@link #preparePattern(String)}.
     * @param objectModel     The <code>Map</code> with objects of the calling environment
     *                        which can be used to select values this matchers matches against.
     * @return                a <code>Map</code> object with replacements for wildcards/regular-expressions
     *                        contained in the pattern. If the return value is null there was no match.
     */
    Map preparedMatch(Object preparedPattern, Map objectModel, Parameters parameters);
}



