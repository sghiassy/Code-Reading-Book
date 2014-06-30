/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.selection;

import org.apache.avalon.framework.component.Component;
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
 * A <code>Selector</code> that matches a string from within the host parameter
 * of the HTTP request.
 *
 *  <map:selector name="host" src="org.apache.cocoon.selection.HostSelectorFactory">
 *       <host name="uk-site" value="www.foo.co.uk"/>
 *  </map:selector>
 *
 *
 *   <map:select type="host">
 *      <map:when test="uk-site">
 *            <map:transform src="stylesheets/page/uk.xsl"/>
 *      </map:when>
 *      <map:otherwise>
 *     <map:transform src="stylesheets/page/us.xsl"/>
 *       </map:otherwise>
 *   </map:select>
 *
 * @author <a href="mailto:cbritton@centervilletech.com">Colin Britton</a>
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:19 $
 */

public class HostSelector extends NamedPatternsSelector {

    public void configure(Configuration conf) throws ConfigurationException {
        configure(conf, "host", "name", "value");
    }

    public boolean select(String expression, Map objectModel, Parameters parameters) {
        // Inform proxies that response varies with the Host header
        ObjectModelHelper.getResponse(objectModel).addHeader("Vary", "Host");
        
        // Get the host request header
        String host = ObjectModelHelper.getRequest(objectModel).getHeader("Host");
        if (host == null) {
            getLogger().debug("No Host header -- failing.");
            return false;
        }
        
        return checkPatterns(expression, host);
    }
}
