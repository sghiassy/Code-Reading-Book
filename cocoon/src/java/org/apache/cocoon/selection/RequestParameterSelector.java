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
import org.apache.cocoon.environment.Request;

import java.util.Map;

/**
 * A <code>Selector</code> that matches a string against a configurable request parameter's value.
 *
 * <p><b>Global and local configuration</b></p>
 * <table border="1">
 * <tr><td><code>parameter-name</code></td><td>Name of the request
 * parameter whose value to match against</td></tr>
 * </table>
 *
 * @author <a href="mailto:haul@informatik.tu-darmstadt.de">Christian Haul</a>
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/11 17:07:06 $
 */
public class RequestParameterSelector extends AbstractLoggable
  implements Configurable, ThreadSafe, Selector {

    protected String defaultName;
    
    public void configure(Configuration config) throws ConfigurationException {
        this.defaultName = config.getChild("parameter-name").getValue(null);
    }

    public boolean select(String expression, Map objectModel, Parameters parameters) {
        String name = parameters.getParameter("parameter-name", this.defaultName);
        
        if (name == null) {
            getLogger().warn("No parameter name given -- failing.");
            return false;
        }
        
        String value = ObjectModelHelper.getRequest(objectModel).getParameter(name);
        if (value == null) {
            getLogger().debug("Request parameter '" + name + "' not set -- failing.");
            return false;
        }
        
        return value.equals(expression);
    }
}
