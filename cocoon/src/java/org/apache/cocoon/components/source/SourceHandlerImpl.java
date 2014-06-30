/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.source;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.logger.Loggable;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.url.URLFactory;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.util.ClassUtils;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version $Id: SourceHandlerImpl.java,v 1.2 2002/01/22 00:17:12 vgritsenko Exp $
 */
public final class SourceHandlerImpl
extends AbstractLoggable
implements Configurable, Disposable, Composable, Contextualizable, SourceHandler {

    /** The component manager */
    private ComponentManager manager;

    /** The url factory */
    private URLFactory urlFactory;

    /** The special Source factories */
    private Map sourceFactories;

    /** The context */
    private Context context;

    /**
     * Configure the SourceFactories
     */
    public void configure(final Configuration conf)
    throws ConfigurationException {
        try {
            getLogger().debug("Getting the SourceFactories");
            HashMap factories = new HashMap();
            Configuration[] configs = conf.getChildren("protocol");
            SourceFactory sourceFactory = null;
            String protocol = null;
            for (int i = 0; i < configs.length; i++) {
                protocol = configs[i].getAttribute("name");
                if (factories.containsKey(protocol)) {
                    throw new ConfigurationException("SourceFactory defined twice for protocol: " + protocol);
                }

                getLogger().debug("\tfor protocol: " + protocol + " " + configs[i].getAttribute("class"));
                sourceFactory = (SourceFactory) ClassUtils.newInstance(configs[i].getAttribute("class"));
                this.init(sourceFactory, configs[i]);
                factories.put(protocol, sourceFactory);
            }

            this.sourceFactories = java.util.Collections.synchronizedMap(factories);
        } catch (ConfigurationException e) {
            throw e;
        } catch (Exception e) {
            getLogger().error("Could not get SourceFactories", e);
            throw new ConfigurationException("Could not get parameters because: " +
                                           e.getMessage());
        }
    }

    /**
     * Get the context
     */
    public void contextualize(Context context)
    throws ContextException {
        this.context = context;
    }

    /**
     * Set the current <code>ComponentManager</code> instance used by this
     * <code>Composable</code>.
     */
    public void compose(ComponentManager manager)
    throws ComponentException {
        this.manager = manager;
        this.urlFactory = (URLFactory)this.manager.lookup(URLFactory.ROLE);
    }

    /**
     * Dispose
     */
    public void dispose() {
        if (this.urlFactory != null) {
            this.manager.release((Component)this.urlFactory);
        }
        Iterator iter = this.sourceFactories.values().iterator();
        SourceFactory current;
        while (iter.hasNext()) {
            current = (SourceFactory) iter.next();
            this.deinit(current);
        }
        this.sourceFactories = null;
    }

    /**
     * Get a <code>Source</code> object.
     */
    public Source getSource(Environment environment, String location)
    throws ProcessingException, MalformedURLException, IOException {
        int protocolEnd = location.indexOf(':');
        if (protocolEnd != -1) {
            String protocol = location.substring(0, protocolEnd);
            SourceFactory sourceFactory = (SourceFactory)this.sourceFactories.get(protocol);
            if (sourceFactory != null) {
                return sourceFactory.getSource(environment, location);
            }
        }

        // default implementation
        return new URLSource(this.urlFactory.getURL(location), this.manager);
    }

    /**
     * Get a <code>Source</code> object.
     */
    public Source getSource(Environment environment, URL base, String location)
    throws ProcessingException, MalformedURLException, IOException {
        String protocol = base.getProtocol();
        SourceFactory sourceFactory = (SourceFactory)this.sourceFactories.get(protocol);
        if (sourceFactory != null) {
            return sourceFactory.getSource(environment, base, location);
        }

        // default implementation
        return new URLSource(this.urlFactory.getURL(base, location), this.manager);
    }

    /**
     * Add a factory
     */
    public void addFactory(String protocol, SourceFactory factory)
    throws ProcessingException {
        try {
            this.init(factory, null);
            this.sourceFactories.put(protocol, factory);
        } catch (ComponentException e) {
            throw new ProcessingException("cannot initialize factory: " + factory, e);
        } catch (ContextException e) {
            throw new ProcessingException("cannot initialize factory: " + factory, e);
        } catch (ConfigurationException e) {
            throw new ProcessingException("cannot configure factory: " + factory, e);
        }
    }

    /**
     * Init a source factory
     */
    private void init(SourceFactory factory, Configuration config)
    throws ContextException, ComponentException, ConfigurationException {
        if (factory instanceof Loggable) {
            ((Loggable) factory).setLogger(getLogger());
        }
        if (factory instanceof Contextualizable) {
            ((Contextualizable) factory).contextualize (this.context);
        }
        if (factory instanceof Composable) {
            ((Composable) factory).compose(this.manager);
        }
        if (config != null && factory instanceof Configurable) {
            ((Configurable) factory).configure(config);
        }
    }

    /**
     * Deinit a source factory
     */
    private void deinit(SourceFactory factory) {
        if (factory instanceof Disposable) {
            ((Disposable) factory).dispose();
        }
    }

}
