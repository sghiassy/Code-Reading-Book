/* ========================================================================= *
 *                                                                           *
 *                 The Apache Software License,  Version 1.1                 *
 *                                                                           *
 *     Copyright (c) 1999, 2000, 2001  The Apache Software Foundation.       *
 *                           All rights reserved.                            *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * Redistribution and use in source and binary forms,  with or without modi- *
 * fication, are permitted provided that the following conditions are met:   *
 *                                                                           *
 * 1. Redistributions of source code  must retain the above copyright notice *
 *    notice, this list of conditions and the following disclaimer.          *
 *                                                                           *
 * 2. Redistributions  in binary  form  must  reproduce the  above copyright *
 *    notice,  this list of conditions  and the following  disclaimer in the *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. The end-user documentation  included with the redistribution,  if any, *
 *    must include the following acknowlegement:                             *
 *                                                                           *
 *       "This product includes  software developed  by the Apache  Software *
 *        Foundation <http://www.apache.org/>."                              *
 *                                                                           *
 *    Alternately, this acknowlegement may appear in the software itself, if *
 *    and wherever such third-party acknowlegements normally appear.         *
 *                                                                           *
 * 4. The names  "The  Jakarta  Project",  "Tomcat",  and  "Apache  Software *
 *    Foundation"  must not be used  to endorse or promote  products derived *
 *    from this  software without  prior  written  permission.  For  written *
 *    permission, please contact <apache@apache.org>.                        *
 *                                                                           *
 * 5. Products derived from this software may not be called "Apache" nor may *
 *    "Apache" appear in their names without prior written permission of the *
 *    Apache Software Foundation.                                            *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES *
 * INCLUDING, BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY *
 * AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.  IN NO EVENT SHALL *
 * THE APACHE  SOFTWARE  FOUNDATION OR  ITS CONTRIBUTORS  BE LIABLE  FOR ANY *
 * DIRECT,  INDIRECT,   INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL *
 * DAMAGES (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS *
 * OR SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) *
 * HOWEVER CAUSED AND  ON ANY  THEORY  OF  LIABILITY,  WHETHER IN  CONTRACT, *
 * STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN *
 * ANY  WAY  OUT OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF  ADVISED  OF THE *
 * POSSIBILITY OF SUCH DAMAGE.                                               *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * This software  consists of voluntary  contributions made  by many indivi- *
 * duals on behalf of the  Apache Software Foundation.  For more information *
 * on the Apache Software Foundation, please see <http://www.apache.org/>.   *
 *                                                                           *
 * ========================================================================= */


package org.apache.tester;


import java.net.URL;
import org.xml.sax.AttributeList;
import org.xml.sax.HandlerBase;
import org.xml.sax.Parser;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.helpers.ParserFactory;


/**
 * SAX parser (based on SAXCount) that exercises the Xerces parser within the
 * environment of a web application.
 *
 * @author Amy Roh
 * @author Craig McClanahan
 * @version $Revision: 1.1 $ $Date: 2001/03/13 19:06:18 $
 */

public class Xerces01Parser extends HandlerBase {


    // ----------------------------------------------------- Instance Variables


    /**
     * The number of XML attributes we have encountered.
     */
    protected int attributes = 0;


    /**
     * The number of characters we have encountered.
     */
    protected int characters = 0;


    /**
     * The number of XML elements we have encountered.
     */
    protected int elements = 0;


    /**
     * The amount of ignorable whitespace we have encountered.
     */
    protected int whitespace = 0;



    // --------------------------------------------------------- Public Methods


    /**
     * Execute the requested parse.
     *
     * @param url The URL of the XML resource to be parsed
     *
     * @exception Exception if any processing exception occurs
     */
    public void parse(URL url) throws Exception {

        // Construct a parser for our use
        Parser parser =
            ParserFactory.makeParser("org.apache.xerces.parsers.SAXParser");
        parser.setDocumentHandler(this);
        parser.setErrorHandler(this);

        // Perform the requested parse
        long before = System.currentTimeMillis();
        parser.parse(url.toString());
        long after = System.currentTimeMillis();

        // Log the results
        StaticLogger.write("Parsing time = " + (after - before) +
                           " milliseconds");
        StaticLogger.write("Processed " + elements + " elements");
        StaticLogger.write("Processed " + attributes + " attributes");
        StaticLogger.write("Processed " + characters + " characters");
        StaticLogger.write("Processed " + whitespace + " whitespaces");

    }


    // ------------------------------------------------------ SAX Error Methods


    /**
     * Receive notification of a parser error.
     *
     * @param e The parser exception being reported
     *
     * @exception SAXException if a parsing error occurs
     */
    public void error(SAXParseException e) throws SAXException {

        StaticLogger.write("[Error] " +
                           getLocationString(e) + ": " +
                           e.getMessage());

    }


    /**
     * Receive notification of a fatal error.
     *
     * @param e The parser exception being reported
     *
     * @exception SAXException if a parsing error occurs
     */
    public void fatalError(SAXParseException e) throws SAXException {

        StaticLogger.write("[Fatal] " +
                           getLocationString(e) + ": " +
                           e.getMessage());

    }


    /**
     * Receive notification of a parser warning.
     *
     * @param e The parser exception being reported
     *
     * @exception SAXException if a parsing error occurs
     */
    public void warning(SAXParseException e) throws SAXException {

        StaticLogger.write("[Warning] " +
                           getLocationString(e) + ": " +
                           e.getMessage());

    }


    /**
     * Return the location at which this exception occurred.
     *
     * @param e The SAXParseException we are reporting on
     */
    private String getLocationString(SAXParseException e) {

        StringBuffer sb = new StringBuffer();
        String systemId = e.getSystemId();
        if (systemId != null) {
            int index = systemId.lastIndexOf('/');
            if (index != -1)
                systemId = systemId.substring(index + 1);
            sb.append(systemId);
        }
        sb.append(':');
        sb.append(e.getLineNumber());
        sb.append(':');
        sb.append(e.getColumnNumber());
        return (sb.toString());

    }


    // ------------------------------------------------------ SAX Event Methods


    /**
     * Character data event handler.
     *
     * @param ch Character array containing the characters
     * @param start Starting position in the array
     * @param length Number of characters to process
     *
     * @exception SAXException if a parsing error occurs
     */
    public void characters(char ch[], int start, int length)
        throws SAXException {

        characters += length;

    }


    /**
     * Ignorable whitespace event handler.
     *
     * @param ch Character array containing the characters
     * @param start Starting position in the array
     * @param length Number of characters to process
     *
     * @exception SAXException if a parsing error occurs
     */
    public void ignorableWhitespace(char ch[], int start, int length)
        throws SAXException {

        whitespace += length;

    }


    /**
     * Start of element event handler.
     *
     * @param name The element type name
     * @param attrs The specified or defaulted attributes
     *
     * @exception SAXException if a parsing error occurs
     */
    public void startElement(String name, AttributeList attrs) {

        elements++;
        if (attrs != null)
            attributes += attrs.getLength();

    }



}
