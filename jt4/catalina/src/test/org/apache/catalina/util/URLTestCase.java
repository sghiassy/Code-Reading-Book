/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/test/org/apache/catalina/util/URLTestCase.java,v 1.2 2001/09/05 17:31:11 craigmcc Exp $
 * $Revision: 1.2 $
 * $Date: 2001/09/05 17:31:11 $
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


import java.net.MalformedURLException;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;


/**
 * Unit tests for the <code>org.apache.catalina.util.URL</code> class.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.2 $ $Date: 2001/09/05 17:31:11 $
 */

public class URLTestCase extends TestCase {


    // ----------------------------------------------------- Instance Variables


    // ----------------------------------------------------------- Constructors


    /**
     * Construct a new instance of this test case.
     *
     * @param name Name of the test case
     */
    public URLTestCase(String name) {

        super(name);

    }


    // --------------------------------------------------- Overall Test Methods


    /**
     * Set up instance variables required by this test case.
     */
    public void setUp() {

        ; // No action required

    }


    /**
     * Return the tests included in this test suite.
     */
    public static Test suite() {

        return (new TestSuite(URLTestCase.class));

    }


    /**
     * Tear down instance variables required by this test case.
     */
    public void tearDown() {

        ; // No action required

    }


    // ------------------------------------------------ Individual Test Methods


    /**
     * Negative tests for absolute URL strings in various patterns.  Each of
     * these should throw <code>MalformedURLException</code>.
     */
    public void testNegativeAbsolute() {

        negative("index.html");
        negative("index.html#ref");
        negative("index.html?name=value");
        negative("index.html?name=value#ref");

        negative("/index.html");
        negative("/index.html#ref");
        negative("/index.html?name=value");
        negative("/index.html?name=value#ref");

    }


    /**
     * Negative tests for <code>normalize()</code>.  Attempts to normalize
     * these legal URLs should throw <code>MalformedURLException</code>.
     */
    public void testNegativeNormalize() {

        normalize("http://localhost/..");
        normalize("http://localhost/..#ref");
        normalize("http://localhost/..?name=value");
        normalize("http://localhost/..?name=value#ref");

        normalize("http://localhost:8080/..");
        normalize("http://localhost:8080/..#ref");
        normalize("http://localhost:8080/..?name=value");
        normalize("http://localhost:8080/..?name=value#ref");
 
        normalize("http://localhost/../");
        normalize("http://localhost/../#ref");
        normalize("http://localhost/../?name=value");
        normalize("http://localhost/../?name=value#ref");

        normalize("http://localhost:8080/../");
        normalize("http://localhost:8080/../#ref");
        normalize("http://localhost:8080/../?name=value");
        normalize("http://localhost:8080/../?name=value#ref");
 
        normalize("http://localhost/index.html/../../foo.html");
        normalize("http://localhost/index.html/../../foo.html#ref");
        normalize("http://localhost/index.html/../../foo.html?name=value");
        normalize("http://localhost/index.html/../../foo.html?name=value#ref");

        normalize("http://localhost:8080/index.html/../../foo.html");
        normalize("http://localhost:8080/index.html/../../foo.html#ref");
        normalize("http://localhost:8080/index.html/../../foo.html?name=value");
        normalize("http://localhost:8080/index.html/../../foo.html?name=value#ref");
 
    }


    /**
     * Negative tests for relative URL strings in various patterns.  Each of
     * these should throw <code>MalformedURLException</code>.
     */
    public void testNegativeRelative() {

        // Commented out because java.net.URL ignores extraneous "../"
        //        negative("http://a/b/c/d;p?q", "../../../g");
        //        negative("http://a/b/c/d;p?q", "/../g");

    }


