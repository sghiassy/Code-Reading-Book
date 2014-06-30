/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.sax;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.component.Component;
import org.xml.sax.Attributes;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;

import java.util.HashMap;

/**
 * This a simple xml compiler which outputs a byte array.
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:13 $
 */

public final class XMLByteStreamCompiler
implements XMLSerializer, Component, Recyclable {

    private HashMap map;
    private int     count;

    /** The buffer for the compile xml byte stream. */
    private byte buf[];

    /** The number of valid bytes in the buffer. */
    private int bufCount;

    public XMLByteStreamCompiler() {
        this.map = new HashMap();
        this.initOutput();
    }

    private void initOutput() {
        this.count = 0;
        this.map.clear();
        this.buf = new byte[2000];
        this.buf[0] = (byte)'C';
        this.buf[1] = (byte)'X';
        this.buf[2] = (byte)'M';
        this.buf[3] = (byte)'L';
        this.buf[4] = (byte)1;
        this.buf[5] = (byte)0;
        this.bufCount = 6;
    }

    public void recycle() {
        this.initOutput();
    }

    private static final int START_DOCUMENT         = 0;
    private static final int END_DOCUMENT           = 1;
    private static final int START_PREFIX_MAPPING   = 2;
    private static final int END_PREFIX_MAPPING     = 3;
    private static final int START_ELEMENT          = 4;
    private static final int END_ELEMENT            = 5;
    private static final int CHARACTERS             = 6;
    private static final int IGNORABLE_WHITESPACE   = 7;
    private static final int PROCESSING_INSTRUCTION = 8;
    private static final int COMMENT                = 9;
    private static final int LOCATOR                = 10;


    public Object getSAXFragment() {
        byte newbuf[] = new byte[this.bufCount];
        System.arraycopy(this.buf, 0, newbuf, 0, this.bufCount);
        return newbuf;
    }

    public void startDocument() throws SAXException {
        this.writeEvent(START_DOCUMENT);
    }

    public void endDocument() throws SAXException {
        this.writeEvent(END_DOCUMENT);
    }

    public void startPrefixMapping(java.lang.String prefix, java.lang.String uri)
    throws SAXException {
        this.writeEvent(START_PREFIX_MAPPING);
        this.writeString(prefix);
        this.writeString(uri);
    }

    public void endPrefixMapping(String prefix) throws SAXException {
       this.writeEvent(END_PREFIX_MAPPING);
       this.writeString(prefix);
    }

    public void startElement(String namespaceURI, String localName, String qName, Attributes atts)
    throws SAXException {
        int length = atts.getLength();
        this.writeEvent(START_ELEMENT);
        this.writeAttributes(length);
        for (int i = 0; i < length; i++) {
            this.writeString(atts.getURI(i));
            this.writeString(atts.getLocalName(i));
            this.writeString(atts.getQName(i));
            this.writeString(atts.getType(i));
            this.writeString(atts.getValue(i));
         }
         this.writeString((namespaceURI == null ? "" : namespaceURI));
         this.writeString(localName);
         this.writeString(qName);
     }

    public void endElement(String namespaceURI, String localName, String qName)
    throws SAXException {
        this.writeEvent(END_ELEMENT);
        this.writeString((namespaceURI == null ? "" : namespaceURI));
        this.writeString(localName);
        this.writeString(qName);
    }

    public void characters(char[] ch, int start, int length)
    throws SAXException {
        this.writeEvent(CHARACTERS);
        this.writeChars(ch, start, length);
    }

    public void ignorableWhitespace(char[] ch, int start, int length)
    throws SAXException {
        this.writeEvent(IGNORABLE_WHITESPACE);
        this.writeChars(ch, start, length);
    }

    public void processingInstruction(String target, String data)
    throws SAXException {
        this.writeEvent(PROCESSING_INSTRUCTION);
        this.writeString(target);
        this.writeString(data);
    }

    public void setDocumentLocator(Locator locator) {
        try {
            this.writeEvent(LOCATOR);
            String publicId = locator.getPublicId();
            String systemId = locator.getSystemId();
            this.writeString(publicId!=null?publicId:"");
            this.writeString(systemId!=null?systemId:"");
            this.write(locator.getLineNumber());
            this.write(locator.getColumnNumber());
        } catch (Exception e) {
             throw new RuntimeException(e.toString());
        }
    }

    public void skippedEntity(java.lang.String name) throws SAXException {
        // ignore.
    }

    /**
     * SAX Event Handling: LexicalHandler
     */
    public void startDTD(String name, String public_id, String system_id) {
        // ignore
    }

    /**
     * SAX Event Handling: LexicalHandler
     */
    public void endDTD() throws SAXException {
        // ignore
    }

    /**
     * SAX Event Handling: LexicalHandler
     */
    public void startEntity(String name) throws SAXException {
        // ignore
    }

    /**
     * SAX Event Handling: LexicalHandler
     */
    public void endEntity(String name) throws SAXException {
        // ignore
    }

    /**
     * SAX Event Handling: LexicalHandler
     */
    public void startCDATA() throws SAXException {
        // ignore
    }

    /**
     * SAX Event Handling: LexicalHandler
     */
    public void endCDATA() throws SAXException {
        // ignore
    }


    /**
     * SAX Event Handling: LexicalHandler
     */
    public void comment(char ary[], int start, int length)
    throws SAXException {
        try {
            this.writeEvent(COMMENT);
            this.writeChars(ary, start, length);
       } catch (Exception e) {
            throw new SAXException(e);
       }
    }

    public final void writeEvent(int event) throws SAXException {
        this.write(event);
    }

    public final void writeAttributes(int attributes) throws SAXException {
        this.write((attributes >>> 8) & 0xFF);
        this.write((attributes >>> 0) & 0xFF);
    }

    public final void writeString(String str) throws SAXException {
        Integer index = (Integer) map.get(str);
        if (index == null) {
            int length = str.length();
            map.put(str, new Integer(count++));
            this.writeChars(str.toCharArray(), 0, length);
        } else {
            int i = index.intValue();
            this.write(((i >>> 8) & 0xFF) | 0x80);
            this.write((i >>> 0) & 0xFF);
        }
    }

    public final void writeChars(char[] ch, int start, int length)
    throws SAXException {
        int utflen = 0;
        int c, count = 0;

        for (int i = 0; i < length; i++) {
            c = ch[i + start];
            if ((c >= 0x0001) && (c <= 0x007F)) {
                utflen++;
            } else if (c > 0x07FF) {
                utflen += 3;
            } else {
                utflen += 2;
            }
        }

        if (utflen > 0x00007FFF)
            throw new SAXException("UTFDataFormatException: String cannot be longer than 32k.");

        byte[] bytearr = new byte[utflen+2];
        bytearr[count++] = (byte) ((utflen >>> 8) & 0xFF);
        bytearr[count++] = (byte) ((utflen >>> 0) & 0xFF);
        for (int i = 0; i < length; i++) {
            c = ch[i + start];
            if ((c >= 0x0001) && (c <= 0x007F)) {
                bytearr[count++] = (byte) c;
            } else if (c > 0x07FF) {
                bytearr[count++] = (byte) (0xE0 | ((c >> 12) & 0x0F));
                bytearr[count++] = (byte) (0x80 | ((c >>  6) & 0x3F));
                bytearr[count++] = (byte) (0x80 | ((c >>  0) & 0x3F));
            } else {
                bytearr[count++] = (byte) (0xC0 | ((c >>  6) & 0x1F));
                bytearr[count++] = (byte) (0x80 | ((c >>  0) & 0x3F));
            }
        }

        this.write(bytearr);
    }

    private void write(byte[] b) {
        int len = b.length;
        if (len == 0) return;
        int newcount = this.bufCount + len;
        if (newcount > this.buf.length) {
            byte newbuf[] = new byte[Math.max(this.buf.length << 1, newcount)];
            System.arraycopy(this.buf, 0, newbuf, 0, this.bufCount);
            this.buf = newbuf;
        }
        System.arraycopy(b, 0, this.buf, this.bufCount, len);
        this.bufCount = newcount;
    }

    private void write(int b) {
        int newcount = this.bufCount + 1;
        if (newcount > this.buf.length) {
            byte newbuf[] = new byte[Math.max(this.buf.length << 1, newcount)];
            System.arraycopy(this.buf, 0, newbuf, 0, this.bufCount);
            this.buf = newbuf;
        }
        this.buf[this.bufCount] = (byte)b;
        this.bufCount = newcount;
    }

}

