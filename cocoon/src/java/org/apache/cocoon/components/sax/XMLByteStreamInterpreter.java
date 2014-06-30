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
import org.apache.cocoon.xml.AbstractXMLProducer;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.util.ArrayList;

/**
 * This a simple xml compiler which takes a byte array as input.
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/22 00:17:12 $
 */

public final class XMLByteStreamInterpreter
extends AbstractXMLProducer
implements XMLDeserializer, Component, Recyclable {

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

    private ArrayList list = new ArrayList();
    private byte[] input;
    private int currentPos;

    public void recycle() {
        super.recycle();
        this.list.clear();
    }

    public void deserialize(Object saxFragment)
    throws SAXException {
        if (!(saxFragment instanceof byte[])) {
            throw new SAXException("XMLDeserializer needs byte array for deserialization.");
        }
        this.input = (byte[])saxFragment;
        this.currentPos = 0;
        this.checkProlog();
        this.parse();
    }

    private void parse() throws SAXException {
        while ( currentPos < input.length) {
            switch (this.readEvent()) {
                case START_DOCUMENT:
                    contentHandler.startDocument();
                    break;
                case END_DOCUMENT:
                    contentHandler.endDocument();
                    break;
                case START_PREFIX_MAPPING:
                    contentHandler.startPrefixMapping(this.readString(), this.readString());
                    break;
                case END_PREFIX_MAPPING:
                    contentHandler.endPrefixMapping(this.readString());
                    break;
                case START_ELEMENT:
                    int attributes = this.readAttributes();
                    AttributesImpl atts = new AttributesImpl();
                    for (int i = 0; i < attributes; i++) {
                        atts.addAttribute(this.readString(), this.readString(), this.readString(), this.readString(), this.readString());
                    }
                    contentHandler.startElement(this.readString(), this.readString(), this.readString(), atts);
                    break;
                case END_ELEMENT:
                    contentHandler.endElement(this.readString(), this.readString(), this.readString());
                    break;
                case CHARACTERS:
                    char[] chars = this.readChars();
                    int len = chars.length;
                    while (len > 0 && chars[len-1]==0) len--;
                    if (len > 0) contentHandler.characters(chars, 0, len);
                    break;
                case IGNORABLE_WHITESPACE:
                    char[] spaces = this.readChars();
                    len = spaces.length;
                    while (len > 0 && spaces[len-1]==0) len--;
                    if (len > 0) contentHandler.characters(spaces, 0, len);
                    break;
                case PROCESSING_INSTRUCTION:
                    contentHandler.processingInstruction(this.readString(), this.readString());
                    break;
                case COMMENT:
                    chars = this.readChars();
                    if (this.lexicalHandler != null) {
                        len = chars.length;
                        while (len > 0 && chars[len-1]==0) len--;
                        if (len > 0) lexicalHandler.comment(chars, 0, len);
                    }
                    break;
                case LOCATOR:
                    {
                    String publicId = this.readString();
                    String systemId = this.readString();
                    int lineNumber = this.read();
                    int columnNumber = this.read();
                    org.xml.sax.helpers.LocatorImpl locator = new org.xml.sax.helpers.LocatorImpl();
                    locator.setPublicId(publicId);
                    locator.setSystemId(systemId);
                    locator.setLineNumber(lineNumber);
                    locator.setColumnNumber(columnNumber);
                    contentHandler.setDocumentLocator(locator);
                    }
                    break;
                default:
                    throw new SAXException ("parsing error: event not supported.");
            }
        }
    }

    private void checkProlog() throws SAXException {
        int valid = 0;
        if (this.read() == 'C') valid++;
        if (this.read() == 'X') valid++;
        if (this.read() == 'M') valid++;
        if (this.read() == 'L') valid++;
        if (this.read() == 1) valid++;
        if (this.read() == 0) valid++;
        if (valid != 6) throw new SAXException("Unrecognized file format.");
    }

    protected int readEvent() throws SAXException {
        return this.read();
    }

    private int readAttributes() throws SAXException {
        int ch1 = this.read();
        int ch2 = this.read();
        return ((ch1 << 8) + (ch2 << 0));
    }

    private String readString() throws SAXException {
        int length = this.readLength();
        int index = length & 0x00007FFF;
        if (length >= 0x00008000) {
            return (String) list.get(index);
        } else {
            char[] chars = this.readChars(index);
            int len = chars.length;
            if (len > 0) {
                while (chars[len-1]==0) len--;
            }
            String str;
            if (len == 0) {
                str = "";
            } else {
                str = new String(chars, 0, len);
            }
            list.add(str);
            return str;
        }
    }

    /**
     * The returned char array might contain any number of zero bytes
     * at the end
     */
    private char[] readChars() throws SAXException {
        return this.readChars(this.readLength());
    }

    private int read() throws SAXException {
        if (currentPos >= input.length)
            throw new SAXException("Reached end of input.");
        return input[currentPos++] & 0xff;
    }

    /**
     * The returned char array might contain any number of zero bytes
     * at the end
     */
    private char[] readChars(int len) throws SAXException {
        char[] str = new char[len];
        byte[] bytearr = new byte[len];
        int c, char2, char3;
        int count = 0;
        int i = 0;

        this.readBytes(bytearr);

        while (count < len) {
            c = (int) bytearr[count] & 0xff;
            switch (c >> 4) {
                case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                    // 0xxxxxxx
                    count++;
                    str[i++] = (char) c;
                    break;
                case 12: case 13:
                    // 110x xxxx   10xx xxxx
                    count += 2;
                    char2 = (int) bytearr[count-1];
                    str[i++] = (char) (((c & 0x1F) << 6) | (char2 & 0x3F));
                    break;
                case 14:
                    // 1110 xxxx  10xx xxxx  10xx xxxx
                    count += 3;
                    char2 = (int) bytearr[count-2];
                    char3 = (int) bytearr[count-1];
                    str[i++] = ((char)(((c & 0x0F) << 12) | ((char2 & 0x3F) << 6) | ((char3 & 0x3F) << 0)));
                    break;
                default:
                    // 10xx xxxx,  1111 xxxx
                    throw new SAXException("UTFDataFormatException");
            }
        }

        return str;
    }

    private void readBytes(byte[] b)
    throws SAXException {
        if (this.currentPos + b.length > this.input.length) {
            // TC:
            // >= prevents getting the last byte
            // 0 1 2 3 4   input.length = 5
            //     |_ currentPos = 2
            // b.length = 3
            // 2 + 3 > 5 ok
            // 2 + 3 >= 5 wrong
            // why has this worked before?
            throw new SAXException("End of input reached.");
        }
        System.arraycopy(this.input, this.currentPos, b, 0, b.length);
        this.currentPos += b.length;
    }

    private int readLength() throws SAXException {
        int ch1 = this.read();
        int ch2 = this.read();
        return ((ch1 << 8) + (ch2 << 0));
    }
}