    /**
     * Positive tests for absolute URL strings in various patterns.
     */
    public void testPositiveAbsolute() {

        positive("http://a/b/c/d;p?q");

        positive("http://localhost/index.html");
        positive("http://localhost/index.html#ref");
        positive("http://localhost/index.html?name=value");
        positive("http://localhost/index.html?name=value#ref");

        positive("http://localhost:8080/index.html");
        positive("http://localhost:8080/index.html#ref");
        positive("http://localhost:8080/index.html?name=value");
        positive("http://localhost:8080/index.html?name=value#ref");

        positive("http://localhost/index.html/.");
        positive("http://localhost/index.html/.#ref");
        positive("http://localhost/index.html/.?name=value");
        positive("http://localhost/index.html/.?name=value#ref");

        positive("http://localhost:8080/index.html/.");
        positive("http://localhost:8080/index.html/.#ref");
        positive("http://localhost:8080/index.html/.?name=value");
        positive("http://localhost:8080/index.html/.?name=value#ref");

        positive("http://localhost/index.html/foo/..");
        positive("http://localhost/index.html/foo/..#ref");
        positive("http://localhost/index.html/foo/..?name=value");
        positive("http://localhost/index.html/foo/..?name=value#ref");

        positive("http://localhost:8080/index.html/foo/..");
        positive("http://localhost:8080/index.html/foo/..#ref");
        positive("http://localhost:8080/index.html/foo/..?name=value");
        positive("http://localhost:8080/index.html/foo/..?name=value#ref");

        positive("http://localhost/index.html/../foo.html");
        positive("http://localhost/index.html/../foo.html#ref");
        positive("http://localhost/index.html/../foo.html?name=value");
        positive("http://localhost/index.html/../foo.html?name=value#ref");

        positive("http://localhost:8080/index.html/../foo.html");
        positive("http://localhost:8080/index.html/../foo.html#ref");
        positive("http://localhost:8080/index.html/../foo.html?name=value");
        positive("http://localhost:8080/index.html/../foo.html?name=value#ref");

        positive("http://localhost");
        positive("http://localhost#ref");
        positive("http://localhost?name=value");
        positive("http://localhost?name=value#ref");

        positive("http://localhost:8080");
        positive("http://localhost:8080#ref");
        positive("http://localhost:8080?name=value");
        positive("http://localhost:8080?name=value#ref");

        positive("http://localhost/");
        positive("http://localhost/#ref");
        positive("http://localhost/?name=value");
        positive("http://localhost/?name=value#ref");

        positive("http://localhost:8080/");
        positive("http://localhost:8080/#ref");
        positive("http://localhost:8080/?name=value");
        positive("http://localhost:8080/?name=value#ref");

    }


