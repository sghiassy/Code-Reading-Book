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
 * Matches a request header (e.g. "referer") against a wildcard expression.
 *
 * <p><b>Global and local configuration</b></p>
 * <table border="1">
 * <tr><td><code>header-name</code></td><td>Name of the request header to
 * match against</td></tr>
 * </table>
 *
 * @author <a href="mailto:haul@informatik.tu-darmstadt.de">Christian Haul</a>
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/12 01:14:33 $
 */
public class WildcardHeaderMatcher extends AbstractWildcardMatcher
    implements Configurable
{
    private String defaultParam;

    public void configure(Configuration config) throws ConfigurationException {
        // Check old name
        this.defaultParam = config.getChild("parameter-name").getValue(null);
        if (defaultParam != null) {
            getLogger().warn("'parameter-name' is deprecated. Please use 'header-name'");
        }
        // Load with new one
        this.defaultParam = config.getChild("header-name").getValue(this.defaultParam);
    }

    protected String getMatchString(Map objectModel, Parameters parameters) {
        
        // Check old name
        String paramName = parameters.getParameter("parameter-name", null);
        if (paramName != null) {
            getLogger().warn("'parameter-name' is deprecated. Please use 'header-name'");
        } else {
            paramName = this.defaultParam;
        }
        
        // Load with new one.
        paramName = parameters.getParameter("header-name", paramName);
        
        if (paramName == null) {
            getLogger().warn("No header name given. FAILING");
            return null;
        }
        
        String result = ObjectModelHelper.getRequest(objectModel).getHeader(paramName);
        if (result == null) {
            getLogger().debug("Header '" + paramName + "' not set.");
        }
        
        return result;
    }
}
