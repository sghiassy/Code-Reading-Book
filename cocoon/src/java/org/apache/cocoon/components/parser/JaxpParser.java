/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.parser;

import org.apache.avalon.excalibur.pool.Poolable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.parameters.ParameterException;
import org.apache.cocoon.components.resolver.Resolver;
import org.apache.cocoon.util.ClassUtils;
import org.apache.cocoon.xml.AbstractXMLProducer;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.Document;
import org.xml.sax.*;

import javax.xml.parsers.*;
import java.io.IOException;

/**
 * An XMLParser that is only dependant on JAXP 1.1 compliant parsers.
 *
 * The configuration can contain the following parameters :
 * <ul>
 * <li>validate (boolean, default = <code>false</code>) : should the parser
 *     validate parsed documents ?
 * </li>
 * <li>namespace-prefixes (boolean, default = <code>false</code>) : do we want
 *     namespaces declarations also as 'xmlns:' attributes ?<br>
 *     <i>Note</i> : setting this to <code>true</code> confuses some XSL
 *     processors (e.g. Saxon).
 * </li>
 * <li>reuse-parsers (boolean, default = <code>true</code>) : do we want to reuse
 *     parsers or create a new parser for each parse ?<br>
 *     <i>Note</i> : even if this parameter is <code>true</code>, parsers are not
 *     recycled in case of parsing errors : some parsers (e.g. Xerces) don't like
 *     to be reused after failure.
 * </li>
 * <li>sax-parser-factory (string, optional) : the name of the <code>SAXParserFactory</code>
 *     implementation class to be used instead of using the standard JAXP mechanism
 *     (<code>SAXParserFactory.newInstance()</code>). This allows to choose
 *     unambiguously the JAXP implementation to be used when several of them are
 *     available in the classpath.
 * </li>
 * <li>document-builder-factory (string, optional) : the name of the
 *     <code>DocumentBuilderFactory</code> implementation to be used (similar to
 *     <code>sax-parser-factory</code> for DOM).
 * </li>
 * </ul>
 *
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.4 $ $Date: 2002/01/25 03:39:41 $
 */
