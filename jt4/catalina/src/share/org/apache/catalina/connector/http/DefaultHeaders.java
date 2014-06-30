/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/connector/http/DefaultHeaders.java,v 1.2 2001/07/22 20:25:07 pier Exp $
 * $Revision: 1.2 $
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


/**
 * HTTP default headers and header names.
 *
 * @author Remy Maucherat
 * @version $Revision: 1.2 $ $Date: 2001/07/22 20:25:07 $
 */

final class DefaultHeaders {


    // -------------------------------------------------------------- Constants


    static final char[] AUTHORIZATION_NAME = "authorization".toCharArray();
    static final char[] ACCEPT_LANGUAGE_NAME = "accept-language".toCharArray();
    static final char[] COOKIE_NAME = "cookie".toCharArray();
    static final char[] CONTENT_LENGTH_NAME = "content-length".toCharArray();
    static final char[] CONTENT_TYPE_NAME = "content-type".toCharArray();
    static final char[] HOST_NAME = "host".toCharArray();
    static final char[] CONNECTION_NAME = "connection".toCharArray();
    static final char[] CONNECTION_CLOSE_VALUE = "close".toCharArray();
    static final char[] EXPECT_NAME = "expect".toCharArray();
    static final char[] EXPECT_100_VALUE = "100-continue".toCharArray();
    static final char[] TRANSFER_ENCODING_NAME =
        "transfer-encoding".toCharArray();


    static final HttpHeader CONNECTION_CLOSE =
        new HttpHeader("connection", "close");
    static final HttpHeader EXPECT_CONTINUE =
        new HttpHeader("expect", "100-continue");
    static final HttpHeader TRANSFER_ENCODING_CHUNKED =
        new HttpHeader("transfer-encoding", "chunked");


    // ----------------------------------------------------------- Constructors


    // ----------------------------------------------------- Instance Variables


    // ------------------------------------------------------------- Properties


    // --------------------------------------------------------- Public Methods


    // --------------------------------------------------------- Object Methods


}
