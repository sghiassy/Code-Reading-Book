/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.xml;

import org.xml.sax.Attributes;
import org.xml.sax.ContentHandler;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;
import org.xml.sax.ext.LexicalHandler;

/**
 * A special purpose <code>XMLConsumer</code> used for including files.
 * It basically ignores the <code>startDocument</code>,
 * </code>endDocument</code>, <code>startDTD</code> and <code>endDTD</code>
 * messages.
 *
 * @author <a href="mailto:bloritsch@apache.org>Berin Loritsch</a>
 * @author <a href="mailto:cziegeler@apache.org>Carsten Ziegeler</a>
 * @version $Revision: 1.1 $ $Date: 2002/01/03 12:31:22 $
 */
public class IncludeXMLConsumer extends AbstractXMLConsumer {
    final private ContentHandler contentHandler;
    final private LexicalHandler lexicalHandler;

    public IncludeXMLConsumer (XMLConsumer consumer) {
        this.contentHandler = consumer;
        this.lexicalHandler = consumer;
    }

    public IncludeXMLConsumer (ContentHandler contentHandler, LexicalHandler lexicalHandler) {
        this.contentHandler = contentHandler;
        this.lexicalHandler = lexicalHandler;
    }

    public IncludeXMLConsumer (ContentHandler contentHandler) {
        this.contentHandler = contentHandler;
        this.lexicalHandler = null;
    }

    public void setDocumentLocator(Locator loc) {
        this.contentHandler.setDocumentLocator(loc);
    }

    public void startDocument() throws SAXException {
        // Ignored
    }

    public void endDocument() throws SAXException {
        // Ignored
    }

    public void startPrefixMapping(String prefix, String uri) throws SAXException {
        this.contentHandler.startPrefixMapping(prefix, uri);
    }

    public void endPrefixMapping(String prefix) throws SAXException {
        this.contentHandler.endPrefixMapping(prefix);
    }

    public void startElement(String uri, String local, String qName, Attributes attr) throws SAXException {
        this.contentHandler.startElement(uri, local, qName, attr);
    }

    public void endElement(String uri, String local, String qName) throws SAXException {
        this.contentHandler.endElement(uri, local, qName);
    }

    public void characters(char[] ch, int start, int end) throws SAXException {
        this.contentHandler.characters(ch, start, end);
    }

    public void ignorableWhitespace(char[] ch, int start, int end) throws SAXException {
        this.contentHandler.ignorableWhitespace(ch, start, end);
    }

    public void processingInstruction(String name, String value) throws SAXException {
        this.processingInstruction(name, value);
    }

    public void skippedEntity(String ent) throws SAXException {
        this.skippedEntity(ent);
    }

    public void startDTD(String name, String public_id, String system_id)
    throws SAXException {
        // Ignored
    }

    public void endDTD() throws SAXException {
        // Ignored
    }

    public void startEntity(String name) throws SAXException {
        if(lexicalHandler != null)
            lexicalHandler.startEntity(name);
    }

    public void endEntity(String name) throws SAXException {
        if(lexicalHandler != null)
            lexicalHandler.endEntity(name);
    }

    public void startCDATA() throws SAXException {
        if(lexicalHandler != null)
            lexicalHandler.startCDATA();
    }

    public void endCDATA() throws SAXException {
        if(lexicalHandler != null)
            lexicalHandler.endCDATA();
    }

    public void comment(char ary[], int start, int length)
        throws SAXException {
        if(lexicalHandler != null)
            lexicalHandler.comment(ary,start,length);
    }
}
