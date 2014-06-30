/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/test/org/apache/naming/resources/WARDirContextTestCase.java,v 1.2 2001/06/01 16:58:22 craigmcc Exp $
 * $Revision: 1.2 $
 * $Date: 2001/06/01 16:58:22 $
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


package org.apache.naming.resources;

import java.io.File;
import java.io.IOException;

import java.util.Date;

import java.util.jar.JarEntry;
import java.util.jar.JarFile;

import javax.naming.NamingException;

import javax.naming.directory.Attribute;
import javax.naming.directory.Attributes;
import javax.naming.directory.DirContext;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;


/**
 * Unit tests for <code>org.apache.naming.resources.WARDirContext</code>.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.2 $ $Date: 2001/06/01 16:58:22 $
 */

public class WARDirContextTestCase extends BaseDirContextTestCase {


    // ----------------------------------------------------------- Constructors


    /**
     * Construct a new instance of this test case.
     *
     * @param name Name of the test case
     */
    public WARDirContextTestCase(String name) {

        super(name);

    }


    // --------------------------------------------------- Overall Test Methods


    /**
     * Set up instance variables required by this test case.  This method
     * <strong>MUST</strong> be implemented by a subclass.
     */
    public void setUp() {

        context = new WARDirContext();
        ((WARDirContext) context).setDocBase(docBase);

    }


    /**
     * Return the tests included in this test suite.  This method
     * <strong>MUST</strong> be implemented by a subclass.
     */
    public static Test suite() {

        return (new TestSuite(WARDirContextTestCase.class));

    }


    /**
     * Tear down instance variables required by this test case.  This method
     * <strong>MUST</strong> be implemented by a subclass.
     */
    public void tearDown() {

        context = null;

    }


    // ------------------------------------------------ Individual Test Methods


    /**
     * Test the attributes returned for the <code>WEB-INF</code> entry.
     */
    public void testGetAttributesWebInf() {

        try {

            // Look up the attributes of this WAR file entry
            JarFile jarFile = new JarFile(docBase);
            assertNotNull("Created JarFile for " + docBase, jarFile);
            JarEntry jarEntry =
                (JarEntry) jarFile.getEntry("WEB-INF");
            assertNotNull("Created JarEntry for WEB-INF", jarEntry);

            // Look up the attributes for the WEB-INF entry
            Attributes attributes = context.getAttributes("WEB-INF");

            // Enumerate and check the attributes for this entry
            checkWebInfAttributes(attributes,
                                  new Date(jarEntry.getTime()),
                                  jarEntry.getSize(),
                                  "WEB-INF",
                                  new Date(jarEntry.getTime()));

        } catch (IOException e) {

            fail("IOException: " + e);

        } catch (NamingException e) {

            fail("NamingException: " + e);

        }

    }


    /**
     * Test the attributes returned for the <code>WEB-INF/web.xml</code>
     * entry.
     */
    public void testGetAttributesWebXml() {

        try {

            // Look up the attributes of this WAR file entry
            JarFile jarFile = new JarFile(docBase);
            assertNotNull("Created JarFile for " + docBase, jarFile);
            JarEntry jarEntry =
                (JarEntry) jarFile.getEntry("WEB-INF/web.xml");
            assertNotNull("Created JarEntry for WEB-INF/web.xml", jarEntry);

            // Look up the attributes for the WEB-INF/web.xml entry
            Attributes attributes = context.getAttributes("WEB-INF/web.xml");

            // Enumerate and check the attributes for this entry
            checkWebXmlAttributes(attributes,
                                  new Date(jarEntry.getTime()),
                                  jarEntry.getSize(),
                                  "web.xml",
                                  new Date(jarEntry.getTime()));

        } catch (IOException e) {

            fail("IOException: " + e);

        } catch (NamingException e) {

            fail("NamingException:  " + e);

        }

    }


}