    /**
     * Positive tests for normalizing absolute URL strings in various patterns.
     */
    public void testPositiveNormalize() {

        normalize("http://a/b/c/d;p?q",
                  "http://a/b/c/d;p?q");

        normalize("http://localhost/index.html",
                  "http://localhost/index.html");
        normalize("http://localhost/index.html#ref",
                  "http://localhost/index.html#ref");
        normalize("http://localhost/index.html?name=value",
                  "http://localhost/index.html?name=value");
        normalize("http://localhost/index.html?name=value#ref",
                  "http://localhost/index.html?name=value#ref");

        normalize("http://localhost:8080/index.html",
                  "http://localhost:8080/index.html");
        normalize("http://localhost:8080/index.html#ref",
                  "http://localhost:8080/index.html#ref");
        normalize("http://localhost:8080/index.html?name=value",
                  "http://localhost:8080/index.html?name=value");
        normalize("http://localhost:8080/index.html?name=value#ref",
                  "http://localhost:8080/index.html?name=value#ref");

        normalize("http://localhost/./index.html",
                  "http://localhost/index.html");
        normalize("http://localhost/./index.html#ref",
                  "http://localhost/index.html#ref");
        normalize("http://localhost/./index.html?name=value",
                  "http://localhost/index.html?name=value");
        normalize("http://localhost/./index.html?name=value#ref",
                  "http://localhost/index.html?name=value#ref");

        normalize("http://localhost:8080/./index.html",
                  "http://localhost:8080/index.html");
        normalize("http://localhost:8080/./index.html#ref",
                  "http://localhost:8080/index.html#ref");
        normalize("http://localhost:8080/./index.html?name=value",
                  "http://localhost:8080/index.html?name=value");
        normalize("http://localhost:8080/./index.html?name=value#ref",
                  "http://localhost:8080/index.html?name=value#ref");

        normalize("http://localhost/index.html/.",
                  "http://localhost/index.html/");
        normalize("http://localhost/index.html/.#ref",
                  "http://localhost/index.html/#ref");
        normalize("http://localhost/index.html/.?name=value",
                  "http://localhost/index.html/?name=value");
        normalize("http://localhost/index.html/.?name=value#ref",
                  "http://localhost/index.html/?name=value#ref");

        normalize("http://localhost:8080/index.html/.",
                  "http://localhost:8080/index.html/");
        normalize("http://localhost:8080/index.html/.#ref",
                  "http://localhost:8080/index.html/#ref");
        normalize("http://localhost:8080/index.html/.?name=value",
                  "http://localhost:8080/index.html/?name=value");
        normalize("http://localhost:8080/index.html/.?name=value#ref",
                  "http://localhost:8080/index.html/?name=value#ref");

        normalize("http://localhost/index.html/./",
                  "http://localhost/index.html/");
        normalize("http://localhost/index.html/./#ref",
                  "http://localhost/index.html/#ref");
        normalize("http://localhost/index.html/./?name=value",
                  "http://localhost/index.html/?name=value");
        normalize("http://localhost/index.html/./?name=value#ref",
                  "http://localhost/index.html/?name=value#ref");

        normalize("http://localhost:8080/index.html/./",
                  "http://localhost:8080/index.html/");
        normalize("http://localhost:8080/index.html/./#ref",
                  "http://localhost:8080/index.html/#ref");
        normalize("http://localhost:8080/index.html/./?name=value",
                  "http://localhost:8080/index.html/?name=value");
        normalize("http://localhost:8080/index.html/./?name=value#ref",
                  "http://localhost:8080/index.html/?name=value#ref");

        normalize("http://localhost/foo.html/../index.html",
                  "http://localhost/index.html");
        normalize("http://localhost/foo.html/../index.html#ref",
                  "http://localhost/index.html#ref");
        normalize("http://localhost/foo.html/../index.html?name=value",
                  "http://localhost/index.html?name=value");
        normalize("http://localhost/foo.html/../index.html?name=value#ref",
                  "http://localhost/index.html?name=value#ref");

        normalize("http://localhost:8080/foo.html/../index.html",
                  "http://localhost:8080/index.html");
        normalize("http://localhost:8080/foo.html/../index.html#ref",
                  "http://localhost:8080/index.html#ref");
        normalize("http://localhost:8080/foo.html/../index.html?name=value",
                  "http://localhost:8080/index.html?name=value");
        normalize("http://localhost:8080/foo.html/../index.html?name=value#ref",
                  "http://localhost:8080/index.html?name=value#ref");

        normalize("http://localhost/index.html/foo.html/..",
                  "http://localhost/index.html/");
        normalize("http://localhost/index.html/foo.html/..#ref",
                  "http://localhost/index.html/#ref");
        normalize("http://localhost/index.html/foo.html/..?name=value",
                  "http://localhost/index.html/?name=value");
        normalize("http://localhost/index.html/foo.html/..?name=value#ref",
                  "http://localhost/index.html/?name=value#ref");

        normalize("http://localhost:8080/index.html/foo.html/..",
                  "http://localhost:8080/index.html/");
        normalize("http://localhost:8080/index.html/foo.html/..#ref",
                  "http://localhost:8080/index.html/#ref");
        normalize("http://localhost:8080/index.html/foo.html/..?name=value",
                  "http://localhost:8080/index.html/?name=value");
        normalize("http://localhost:8080/index.html/foo.html/..?name=value#ref",
                  "http://localhost:8080/index.html/?name=value#ref");

        normalize("http://localhost/index.html/foo.html/../",
                  "http://localhost/index.html/");
        normalize("http://localhost/index.html/foo.html/../#ref",
                  "http://localhost/index.html/#ref");
        normalize("http://localhost/index.html/foo.html/../?name=value",
                  "http://localhost/index.html/?name=value");
        normalize("http://localhost/index.html/foo.html/../?name=value#ref",
                  "http://localhost/index.html/?name=value#ref");

        normalize("http://localhost:8080/index.html/foo.html/../",
                  "http://localhost:8080/index.html/");
        normalize("http://localhost:8080/index.html/foo.html/../#ref",
                  "http://localhost:8080/index.html/#ref");
        normalize("http://localhost:8080/index.html/foo.html/../?name=value",
                  "http://localhost:8080/index.html/?name=value");
        normalize("http://localhost:8080/index.html/foo.html/../?name=value#ref",
                  "http://localhost:8080/index.html/?name=value#ref");

    }


