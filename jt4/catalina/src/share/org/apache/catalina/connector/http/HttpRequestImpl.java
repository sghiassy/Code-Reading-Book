/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/connector/http/HttpRequestImpl.java,v 1.11 2001/07/22 20:25:07 pier Exp $
 * $Revision: 1.11 $
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
import java.security.AccessController;
import java.security.PrivilegedAction;
import java.util.ArrayList;
import java.util.Enumeration;
import javax.servlet.ServletInputStream;
import org.apache.catalina.connector.HttpRequestBase;
import org.apache.catalina.util.Enumerator;


/**
 * Implementation of <b>HttpRequest</b> specific to the HTTP connector.
 *
 * @author Craig R. McClanahan
 * @author Remy Maucherat
 * @version $Revision: 1.11 $ $Date: 2001/07/22 20:25:07 $
 */

final class HttpRequestImpl
    extends HttpRequestBase {


    // -------------------------------------------------------------- Constants


    /**
     * Initial pool size.
     */
    protected static final int INITIAL_POOL_SIZE = 10;


    /**
     * Pool size increment.
     */
    protected static final int POOL_SIZE_INCREMENT = 5;


    // ----------------------------------------------------- Instance Variables


    /**
     * The InetAddress of the remote client of ths request.
     */
    protected InetAddress inet = null;


    /**
     * Descriptive information about this Request implementation.
     */
    protected static final String info =
        "org.apache.catalina.connector.http.HttpRequestImpl/1.0";


    /**
     * Headers pool.
     */
    protected HttpHeader[] headerPool = new HttpHeader[INITIAL_POOL_SIZE];


    /**
     * Position of the next available header in the pool.
     */
    protected int nextHeader = 0;


    /**
     * Connection header.
     */
    protected HttpHeader connectionHeader = null;


    /**
     * Transfer encoding header.
     */
    protected HttpHeader transferEncodingHeader = null;


    // ------------------------------------------------------------- Properties


    /**
     * [Package Private] Return the InetAddress of the remote client of
     * this request.
     */
    InetAddress getInet() {

        return (inet);

    }


    /**
     * [Package Private] Set the InetAddress of the remote client of
     * this request.
     *
     * @param inet The new InetAddress
     */
    void setInet(InetAddress inet) {

        this.inet = inet;

    }


    /**
     * Return descriptive information about this Request implementation and
     * the corresponding version number, in the format
     * <code>&lt;description&gt;/&lt;version&gt;</code>.
     */
    public String getInfo() {

        return (info);

    }


    // --------------------------------------------------------- Public Methods


    /**
     * Release all object references, and initialize instance variables, in
     * preparation for reuse of this object.
     */
    public void recycle() {

        super.recycle();
        inet = null;
        nextHeader = 0;
        connectionHeader = null;

    }


    /**
     * Create and return a ServletInputStream to read the content
     * associated with this Request.  The default implementation creates an
     * instance of RequestStream associated with this request, but this can
     * be overridden if necessary.
     *
     * @exception IOException if an input/output error occurs
     */
    public ServletInputStream createInputStream() throws IOException {

        return (new HttpRequestStream(this, (HttpResponseImpl) response));

    }


    /**
     * Allocate new header.
     *
     * @return an HttpHeader buffer allocated from the pool
     */
    HttpHeader allocateHeader() {
        if (nextHeader == headerPool.length) {
            // Grow the pool
            HttpHeader[] newHeaderPool =
                new HttpHeader[headerPool.length + POOL_SIZE_INCREMENT];
            for (int i = 0; i < nextHeader; i++) {
                newHeaderPool[i] = headerPool[i];
            }
            headerPool = newHeaderPool;
        }
        if (headerPool[nextHeader] == null)
            headerPool[nextHeader] = new HttpHeader();
        return headerPool[nextHeader];
    }


    /**
     * Go to the next header.
     */
    void nextHeader() {
        nextHeader++;
    }


    /**
     * Add a Header to the set of Headers associated with this Request.
     *
     * @param name The new header name
     * @param value The new header value
     * @deprecated Don't use
     */
    public void addHeader(String name, String value) {

        if (nextHeader == headerPool.length) {
            // Grow the pool
            HttpHeader[] newHeaderPool =
                new HttpHeader[headerPool.length + POOL_SIZE_INCREMENT];
            for (int i = 0; i < nextHeader; i++) {
                newHeaderPool[i] = headerPool[i];
            }
            headerPool = newHeaderPool;
        }
        headerPool[nextHeader++] = new HttpHeader(name, value);

    }


    /**
     * Return the first value of the specified header, if any; otherwise,
     * return <code>null</code>
     *
     * @param header Header we want to retrieve
     */
    public HttpHeader getHeader(HttpHeader header) {

        for (int i = 0; i < nextHeader; i++) {
            if (headerPool[i].equals(header))
                return headerPool[i];
        }
        return null;

    }


    /**
     * Return the first value of the specified header, if any; otherwise,
     * return <code>null</code>
     *
     * @param headerName Name of the requested header
     */
    public HttpHeader getHeader(char[] headerName) {

        for (int i = 0; i < nextHeader; i++) {
            if (headerPool[i].equals(headerName))
                return headerPool[i];
        }
        return null;

    }


    /**
     * Perform whatever actions are required to flush and close the input
     * stream or reader, in a single operation.
     *
     * @exception IOException if an input/output error occurs
     */
    public void finishRequest() throws IOException {

        // If neither a reader or an is have been opened, do it to consume
        // request bytes, if any
        if ((reader == null) && (stream == null) && (getContentLength() != 0)
            && (getProtocol() != null) && (getProtocol().equals("HTTP/1.1")))
            getInputStream();

        super.finishRequest();

    }


    // ------------------------------------------------- ServletRequest Methods


    /**
     * Return the Internet Protocol (IP) address of the client that sent
     * this request.
     */
    public String getRemoteAddr() {

        return (inet.getHostAddress());

    }


    /**
     * Return the fully qualified name of the client that sent this request,
     * or the IP address of the client if the name cannot be determined.
     */
    public String getRemoteHost() {

        if (connector.getEnableLookups())
            return (inet.getHostName());
        else
            return (getRemoteAddr());

    }


    // --------------------------------------------- HttpServletRequest Methods


    /**
     * Return the first value of the specified header, if any; otherwise,
     * return <code>null</code>
     *
     * @param name Name of the requested header
     */
    public String getHeader(String name) {

        name = name.toLowerCase();
        for (int i = 0; i < nextHeader; i++) {
            if (headerPool[i].equals(name))
                return new String(headerPool[i].value, 0,
                                  headerPool[i].valueEnd);
        }
        return null;


    }


    /**
     * Return all of the values of the specified header, if any; otherwise,
     * return an empty enumeration.
     *
     * @param name Name of the requested header
     */
    public Enumeration getHeaders(String name) {

        name = name.toLowerCase();
        ArrayList tempArrayList = new ArrayList();
        for (int i = 0; i < nextHeader; i++) {
            if (headerPool[i].equals(name))
                tempArrayList.add(new String(headerPool[i].value, 0,
                                             headerPool[i].valueEnd));
        }
        return (Enumeration) new Enumerator(tempArrayList);

    }


    /**
     * Return the names of all headers received with this request.
     */
    public Enumeration getHeaderNames() {
        ArrayList tempArrayList = new ArrayList();
        for (int i = 0; i < nextHeader; i++) {
            tempArrayList.add(new String(headerPool[i].name, 0,
                                         headerPool[i].nameEnd));
        }
        return (Enumeration) new Enumerator(tempArrayList);

    }

}
