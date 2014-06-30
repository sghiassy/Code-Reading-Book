/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.xml.dom;

import org.apache.avalon.framework.logger.Loggable;
import org.apache.batik.css.CSSDocumentHandler;
import org.apache.batik.dom.svg.SAXSVGDocumentFactory;
import org.apache.batik.dom.svg.SVGDOMImplementation;
import org.apache.cocoon.xml.XMLConsumer;
import org.apache.log.Logger;
import org.w3c.dom.Document;
import org.xml.sax.SAXException;

/**
 * The <code>SVGBuilder</code> is a utility class that will generate a
 * SVG-DOM Document from SAX events using Batik's SVGDocumentFactory.
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:23 $
 */
public class SVGBuilder extends SAXSVGDocumentFactory implements XMLConsumer, Loggable {
    protected Logger log;

    private static final String SAX_PARSER
        = "org.apache.xerces.parsers.SAXParser";

    private final static String CSS_PARSER_CLASS_NAME =
        "org.apache.batik.css.parser.Parser";

    static {
        CSSDocumentHandler.setParserClassName(CSS_PARSER_CLASS_NAME);
    }

    /**
     * Construct a new instance of this TreeGenerator.
     */
    protected SVGBuilder() {
        super(SAX_PARSER);
    }

    public void setLogger(Logger logger) {
        if (this.log == null) {
            this.log = logger;
        }
    }

    /**
     * Return the newly built Document.
     */
    public Document getDocument() {
        return(this.document);
    }

    /**
     * Receive notification of the beginning of a document.
     *
     * @exception SAXException If this method was not called appropriately.
     */
    public void startDocument()
    throws SAXException {
        try {
            // Create SVG Document
            String namespaceURI = SVGDOMImplementation.SVG_NAMESPACE_URI;
            this.document = implementation.createDocument(namespaceURI, "svg", null);
            super.startDocument();
        } catch (Exception ex){
            log.error("SVGBuilder: startDocument", ex);
            ex.printStackTrace();
            throw new SAXException("SVGBuilder: startDocument", ex);
        }
    }

    /**
     * Receive notification of the beginning of a document.
     *
     * @exception SAXException If this method was not called appropriately.
     */
    public void endDocument ()
    throws SAXException {
        try {
            super.endDocument();

            // FIXME: Hack.
            ((org.apache.batik.dom.svg.SVGOMDocument)this.document).setURLObject(new java.net.URL("http://xml.apache.org"));

            this.notify(this.document);
        } catch (Exception ex){
            log.error("SVGBuilder: endDocument", ex);
            ex.printStackTrace();
            throw new SAXException("SVGBuilder: endDocument", ex);
        }
    }

    /**
     * Receive notification of a successfully completed DOM tree generation.
     */
    protected void notify(Document doc)
    throws SAXException {

    }
}
