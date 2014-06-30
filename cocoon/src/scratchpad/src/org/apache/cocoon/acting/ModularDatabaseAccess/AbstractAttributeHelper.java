/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting.ModularDatabaseAccess;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.AbstractLoggable;

import org.apache.cocoon.util.HashMap;

/**
 * AbstractDatabaseHelper gives you the infrastructure for easily
 * deploying more Attributehelpers.  In order to get at the
 * Logger, use getLogger().
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:35 $ */
public abstract class AbstractAttributeHelper extends AbstractLoggable
    implements AttributeHelper, Configurable, Disposable { 

    /**
     * Stores (global) configuration parameters as <code>key</code> /
     * <code>value</code> pairs.
     */
    protected HashMap settings = null;

    /**
     * Configures the database access helper.
     *
     * Takes all elements nested in component declaration and stores
     * them as key-value pairs in <code>settings</code>. Nested
     * configuration option are not catered for. This way global
     * configuration options can be used.
     *
     * For nested configurations override this function.  
     * */
    public void configure(Configuration conf) throws ConfigurationException {
        
        if (conf != null) {
            String key = null;
            String val = null;
            Configuration[] parameters = conf.getChildren();
            this.settings = new HashMap(parameters.length);
            for ( int i = 0; i < parameters.length; i++) {
                key = parameters[i].getName();
                val = parameters[i].getValue();
                if ( key != null )
                    this.settings.put(key, val);
            }
        }
    }

    /**
     *  dispose
     */
    public void dispose() {
        // Purposely empty so that we don't need to implement it in every
        // class.
    }
}
