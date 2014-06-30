/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentSelector;

import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.configuration.DefaultConfiguration;

import org.apache.avalon.excalibur.component.ExcaliburComponentManager;
import org.apache.avalon.excalibur.component.ExcaliburComponentSelector;
import org.apache.avalon.excalibur.component.RoleManager;

import org.apache.cocoon.components.ExtendedComponentSelector;
import org.apache.cocoon.components.LifecycleHelper;

import org.apache.cocoon.generation.Generator;
import org.apache.cocoon.transformation.Transformer;
import org.apache.cocoon.serialization.Serializer;

import org.apache.cocoon.treeprocessor.CategoryNode;
import org.apache.cocoon.treeprocessor.CategoryNodeBuilder;
import org.apache.cocoon.treeprocessor.TreeBuilder;

import org.apache.cocoon.util.StringUtils;

import java.util.*;

/**
 * The tree builder for the sitemap language.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/15 11:10:54 $
 */

public class SitemapLanguage extends TreeBuilder {

    private static final String ATTR_PREFIX = "org.apache.cocoon.treeprocessor.ViewNodeBuilder";

    /**
     * Build a component manager with the contents of the &lt;map:components&gt; element of
     * the tree.
     */
    protected ComponentManager createComponentManager(Configuration tree) throws Exception {
        
        // Get the map:component node
        // Don't check namespace here : this will be done by node builders
        Configuration config = tree.getChild("components", false);
        
        if (config == null) {
            getLogger().debug("Sitemap has no components definition at " + tree.getLocation());
            config = new DefaultConfiguration("", "");
        }
        
        ComponentManager manager = new ExcaliburComponentManager(this.parentManager) {
            public void setRoleManager(RoleManager rm) {
                super.setRoleManager(rm);
            }
        };
        
        LifecycleHelper.setupComponent(manager,
            getLogger(),
            this.context,
            this.parentManager,
            this.roleManager,
            this.logKit,
            config
        );
        
        // Set parent of all selectors.
        if (this.parentManager != null) {
            
            for (int i = 0; i < ComponentsSelector.SELECTOR_ROLES.length; i++) {
                
                String role = ComponentsSelector.SELECTOR_ROLES[i];
                
                ComponentSelector parentSelector = null;
                try {
                    parentSelector = (ComponentSelector)this.parentManager.lookup(role);
                } catch(Exception e) {
                    // ignore and keep it null
                }
                
                if (parentSelector != null) {
                
                    ExtendedComponentSelector localSelector = null;
                    try {
                        localSelector = (ExtendedComponentSelector)manager.lookup(role);
                        
                        if (localSelector != parentSelector) {
                            // local selector wasn't given by chaining to the parent manager
                            localSelector.setParentSelector(parentSelector);
                        }
                        manager.release(localSelector);

                    } catch(Exception e) {
                        // ignore
                    }
                    
                    parentManager.release(parentSelector);
                }
            }
        }
        
        // Parse generators and transformers labels
        
        Configuration[] generators = config.getChild("generators").getChildren();
        // Note : we don't getChildren("generator") because there can be some
        // shortcut declarations that have other names
        
        for (int i = 0; i < generators.length; i++) {
            this.setComponentLabels(
                Generator.ROLE,
                generators[i].getAttribute("name"),
                generators[i].getAttribute("label", null)
            );
        }
        
        Configuration[] transformers = config.getChild("transformers").getChildren();
        for (int i = 0; i < transformers.length; i++) {
            this.setComponentLabels(
                Transformer.ROLE,
                transformers[i].getAttribute("name"),
                transformers[i].getAttribute("label", null)
            );
        }

        return manager;
    }
    
    //---- Views management
    
    /** Collection of view names for each label */
    private Map labelViews = new HashMap();
    
    /** Collection of labels for each component, represented by a 'role':'hint' key */
    private Map componentLabels = new HashMap();
    
    /** The views CategoryNode */
    private CategoryNode viewsNode;
    
    /** Are we currently building a view ? */
    private boolean isBuildingView = false;
    
    /**
     * Pseudo-label for views <code>from-position="first"</code> (i.e. generator).
     */
    public static final String FIRST_POS_LABEL = "!first!";
    
    /**
     * Pseudo-label for views <code>from-position="last"</code> (i.e. serializer).
     */
    public static final String LAST_POS_LABEL = "!last!";
    
    /**
     * Set to <code>true</code> while building the internals of a &lt;map:view&gt;
     */
    public void setBuildingView(boolean building) {
        this.isBuildingView = building;
    }
    
    /**
     * Are we currently building a view ?
     */
    public boolean isBuildingView() {
        return this.isBuildingView;
    }
    
