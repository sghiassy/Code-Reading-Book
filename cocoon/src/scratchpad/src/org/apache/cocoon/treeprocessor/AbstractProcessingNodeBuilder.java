/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.framework.logger.AbstractLoggable;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentException;

import org.apache.cocoon.sitemap.PatternException;

import java.util.Map;
import java.util.HashMap;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */


public abstract class AbstractProcessingNodeBuilder extends AbstractLoggable
  implements ProcessingNodeBuilder {

    protected TreeBuilder treeBuilder;
    
    public void setBuilder(TreeBuilder treeBuilder) {
        this.treeBuilder = treeBuilder;
    }

    /**
     * Does this node accept parameters ? Default is true : if a builder that doesn't
     * have parameters doesn't override this method, erroneous parameters will be silently
     * ignored.
     */
    protected boolean hasParameters() {
        return true;
    }

    /**
     * Get &lt;xxx:parameter&gt; elements as a <code>Map</code> of </code>ListOfMapResolver</code>s,
     * that can be turned into parameters using <code>ListOfMapResolver.buildParameters()</code>.
     *
     * @return the Map of ListOfMapResolver, or <code>null</code> if there are no parameters.
     */
    protected Map getParameters(Configuration config) throws ConfigurationException {
        Configuration[] children = config.getChildren("parameter");
        
        if (children.length == 0) {
            return null;
        }
        
        Map params = new HashMap();
        for (int i = 0; i < children.length; i++) {
            Configuration child = children[i];
            if (true) { // FIXME : check namespace
                String value = child.getAttribute("value");
                try {
                    params.put(child.getAttribute("name"), MapStackResolver.getResolver(value));
                } catch(PatternException pe) {
                    String msg = "Invalid pattern '" + value + " at " + child.getLocation();
                    getLogger().error(msg, pe);
                    throw new ConfigurationException(msg, pe);
                }
            }
        }
        
        return params;
    }

    /**
     * Check if the namespace URI of the given configuraition is the same as the
     * one given by the builder.
     */
    protected void checkNamespace(Configuration config) throws ConfigurationException {
        if (!this.treeBuilder.getNamespace().equals(config.getNamespace()))
        {
            String msg = "Invalid namespace '" + config.getNamespace() + "' at " + config.getLocation();
            getLogger().error(msg);
            throw new ConfigurationException(msg);
        }
    }
}
