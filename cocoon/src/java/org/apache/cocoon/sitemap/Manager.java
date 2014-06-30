/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import org.apache.avalon.excalibur.component.RoleManager;
import org.apache.avalon.excalibur.logger.LogKitManageable;
import org.apache.avalon.excalibur.logger.LogKitManager;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.environment.Environment;

import java.util.HashMap;
import java.util.Iterator;
/**
 * This class manages all sub <code>Sitemap</code>s of a <code>Sitemap</code>
 * Invokation of sub sitemaps will be done by this instance as well
 * checking regeneration of the sub <code>Sitemap</code>
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/25 03:49:57 $
 */
public class Manager
        extends AbstractLoggable
        implements Component, Configurable, Composable, Contextualizable, ThreadSafe, LogKitManageable {
    protected Context context;

    /** The vectors of sub sitemaps */
    protected HashMap sitemaps = new HashMap();

    /** The configuration */
    protected Configuration conf;

    /** The component manager */
    protected ComponentManager manager;

    /** The sitemap role manager */
    protected RoleManager sitemapRoles;

    /** The sitemap logkit manager */
    protected LogKitManager sitemapLogKitManager;

    protected Class handlerClass = Handler.class;

    /**
     * Set the role manager
     */
    public void setRoleManager(RoleManager roles) {
        this.sitemapRoles = roles;
    }

    /**
     * Set the logkit manager
     */
    public void setLogKitManager(LogKitManager logkit) {
        this.sitemapLogKitManager = logkit;
    }

    /** get a configuration
     * @param conf the configuration
     */
    public void configure(Configuration conf) throws ConfigurationException {
        this.conf = conf;

        String value = conf.getAttribute("handler-class", null);
        if (value != null) {
          try {
            handlerClass = Class.forName(value);
          }
          catch (ClassNotFoundException ex) {
            throw new ConfigurationException("Cannot find Handler class "
                                             + value);
          }
        }
    }

    /** get a context
     * @param context the context object
     */
    public void contextualize(Context context) throws ContextException {
        this.context = context;
    }

    /** get a component manager
     * @param manager the component manager
     */
    public void compose(ComponentManager manager) {
        this.manager = manager;
    }

    /** invokes the sitemap handler to process a request
     * @param environment the environment
     * @param uri_prefix the prefix to the URI
     * @param source the source of the sitemap
     * @param check_reload should the sitemap be automagically reloaded
     * @param reload_asynchron should the sitemap be reloaded asynchron
     * @throws Exception there may be several excpetions thrown
     * @return states if the requested resource was produced
     */
    public boolean invoke(ComponentManager newManager, Environment environment, String uri_prefix, String source, boolean check_reload,
        boolean reload_asynchron) throws Exception {
            // get a sitemap handler
            Handler sitemapHandler = getHandler(newManager, environment, source, check_reload, reload_asynchron);
            // setup to invoke the processing
            setupProcessing(environment, sitemapHandler, uri_prefix, source);
            String prefix = environment.getURIPrefix();
            String uri    = environment.getURI();
            try {
                environment.changeContext(uri_prefix, source);
                return sitemapHandler.process(environment);
            } finally{
                environment.setContext(prefix, uri);
            }
    }

    /** invokes the sitemap handler to process a request
     * @param environment the environment
     * @param uri_prefix the prefix to the URI
     * @param source the source of the sitemap
     * @param check_reload should the sitemap be automagically reloaded
     * @param reload_asynchron should the sitemap be reloaded asynchron
     * @throws Exception there may be several excpetions thrown
     * @return states if the requested resource was produced
     */
    public boolean invoke(ComponentManager newManager, Environment environment, String uri_prefix, String source, boolean check_reload,
        boolean reload_asynchron, StreamPipeline pipeline, EventPipeline eventPipeline) throws Exception {
            // get a sitemap handler
            Handler sitemapHandler = getHandler(newManager, environment, source, check_reload, reload_asynchron);
            // setup to invoke the processing
            setupProcessing(environment, sitemapHandler, uri_prefix, source);
            String prefix = environment.getURIPrefix();
            String uri    = environment.getURI();
            try {
                environment.changeContext(uri_prefix, source);
                return sitemapHandler.process(environment, pipeline, eventPipeline);
            } finally{
                environment.setContext(prefix, uri);
            }
    }

    /** has the sitemap changed
     * @return whether the sitemap file has changed
     */
    public boolean hasChanged() {
        Handler sitemapHandler = null;
        Iterator iter = sitemaps.values().iterator();
        while (iter.hasNext()) {
            sitemapHandler = (Handler)iter.next();
            if ((sitemapHandler != null) && (sitemapHandler.hasChanged())) {
                return true;
            }
        }
        return false;
    }

    private Handler getHandler(final ComponentManager newManager, final Environment environment, final String source, final boolean check_reload,
        final boolean reload_asynchron) throws Exception {
            Handler sitemapHandler = (Handler)sitemaps.get(source);
            if (sitemapHandler != null) {
                // The following is a little bit complicated:
                // If the sitemap handler is not available, it is regenerated
                // If it is available:
                //    The handler is only queried if it has changed if
                //    the check_reload flag is true
                if (sitemapHandler.available()) {
                    if (check_reload && sitemapHandler.hasChanged()) {
                        // The sitemap has been changed.
                        if (reload_asynchron) {
                            // In asynchron mode the first request entering this
                            // starts the regeneration. All other request comming
                            // in, use the old sitemap until the new is generated.
                            if (!sitemapHandler.isRegenerating()) {
                                sitemapHandler.regenerateAsynchronously(environment);
                            }
                        } else {
                            // In synchron mode the first request starts the
                            // regeneration, all other requests are blocked
                            // until the regeneration is finished. This is done
                            // by the synchronized statement
                            if (!sitemapHandler.isRegenerating()) {
                                synchronized (this) {
                                    sitemapHandler.regenerate(environment);
                                }
                            } else {
                                synchronized (this) {
                                    // Dummy statement
                                    this.hashCode();
                                }
                            }
                        }
                    }
                } else {
                    sitemapHandler.regenerate(environment);
                }
            } else {
                // Also this looks a little bit strange, it should work
                // if no handler is available, the generation of the new
                // handler is synchronized.
                // The first incoming request creates the handler and blocks
                // all incoming requests. When the generation is finished
                // all other requests enter (one after the other) this method
                // and get the new created handler with the first get
                // statement
                synchronized (this) {
                    sitemapHandler = (Handler)sitemaps.get(source);
                    if (sitemapHandler == null) {
                        sitemapHandler = (Handler)handlerClass.newInstance();
                        sitemapHandler.setSourceFileName(source);
                        sitemapHandler.setCheckReload(check_reload);
                        sitemapHandler.setLogger(getLogger());
                        sitemapHandler.compose(newManager);
                        sitemapHandler.contextualize(this.context);
                        if (sitemapHandler instanceof Configurable)
                          ((Configurable)sitemapHandler).configure(conf);
                        sitemapHandler.regenerate(environment);
                        sitemaps.put(source, sitemapHandler);
                    }
                }
            }
            return sitemapHandler;
    }

    private void setupProcessing(Environment environment, Handler sitemapHandler, String uri_prefix,
        String source) throws Exception {
            if (!sitemapHandler.available()) {
                if (sitemapHandler.getException() != null) {
                    throw sitemapHandler.getException();
                } else {
                    throw new ProcessingException("The sitemap handler's sitemap is not available. " +
                        "Please check logs for the exact error.");
                }
            }
    }
}
