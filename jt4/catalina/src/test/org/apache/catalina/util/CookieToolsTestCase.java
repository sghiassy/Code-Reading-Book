/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/test/org/apache/catalina/util/CookieToolsTestCase.java,v 1.3 2001/09/05 18:35:32 craigmcc Exp $
 * $Revision: 1.3 $
 * $Date: 2001/09/05 18:35:32 $
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


package org.apache.catalina.util;

import javax.servlet.http.Cookie;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;


/**
 * Unit tests for the <code>CookieTools</code> class.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.3 $ $Date: 2001/09/05 18:35:32 $
 */

public class CookieToolsTestCase extends TestCase {


    public static void main(String args[]) {
        System.out.println("TestCase started");
    }

    // ----------------------------------------------------- Instance Variables


    /**
     * A "version 0" cookie.
     */
    protected Cookie version0 = null;


    /**
     * A "version 1" cookie.
     */
    protected Cookie version1 = null;



    // ----------------------------------------------------------- Constructors


    /**
     * Construct a new instance of this test case.
     *
     * @param name Name of the test case
     */
    public CookieToolsTestCase(String name) {

        super(name);

    }


    // --------------------------------------------------- Overall Test Methods


    /**
     * Set up instance variables required by this test case.
     */
    public void setUp() {

        version0 = new Cookie("Version 0 Name", "Version 0 Value");
        version0.setComment("Version 0 Comment");
        version0.setDomain("localhost");
        version0.setPath("/version0");
        version0.setVersion(0);

        version1 = new Cookie("Version 1 Name", "Version 1 Value");
        version1.setComment("Version 1 Comment");
        version1.setDomain("localhost");
        version1.setPath("/version1");
        version1.setVersion(1);

    }


    /**
     * Return the tests included in this test suite.
     */
    public static Test suite() {

        return (new TestSuite(CookieToolsTestCase.class));

    }


    /**
     * Tear down instance variables required by this test case.
     */
    public void tearDown() {

        version0 = null;
        version1 = null;

    }


    // ------------------------------------------------ Individual Test Methods


    /**
     * Check the value returned by <code>getCookieHeaderName()</code>.
     */
    public void testGetCookieHeaderName() {

        assertEquals("Version 0 cookie header name", "Set-Cookie",
                     CookieTools.getCookieHeaderName(version0));
        assertEquals("Version 1 cookie header name", "Set-Cookie2",
                     CookieTools.getCookieHeaderName(version1));

    }


    /**
     * Check the value returned by <code>getCookieHeaderValue()</code>
     */
    public void testGetCookieHeaderValue() {

        StringBuffer sb = null;

        sb = new StringBuffer();
        CookieTools.getCookieHeaderValue(version0, sb);
        assertEquals("Version 0 cookie header value",
                     "Version+0+Name=Version+0+Value;Domain=localhost;Path=/version0",
                     sb.toString());

        sb = new StringBuffer();
        CookieTools.getCookieHeaderValue(version1, sb);
        assertEquals("Version 1 cookie header value",
                     "Version+1+Name=Version+1+Value;Version=1;Comment=Version+1+Comment;Domain=localhost;Discard;Path=\"/version1\"",
                     sb.toString());

    }


}
