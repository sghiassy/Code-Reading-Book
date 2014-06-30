/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.generation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.activity.Initializable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.ResourceNotFoundException;
import org.apache.cocoon.caching.CacheValidity;
import org.apache.cocoon.caching.Cacheable;
import org.apache.cocoon.environment.SourceResolver;
import org.xml.sax.SAXException;

import org.xmldb.api.DatabaseManager;   
import org.xmldb.api.base.Collection;   
import org.xmldb.api.base.Database;   
import org.xmldb.api.base.ErrorCodes;   
import org.xmldb.api.base.Resource;   
import org.xmldb.api.base.Service;   
import org.xmldb.api.base.XMLDBException;   
import org.xmldb.api.modules.XMLResource; 

import java.io.IOException;
import java.util.Map;

/**
 * This class implements generation of XML documents from a
 * XML:DB compliant database.
 * It must to be configured as follows:
 * <pre>
 * &lt;driver&gt;
 *   (a valid DB:XML compliant driver)
 * &lt;/driver&gt;
 * &lt;base&gt;
 *   xmldb:yourdriver://host/an/optional/path/to/be/prepended
 * &lt;/base&gt;
 * </pre>
 *
 * NOTE: the driver can be any DB:XML compliant driver (although this 
 * component has been tested only with 
 * <a href="http://www.dbxml.org">dbXML</a>, and the trailing
 * slash in the base tag is important!
 *
 * @author <a href="mailto:gianugo@rabellino.it">Gianugo Rabellino</a>
 */
 
 public class XMLDBGenerator extends ComposerGenerator
   implements Cacheable, Recyclable,Configurable,Initializable {
     
    protected String driver;
    protected String base;
    protected String col;
    protected String res;
    protected Database database;
    protected Collection collection;
    protected XMLResource xmlResource;

    public void compose(ComponentManager manager) throws ComponentException {
        super.compose(manager);
    }
    
    /**
     * Recycle the component, keep only the configuration variables
     * and the database instance for reuse.
     *
     */
     
    public void recycle() {
        super.recycle();
        this.col = null;
        this.res = null;
        this.xmlResource = null;
        this.collection = null;
    }
    
   /**
    * Configure the component. This class is expecting a configuration
    * like the following one:
    * <pre>
    * &lt;driver&gt;org.dbxml.client.xmldb.DatabaseImpl&lt;/driver&gt;
    * &lt;base&gt;xmldb:dbxml:///db/&lt;/base&gt;
    * </pre>
    * NOTE: the driver can be any DB:XML compliant driver (although this 
    * component has been tested only with 
    * <a href="http://www.dbxml.org">dbXML</a>, and the trailing
    * slash in the base tag is important!
    *
    * @exception ConfigurationException (configuration invalid or missing)
    */

    public void configure(Configuration conf)
      throws ConfigurationException {
      if (conf != null) {
        this.driver = conf.getChild("driver").getValue();
        this.base = conf.getChild("base").getValue();
      } else {
        throw new ConfigurationException("XML:DB configuration not found");
      }
    }

   /**
    * Initialize the component getting a database instance. 
    *
    * @exception Exception if an error occurs
    */ 

    public void initialize() throws Exception {
      try {

        Class c = Class.forName(driver);
        database = (Database)c.newInstance();
        DatabaseManager.registerDatabase(database);

      } catch (XMLDBException xde) {

        this.getLogger().error("Unable to connect to the XML:DB database");
        throw new ProcessingException("Unable to connect to the XMLDB database" 
          + xde.getMessage());

      } catch (Exception e) {

        this.getLogger().error("There was a problem setting up the connection");
        this.getLogger().error("Make sure that your driver is available");
        throw new ProcessingException("Problem setting up the connection: " 
          + e.getMessage());

      } 
    }

    public void setup(SourceResolver resolver, 
                      Map objectModel, 
                      String src, 
                      Parameters par)
      throws ProcessingException, SAXException,IOException {

      super.setup(resolver, objectModel, src, par);

    }
    
    /**
     * The component isn't cached (yet)
     */
    public CacheValidity getValidity() {
      return null;
    }
   
    /**
     * The component isn't cached (yet)
     */
    public CacheValidity generateValidity() {
      return null;
    }
    
    /**
     * The component isn't cached (yet)
     */
    public long generateKey() {
      return 0;
    }
   
   /** 
    * Parse the requested URI, connect to the XML:DB database 
    * and fetch the requested resource.
    *
    * @exception ProcessingException something unexpected happened with the DB
    */ 
    public void generate()
      throws IOException, SAXException, ProcessingException {
      String col = "/";

      if (source.indexOf('/') != -1)
        col = "/" + source.substring(0, source.lastIndexOf('/'));        
      res = source.substring(source.lastIndexOf('/') + 1);

      try {
        collection = DatabaseManager.getCollection(base + col);
        xmlResource = (XMLResource) collection.getResource(res);

        if (xmlResource == null)
          throw new ResourceNotFoundException("Document " + col + "/" + res + 
            " not found");

        xmlResource.getContentAsSAX(this.xmlConsumer);

        collection.close();

      } catch (XMLDBException xde) {

        throw new ProcessingException("Unable to fetch content: " +
          xde.getMessage());

      } catch (NullPointerException npe) {

          this.getLogger().error("The XML:DB driver raised an exception");
          this.getLogger().error("probably the document was not found");

          throw new ProcessingException("Null pointer exception while " + 
            "retrieving document : " + npe.getMessage());

      } 
    }
}