    /**
     * Registers the view labels for a component. They're merged later on with the
     * ones defined on each statement.
     */
    private void setComponentLabels(String role, String hint, String labels) {
        getLogger().debug("views:setComponentLabels(" + role + ", " + hint + ", " + labels + ")");
        Collection labelColl = splitLabels(labels);
        this.componentLabels.put(role + ":" + hint, labelColl);
    }

    /**
     * Add a view for a label. This is used to register all views that start from
     * a given label.
     *
     * @parameter label the label (or pseudo-label) for the view
     * @parameter view the view name
     */
    public void addViewForLabel(String label, String view) {
        getLogger().debug("views:addViewForLabel(" + label + ", " + view + ")");
        Set views = (Set)this.labelViews.get(label);
        if (views == null) {
            views = new HashSet();
            this.labelViews.put(label, views);
        }
        
        views.add(view);
    }
    
    /**
     * Get the names of views for a given statement. If the cocoon view exists in the returned
     * collection, the statement can directly branch to the view-handling node.
     *
     * @param role the component role (e.g. <code>Generator.ROLE</code>)
     * @param hint the component hint, i.e. the 'type' attribute
     * @param statement the sitemap statement
     * @return the view names for this statement
     */
    public Collection getViewsForStatement(String role, String hint, Configuration statement) throws Exception {

        String statementLabels = statement.getAttribute("label", null);
        
        if (this.isBuildingView) {
            // Labels are forbidden inside view definition
            if (statementLabels != null) {
                String msg = "Cannot put a 'label' attribute inside view definition at " + statement.getLocation();
                getLogger().error(msg);
                throw new ConfigurationException(msg);
            }
            
            // We are currently building a view. Don't recurse !
            return null;
        }
        
        // Compute the views attached to this component
        Set views = null;
        
        // Build the set for all labels for this statement
        Set labels = new HashSet();
        
        // 1 - labels defined on the component
        Collection coll = (Collection)this.componentLabels.get(role + ":" + hint);
        if (coll != null) {
            labels.addAll(coll);
        }

        // 2 - labels defined on this statement
        if (statementLabels != null) {
            coll = splitLabels(statementLabels);
            labels.addAll(coll);
        }
        
        // 3 - pseudo-label depending on the role
        if (role.equals(Generator.ROLE)) {
            labels.add("!first!");
        } else if (role.equals(Serializer.ROLE)) {
            labels.add("!last!");
        }
        
        // Build the set of views attached to these labels
        views = new HashSet();
        
        // Iterate on all labels for this statement
        Iterator labelIter = labels.iterator();
        while(labelIter.hasNext()) {
            
            // Iterate on all views for this labek
            coll = (Collection)this.labelViews.get(labelIter.next());
            if (coll != null) {
                Iterator viewIter = coll.iterator();
                while(viewIter.hasNext()) {
                    String viewName = (String)viewIter.next();
                    
                    views.add(viewName);
                }
            }
        }
        
        // Don't keep empty result
        if (views.size() == 0) {
            views = null;
            
            getLogger().debug(statement.getName() + " has no views at " + statement.getLocation());
        } else {
            if (getLogger().isDebugEnabled()) {
                // Dump matching views
                StringBuffer buf = new StringBuffer(statement.getName() + " will match views [");
                Iterator iter = views.iterator();
                while(iter.hasNext()) {
                    buf.append(iter.next()).append(" ");
                }
                buf.append("] at ").append(statement.getLocation());
                
                getLogger().debug(buf.toString());
            }
        }
        
        return views;
    }
    
    /**
     * Before linking nodes, lookup the view category node used in {@link #getViewNodes(Collection)}.
     */
    protected void linkNodes() throws Exception {
        // Get the views category node
        this.viewsNode = CategoryNodeBuilder.getCategoryNode(this, "views");
        
        super.linkNodes();
    }

    /**
     * Get the {view name, view node} map for a collection of view names.
     * This allows to resolve view nodes at build time, thus avoiding runtime lookup.
     *
     * @param viewNames the view names
     * @return association of names to views
     */
    public Map getViewNodes(Collection viewNames) throws Exception {
        if (viewNames == null) {
            return null;
        }
        
        if (this.viewsNode == null) {
            return null;
        }
        
        Map result = new HashMap();
        
        Iterator iter = viewNames.iterator();
        while(iter.hasNext()) {
            String viewName = (String)iter.next();
            result.put(viewName, viewsNode.getNodeByName(viewName));
        }
        
        return result;
    }

    /**
     * Split a list of space/comma separated labels into a Collection
     *
     * @return the collection of labels (may be empty, nut never null)
     */
    private static final Collection splitLabels(String labels) {
        if (labels == null) {
            return Collections.EMPTY_SET;
        } else {
            return Arrays.asList(StringUtils.split(labels, ", \t\n\r"));
        }
    }
    
}
