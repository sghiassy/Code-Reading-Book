/*
 *  Copyright (C) The Apache Software Foundation. All rights reserved.        *
 *  ------------------------------------------------------------------------- *
 *  This software is published under the terms of the Apache Software License *
 *  version 1.1, a copy of which has been included  with this distribution in *
 *  the LICENSE file.                                                         *
 */
package org.apache.cocoon.components.search;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Stack;

import org.apache.lucene.document.DateField;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;

import org.xml.sax.Attributes;
import org.xml.sax.ContentHandler;
import org.xml.sax.InputSource;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

/**
 * Parse XML and generate lucene document(s)
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Id: LuceneIndexContentHandler.java,v 1.3 2002/01/25 04:34:18 vgritsenko Exp $
 */
public class LuceneIndexContentHandler implements ContentHandler
{
    public static final String LUCENE_URI = "http://apache.org/cocoon/lucene/1.0";

    /** If this attribute is specified on element, values of all attributes
     * if this element added to the text of the element, and to the document
     * body text */
    public static final String LUCENE_ATTR_TO_TEXT_ATTRIBUTE = "text-attr";

    StringBuffer bodyText;
    private List documents;
    private Document bodyDocument;
    private Stack elementStack;

    /**
     *Constructor for the LuceneIndexContentHandler object
     *
     * @since
     */
    public LuceneIndexContentHandler() {
        this.bodyText = new StringBuffer();
        this.bodyDocument = new Document();
        this.documents = new ArrayList();
        this.documents.add(this.bodyDocument);
        this.elementStack = new Stack();
    }

    /**
     *Sets the documentLocator attribute of the LuceneIndexContentHandler object
     *
     * @param  locator  The new documentLocator value
     * @since
     */
    public void setDocumentLocator(Locator locator) { }


    /**
     *Description of the Method
     *
     * @return    Description of the Returned Value
     * @since
     */
    public List allDocuments() {
        return documents;
    }


    /**
     *Description of the Method
     *
     * @return    Description of the Returned Value
     * @since
     */
    public Iterator iterator() {
        return documents.iterator();
    }


    /**
     *Description of the Method
     *
     * @param  ch      Description of Parameter
     * @param  start   Description of Parameter
     * @param  length  Description of Parameter
     * @since
     */
    public void characters(char[] ch, int start, int length) {

        if (ch.length > 0 && start >= 0 && length > 1) {
            String text = new String(ch, start, length);
            if (elementStack.size() > 0) {
                IndexHelperField tos = (IndexHelperField) elementStack.peek();
                tos.appendText(text);
            }
            bodyText.append(text);
        }
    }


    /**
     *Description of the Method
     *
     * @since
     */
    public void endDocument() {
        /*
         *  empty
         */
        bodyDocument.add(Field.UnStored(LuceneXMLIndexer.BODY_FIELD, bodyText.toString()));
    }


    /**
     *Description of the Method
     *
     * @param  namespaceURI  Description of Parameter
     * @param  localName     Description of Parameter
     * @param  qName         Description of Parameter
     * @since
     */
    public void endElement(String namespaceURI, String localName, String qName) {
        IndexHelperField tos = (IndexHelperField) elementStack.pop();
        String lname = tos.getLocalFieldName();
        StringBuffer text = tos.getText();

        // (VG): Atts are never null, see startElement
        Attributes atts = tos.getAttributes();
        boolean attributesToText = atts.getIndex(LUCENE_URI, LUCENE_ATTR_TO_TEXT_ATTRIBUTE) != -1;
        for (int i = 0; i < atts.getLength(); i++) {
            if (LUCENE_URI.equals(atts.getURI(i))) continue;

            String atts_lname = atts.getLocalName(i);
            String atts_value = atts.getValue(i);
            bodyDocument.add(Field.UnStored(lname + "@" + atts_lname, atts_value));
            if (attributesToText) {
                text.append(atts_value);
                text.append(' ');
                bodyText.append(atts_value);
                bodyText.append(' ');
            }
        }

        if (text != null && text.length() > 0) {
            bodyDocument.add(Field.UnStored(lname, text.toString()));
        }
    }


    /**
     *Description of the Method
     *
     * @param  prefix  Description of Parameter
     * @since
     */
    public void endPrefixMapping(String prefix) { }


    /**
     *Description of the Method
     *
     * @param  ch      Description of Parameter
     * @param  start   Description of Parameter
     * @param  length  Description of Parameter
     * @since
     */
    public void ignorableWhitespace(char[] ch, int start, int length) { }


    /**
     *Description of the Method
     *
     * @param  target  Description of Parameter
     * @param  data    Description of Parameter
     * @since
     */
    public void processingInstruction(String target, String data) { }


    /**
     *Description of the Method
     *
     * @param  name  Description of Parameter
     * @since
     */
    public void skippedEntity(String name) { }


    /**
     *Description of the Method
     *
     * @since
     */
    public void startDocument() { }


    /**
     *Description of the Method
     *
     * @param  namespaceURI  Description of Parameter
     * @param  localName     Description of Parameter
     * @param  qName         Description of Parameter
     * @param  atts          Description of Parameter
     * @since
     */
    public void startElement(String namespaceURI, String localName, String qName, Attributes atts) {
        IndexHelperField ihf = new IndexHelperField(localName, qName, new AttributesImpl(atts));
        elementStack.push(ihf);
    }


    /**
     *Description of the Method
     *
     * @param  prefix  Description of Parameter
     * @param  uri     Description of Parameter
     * @since
     */
    public void startPrefixMapping(String prefix, String uri) { }
}
