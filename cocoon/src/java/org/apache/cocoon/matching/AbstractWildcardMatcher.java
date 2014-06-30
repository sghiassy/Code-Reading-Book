/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.matching;

import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.environment.ObjectModelHelper;
import org.apache.cocoon.matching.helpers.WildcardHelper;

import java.util.HashMap;
import java.util.Map;


/**
 * Base class for wildcard matchers
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:17 $
 */

public abstract class AbstractWildcardMatcher extends AbstractPreparableMatcher implements ThreadSafe {

    /**
     * Compile the pattern in an <code>int[]</code>.
     */
    public Object preparePattern(String pattern) {
        return WildcardHelper.compilePattern(pattern);
    }
    
    /**
     * Match the prepared pattern against the result of {@link #getMatchString(Map, Parameters)}.
     */
    public Map preparedMatch(Object preparedPattern, Map objectModel, Parameters parameters) {
        
        String match = getMatchString(objectModel, parameters);
        
        if (match == null) {
            return null;
        }
        
        HashMap map = new HashMap();
        
        if (WildcardHelper.match(map, match, (int[])preparedPattern)) {
            return map;
        } else {
            return null;
        }
    }
    
    /**
     * Get the string to test against the wildcard expression. To be defined
     * by concrete subclasses.
     */
    protected abstract String getMatchString(Map objectModel, Parameters parameters);
}
