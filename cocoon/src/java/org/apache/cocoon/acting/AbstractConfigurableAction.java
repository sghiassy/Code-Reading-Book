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
import org.apache.cocoon.util.HashMap;

/**
 * AbstractConfigurableAction gives you the infrastructure for easily
 * deploying more Actions that take default parameters.
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $ */
public abstract class AbstractConfigurableAction extends AbstractAction implements Configurable {

    /**
     * Stores (global) configuration parameters as <code>key</code> /
     * <code>value</code> pairs.
     */
    protected HashMap settings = null;

    /**
     * Configures the Action.
     *
     * Takes the children from the <code>Configuration</code> and stores them
     * them as key (configuration name) and value (configuration value)
     * in <code>settings</code>.
     * <br/>
     * This automates parsing of flat string-only configurations.
     * For nested configurations, override this function in your action.
     */
    public void configure(Configuration conf) throws ConfigurationException {
        if (conf != null) {
    	    String key = null;
    	    String val = null;
    	    Configuration[] parameters = conf.getChildren();
    	    this.settings = new HashMap(parameters.length);
    	    for ( int i = 0; i < parameters.length; i++) {
    		    key = parameters[i].getName();
    		    val = parameters[i].getValue(null);
    		    if ( key != null )
    		        this.settings.put(key, val);
    	    }
    	}
    }

}
