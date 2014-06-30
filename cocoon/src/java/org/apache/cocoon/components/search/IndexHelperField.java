/*
 *  Copyright (C) The Apache Software Foundation. All rights reserved.        *
 *  ------------------------------------------------------------------------- *
 *  This software is published under the terms of the Apache Software License *
 *  version 1.1, a copy of which has been included  with this distribution in *
 *  the LICENSE file.                                                         *
 */
package org.apache.cocoon.components.search;
import org.apache.lucene.document.DateField;

import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.xml.sax.Attributes;

import org.xml.sax.ContentHandler;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;

/**
 * A helper class for generating a lucene document in a SAX ContentHandler.
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Id: IndexHelperField.java,v 1.2 2002/01/25 04:34:18 vgritsenko Exp $
 */
class IndexHelperField
{
    String localFieldName;
    String qualifiedFieldName;
    StringBuffer text;
    Attributes attributes;


    /**
     *Constructor for the IndexHelperField object
     *
     * @param  lfn   Description of Parameter
     * @param  qfn   Description of Parameter
     * @param  atts  Description of Parameter
     * @since
     */
    IndexHelperField(String lfn, String qfn, Attributes atts) {
        this.localFieldName = lfn;
        this.qualifiedFieldName = qfn;
        this.attributes = atts;
        this.text = new StringBuffer();
    }


    /**
     *Gets the localFieldName attribute of the IndexHelperField object
     *
     * @return    The localFieldName value
     * @since
     */
    public String getLocalFieldName() {
        return localFieldName;
    }


    /**
     *Gets the qualifiedFieldName attribute of the IndexHelperField object
     *
     * @return    The qualifiedFieldName value
     * @since
     */
    public String getQualifiedFieldName() {
        return qualifiedFieldName;
    }


    /**
     *Gets the attributes attribute of the IndexHelperField object
     *
     * @return    The attributes value
     * @since
     */
    public Attributes getAttributes() {
        return attributes;
    }


    /**
     *Gets the text attribute of the IndexHelperField object
     *
     * @return    The text value
     * @since
     */
    public StringBuffer getText() {
        return text;
    }


    /**
     *Description of the Method
     *
     * @param  text  Description of Parameter
     * @since
     */
    public void appendText(String text) {
        this.text.append(text);
    }


    /**
     *Description of the Method
     *
     * @param  str     Description of Parameter
     * @param  offset  Description of Parameter
     * @param  length  Description of Parameter
     * @since
     */
    public void appendText(char[] str, int offset, int length) {
        this.text.append(str, offset, length);
    }
}

