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
 * Matches a request header (e.g. "referer") against a regular expression.
 *
 * <p><b>Global and local configuration</b></p>
 * <table border="1">
 * <tr><td><code>header-name</code></td><td>Name of the request header to
 * match against</td></tr>
 * </table>
 *
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/12 01:14:33 $
 */

public class RegexpHeaderMatcher extends AbstractRegexpMatcher 
    implements Configurable
{
    private String defaultParam;

    public void configure(Configuration config) throws ConfigurationException {

        this.defaultParam = config.getChild("header-name").getValue(null);
    }

    protected String getMatchString(Map objectModel, Parameters parameters) {
        
        String paramName = parameters.getParameter("header-name", this.defaultParam);
        if (paramName == null) {
            getLogger().warn("No header name given. FAILING");
            return null;
        }
        
        String result = ObjectModelHelper.getRequest(objectModel).getHeader(paramName);
        if (result == null) {
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("Header '" + paramName + "' not set.");
            }
        }
        
        return result;
    }
}
