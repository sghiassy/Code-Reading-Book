/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.parser;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.thread.SingleThreaded;
import org.apache.cocoon.components.resolver.Resolver;
import org.apache.cocoon.xml.AbstractXMLProducer;
import org.apache.xerces.dom.DocumentImpl;
import org.apache.xerces.dom.DocumentTypeImpl;
import org.apache.xerces.parsers.DOMParser;
import org.apache.xerces.parsers.SAXParser;
import org.w3c.dom.Document;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

import java.io.IOException;

/**
 *
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/14 16:18:55 $
 */
public class XercesParser extends AbstractXMLProducer
implements Parser, ErrorHandler, Composable, Disposable, SingleThreaded {

    /** the SAX Parser */
    final SAXParser parser;

    /** the component manager */
    protected ComponentManager manager;

    /** the Entity Resolver */
    protected Resolver resolver = null;

    public XercesParser ()
    throws SAXException {
        this.parser = new SAXParser();

        this.parser.setFeature("http://xml.org/sax/features/validation",false);
        this.parser.setFeature("http://xml.org/sax/features/namespaces",true);
    }

    /**
     * Get the Entity Resolver from the component manager
     */
    public void compose(ComponentManager manager) throws ComponentException {
        this.manager = manager;
        getLogger().debug("Looking up " + Resolver.ROLE);
        if ( manager.hasComponent( Resolver.ROLE ) ) {
            this.resolver = (Resolver)manager.lookup(Resolver.ROLE);
        }
    }

    /**
     * Dispose
     */
    public void dispose() {
        if (this.manager != null) {
            this.manager.release( this.resolver );
        }
    }

    public void parse(InputSource in)
    throws SAXException, IOException {
      this.parser.setProperty("http://xml.org/sax/properties/lexical-handler",
                              super.lexicalHandler);
        this.parser.setErrorHandler(this);
        this.parser.setContentHandler(super.contentHandler);
        if(this.resolver != null)
          this.parser.setEntityResolver(this.resolver);
        this.parser.parse(in);
    }

    /**
     * Create a new Document object.
     */
    public Document newDocument() {
        return(newDocument(null,null,null));
    }

    /**
     * Create a new Document object with a specified DOCTYPE.
     */
    public Document newDocument(String name) {
        return(newDocument(name,null,null));
    }

    /**
     * Create a new Document object with a specified DOCTYPE, public ID and
     * system ID.
     */
    public Document newDocument(String name, String pub, String sys) {
        DocumentImpl doc=new DocumentImpl();
        if ((pub!=null)||(sys!=null)) {
            DocumentTypeImpl dtd=new DocumentTypeImpl(doc,name,pub,sys);
            doc.appendChild(dtd);
        } else if (name!=null) {
            DocumentTypeImpl dtd=new DocumentTypeImpl(doc,name);
            doc.appendChild(dtd);
        }
        return(doc);
    }

    /**
     * Parses a new Document object from the given InputSource.
     */
    public Document parseDocument(InputSource input) throws SAXException, IOException {
        DOMParser parser = null;

        try {
            parser = new DOMParser();

            parser.setFeature("http://xml.org/sax/features/validation",false);
            parser.setFeature("http://xml.org/sax/features/namespaces",true);

            parser.parse(input);
        } catch (Exception pce) {
            getLogger().error("Could not build DocumentBuilder", pce);
            return null;
        }

        return parser.getDocument();
    }

    /**
     * Receive notification of a recoverable error.
     */
    public void error(SAXParseException e)
    throws SAXException {
        throw new SAXException("Error parsing "+e.getSystemId()+" (line "+
                               e.getLineNumber()+" col. "+e.getColumnNumber()+
                               "): "+e.getMessage(),e);
    }

    /**
     * Receive notification of a fatal error.
     */
    public void fatalError(SAXParseException e)
    throws SAXException {
        throw new SAXException("Fatal error parsing "+e.getSystemId()+" (line "+
                               e.getLineNumber()+" col. "+e.getColumnNumber()+
                               "): "+e.getMessage(),e);
    }

    /**
     * Receive notification of a warning.
     */
    public void warning(SAXParseException e)
    throws SAXException {
        throw new SAXException("Warning parsing "+e.getSystemId()+" (line "+
                               e.getLineNumber()+" col. "+e.getColumnNumber()+
                               "): "+e.getMessage(),e);
    }
}
