/**
 ****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 ****************************************************************************
 */
package org.apache.cocoon.xml;

import org.xml.sax.Attributes;
import org.xml.sax.ContentHandler;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;
import org.xml.sax.ext.LexicalHandler;

/**
 * @author <a href="mailto:cziegeler@sundn.de">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:22 $
 */

public final class XMLMulticaster implements XMLConsumer {

    /**
     * The XMLMulticaster forwards incomming sax events to a list of
     * receiving objects.
     */
    private ContentHandler[] contentHandlerList;
    private LexicalHandler[] lexicalHandlerList;

    /**
     * Create a new XMLMulticaster with two consumers
     */
    public XMLMulticaster(XMLConsumer firstConsumer, XMLConsumer secondConsumer) {
        this.contentHandlerList = new ContentHandler[] {firstConsumer, secondConsumer};
        this.lexicalHandlerList = new LexicalHandler[] {firstConsumer, secondConsumer};
    }

    /**
     * Create a new XMLMulticaster from two contentHandler/lexicalHandler pairs
     */
    public XMLMulticaster(ContentHandler firstContentHandler,
                          LexicalHandler firstLexicalHandler,
                          ContentHandler secondContentHandler,
                          LexicalHandler secondLexicalHandler) {
        this.contentHandlerList = new ContentHandler[] {firstContentHandler, secondContentHandler};
        this.lexicalHandlerList = new LexicalHandler[] {firstLexicalHandler, secondLexicalHandler};
    }

    public XMLMulticaster(ContentHandler[] chList,
                          LexicalHandler[] lhList) {
        this.contentHandlerList = chList;
        this.lexicalHandlerList = lhList;
    }

    public void startDocument() throws SAXException {
        for(int i=0; i<this.contentHandlerList.length; i++) {
            this.contentHandlerList[i].startDocument();
        }
    }

    public void endDocument() throws SAXException {
        for(int i=0; i<this.contentHandlerList.length; i++) {
                this.contentHandlerList[i].endDocument();
        }
    }

    public void startPrefixMapping(java.lang.String prefix, java.lang.String uri) throws SAXException {
        for(int i=0; i<this.contentHandlerList.length; i++)
                this.contentHandlerList[i].startPrefixMapping(prefix, uri);
    }

    public void endPrefixMapping(java.lang.String prefix) throws SAXException {
        for(int i=0; i<this.contentHandlerList.length; i++)
                this.contentHandlerList[i].endPrefixMapping(prefix);
    }

    public void startElement(java.lang.String namespaceURI, java.lang.String localName, java.lang.String qName, Attributes atts) throws SAXException {
        for(int i=0; i<this.contentHandlerList.length; i++)
                this.contentHandlerList[i].startElement(namespaceURI, localName, qName, atts);
    }

    public void endElement(java.lang.String namespaceURI, java.lang.String localName, java.lang.String qName) throws SAXException {
        for(int i=0; i<this.contentHandlerList.length; i++)
                this.contentHandlerList[i].endElement(namespaceURI, localName, qName);
    }

    public void characters(char[] ch, int start, int length) throws SAXException {
        for(int i=0; i<this.contentHandlerList.length; i++)
                this.contentHandlerList[i].characters(ch, start, length);
    }

    public void ignorableWhitespace(char[] ch, int start, int length) throws SAXException {
        for(int i=0; i<this.contentHandlerList.length; i++)
                this.contentHandlerList[i].ignorableWhitespace(ch, start, length);
    }

    public void processingInstruction(java.lang.String target, java.lang.String data) throws SAXException {
        for(int i=0; i<this.contentHandlerList.length; i++)
                this.contentHandlerList[i].processingInstruction(target, data);
    }

    public void setDocumentLocator(Locator locator) {
        for(int i=0; i<this.contentHandlerList.length; i++)
                this.contentHandlerList[i].setDocumentLocator(locator);
    }

    public void skippedEntity(java.lang.String name) throws SAXException {
        for(int i=0; i<this.contentHandlerList.length; i++)
                this.contentHandlerList[i].skippedEntity(name);
    }

    public void startDTD(String name, String public_id, String system_id)
                        throws SAXException {
        for(int i=0; i<this.lexicalHandlerList.length; i++)
            if (this.lexicalHandlerList[i] != null)
                this.lexicalHandlerList[i].startDTD(name, public_id, system_id);
    }

    public void endDTD() throws SAXException {
        for(int i=0; i<this.lexicalHandlerList.length; i++)
            if (this.lexicalHandlerList[i] != null)
                this.lexicalHandlerList[i].endDTD();
    }

    public void startEntity(String name) throws SAXException {
        for(int i=0; i<this.lexicalHandlerList.length; i++)
            if (this.lexicalHandlerList[i] != null)
                 this.lexicalHandlerList[i].startEntity(name);
    }

    public void endEntity(String name) throws SAXException {
        for(int i=0; i<this.lexicalHandlerList.length; i++)
            if (this.lexicalHandlerList[i] != null)
                this.lexicalHandlerList[i].endEntity(name);
    }

    public void startCDATA() throws SAXException {
        for(int i=0; i<this.lexicalHandlerList.length; i++)
            if (this.lexicalHandlerList[i] != null)
                this.lexicalHandlerList[i].startCDATA();
    }

    public void endCDATA() throws SAXException {
        for(int i=0; i<this.lexicalHandlerList.length; i++)
            if (this.lexicalHandlerList[i] != null)
                this.lexicalHandlerList[i].endCDATA();
    }

    public void comment(char ary[], int start, int length)
                        throws SAXException {
        for(int i=0; i<this.lexicalHandlerList.length; i++)
            if (this.lexicalHandlerList[i] != null)
                this.lexicalHandlerList[i].comment(ary, start, length);
    }
}
