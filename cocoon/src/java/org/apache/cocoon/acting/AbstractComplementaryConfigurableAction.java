/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.configuration.SAXConfigurationHandler;
import org.apache.cocoon.Constants;
import org.apache.cocoon.components.source.SourceHandler;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.environment.Environment;

import java.util.HashMap;
import java.util.Map;

/**
 * Set up environment for configurable form handling data.  This group
 * of actions are unique in that they employ a terciary mapping.
 *
 * Each configuration file must use the same format in order to be
 * effective.  The name of the root configuration element is irrelevant.
 *
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $
 */
public abstract class AbstractComplementaryConfigurableAction extends ConfigurableComposerAction {
    private static Map configurations = new HashMap();

    /**
     * Set up the complementary configuration file.  Please note that
     * multiple Actions can share the same configurations.  By using
     * this approach, we can limit the number of config files.
     * Also note that the configuration file does not have to be a file.
     *
     * Defaults to reload configuration file it has changed.
     */
    protected Configuration getConfiguration(String descriptor) throws ConfigurationException {
    boolean reloadable = Constants.DESCRIPTOR_RELOADABLE_DEFAULT;
    if (this.settings.containsKey("reloadable"))
        reloadable = Boolean.getBoolean((String) this.settings.get("reloadable"));
    return this.getConfiguration(descriptor, null, reloadable);
    }

    /**
     * @deprecated please use the getConfiguration(String, SourceResolver, boolean)
     *             version of this method instead.
     */
    protected Configuration getConfiguration(String descriptor, boolean reloadable) throws ConfigurationException {
        return this.getConfiguration( descriptor, null, reloadable );
    }

    /**
     * Set up the complementary configuration file.  Please note that
     * multiple Actions can share the same configurations.  By using
     * this approach, we can limit the number of config files.
     * Also note that the configuration file does not have to be a file.
     */
    protected Configuration getConfiguration(String descriptor, SourceResolver resolver, boolean reloadable) throws ConfigurationException {
        ConfigurationHelper conf = null;

        if (descriptor == null) {
            throw new ConfigurationException("The form descriptor is not set!");
        }

        synchronized (AbstractComplementaryConfigurableAction.configurations) {
            conf = (ConfigurationHelper) AbstractComplementaryConfigurableAction.configurations.get(descriptor);

            if (reloadable || conf == null) {
                SourceHandler sourceHandler = null;
                Source resource = null;

                try {
                    sourceHandler = (SourceHandler) this.manager.lookup(SourceHandler.ROLE);
                    if (descriptor.startsWith("file:")) {
                        resource = sourceHandler.getSource((Environment)resolver,((Environment)resolver).getContext(), descriptor.substring(5));
                    } else
                        resource = sourceHandler.getSource(null, descriptor);

                    if (conf == null || conf.lastModified < resource.getLastModified()) {
                        getLogger().debug("(Re)Loading " + descriptor);
                        if (conf == null)
                            conf = new ConfigurationHelper();

                        SAXConfigurationHandler builder = new SAXConfigurationHandler();
                        resource.toSAX(builder);

                        conf.lastModified = resource.getLastModified();
                        conf.configuration = builder.getConfiguration();

                        this.cacheConfiguration(descriptor, conf);
                    } else {
                        getLogger().debug("Using cached configuration for " + descriptor);
                    }
                } catch (Exception e) {
                    getLogger().error("Could not configure Database mapping environment", e);
                    throw new ConfigurationException("Error trying to load configurations for resource: " + (resource == null ? "null" : resource.getSystemId()));
                } finally {
                    if (resource != null) resource.recycle();
                    if (sourceHandler != null) this.manager.release((Component) sourceHandler);
                }
        } else {
        getLogger().debug("Using fixed cached configuration for " + descriptor);
        }
        }

        return conf.configuration;
    }

    /**
     * Cache the configuration so that we can use it later.
     */
    private void cacheConfiguration(String descriptor, ConfigurationHelper conf) {
        synchronized (AbstractComplementaryConfigurableAction.configurations) {
            AbstractComplementaryConfigurableAction.configurations.put(descriptor, conf);
        }
    }
}
