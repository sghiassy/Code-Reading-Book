/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.CascadingRuntimeException;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.configuration.DefaultConfiguration;

import org.apache.cocoon.components.pipeline.OutputComponentSelector;
import org.apache.cocoon.components.ExtendedComponentSelector;

import org.apache.cocoon.acting.Action;
import org.apache.cocoon.generation.Generator;
import org.apache.cocoon.matching.Matcher;
import org.apache.cocoon.reading.Reader;
import org.apache.cocoon.selection.Selector;
import org.apache.cocoon.serialization.Serializer;
import org.apache.cocoon.transformation.Transformer;

import java.util.*;

/**
 * Component selector for sitemap components.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/15 11:10:54 $
 */

public class ComponentsSelector extends ExtendedComponentSelector implements OutputComponentSelector {
    
    public static final int UNKNOWN     = -1;
    public static final int GENERATOR   = 0;
    public static final int TRANSFORMER = 1;
    public static final int SERIALIZER  = 2;
    public static final int READER      = 3;
    public static final int MATCHER     = 4;
    public static final int SELECTOR    = 5;
    public static final int ACTION      = 6;
    
    public static final String[] SELECTOR_ROLES = {
        Generator.ROLE   + "Selector",
        Transformer.ROLE + "Selector",
        Serializer.ROLE  + "Selector",
        Reader.ROLE      + "Selector",
        Matcher.ROLE     + "Selector",
        Selector.ROLE    + "Selector",
        Action.ROLE      + "Selector"
    };
    
    public static final String[] COMPONENT_NAMES = {
        "generator",
        "transformer",
        "serializer",
        "reader",
        "matcher",
        "selector",
        "action"
    };

    /** The role as an integer */
    private int roleId;
    
    /** The mime-type for hints */
    private Map mimeTypes;
    
    /** The set of known hints, used to add standard components (see ensureExists) */
    private Set knownHints = new HashSet();
    
    /** The parent selector, if it's of the current class */
    private ComponentsSelector parentComponentsSelector;
    
    public void setParentSelector(ComponentSelector selector) {
        super.setParentSelector(selector);
        
        if (selector instanceof ComponentsSelector) {
            this.parentComponentsSelector = (ComponentsSelector)selector;
        }
    }
    
    /**
     * Return the component instance name according to the selector role
     * (e.g. "action" for "org.apache.cocoon.acting.Action").
     */
    protected String getComponentInstanceName() {
        return (this.roleId == UNKNOWN) ? null : COMPONENT_NAMES[this.roleId];
    }

    /**
     * Get the attribute for class names. This is "src" for known roles, and
     * "class" (the default) for other roles.
     */
    protected String getClassAttributeName() {
        return (this.roleId == UNKNOWN) ? "class" : "src";
    }
    

    public void configure(Configuration config) throws ConfigurationException {
        
        // How are we ?
        String role = getRoleName(config);
        this.roleId = UNKNOWN; // unknown
        for (int i = 0; i < SELECTOR_ROLES.length; i++) {
            if (SELECTOR_ROLES[i].equals(role)) {
                this.roleId = i;
                break;
            }
        }
        
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Setting up sitemap component selector for " +
                role + " (role id = " + this.roleId + ")");
        }
        
        // Only matchers and serializers can have a MIME type
        if (this.roleId == SERIALIZER || this.roleId == READER) {
            this.mimeTypes = new HashMap();
        }
        
        super.configure(config);
    }

    /**
     * Add a component in this selector. If needed, also register it's MIME type.
     */
    public void addComponent(Object hint, Class clazz, Configuration config) throws ComponentException {
        
        super.addComponent(hint, clazz, config);

        // Add to known hints
        this.knownHints.add(hint);
        
        if (this.roleId == SERIALIZER || this.roleId == READER) {
            // Get mime-type
            String mimeType = config.getAttribute("mime-type", null);
            if (mimeType != null) {
                this.mimeTypes.put(hint, mimeType);
            }
        }
    }
    
    /**
     * Ensure system-defined components exist (e.g. !content-aggregator!) and initialize
     * the selector.
     */
    public void initialize() /*throws Exception*/ {
    
    // FIXME : need to catch exceptions since ECS doesn't propagate the throws clause of Initializable
    try {
        
        Configuration emptyConfig = new DefaultConfiguration("", "");
        
        // Ensure all system-defined hints exist.
        // NOTE : checking this here means they can be user-defined in the sitemap
        switch(this.roleId) {
            case GENERATOR :
                ensureExists("!notifying-generator!",
                    org.apache.cocoon.sitemap.NotifyingGenerator.class, emptyConfig);

                ensureExists("!content-aggregator!",
                    org.apache.cocoon.sitemap.ContentAggregator.class, emptyConfig);
            break;
            
            case TRANSFORMER :
                ensureExists("!link-translator!",
                    org.apache.cocoon.sitemap.LinkTranslator.class, emptyConfig);
            break;
        }
        
        super.initialize();
        
        // Don't keep known hints (they're no more needed)
        this.knownHints = null;
        
    } catch(Exception e) {
        throw new CascadingRuntimeException("Cannot setup default components", e);
    }
    
    }
    
    /**
     * Ensure a component exists or add it otherwhise. We cannot simply call hasComponent()
     * since it requires to be initialized, and we want to add components, and this must
     * be done before initialization.
     */
    private void ensureExists(Object hint, Class clazz, Configuration config) throws ComponentException {
        
        if (! this.knownHints.contains(hint)) {
            this.addComponent(hint, clazz, config);
        }
    }

    /**
     * Get the MIME type for a given hint.
     */
    public String getMimeTypeForHint(Object hint) {
        
        if (this.mimeTypes == null) {
            return null;
            
        } else {
            String mimeType = (String)this.mimeTypes.get(hint);
            
            if (mimeType != null) {
                return mimeType;
                
            } else if (this.parentComponentsSelector != null) {
                return this.parentComponentsSelector.getMimeTypeForHint(hint);
                
            } else {
                return null;
            }
        }
    }
}
