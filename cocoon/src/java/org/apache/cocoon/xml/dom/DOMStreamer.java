/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.xml.dom;

import org.apache.cocoon.xml.AbstractXMLProducer;
import org.apache.cocoon.xml.EmbeddedXMLPipe;
import org.apache.cocoon.xml.XMLConsumer;
import org.w3c.dom.Node;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.ext.LexicalHandler;

import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.sax.SAXResult;

/**
 * The <code>DOMStreamer</code> is a utility class that will generate SAX
 * events from a W3C DOM Document.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:23 $
 */
public class DOMStreamer extends AbstractXMLProducer {

    /** The transformer factory shared by all instances */
    protected static TransformerFactory factory = TransformerFactory.newInstance();

    /** The private transformer for this instance */
    protected Transformer transformer;

    /**
     * Create a new <code>DOMStreamer</code> instance.
     */
    public DOMStreamer() {
        super();
    }

    /**
     * Create a new <code>DOMStreamer</code> instance.
     */
    public DOMStreamer(XMLConsumer consumer) {
        this(consumer, consumer);
    }

    /**
     * Create a new <code>DOMStreamer</code> instance.
     */
    public DOMStreamer(ContentHandler content) {
        this(content,null);
    }

    /**
     * Create a new <code>DOMStreamer</code> instance.
     */
    public DOMStreamer(ContentHandler content, LexicalHandler lexical) {
        this();
        super.setContentHandler(content);
        super.setLexicalHandler(lexical);
    }

    /**
     * Start the production of SAX events.
     */
    public void stream(Node node)
    throws SAXException {
        if (this.transformer == null) {
            try {
                this.transformer = factory.newTransformer();
            } catch (TransformerConfigurationException e) {
                getLogger().error("DOMStreamer", e);
                throw new SAXException(e);
            }
        }
        DOMSource source = new DOMSource(node);
        
        ContentHandler handler;
        if (node.getNodeType() == Node.DOCUMENT_NODE) {
            // Pass all SAX events
            handler = super.contentHandler;
        } else {
            // Strip start/endDocument
            handler = new EmbeddedXMLPipe(super.contentHandler);
        }
        
        SAXResult result = new SAXResult(handler);
        result.setLexicalHandler(super.lexicalHandler);

        try {
            transformer.transform(source, result);
        } catch (TransformerException e) {
            getLogger().error("DOMStreamer", e);
            throw new SAXException(e);
        }
    }
}
