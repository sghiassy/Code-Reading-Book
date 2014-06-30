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
import org.apache.cocoon.environment.Request;

import java.util.HashMap;
import java.util.Map;

/**
 * This class allows for matching based on a request parameter.
 * If the specified request parameter exists, its value is retrieved for later
 * xpath substitution.
 *
 * <p><b>Example:</b></p>
 * <pre>
 * &lt;map:match type="request" pattern="dest"&gt;
 *     &lt;map:redirect-to uri="{1}"/&gt;
 * &lt;/map:match&gt;
 * </pre>
 *
 * @author <a href="mailto:Marcus.Crafter@osa.de">Marcus Crafter</a>
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.2 $
 */
public class RequestParameterMatcher implements Matcher, ThreadSafe
{
    /**
     * Match method to see if the request parameter exists. If it does
     * have a value the parameter is added to the array list for later
     * substitution with a xpath attribute.
     *
     * @param pattern name of request parameter to find
     * @param objectModel environment passed through via cocoon
     * @returns null or map containing value of request parameter 'pattern'
     */
    public Map match(String pattern, Map objectModel, Parameters parameters) {
        Request request = ObjectModelHelper.getRequest(objectModel);

        String parameter = request.getParameter(pattern);
        if (parameter == null) {
            return null; // no parameter defined
        } else {
            Map map = new HashMap();
            map.put("1", parameter);
            return map; // parameter defined, return map
        }
    }
}