public class JaxpParser extends AbstractXMLProducer
implements Parser, ErrorHandler, Composable, Parameterizable, Disposable, Poolable {

    /** the SAX Parser factory */
    protected SAXParserFactory factory;

    /** the Document Builder factory */
    protected DocumentBuilderFactory docFactory;

    /** The SAX reader. It is created lazily by {@link #setupXMLReader()}
        and cleared if a parsing error occurs. */
    protected XMLReader reader;

    /** The DOM builder. It is created lazily by {@link #setupDocumentBuilder()}
        and cleared if a parsing error occurs. */
    protected DocumentBuilder docBuilder;

    /** the component manager */
    protected ComponentManager manager;

    /** the Entity Resolver */
    protected Resolver resolver;

    /** do we want namespaces also as attributes ? */
    protected boolean nsPrefixes;

    /** do we want to reuse parsers ? */
    protected boolean reuseParsers;

    /**
     * Get the Entity Resolver from the component manager
     */
    public void compose(ComponentManager manager)
    throws ComponentException {
        this.manager = manager;
        if ( manager.hasComponent( Resolver.ROLE ) ) {
            getLogger().debug("Looking up " + Resolver.ROLE);
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

    /**
     * Configure
     */
    public void parameterize(Parameters params)
    throws ParameterException  {
        // Validation and namespace prefixes parameters
        boolean validate = params.getParameterAsBoolean("validate", false);
        this.nsPrefixes = params.getParameterAsBoolean("namespace-prefixes", false);
        this.reuseParsers = params.getParameterAsBoolean("reuse-parsers", true);

        // Get the SAXFactory
        String className = params.getParameter("sax-parser-factory", null);
        if (className == null) {
            factory = SAXParserFactory.newInstance();
        } else {
            // Will use specific class
            try {
                Class factoryClass = ClassUtils.loadClass(className);
                factory = (SAXParserFactory)factoryClass.newInstance();
            } catch(Exception e) {
                throw new ParameterException("Cannot load SAXParserFactory class " + className, e);
            }
        }
        // FIXME(VG): This always prints "SAXParserFactory: null"
        // getLogger().debug("SAXParserFactory: " + ClassUtils.which(factory.getClass()));
        factory.setNamespaceAware(true);
        factory.setValidating(validate);

        // Get the DocumentFactory
        className = params.getParameter("document-builder-factory", null);
        if (className == null) {
            this.docFactory = DocumentBuilderFactory.newInstance();
        } else {
            // Will use specific class
            try {
                Class factoryClass = ClassUtils.loadClass(className);
                this.docFactory = (DocumentBuilderFactory)factoryClass.newInstance();
            } catch(Exception e) {
                throw new ParameterException("Cannot load DocumentBuilderFactory class " + className, e);
            }
        }
        // FIXME(VG): This always prints "DocumentBuilderFactory: null"
        // getLogger().debug("DocumentBuilderFactory: " + ClassUtils.which(docFactory.getClass()));

        docFactory.setNamespaceAware(true);
        docFactory.setValidating(validate);
    }

    public void parse(InputSource in)
    throws SAXException, IOException {
        setupXMLReader();
        try {
            this.reader.setProperty("http://xml.org/sax/properties/lexical-handler", super.lexicalHandler);
        } catch (SAXException e) {
            getLogger().warn("SAX2 driver does not support property: "+
                             "'http://xml.org/sax/properties/lexical-handler'");
        }

        this.reader.setErrorHandler(this);
        this.reader.setContentHandler(super.contentHandler);
        if(this.resolver != null) {
            reader.setEntityResolver(this.resolver);
        }

        // Ensure we will use a fresh new parser at next parse in case of failure
        XMLReader tmpReader = this.reader;
        this.reader = null;

        tmpReader.parse(in);

        // Here, parsing was successful : restore this.reader
        if (this.reuseParsers)
            this.reader = tmpReader;
    }

    /**
     * Create a new Document object.
     */
    public Document newDocument() {
        setupDocumentBuilder();
        return this.docBuilder.newDocument();
    }

    /**
     * Create a new Document object with a specified DOCTYPE.
     */
    public Document newDocument(String name) {
        return this.newDocument(name, null, null);
    }

    /**
     * Create a new Document object with a specified DOCTYPE, public ID and
     * system ID.
     */
    public Document newDocument(String name, String publicId, String systemId) {
        setupDocumentBuilder();
        // Fixme: is there a better way to achieve this?
        DOMImplementation impl = this.docBuilder.newDocument().getImplementation();
        return impl.createDocument(
            null,
            name,
            impl.createDocumentType(name, publicId, systemId)
        );
    }

    /**
     * Parses a new Document object from the given InputSource.
     */
    public Document parseDocument(InputSource input) throws SAXException, IOException {
        setupDocumentBuilder();

        // Ensure we will use a fresh new parser at next parse in case of failure
        DocumentBuilder tmpBuilder = this.docBuilder;
        this.docBuilder = null;

        Document result = tmpBuilder.parse(input);

        // Here, parsing was successful : restore this.builder
        if (this.reuseParsers)
            this.docBuilder = tmpBuilder;

        return result;
    }

    /**
     * Creates a new <code>XMLReader</code> if needed.
     */
    protected void setupXMLReader() throws SAXException {
        if (this.reader == null) {
            // Create the XMLReader
            try {
                this.reader = factory.newSAXParser().getXMLReader();
                this.reader.setFeature("http://xml.org/sax/features/namespace-prefixes", nsPrefixes);
            } catch(Exception e) {
                getLogger().error("Cannot produce a valid parser", e);
                throw new SAXException("Cannot produce a valid parser", e);
            }
        }
    }

    /**
     * Creates a new <code>DocumentBuilder</code> if needed.
     */
    protected void setupDocumentBuilder() {
        if (this.docBuilder == null) {
            try {
                this.docBuilder = this.docFactory.newDocumentBuilder();
            } catch (ParserConfigurationException pce) {
                getLogger().error("Could not create DocumentBuilder", pce);
                throw new org.apache.avalon.framework.CascadingRuntimeException(
                    "Could not create DocumentBuilder", pce);
            }
        }
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
