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

import org.apache.cocoon.Constants;

import java.util.Map;

/**
 * This class generates source code to implement a selector that
 * matches a string against an arbitrary session attribute.
 *
 * <p><b>Global and local configuration</b></p>
 * <table border="1">
 * <tr><td><code>org.apache.cocoon.SessionState</code></td><td>String identifying the session attribute.</td></tr>
 * </table>
 *
 * @deprecated use SessionAttributeSelector instead
 *
 * @author <a href="mailto:haul@informatik.tu-darmstadt.de">Christian Haul</a>
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:19 $
 */
public class SessionStateSelector extends SessionAttributeSelector {

    public SessionStateSelector() {
        this.defaultName = Constants.SESSION_STATE_ATTRIBUTE;
    }

    public void configure(Configuration config)
    {
        // ignore
    }
}
