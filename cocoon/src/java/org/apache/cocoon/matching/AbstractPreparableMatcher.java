/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.matching;

import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.sitemap.PatternException;

import java.util.Map;

/**
 * A matcher that can prepare patterns during sitemap setup for faster match at request time.
 * This is also a regular matcher, meaning the sitemap can decide either to prepare the pattern
 * or to match with a request-time evaluated pattern (for {..} substitution).
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:17 $
 */
public abstract class AbstractPreparableMatcher extends AbstractLoggable implements PreparableMatcher {

    /**
     * Match the pattern by preparing it and matching the prepared pattern.
     */
    public Map match (String pattern, Map objectModel, Parameters parameters)
      throws PatternException {
        return preparedMatch(preparePattern(pattern), objectModel, parameters);
    }
}
