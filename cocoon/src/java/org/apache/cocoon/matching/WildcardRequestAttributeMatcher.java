/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.matching;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.environment.ObjectModelHelper;

import java.util.Map;

/**
 * Matches a request attribute against a wildcard expression.
 *
 * <p><b>Global and local configuration</b></p>
 * <tableborder="1">
 * <tr><td><code>attribute-name</code></td><td>String identifying the request attribute</td></tr>
 * </table>
 *
 * @author <a href="mailto:haul@informatik.tu-darmstadt.de">Christian Haul</a>
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 18:40:08 $
 */
public class WildcardRequestAttributeMatcher extends AbstractWildcardMatcher
    implements Configurable
{
    private String defaultParam;

    public void configure(Configuration config) throws ConfigurationException {
        this.defaultParam = config.getChild("attribute-name").getValue(null);
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Default attribute-name is = '" + this.defaultParam + "'");
        }
    }

    protected String getMatchString(Map objectModel, Parameters parameters) {
        
        String paramName = parameters.getParameter("attribute-name", this.defaultParam);
        if (paramName == null) {
            getLogger().warn("No attribute name given. FAILING");
            return null;
        }
        
        Object result = ObjectModelHelper.getRequest(objectModel).getAttribute(paramName);
        if (result == null) {
            getLogger().debug("Request attribute '" + paramName + "' not set.");
            return null;
        }
        
        return result.toString();
    }
}
