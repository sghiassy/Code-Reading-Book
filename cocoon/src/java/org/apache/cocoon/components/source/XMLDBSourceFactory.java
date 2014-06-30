/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.source;

import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.sitemap.Sitemap;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * This class implements the xmldb:// pseudo-protocol and allows to get XML
 * content from an XML:DB enabled XML database.
 *
 * @author <a href="mailto:gianugo@rabellino.it">Gianugo Rabellino</a>
 */

public final class XMLDBSourceFactory
  extends AbstractLoggable
  implements SourceFactory, Configurable, Composable {


    /** The driver implementation class */
    protected String driver;

    /** The driver implementation class */
    protected ComponentManager m_manager;

    /** A Map containing the driver list */
    protected HashMap driverMap;

    /**
     * Configure the instance.
     */ 
    public void configure(final Configuration conf) 
      throws ConfigurationException {

      if (conf != null) {

        driverMap = new HashMap();

        Configuration[] xmldbConfigs = conf.getChildren("driver");

        for (int i = 0; i < xmldbConfigs.length; i++) {
          driverMap.put(xmldbConfigs[i].getAttribute("type"),
            xmldbConfigs[i].getAttribute("class"));
        }

      } else {
        throw new ConfigurationException("XMLDB configuration not found");
      }

    }

    /**
     * Compose this Composable object. We need to pass on the 
     * ComponentManager to the actual Source.
     */
    
    public void compose(ComponentManager cm) {
      this.m_manager = cm;
    }

    /**
     * Resolve the source
     */
    public Source getSource(Environment environment, String location)
      throws ProcessingException, IOException, MalformedURLException {

      int start = location.indexOf(':') + 1;
      int end = location.indexOf(':', start);

      if (start == -1 || end == -1) {
        if (this.getLogger().isWarnEnabled()) {
        this.getLogger().warn("Mispelled XML:DB URL. " +
          "The syntax is \"xmldb:databasetype://host/collection/resource\""); 
        throw new MalformedURLException("Mispelled XML:DB URL. " +
          "The syntax is \"xmldb:databasetype://host/collection/resource\""); 
        }  
      }    

      String type = location.substring(start, end);

      driver = (String)driverMap.get(type);

      if (driver == null) {
        this.getLogger().error("Unable to find a driver for the \"" +
          type + " \" database type, please check the configuration");
        throw new ProcessingException("Unable to find a driver for the \"" +
          type + " \" database type, please check the configuration");
      }    

      return new XMLDBSource(environment, m_manager, this.getLogger(),
        driver, location);
    }

    /**
     * Resolve the source
     */
    public Source getSource(Environment environment, URL base, String location)
      throws ProcessingException, IOException, MalformedURLException {
      return getSource(environment, base.toExternalForm() + location);
    }
}
