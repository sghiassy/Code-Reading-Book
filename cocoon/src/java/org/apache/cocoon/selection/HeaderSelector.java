/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.selection;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.environment.ObjectModelHelper;

import java.util.Map;

/**
 * A <code>Selector</code> that matches a string against a configurable
 * request header, e.g. "referer".
 *
 * <p><b>Global and local configuration</b></p>
 * <table border="1">
 * <tr><td><code>header-name</code></td><td>Name of the request header to
 * match against</td></tr>
 * </table>
 *
 * @author <a href="mailto:haul@informatik.tu-darmstadt.de">Christian Haul</a>
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:19 $
 */
public class HeaderSelector extends AbstractLoggable
  implements Configurable, ThreadSafe, Selector {

    protected String defaultName;
    
    public void configure(Configuration config) throws ConfigurationException {
        // Check old name
        this.defaultName = config.getChild("parameter-name").getValue(null);
        if (defaultName != null) {
            getLogger().warn("'parameter-name' is deprecated. Please use 'header-name'");
        }
        // Load with new one
        this.defaultName = config.getChild("header-name").getValue(this.defaultName);
    }

    public boolean select(String expression, Map objectModel, Parameters parameters) {
        // Check old name
        String name = parameters.getParameter("parameter-name", null);
        if (name != null) {
            getLogger().warn("'parameter-name' is deprecated. Please use 'header-name'");
        } else {
            name = this.defaultName;
        }
        
        // Load with new one.
        name = parameters.getParameter("header-name", name);
        
        if (name == null) {
            getLogger().warn("No header name given -- failing.");
            return false;
        }
        
        String value = ObjectModelHelper.getRequest(objectModel).getHeader(name);
        if (value == null) {
            getLogger().debug("Header '" + name + "' not set -- failing.");
            return false;
        }
        
        return value.equals(expression);
    }
}
