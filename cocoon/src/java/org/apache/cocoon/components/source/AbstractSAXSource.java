
/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.source;

import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.ResourceNotFoundException;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.ModifiableSource;
import org.apache.cocoon.serialization.Serializer;
import org.apache.log.Logger;

import org.xml.sax.InputSource;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.IOException;
import java.io.InputStream;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import java.net.MalformedURLException;
import java.net.URL;

/**
 * This abstract class provides convenience methods to implement
 * a SAX based Source. Implement toSAX() and getSystemId() and
 * optionally override getLastModified() and getContentLength() to
 * obtain a valid Source implementation.
 *
 * @author <a href="mailto:gianugo@apache.org">Gianugo Rabellino</a>
 * @version $Id: AbstractSAXSource.java,v 1.3 2002/01/25 03:41:13 vgritsenko Exp $
 */

public abstract class AbstractSAXSource
  implements Source {

    /** The Logger instance */
    protected Logger log;

    /** The ComponentManager instance */
    protected ComponentManager manager;

    /**
     * The constructor.
     * 
     * @param environment the Cocoon Environment.
     * @param manager an Avalon Component Manager
     * @param logger A LogKit logger
     */
    
    public AbstractSAXSource(Environment environment, 
                       ComponentManager manager,
                       Logger logger) {
      this.log = logger;
      this.manager = manager;

    }

    /**
     * Get an InputSource for the given URL. Shamelessly stolen
     * from SitemapSource.
     *
     */
    
    public InputStream getInputStream() 
      throws ProcessingException, IOException {

        ComponentSelector serializerSelector = null;
        Serializer serializer = null;
        try {

            serializerSelector = (ComponentSelector) this.manager.lookup(Serializer.ROLE + "Selector");
            serializer = (Serializer)serializerSelector.select("xml");
            ByteArrayOutputStream os = new ByteArrayOutputStream();
            serializer.setOutputStream(os);

            this.toSAX(serializer);

            return new ByteArrayInputStream(os.toByteArray());
        } catch (ComponentException cme) {
            throw new ProcessingException("could not lookup pipeline components", cme);
        } catch (ProcessingException e) {
            throw e;
        } catch (Exception e) {
            throw new ProcessingException("Exception during processing of " + this.getSystemId(), e);
        } finally {
            if (serializer != null) serializerSelector.release(serializer);
            if (serializerSelector != null) this.manager.release(serializerSelector);
        }
    } 

    /**
     * Get an InputSource for the given URL.
     *
     */

    public InputSource getInputSource()
      throws ProcessingException, IOException {
      InputSource is = new InputSource(this.getInputStream());
      is.setSystemId(this.getSystemId());

      return is;
    }  

    /**
     * Implement this method to obtain SAX events.
     *
     */

    public abstract void toSAX(ContentHandler handler) 
      throws SAXException, ProcessingException; 

    /**
     * Implement this method to set the unique identifier. 
     *
     */

    public abstract String getSystemId();

    /**
     * Override this method to set the Content Length
     *
     */

    public long getContentLength() {
      return -1;
    }

    /**
     * Override this method to set the Last Modification date
     *
     */

    public long getLastModified() {
      return 0;
    }
}
