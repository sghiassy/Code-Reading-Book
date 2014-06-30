/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.serialization;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.Loggable;
import org.apache.cocoon.caching.CacheValidity;
import org.apache.cocoon.caching.Cacheable;
import org.apache.cocoon.caching.NOPCacheValidity;
import org.apache.cocoon.components.renderer.ExtendableRendererFactory;
import org.apache.cocoon.components.renderer.RendererFactory;
import org.apache.cocoon.util.ClassUtils;
import org.apache.fop.apps.Driver;
import org.apache.fop.apps.Options;
import org.apache.fop.render.Renderer;

import java.io.OutputStream;

public class FOPSerializer
extends AbstractSerializer
implements Configurable, Cacheable {

    /**
      * The <code>Options</code> used by FOP.
      */
    protected Options options;

    /**
      * The <code>Driver</code> which is FOP.
      */
    protected Driver driver;

    /**
      * The current <code>Renderer</code>.
      */
    protected Renderer renderer;

    /**
      * The Renderer Factory to use
      */
    protected static RendererFactory factory = ExtendableRendererFactory.getRendererFactoryImplementation();

    /**
      * The current <code>mime-type</code>.
      */
    protected String mimetype;

    /**
     * The renderer name if configured
     */
    protected String rendererName;

    /**
      * Set the configurations for this serializer.
      */
    public void configure(Configuration conf)
        throws ConfigurationException {
        String userConfig = null;
        java.io.File userConfigFile = null;

        if (conf != null) {
            Configuration child = conf.getChild("user-config");
            if (child != null) {
                try {
                    userConfig = child.getAttribute("src");
                } catch(Exception ex) {
                    // No config file specified
                }
            }
        }

        // Check for null, use external or inbuilt config.
        if(userConfig != null) {
            try {
                userConfigFile = new java.io.File(userConfig);
                options = new Options(userConfigFile);
                getLogger().debug("Using config file " + userConfig);
            } catch (Exception ex) {
                getLogger().error("Cannot load  config " + userConfig, ex);
                throw new ConfigurationException("Cannot load config " + userConfig, ex);
            }
        } else {
            try {
                options = new Options();
                getLogger().debug("Using default config file");
            } catch (Exception e) {
                getLogger().error("Cannot load default config ", e);
            }
        }

        // Get the mime type.
        this.mimetype = conf.getAttribute("mime-type");
        getLogger().debug("FOPSerializer mime-type:" + mimetype);

        // Using the Renderer Factory, get the default renderer
        // for this MIME type.
        this.renderer = factory.createRenderer(mimetype);
        // Iterate through the parameters, looking for a renderer reference
        Configuration[] parameters = conf.getChildren("parameter");
        for (int i = 0; i < parameters.length; i++) {
            String name = parameters[i].getAttribute("name");
            if ("renderer".equals(name)) {
                this.rendererName = parameters[i].getAttribute("value");
                try {
                    this.renderer = (Renderer)ClassUtils.newInstance(rendererName);
                } catch (Exception ex) {
                    getLogger().error("Cannot load  class " + rendererName, ex);
                    throw new ConfigurationException("Cannot load class " + rendererName, ex);
                }
            }
        }

        // Do we have a renderer yet?
        if (this.renderer == null ) {
            throw new ConfigurationException(
                "Could not autodetect renderer for FOPSerializer and "
                + "no renderer was specified in the sitemap configuration."
            );
        }
    }

    /**
     * Return the MIME type.
     */
    public String getMimeType() {
        return mimetype;
    }

    /**
     * Create the FOP driver
     * Set the <code>OutputStream</code> where the XML should be serialized.
     */
    public void setOutputStream(OutputStream out) {
        // load the fop driver
        driver = new Driver();

        if(driver instanceof Loggable) {
            driver.setLogger(getLogger().getChildLogger("fop"));
        }
        if (this.rendererName == null) {
            this.renderer = factory.createRenderer(mimetype);
        } else {
            try {
                this.renderer = (Renderer)ClassUtils.newInstance(rendererName);
            } catch (Exception ex) {
                getLogger().error("Cannot load  class " + rendererName, ex);
                throw new RuntimeException("Cannot load class " + rendererName);
            }
        }
        driver.setRenderer(this.renderer);
        driver.setOutputStream(out);
        setContentHandler(driver.getContentHandler());
    }

    /**
     * Generate the unique key.
     * This key must be unique inside the space of this component.
     * This method must be invoked before the generateValidity() method.
     *
     * @return The generated key or <code>0</code> if the component
     *              is currently not cacheable.
     */
    public long generateKey() {
        return 1;
    }

    /**
     * Generate the validity object.
     * Before this method can be invoked the generateKey() method
     * must be invoked.
     *
     * @return The generated validity object or <code>null</code> if the
     *         component is currently not cacheable.
     */
    public CacheValidity generateValidity() {
        return NOPCacheValidity.CACHE_VALIDITY;
    }

    /**
      * Recycle serializer by removing references
      */
    public void recycle() {
        super.recycle();
        options = null;
        driver = null;
        this.renderer = null;
    }

    /**
    * Test if the component wants to set the content length
    */
    public boolean shouldSetContentLength() {
        return true;
    }
}
