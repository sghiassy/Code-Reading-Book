/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.saxconnector;

import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.xml.AbstractXMLPipe;
import org.xml.sax.Attributes;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;

import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.sax.SAXTransformerFactory;
import javax.xml.transform.sax.TransformerHandler;
import javax.xml.transform.stream.StreamResult;
import java.io.IOException;
import java.io.StringWriter;
import java.io.Writer;
import java.util.Map;

/**
 * This SAX connector logs all intermediate sax streams to the log.
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:13 $
 */
public class LoggingSAXConnector
extends AbstractXMLPipe
implements SAXConnector {

    /** The writer getting the xml */
    protected Writer writer;

    /** The SAX Transformer factory */
    protected static SAXTransformerFactory transFactory =
            (SAXTransformerFactory)SAXTransformerFactory.newInstance();

    protected TransformerHandler handler;

    /** Setup this SAXConnector.
     */
    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters params)
    throws ProcessingException, SAXException, IOException {
        this.writer = new StringWriter();
        try {
            this.handler = transFactory.newTransformerHandler();
            this.handler.setResult(new StreamResult(writer));
            // handler.getTransformer().setOutputProperties(format);
        } catch (TransformerConfigurationException tce) {
            this.getLogger().debug("Unable to create new transformer.", tce);
            throw new ProcessingException("Unable to create new transformer.", tce);
        }
    }

    /** Recycle the SAXConnector
     */
    public void recycle() {
        // in case of any error we print out the xml content
        // as the endDocument() was never reached
        if (this.writer != null) {
            this.getLogger().debug("XMLContent before : " + super.contentHandler);
            this.getLogger().debug(this.writer.toString());
        }
        super.recycle();
        this.writer = null;
        this.handler = null;
    }

    public void setDocumentLocator(Locator locator) {
        this.handler.setDocumentLocator(locator);
        super.setDocumentLocator(locator);
    }

    public void startDocument()
    throws SAXException {
        this.handler.startDocument();
        super.startDocument();
    }

    public void endDocument()
    throws SAXException {
        this.handler.endDocument();
        super.endDocument();
        if (this.writer != null) {
            this.getLogger().debug("XMLContent before : " + super.contentHandler);
            this.getLogger().debug(this.writer.toString());
            this.writer = null;
        }
    }

    public void startPrefixMapping(String prefix, String uri)
    throws SAXException {
        this.handler.startPrefixMapping(prefix, uri);
        super.startPrefixMapping(prefix, uri);
    }

    public void endPrefixMapping(String prefix)
    throws SAXException {
        this.handler.endPrefixMapping(prefix);
        super.endPrefixMapping(prefix);
    }

    public void startElement(String namespace, String name, String raw,
                         Attributes attr)
    throws SAXException {
        this.handler.startElement(namespace, name, raw, attr);
        super.startElement(namespace, name, raw, attr);
    }

    public void endElement(String namespace, String name, String raw)
    throws SAXException {
        this.handler.endElement(namespace, name, raw);
        super.endElement(namespace, name, raw);
    }

    public void characters(char ary[], int start, int length)
    throws SAXException {
        this.handler.characters(ary, start, length);
        super.characters(ary, start, length);
    }

    public void ignorableWhitespace(char ary[], int start, int length)
    throws SAXException {
        this.handler.ignorableWhitespace(ary, start, length);
        super.ignorableWhitespace(ary, start, length);
    }

    public void processingInstruction(String target, String data)
    throws SAXException {
        this.handler.processingInstruction(target, data);
        super.processingInstruction(target, data);
    }

    public void skippedEntity(String name)
    throws SAXException {
        this.handler.skippedEntity(name);
        super.skippedEntity(name);
    }

    public void startDTD(String name, String public_id, String system_id)
    throws SAXException {
        this.handler.startDTD(name, public_id, system_id);
        super.startDTD(name, public_id, system_id);
    }

    public void endDTD() throws SAXException {
        this.handler.endDTD();
        super.endDTD();
    }

    public void startEntity(String name) throws SAXException {
        this.handler.startEntity(name);
        super.startEntity(name);
    }

    public void endEntity(String name) throws SAXException {
        this.handler.endEntity(name);
        super.endEntity(name);
    }

    public void startCDATA() throws SAXException {
        this.handler.startCDATA();
        super.startCDATA();
    }

    public void endCDATA() throws SAXException {
        this.handler.endCDATA();
        super.endCDATA();
    }

    public void comment(char ary[], int start, int length)
    throws SAXException {
        this.handler.comment(ary, start, length);
        super.comment(ary, start, length);
    }

}