    /**
     * Positive tests for relative URL strings in various patterns.
     */
    public void testPositiveRelative() {

        // Test cases based on RFC 2396, Appendix C
        positive("http://a/b/c/d;p?q", "http:h");
        positive("http://a/b/c/d;p?q", "g");
        positive("http://a/b/c/d;p?q", "./g");
        positive("http://a/b/c/d;p?q", "g/");
        positive("http://a/b/c/d;p?q", "/g");
        //        positive("http://a/b/c/d;p?q", "//g");
        positive("http://a/b/c/d;p?q", "?y");
        positive("http://a/b/c/d;p?q", "g?y");
        //        positive("http://a/b/c/d;p?q", "#s");
        positive("http://a/b/c/d;p?q", "g#s");
        positive("http://a/b/c/d;p?q", "g?y#s");
        positive("http://a/b/c/d;p?q", ";x");
        positive("http://a/b/c/d;p?q", "g;x");
        positive("http://a/b/c/d;p?q", "g;x?y#s");
        positive("http://a/b/c/d;p?q", ".");
        positive("http://a/b/c/d;p?q", "./");
        positive("http://a/b/c/d;p?q", "..");
        positive("http://a/b/c/d;p?q", "../");
        positive("http://a/b/c/d;p?q", "../g");
        positive("http://a/b/c/d;p?q", "../..");
        positive("http://a/b/c/d;p?q", "../../");
        positive("http://a/b/c/d;p?q", "../../g");
        // Commented because java.net.URL doesn't normalize out the "/./"????
        //        positive("http://a/b/c/d;p?q", "/./g");
        positive("http://a/b/c/d;p?q", "g.");
        positive("http://a/b/c/d;p?q", ".g");
        positive("http://a/b/c/d;p?q", "g..");
        positive("http://a/b/c/d;p?q", "..g");
        positive("http://a/b/c/d;p?q", "./../g");
        positive("http://a/b/c/d;p?q", "./g/.");
        positive("http://a/b/c/d;p?q", "g/./h");
        positive("http://a/b/c/d;p?q", "g/../h");
        positive("http://a/b/c/d;p?q", "g;x=1/./y");
        positive("http://a/b/c/d;p?q", "g;x=1/../y");
        positive("http://a/b/c/d;p?q", "g?y/./x");
        positive("http://a/b/c/d;p?q", "g?y/../x");
        positive("http://a/b/c/d;p?q", "g#s/./x");
        positive("http://a/b/c/d;p?q", "g#s/../x");
        positive("http://a/b", "c");
        positive("http://a/b/", "c");

    }


    // -------------------------------------------------------- Private Methods


    /**
     * Check that both our URL class and <code>java.net.URL</code> throw
     * <code>MalformedURLException</code> on an absolute URL specification.
     *
     * @param spec Absolute URL specification to be checked
     */
    private void negative(String spec) {

        try {
            java.net.URL url = new java.net.URL(spec);
            fail(spec + " should have failed on java.net.URL " +
                 "but returned " + url.toExternalForm());
        } catch (MalformedURLException e) {
            ; // Expected response
        }

        try {
            URL url = new URL(spec);
            fail(spec + " should have failed on tested URL " +
                 "but returned " + url.toExternalForm());
        } catch (MalformedURLException e) {
            ; // Expected response
        }

    }


    /**
     * Check that both our URL class and <code>java.net.URL</code> throw
     * <code>MalformedURLException</code> on an absolute URL specification
     * plus the corresponding relative URL specification.
     *
     * @param abs Absolute URL specification to be checked
     * @param rel Relative URL specification to be checked
     */
    private void negative(String abs, String rel) {

        java.net.URL baseNet = null;
        URL baseUrl = null;

        try {
            baseNet = new java.net.URL(abs);
        } catch (MalformedURLException e) {
            fail(abs + " net URL threw " + e);
        }

        try {
            baseUrl = new URL(abs);
        } catch (MalformedURLException e) {
            fail(abs + " url URL threw " + e);
        }

        try {
            java.net.URL url = new java.net.URL(baseNet, rel);
            fail(rel + " should have failed on java.net.URL " +
                 "but returned " + url.toExternalForm());
        } catch (MalformedURLException e) {
            ; // Expected response
        }

        try {
            URL url = new URL(baseUrl, rel);
            fail(rel + " should have failed on tested URL " +
                 "but returned " + url.toExternalForm());
        } catch (MalformedURLException e) {
            ; // Expected response
        }

    }


