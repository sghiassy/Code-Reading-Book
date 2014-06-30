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
 * A <code>Selector</code> that matches a string against a configurable request
 * attribute's string represenation.
 *
 * <p><b>Global and local configuration</b></p>
 * <table border="1">
 * <tr><td><code>attribute-name</code></td><td>String identifying the request attribute.</td></tr>
 * </table>
 *
 * @author <a href="mailto:haul@informatik.tu-darmstadt.de">Christian Haul</a>
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/17 04:05:23 $
 */
public class RequestAttributeSelector extends AbstractLoggable
  implements Configurable, ThreadSafe, Selector {

    protected String defaultName;

    public void configure(Configuration config) throws ConfigurationException {
        this.defaultName = config.getChild("attribute-name").getValue(null);
    }

    public boolean select(String expression, Map objectModel, Parameters parameters) {
        String name = parameters.getParameter("attribute-name", this.defaultName);

        if (name == null) {
            getLogger().warn("No attribute name given -- failing.");
            return false;
        }

        Object value = ObjectModelHelper.getRequest(objectModel).getAttribute(name);
        if (value == null) {
            getLogger().debug("Request attribute '" + name + "' not set -- failing.");
            return false;
        }

        return value.toString().equals(expression);
    }
}
