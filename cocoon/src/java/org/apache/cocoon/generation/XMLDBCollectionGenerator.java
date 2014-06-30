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
import org.xml.sax.helpers.AttributesImpl;

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
 * This class implements generation of a XML:DB collection 
 * contents as a directory listing.
 *
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
 
 public class XMLDBCollectionGenerator extends ComposerGenerator
   implements Cacheable, Recyclable,Configurable,Initializable {

    protected static final String URI = 
      "http://apache.org/cocoon/xmldb/1.0";
    protected static final String PREFIX = "collection";
    protected static final String RESOURCE_COUNT_ATTR = "resources";
    protected static final String COLLECTION_COUNT_ATTR  = "collections";
    protected static final String COLLECTION  = "collection";
    protected static final String QCOLLECTION  = PREFIX + ":collection";
    protected static final String RESOURCE  = "resource";
    protected static final String QRESOURCE  = PREFIX + ":resource";
     
    protected String driver;
    protected String base;
    protected String col;
    protected Database database;
    protected Collection collection;
    protected AttributesImpl attributes = new AttributesImpl();

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
        this.collection = null;
        //this.attributes = null;
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
        throw new ConfigurationException("XMLDB configuration not found");
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
        throw new ProcessingException("Unable to connect to the XML DB" 
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
      //String col = "/";

      //if (source.indexOf('/') != -1)
      col = source;        


      try {
        collection = DatabaseManager.getCollection(base + col);

        if (collection == null)
          throw new ResourceNotFoundException("Collection " + col + 
            " not found");

        collectionToSAX(collection);

        collection.close();

      } catch (XMLDBException xde) {

        throw new ProcessingException("Unable to fetch content '" 
          + source + "':" + xde.getMessage());

      } catch (NullPointerException npe) {

          this.getLogger().error("The XML:DB driver raised an exception");
          this.getLogger().error("probably the document was not found");
          throw new ProcessingException("Null pointer exception while " + 
          "retrieving document : " + npe.getMessage());

      } 
    }

    /** 
     * Output SAX events listing the collection.
     *
     * @exception SAXException
     */

    public void collectionToSAX(Collection collection)
      throws SAXException {

      String ncollections;
      String nresources;
      String[] resources;
      String[] collections;

      try {
        ncollections = Integer.toString(collection.getChildCollectionCount());
        nresources = Integer.toString(collection.getResourceCount());

        attributes.clear();
        attributes.addAttribute("", RESOURCE_COUNT_ATTR,
          RESOURCE_COUNT_ATTR, "CDATA", nresources);
        attributes.addAttribute("", COLLECTION_COUNT_ATTR,
          COLLECTION_COUNT_ATTR, "CDATA", ncollections);

        collections = collection.listChildCollections();
        resources = collection.listResources();

        this.xmlConsumer.startDocument();
        this.xmlConsumer.startPrefixMapping(PREFIX, URI);

        this.xmlConsumer.startElement(URI, "collections", 
          "collection:collections", attributes);

        // Print child collections

        for (int i = 0; i < collections.length; i++) {
          attributes.clear();
          attributes.addAttribute("", "name", "name", "CDATA", collections[i]);
          this.xmlConsumer.startElement(URI, COLLECTION, 
            QCOLLECTION, attributes);
          this.xmlConsumer.endElement(URI, COLLECTION, COLLECTION);
        }

        // Print child resources

        for (int i = 0; i < resources.length; i++) {
          attributes.clear();
          attributes.addAttribute("", "name", "name", "CDATA", resources[i]);
          this.xmlConsumer.startElement(URI, RESOURCE, 
            QRESOURCE, attributes);
          this.xmlConsumer.endElement(URI, RESOURCE, RESOURCE);
        }

        this.xmlConsumer.endElement(URI, "collections", 
          "collection:collections");
          
        this.xmlConsumer.endPrefixMapping(PREFIX);
        this.xmlConsumer.endDocument();
        
      } catch (XMLDBException xde) {
        this.getLogger().warn("Collection listing failed" + xde.getMessage());
        throw new SAXException("Collection listing failed" + xde.getMessage());
      }

    }

}