    /**
     * Attempts to normalize the specified URL should throw
     * MalformedURLException.
     *
     * @param spec Unnormalized version of the URL specification
     */
    private void normalize(String spec) {

        URL url = null;
        try {
            url = new URL(spec);
        } catch (Throwable t) {
            fail(spec + " should not have thrown " + t);
        }

        try {
            url.normalize();
            fail(spec + ".normalize() should have thrown MUE");
        } catch (MalformedURLException e) {
            ; // Expected result
        }

    }


    /**
     * It should be possible to normalize the specified URL into the
     * specified normalized form.
     *
     * @param spec Unnormalized version of the URL specification
     * @param norm Normalized version of the URL specification
     */
    private void normalize(String spec, String norm) {

        try {
            URL url = new URL(spec);
            url.normalize();
            assertEquals(spec + ".normalize()", norm, url.toExternalForm());
        } catch (Throwable t) {
            fail(spec + ".normalize() threw " + t);
        }

    }


    /**
     * Check the details of our URL class against <code>java.net.URL</code>
     * for an absolute URL specification.
     *
     * @param spec Absolute URL specification to be checked
     */
    private void positive(String spec) {

        // Compare results with what java.net.URL returns
        try {
            URL url = new URL(spec);
            java.net.URL net = new java.net.URL(spec);
            assertEquals(spec + " toExternalForm()",
                         net.toExternalForm(),
                         url.toExternalForm());
            assertEquals(spec + ".getAuthority()",
                         net.getAuthority(),
                         url.getAuthority());
            assertEquals(spec + ".getFile()",
                         net.getFile(),
                         url.getFile());
            assertEquals(spec + ".getHost()",
                         net.getHost(),
                         url.getHost());
            assertEquals(spec + ".getPath()",
                         net.getPath(),
                         url.getPath());
            assertEquals(spec + ".getPort()",
                         net.getPort(),
                         url.getPort());
            assertEquals(spec + ".getProtocol()",
                         net.getProtocol(),
                         url.getProtocol());
            assertEquals(spec + ".getQuery()",
                         net.getQuery(),
                         url.getQuery());
            assertEquals(spec + ".getRef()",
                         net.getRef(),
                         url.getRef());
            assertEquals(spec + ".getUserInfo()",
                         net.getUserInfo(),
                         url.getUserInfo());
        } catch (Throwable t) {
            fail(spec + " positive test threw " + t);
        }

    }


    /**
     * Check the details of our URL class against <code>java.net.URL</code>
     * for a relative URL specification.
     *
     * @param abs Absolute URL specification for base reference
     * @param rel Relative URL specification to resolve
     */
    private void positive(String abs, String rel) {

        // Compare results with what java.net.URL returns
        try {
            URL urlBase = new URL(abs);
            java.net.URL netBase = new java.net.URL(abs);
            URL url = new URL(urlBase, rel);
            java.net.URL net = new java.net.URL(netBase, rel);
            assertEquals(rel + " toExternalForm()",
                         net.toExternalForm(),
                         url.toExternalForm());
            assertEquals(rel + ".getAuthority()",
                         net.getAuthority(),
                         url.getAuthority());
            assertEquals(rel + ".getFile()",
                         net.getFile(),
                         url.getFile());
            assertEquals(rel + ".getHost()",
                         net.getHost(),
                         url.getHost());
            assertEquals(rel + ".getPath()",
                         net.getPath(),
                         url.getPath());
            assertEquals(rel + ".getPort()",
                         net.getPort(),
                         url.getPort());
            assertEquals(rel + ".getProtocol()",
                         net.getProtocol(),
                         url.getProtocol());
            assertEquals(rel + ".getQuery()",
                         net.getQuery(),
                         url.getQuery());
            assertEquals(rel + ".getRef()",
                         net.getRef(),
                         url.getRef());
            assertEquals(rel + ".getUserInfo()",
                         net.getUserInfo(),
                         url.getUserInfo());
        } catch (Throwable t) {
            fail(rel + " positive test threw " + t);
        }

    }


}
