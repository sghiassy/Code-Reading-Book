/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/connector/http/HttpHeader.java,v 1.3 2001/07/22 20:25:07 pier Exp $
 * $Revision: 1.3 $
 * $Date: 2001/07/22 20:25:07 $
 *
 * ====================================================================
 *
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowlegement:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowlegement may appear in the software itself,
 *    if and wherever such third-party acknowlegements normally appear.
 *
 * 4. The names "The Jakarta Project", "Tomcat", and "Apache Software
 *    Foundation" must not be used to endorse or promote products derived
 *    from this software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 * [Additional notices, if required by prior licensing conditions]
 *
 */


package org.apache.catalina.connector.http;


import java.io.IOException;
import java.net.InetAddress;
import javax.servlet.ServletInputStream;
import org.apache.catalina.connector.HttpRequestBase;


/**
 * HTTP header enum type.
 *
 * @author Remy Maucherat
 * @version $Revision: 1.3 $ $Date: 2001/07/22 20:25:07 $
 */

final class HttpHeader {


    // -------------------------------------------------------------- Constants


    public static final int INITIAL_NAME_SIZE = 32;
    public static final int INITIAL_VALUE_SIZE = 64;
    public static final int MAX_NAME_SIZE = 128;
    public static final int MAX_VALUE_SIZE = 4096;


    // ----------------------------------------------------------- Constructors


    public HttpHeader() {

        this(new char[INITIAL_NAME_SIZE], 0, new char[INITIAL_VALUE_SIZE], 0);

    }


    public HttpHeader(char[] name, int nameEnd, char[] value, int valueEnd) {

        this.name = name;
        this.nameEnd = nameEnd;
        this.value = value;
        this.valueEnd = valueEnd;

    }


    public HttpHeader(String name, String value) {

        this.name = name.toLowerCase().toCharArray();
        this.nameEnd = name.length();
        this.value = value.toCharArray();
        this.valueEnd = value.length();

    }


    // ----------------------------------------------------- Instance Variables


    public char[] name;
    public int nameEnd;
    public char[] value;
    public int valueEnd;
    protected int hashCode = 0;


    // ------------------------------------------------------------- Properties


    // --------------------------------------------------------- Public Methods


    /**
     * Release all object references, and initialize instance variables, in
     * preparation for reuse of this object.
     */
    public void recycle() {

        nameEnd = 0;
        valueEnd = 0;
        hashCode = 0;

    }


    /**
     * Test if the name of the header is equal to the given char array.
     * All the characters must already be lower case.
     */
    public boolean equals(char[] buf) {
        return equals(buf, buf.length);
    }


    /**
     * Test if the name of the header is equal to the given char array.
     * All the characters must already be lower case.
     */
    public boolean equals(char[] buf, int end) {
        if (end != nameEnd)
            return false;
        for (int i=0; i<end; i++) {
            if (buf[i] != name[i])
                return false;
        }
        return true;
    }


    /**
     * Test if the name of the header is equal to the given string.
     * The String given must be made of lower case characters.
     */
    public boolean equals(String str) {
        return equals(str.toCharArray(), str.length());
    }


    /**
     * Test if the value of the header is equal to the given char array.
     */
    public boolean valueEquals(char[] buf) {
        return valueEquals(buf, buf.length);
    }


    /**
     * Test if the value of the header is equal to the given char array.
     */
    public boolean valueEquals(char[] buf, int end) {
        if (end != valueEnd)
            return false;
        for (int i=0; i<end; i++) {
            if (buf[i] != value[i])
                return false;
        }
        return true;
    }


    /**
     * Test if the value of the header is equal to the given string.
     */
    public boolean valueEquals(String str) {
        return valueEquals(str.toCharArray(), str.length());
    }


    /**
     * Test if the value of the header includes the given char array.
     */
    public boolean valueIncludes(char[] buf) {
        return valueIncludes(buf, buf.length);
    }


    /**
     * Test if the value of the header includes the given char array.
     */
    public boolean valueIncludes(char[] buf, int end) {
        char firstChar = buf[0];
        int pos = 0;
        while (pos < valueEnd) {
            pos = valueIndexOf(firstChar, pos);
            if (pos == -1)
                return false;
            if ((valueEnd - pos) < end)
                return false;
            for (int i = 0; i < end; i++) {
                if (value[i + pos] != buf[i])
                    break;
                if (i == (end-1))
                    return true;
            }
            pos++;
        }
        return false;
    }


    /**
     * Test if the value of the header includes the given string.
     */
    public boolean valueIncludes(String str) {
        return valueIncludes(str.toCharArray(), str.length());
    }


    /**
     * Returns the index of a character in the value.
     */
    public int valueIndexOf(char c, int start) {
        for (int i=start; i<valueEnd; i++) {
            if (value[i] == c)
                return i;
        }
        return -1;
    }


    /**
     * Test if the name of the header is equal to the given header.
     * All the characters in the name must already be lower case.
     */
    public boolean equals(HttpHeader header) {
        return (equals(header.name, header.nameEnd));
    }


    /**
     * Test if the name and value of the header is equal to the given header.
     * All the characters in the name must already be lower case.
     */
    public boolean headerEquals(HttpHeader header) {
        return (equals(header.name, header.nameEnd))
            && (valueEquals(header.value, header.valueEnd));
    }


    // --------------------------------------------------------- Object Methods


    /**
     * Return hash code. The hash code of the HttpHeader object is the same
     * as returned by new String(name, 0, nameEnd).hashCode().
     */
    public int hashCode() {
        int h = hashCode;
        if (h == 0) {
            int off = 0;
            char val[] = name;
            int len = nameEnd;
            for (int i = 0; i < len; i++)
                h = 31*h + val[off++];
            hashCode = h;
        }
        return h;
    }


    public boolean equals(Object obj) {
        if (obj instanceof String) {
            return equals(((String) obj).toLowerCase());
        } else if (obj instanceof HttpHeader) {
            return equals((HttpHeader) obj);
        }
        return false;
    }


}
