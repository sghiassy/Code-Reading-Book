/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.excalibur.component.RoleManageable;
import org.apache.avalon.excalibur.component.RoleManager;
import org.apache.avalon.excalibur.logger.LogKitManageable;
import org.apache.avalon.excalibur.logger.LogKitManager;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.activity.Initializable;

import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.component.Composable;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.configuration.NamespacedSAXConfigurationHandler;
import org.apache.avalon.framework.configuration.SAXConfigurationHandler;

import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;

import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.Processor;
import org.apache.cocoon.components.LifecycleHelper;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.components.source.CocoonSourceFactory;
import org.apache.cocoon.components.source.DelayedRefreshSourceWrapper;
import org.apache.cocoon.components.source.SourceHandler;
import org.apache.cocoon.components.source.URLSource;
import org.apache.cocoon.components.url.URLFactory;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Source;

import java.io.InputStream;
import java.util.*;

/**
 * Interpreted tree-traversal implementation of a pipeline assembly language.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/30 14:35:07 $
 */

public class TreeProcessor extends AbstractLoggable implements ThreadSafe, Processor,
  Composable, Configurable, LogKitManageable, RoleManageable, Initializable, Contextualizable, Disposable {
    
    private static final String XCONF_URL = "resource://org/apache/cocoon/treeprocessor/treeprocessor.xconf";
    
    /** The parent TreeProcessor, if any */
    protected TreeProcessor parent;
    
    /** The context */
    protected Context context;
    
    /** The component manager */
    protected ComponentManager manager;
    
    /** The logkit manager to get Loggers */
    protected LogKitManager logKit;
    
    /** The role manager */
    protected RoleManager roleManager;
    
    /** The language used by this processor */
    protected String language;
    
    /** The configuration for each language */
    protected Map languageConfigs;
    
    /** The root node of the processing tree */
    protected ProcessingNode rootNode;
    
    /** The list of processing nodes that should be disposed when disposing this processor */
    protected List disposableNodes;
    
    /** Last modification time */
    protected long lastModified = 0;
    
    /** The relative file name of the tree definition */
    protected String sourceName;
    
    /** The source of the tree definition */
    protected Source source;
    
    /** Delay for <code>sourceLastModified</code>. */
    protected long lastModifiedDelay;
    
    /** The current language configuration */
    protected Configuration currentLanguage;
    
    protected SourceHandler sourceHandler;
    
    protected Class treeBuilderClass;
    
    /**
     * Create a TreeProcessor.
     */
    public TreeProcessor() {
        // Language can be overriden in the configuration.
        this.language = "sitemap";
    }
        
    /**
     * Create a child processor for a given language
     */
    protected TreeProcessor(TreeProcessor parent, ComponentManager manager, String language) {
        this.parent = parent;
        this.language = (language == null) ? parent.language : language;
        
        // Copy all that can be copied from the parent
        this.context = parent.context;
        this.logKit = parent.logKit;
        this.languageConfigs = parent.languageConfigs;
        
        // We have our own CM
        this.manager = manager;
        
        // Other fields are setup in initialize()
    }
    
    /**
     * Create a new child of this processor (used for mounting submaps).
     *
     * @param manager the component manager to be used by the child processor.
     * @param language the language to be used by the child processor.
     * @return a new child processor.
     */
    public TreeProcessor createChildProcessor(ComponentManager manager, String language) throws Exception {
        TreeProcessor child = new TreeProcessor(this, manager, language);
        child.setLogger(getLogger());
        child.initialize();
        return child;
    }

    public void contextualize(Context context) throws ContextException {
        this.context = context;
    }
    
    public void compose(ComponentManager manager) throws ComponentException {
        this.manager = manager;
    }

    public void setLogKitManager(LogKitManager logKit) {
        this.logKit = logKit;
    }
    
    public void setRoleManager(RoleManager rm) {
        this.roleManager = rm;
    }


/*
  <processor>
    <reload delay="10"/>
    <root-language name="sitemap"/>
    <language>...</language>
  </processor>
*/
    public void configure(Configuration config) throws ConfigurationException {
        
        Configuration rootLangConfig = config.getChild("root-language", false);
        if (rootLangConfig != null) {
            this.language = rootLangConfig.getAttribute("name");
        }
        
        // Reload check delay. Default is 1 second.
        this.lastModifiedDelay = config.getChild("reload").getAttributeAsLong("delay", 1000L);
        
        // Read the builtin languages definition file
        Configuration builtin;
        
        try {
            URLFactory factory = (URLFactory)this.manager.lookup(URLFactory.ROLE);
            URLSource source = new URLSource(factory.getURL(XCONF_URL), this.manager);
            try {
                SAXConfigurationHandler handler = new SAXConfigurationHandler();
                source.toSAX(handler);
                builtin = handler.getConfiguration();
            } finally {
                this.manager.release((Component)factory);
                if (source != null) {
                    source.recycle();
                }
            }
        } catch(Exception e) {
            String msg = "Error while reading treeprocessor.xconf : " + e.getMessage();
            getLogger().error(msg, e);
            throw new ConfigurationException(msg, e);
        }
        
        this.languageConfigs = new HashMap();

        // Add builtin languages
        addLanguages("builtin", builtin.getChildren("language"));
        
        // Add additional languages from the configuration
        addLanguages("additional", config.getChildren("language"));
    }
    
    private void addLanguages(String type, Configuration[] languages)
      throws ConfigurationException {
        
        for (int i = 0; i < languages.length; i++) {
            
            String name = languages[i].getAttribute("name");
            
            if (this.languageConfigs.containsKey(name)) {
                getLogger().info("Redefining language '" + name + "' in " + type + " configuration.");
            } else {
                getLogger().debug("Adding " + type + " language '" + name + "'");
            }

            this.languageConfigs.put(name, languages[i]);
        }
    }

    public void initialize() throws Exception {

        this.currentLanguage = (Configuration)this.languageConfigs.get(this.language);
        if (this.currentLanguage == null) {
            throw new ConfigurationException("No configuration defined for language '" + this.language + "'");
        }
        
        Configuration fileConfig = this.currentLanguage.getChild("file", false);
        if (fileConfig == null) {
            throw new ConfigurationException("Missing 'file' configuration for language '" + this.language + "', at " +
                this.currentLanguage.getLocation());
        }
        
        this.sourceName = fileConfig.getAttribute("name");

        // Get a new Source handler
        this.sourceHandler = (SourceHandler)this.manager.lookup(SourceHandler.ROLE);
        
        // and add the special "cocoon:" source factory
        this.sourceHandler.addFactory("cocoon", new CocoonSourceFactory(this, this.manager));
        
        // Get the TreeBuider class
        String builderClassName =
            this.currentLanguage.getAttribute("class", TreeBuilder.class.getName());

        try {
            this.treeBuilderClass = Thread.currentThread().getContextClassLoader().loadClass(builderClassName);
        } catch(Exception e) {
            String msg = "Cannot create class '" + builderClassName + "' at " +
                this.currentLanguage.getLocation();
            getLogger().error(msg, e);
            throw new ConfigurationException(msg, e);
        }
    }
    
    public boolean process(Environment environment) throws Exception {
        InvokeContext context = new InvokeContext();
        try {
            return process(environment, context);
        } finally {
            context.dispose();
        }
    }

    public boolean process(Environment environment, StreamPipeline pipeline, EventPipeline eventPipeline)
      throws Exception {
        InvokeContext context = new InvokeContext(pipeline, eventPipeline);
        try {
            return process(environment, context);
        } finally {
            context.dispose();
        }
    }

    protected boolean process(Environment environment, InvokeContext context)
      throws Exception {

        SourceHandler oldSourceHandler = environment.getSourceHandler();

        try {
            environment.setSourceHandler(this.sourceHandler);
            if (this.rootNode == null || this.source.getLastModified() > this.lastModified) {
                setupRootNode(environment);
            }
            return this.rootNode.invoke(environment, context);
        } finally {
            environment.setSourceHandler(oldSourceHandler);
        }
    }
    
    protected synchronized void setupRootNode(Environment env) throws Exception {
        
        // Now that we entered the synchronized area, recheck what's already
        // been checked in process().
        if (this.rootNode != null && source.getLastModified() <= this.lastModified) {
            // Nothing changed
            return;
        }
        
        long startTime = System.currentTimeMillis();
        
        if (this.rootNode == null) {
            // First call : create source
            // FIXME : make the delay configurable
            this.source = new DelayedRefreshSourceWrapper(env.resolve(this.sourceName), 1000L);
            
        } else {
            // Dispose existing tree, we will build a new one.
            disposeTree();
        }
        
        // Read the tree definition file as a Configuration
        getLogger().debug("Building " + this.language + " from " + source.getSystemId());
        
        // Build a namespace-aware configuration object
        SAXConfigurationHandler handler = new NamespacedSAXConfigurationHandler();
        source.toSAX(handler);
        Configuration treeConfig = handler.getConfiguration();
        
        TreeBuilder treeBuilder = (TreeBuilder)this.treeBuilderClass.newInstance();
        LifecycleHelper.setupComponent(treeBuilder,
            getLogger(),
            this.context,
            this.manager,
            this.roleManager,
            this.logKit,
            this.currentLanguage);

        treeBuilder.setProcessor(this);
        
        // Build the tree
        ProcessingNode root;
        try {
            root = treeBuilder.build(treeConfig);
        } catch(Exception e) {
            getLogger().debug("Failed to build processing tree from " + source.getSystemId(), e);
            throw e;
        }
        
        LifecycleHelper.decommission(treeBuilder);
            
        this.lastModified = System.currentTimeMillis();
        
        if (getLogger().isDebugEnabled()) {
            double time = (this.lastModified - startTime) / 1000.0;
            getLogger().debug("TreeProcessor built in " + time + " secs from " + source.getSystemId());
            //System.out.println("TreeProcessor built in " + time + " secs from " + source.getSystemId());
        }
        
        // Finished
        this.rootNode = root;
    }

    public void dispose() {
        disposeTree();        
        this.manager.release(this.sourceHandler);
    }
    
    /**
     * Dispose all nodes in the tree that are disposable
     */
    protected void disposeTree() {
        if (this.disposableNodes != null) {
            Iterator iter = this.disposableNodes.iterator();
            while (iter.hasNext()) {
                ((Disposable)iter.next()).dispose();
            }
        }
    }
}
