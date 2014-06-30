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

import java.util.HashMap;
import java.util.Map;

/**
 * This class allows for matching based on a request attribute.
 * If the specified request attribute exists, its string representation
 * is retrieved for later xpath substitution.
 *
 * <p><b>Example:</b></p>
 * <pre>
 * &lt;map:match type="request-attribute" pattern="next"&gt;
 *     &lt;map:redirect-to src="{1}"/&gt;
 * &lt;/map:match&gt;
 * </pre>
 *
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $
 */
public class RequestAttributeMatcher implements Matcher, ThreadSafe
{
    /**
     * Match method to see if the request attribute exists. If it does
     * have a value the string represenation of attribute is added to 
     * the array list for later substitution with a xpath attribute.
     *
     * @param pattern name of request attribute to find
     * @param objectModel environment passed through via cocoon
     * @returns null or map containing value of request attribute 'pattern'
     */
    public Map match(String pattern, Map objectModel, Parameters parameters) {

        Object attribute = ObjectModelHelper.getRequest(objectModel).getAttribute(pattern);

        if (attribute == null) {
            return null;
        } else {
            Map map = new HashMap();
            map.put("1", attribute.toString());
            return map;
        }
    }
}
