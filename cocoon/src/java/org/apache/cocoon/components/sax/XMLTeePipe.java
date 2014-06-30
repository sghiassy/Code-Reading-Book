/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.sax;

import org.apache.cocoon.xml.XMLConsumer;
import org.apache.cocoon.xml.XMLPipe;
import org.apache.cocoon.xml.XMLProducer;
import org.xml.sax.Attributes;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;


/**
 * This is a simple Tee Component.
 * The incoming events are forwarded to two other components.
 *
 * @author <a href="mailto:cziegeler@sundn.de">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:13 $
 */

public final class XMLTeePipe
implements XMLPipe {

    /**
     * Set the <code>XMLConsumer</code> that will receive XML data.
     */
    public void setConsumer(XMLConsumer consumer) {
        ((XMLProducer)this.firstConsumer).setConsumer(consumer);
    }

    private XMLConsumer firstConsumer;
    private XMLConsumer secondConsumer;

    /**
     * Create a new XMLTeePipe with two consumers
     */
    public XMLTeePipe(XMLConsumer firstPipe,
                      XMLConsumer secondConsumer) {
        this.firstConsumer = firstPipe;
        this.secondConsumer = secondConsumer;
    }

    public void recycle() {
        this.firstConsumer = null;
        this.secondConsumer = null;
    }

    public void startDocument() throws SAXException {
        this.firstConsumer.startDocument();
        this.secondConsumer.startDocument();
    }

    public void endDocument() throws SAXException {
        this.firstConsumer.endDocument();
        this.secondConsumer.endDocument();
    }

    public void startPrefixMapping(String prefix, String uri) throws SAXException {
        this.firstConsumer.startPrefixMapping(prefix, uri);
        this.secondConsumer.startPrefixMapping(prefix, uri);
    }

    public void endPrefixMapping(String prefix) throws SAXException {
        this.firstConsumer.endPrefixMapping(prefix);
        this.secondConsumer.endPrefixMapping(prefix);
    }

    public void startElement(String namespaceURI, String localName, String qName, Attributes atts)
    throws SAXException {
        this.firstConsumer.startElement(namespaceURI, localName, qName, atts);
        this.secondConsumer.startElement(namespaceURI, localName, qName, atts);
    }

    public void endElement(String namespaceURI, String localName, String qName)
    throws SAXException {
        this.firstConsumer.endElement(namespaceURI, localName, qName);
        this.secondConsumer.endElement(namespaceURI, localName, qName);
    }

    public void characters(char[] ch, int start, int length) throws SAXException {
        this.firstConsumer.characters(ch, start, length);
        this.secondConsumer.characters(ch, start, length);
    }

    public void ignorableWhitespace(char[] ch, int start, int length) throws SAXException {
        this.firstConsumer.ignorableWhitespace(ch, start, length);
        this.secondConsumer.ignorableWhitespace(ch, start, length);
    }

    public void processingInstruction(String target, String data) throws SAXException {
        this.firstConsumer.processingInstruction(target, data);
        this.secondConsumer.processingInstruction(target, data);
    }

    public void setDocumentLocator(Locator locator) {
        this.firstConsumer.setDocumentLocator(locator);
        this.secondConsumer.setDocumentLocator(locator);
    }

    public void skippedEntity(String name) throws SAXException {
        this.firstConsumer.skippedEntity(name);
        this.secondConsumer.skippedEntity(name);
    }

    public void startDTD(String name, String public_id, String system_id)
    throws SAXException {
        this.firstConsumer.startDTD(name, public_id, system_id);
        this.secondConsumer.startDTD(name, public_id, system_id);
    }

    public void endDTD() throws SAXException {
        this.firstConsumer.endDTD();
        this.secondConsumer.endDTD();
    }

    public void startEntity(String name) throws SAXException {
        this.firstConsumer.startEntity(name);
        this.secondConsumer.startEntity(name);
    }

    public void endEntity(String name) throws SAXException {
        this.firstConsumer.endEntity(name);
        this.secondConsumer.endEntity(name);
    }

    public void startCDATA() throws SAXException {
        this.firstConsumer.startCDATA();
        this.secondConsumer.startCDATA();
    }

    public void endCDATA() throws SAXException {
        this.firstConsumer.endCDATA();
        this.secondConsumer.endCDATA();
    }

    public void comment(char ary[], int start, int length)
    throws SAXException {
        this.firstConsumer.comment(ary, start, length);
        this.secondConsumer.comment(ary, start, length);
    }

}
