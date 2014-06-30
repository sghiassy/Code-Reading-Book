/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import org.apache.avalon.excalibur.component.ExcaliburComponentSelector;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

import org.apache.cocoon.components.pipeline.OutputComponentSelector;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/** Default component manager for Cocoon's sitemap components.
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @author <a href="mailto:giacomo@apache.org">Giacomo Pati</a>
 * @version CVS $Id: SitemapComponentSelector.java,v 1.3 2002/01/22 00:17:13 vgritsenko Exp $
 */
public class SitemapComponentSelector extends ExcaliburComponentSelector
  implements OutputComponentSelector {
    private Map mime_types;
    private SitemapComponentSelector parentSelector;

    /** Dynamic component handlers mapping. */
    private Map componentMapping;

    /** The conctructors (same as the Avalon ComponentManager)
     */
    public SitemapComponentSelector() {
        super();
        this.mime_types = new HashMap();
        componentMapping = Collections.synchronizedMap(new HashMap());
    }

    public void setParentSelector(SitemapComponentSelector newSelector) {
        if (this.parentSelector == null) {
            this.parentSelector = newSelector;
        }
    }

    public Component select(Object hint) throws ComponentException {
        Component component = null;

        try {
            component = super.select(hint);
        } catch (ComponentException ce) {
            if (this.parentSelector != null) {
                component = this.parentSelector.select(hint);
                componentMapping.put(component, this.parentSelector);
            } else {
                throw ce;
            }
        }

        return component;
    }

    public void release(Component component) {
        SitemapComponentSelector selector = (SitemapComponentSelector)componentMapping.get(component);
        if(selector != null) {
            componentMapping.remove(component);
            selector.release(component);
        } else {
            super.release(component);
        }
    }

    public void initialize() {
        super.initialize();
        this.mime_types = Collections.unmodifiableMap(this.mime_types);
    }
        
    public String getMimeTypeForHint(Object hint) {
        if (this.mime_types.containsKey(hint)) {
            return (String)this.mime_types.get(hint);
        }
        if (this.parentSelector != null) {
            return this.parentSelector.getMimeTypeForHint(hint);
        }
        return null;
    }

    public void addSitemapComponent(Object hint, Class component, Configuration conf, String mime_type) throws ComponentException,
        ConfigurationException {
            super.addComponent(hint, component, conf);
            this.mime_types.put(hint, mime_type);
    }
}
