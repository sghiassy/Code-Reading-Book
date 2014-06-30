/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Response;
import org.apache.cocoon.environment.SourceResolver;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/**
 * This action adds HTTP headers to the response.
 *
 * @author <a href="mailto:balld@apache.org">Donald Ball</a>
 * @version CVS $Revision:
 */
public class HttpHeaderAction extends AbstractConfigurableAction implements ThreadSafe, Configurable {

    private static Object[] defaults = {};
    
    public void configure(Configuration conf) throws ConfigurationException {
        if (conf != null) {
	    String[] names = {};
	    super.configure(conf);
	    defaults=(settings.keySet()).toArray();
	};
    }

    public Map act(Redirector redirector, SourceResolver resolver, Map objectModel, String source, Parameters parameters) throws Exception {
        Map results = new HashMap();
	HashMap isDone = new HashMap();
	Integer dummy = new Integer(1);

        Response response = (Response)objectModel.get(Constants.RESPONSE_OBJECT);
        String[] names = parameters.getNames();

	// parameters
        for (int i=0; i<names.length; i++) {
	    isDone.put(names[i], dummy);
            response.setHeader(names[i],parameters.getParameter(names[i]));
            results.put(names[i],parameters.getParameter(names[i]));
        }
	// defaults, that are not overridden
        for (int i=0; i<defaults.length; i++) {
	    if (! isDone.containsKey(defaults[i])) {
		response.setHeader((String) defaults[i], (String) settings.get(defaults[i]));
		results.put((String) defaults[i],(String) settings.get(defaults[i]));
	    }
        }

        return Collections.unmodifiableMap(results);
    }

}
