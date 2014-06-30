/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup.xsp;

import org.apache.avalon.framework.logger.Loggable;
import org.apache.avalon.excalibur.pool.Recyclable;

import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.language.markup.AbstractMarkupLanguage;
import org.apache.cocoon.components.language.markup.LogicsheetCodeGenerator;
import org.apache.cocoon.components.language.programming.ProgrammingLanguage;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.log.Logger;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.XMLFilter;
import org.xml.sax.helpers.AttributesImpl;
import org.xml.sax.helpers.XMLFilterImpl;

import java.io.File;
import java.io.IOException;
import java.util.*;

/**
 * This class implements <code>MarkupLanguage</code> for Cocoon's
 * <a href="http://xml.apache.org/cocoon/userdocs/xsp/">XSP</a>.
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @author <a href="mailto:ssahuc@apache.org">Sebastien Sahuc</a>
 * @version CVS $Revision: 1.4 $ $Date: 2002/01/29 15:46:08 $
 */
public class XSPMarkupLanguage extends AbstractMarkupLanguage {

    /**
     * store the dependencies.
     *
     * FIXME (SSA) Should not be shared between different calls.
     * Should be passed as argument of method getPreprocessFilter ?
     */
    private Set dependencies;

    /**
     * The default constructor.
     */
    public XSPMarkupLanguage() throws SAXException, IOException {
        super();
        dependencies = new HashSet();
    }

    public void recycle()
    {
        super.recycle();
        this.dependencies.clear();
    }


    /**
     * Return the XSP language name: <i>xsp</i> :-)
     *
     * @return The <i>xsp</i> constant
     */
    public String getName() {
        return "xsp";
    }

    /**
     * FIXME (SSA) : See interface. For now returns null.
     *
     * Return the document-declared encoding or <code>null</code> if it's the
     * platform's default encoding
     *
     * @return The document-declared encoding
     */
    public String getEncoding() {
        return null;
    }

    /**
     * Prepare the input source for logicsheet processing and code generation
     * with a preprocess filter.
     * The return <code>XMLFilter</code> object is the first filter on the
     * transformer chain.
     *
     * The XSP preprocess filter adds information on the root element such as
     * creation-date, file-name and file-path, plus it use the the passed
     * programming language to quote <code>Strings</code> on PCDATA node.
     *
     * @param filename The source filename
     * @param language The target programming language
     * @return The preprocess filter
     *
     * @see XSPMarkupLanguage.PreProcessFilter
     */
    protected XMLFilter getPreprocessFilter( String filename, ProgrammingLanguage language  )
    {
        PreProcessFilter filter = new PreProcessFilter(filename, language);
        filter.setLogger(getLogger());
        return filter;
    }

    /**
     * Add a dependency on an external file to the document for inclusion in
     * generated code. This is used to populate a list of <code>File</code>'s
     * tested for change on each invocation; this information is used to assert
     * whether regeneration is necessary. XSP uses &lt;xsp:dependency&gt;
     * elements for this purpose.
     *
     * @param location The file path of the dependent file
     * @see <code>AbstractMarkupLanguage</code>, <code>ServerPagesGenerator</code>
     *      and <code>AbstractServerPage</code>
     */
    protected void addDependency(String location) {
        dependencies.add(location);
    }

    // This is required here to avoid IllegalAccessError when 
    // XSPTransformerChainBuilderFilter invokes the method.
    protected void addLogicsheetToList(LanguageDescriptor language,
                                       String logicsheetLocation,
                                       SourceResolver resolver)
        throws IOException, SAXException, ProcessingException
    {
        super.addLogicsheetToList(language, logicsheetLocation, resolver);
    }

    /**
     * Returns a filter that chain on the fly the requested transformers for source
     * code generation. This method scans the input SAX events for
     * &lt;?xml-logicsheet?&gt; processing instructions and top-level
     * &lt;xsp:logicsheet&gt; elements. Logicsheet declarations are removed from
     * the input document.
     *
     * @param logicsheetMarkupGenerator the logicsheet markup generator
     * @param language the language descriptor
     * @param resolver the entity resolver
     * @return XMLFilter the filter that build on the fly the transformer chain
     */
    protected TransformerChainBuilderFilter getTransformerChainBuilder(
        LogicsheetCodeGenerator logicsheetMarkupGenerator,
        SourceResolver resolver)
    {
        XSPTransformerChainBuilderFilter filter =
            new XSPTransformerChainBuilderFilter(
                logicsheetMarkupGenerator, resolver);
        filter.setLogger(getLogger());
        return filter;
    }

//
//  Inner classes
//

    /**
     * Preprocess filter for XSP Markup language.
     * It looks for PI event other that &lt;?xml-logisheet href=&quot;...&quot;&gt;
     * for quoting them;
     * It adds creation-date, file-name and file-path attributes to the root
     * Element;
     * And it quotes the PCDATA based by calling the quote method of the
     * programming language.
     *
     * @see org.xml.sax.ContentHandler
     */
    protected class PreProcessFilter extends XMLFilterImpl implements Loggable {
        protected Logger log;

        private Stack stack;

        private String filename;

        private boolean isRootElem;

        private ProgrammingLanguage language;

        /**
         * @param filename the filename
         * @param the programming language
         */
        public PreProcessFilter (String filename, ProgrammingLanguage language) {
            super ();
            this.filename = filename;
            this.language = language;
        }

        public void setLogger(Logger logger) {
            if (this.log == null) {
                this.log = logger;
            }
        }

        public void startDocument() throws SAXException {
            super.startDocument();
            isRootElem = true;
            stack = new Stack();
        }

        public void processingInstruction(String target, String data) throws SAXException {
            if (!"xml-logicsheet".equals(target)) {
              data = this.language.quoteString(data);
            }
            super.processingInstruction(target, data);
        }

        public void startElement (String namespaceURI, String localName,
                          String qName, Attributes atts) throws SAXException {
             if (isRootElem) {
                 stack.push(new String[] { namespaceURI, localName, qName} );
                 isRootElem=false;
                // Store path and file name
                int pos = this.filename.lastIndexOf(File.separatorChar);
                String name = this.filename.substring(pos + 1);
                String path = this.filename.substring(0, pos).replace(File.separatorChar, '/');
                // update the attributes
                AttributesImpl newAtts = new AttributesImpl(atts);
                newAtts.addAttribute("", "file-name", "file-name", "CDATA", name);
                newAtts.addAttribute("", "file-path", "file-path", "CDATA", path);
                newAtts.addAttribute("", "creation-date", "creation-date", "CDATA",
                    String.valueOf(System.currentTimeMillis())
                );
                // forward element with the modified attribute
                super.startElement(namespaceURI, localName, qName, newAtts);
            } else {
                stack.push(new String[] { namespaceURI, localName, qName} );
                super.startElement(namespaceURI, localName, qName, atts);
            }
        }

        public void endElement (String namespaceURI, String localName,
                              String qName) throws SAXException {
            stack.pop();
            super.endElement(namespaceURI, localName, qName);
        }

        public void characters(char[] ch, int start, int length) throws SAXException {
            String[] tag = (String[]) stack.peek();
            String tagName = tag[2];
            if (
              tagName.equals("xsp:expr") ||
              tagName.equals("xsp:logic") ||
              tagName.equals("xsp:structure") ||
              tagName.equals("xsp:include")
            ) {
              super.characters(ch, start, length);
            } else {
                // Quote the string depending on the programming language
                String value = this.language.quoteString(String.valueOf(ch, start, length));
                // Create a new element <xsp:text that wrap the quoted PCDATA
                super.startElement(Constants.XSP_URI, "text", "xsp:text", new AttributesImpl() );
                super.characters(value.toCharArray(), 0, value.length());
                super.endElement(Constants.XSP_URI, "text", "xsp:text");
            }
        }
    }


    /**
     * This filter builds on the fly a chain of transformers. It extends the
     * <code>AbstractMarkupLanguage.TransformerChainBuilderFilter</code> so
     * it can add XSP specific features such as:
     * <ul>
     * <li>Looking for &lt;?xml-logisheet href=&quot;...&quot?;&gt; PI and
     *     &lt;xsp:xml-logisheet location=&quot;...&quot;&gt; elements to register
     *     user defined logicsheets;</li>
     * <li>Adding all the dependencies related to the XSP pages as
     *     &lt;xsp:dependency;&gt;...&lt;/xsp:dependency;&gt;</li>
     * </ul>
     *
     * @see org.xml.sax.ContentHandler
     */
    protected  class XSPTransformerChainBuilderFilter extends TransformerChainBuilderFilter implements Loggable {

        protected Logger log;

        private List startPrefix;

        private Object[] rootElement;

        private StringBuffer rootChars;

        private boolean isRootElem;

        private boolean insideRootElement;

        private boolean finished;

        /**
         * @param logicsheetMarkupGenerator the code generator
         * @param resolver the entity resolver
         */
        protected XSPTransformerChainBuilderFilter(
            LogicsheetCodeGenerator logicsheetMarkupGenerator,
            SourceResolver resolver)
        {
            super(logicsheetMarkupGenerator, resolver);
        }

        public void setLogger(Logger logger) {
            if (this.log == null) {
                this.log = logger;
            }
        }

        public void processingInstruction(String target, String data) throws SAXException {
            // Retrieve logicsheets declared by processing-instruction
            if ("xml-logicsheet".equals(target)) {
                int start = data.indexOf("href");
                if (start >= 0) {
                    // add 6, for lenght of 'href', plus '=' char, plus '"' char
                    start += 6;
                    // get the quote char. Can be " or '
                    char quote = data.charAt(start-1);
                    int end = data.indexOf(quote, start);
                    String href = data.substring(start, end);

                    try {
                        XSPMarkupLanguage.this.addLogicsheetToList(
                            language, href, this.resolver
                        );
                    } catch (ProcessingException pe) {
                        log.warn("ProcessingException in SitemapMarkupLanguage", pe);
                        throw new SAXException (pe);
                    } catch (IOException ioe) {
                        log.warn("XSPMarkupLanguage.processingInstruction", ioe);
                        throw new SAXException (ioe);
                    }
                }
                // Do not forward the PI event.
                return;
            }

            // Call super when this is not a logicsheet related PI
            super.processingInstruction(target,data);
        }

        public void startDocument () throws SAXException {
            isRootElem=true;
            insideRootElement=false;
            finished=false;
            startPrefix = new ArrayList();
            rootChars = new StringBuffer();
        }

        public void startElement (String namespaceURI, String localName,
                String qName, Attributes atts) throws SAXException {
            if (finished) {
                // Call super method
                super.startElement(namespaceURI, localName, qName, atts);
            } else {
                // Need more work
                if(isRootElem) {
                    isRootElem = false;
                    // Cache the root element and resend the SAX event when
                    // we've finished dealing with <xsp:logicsheet > elements
                    rootElement = new Object[4];
                    rootElement[0]=namespaceURI;
                    rootElement[1]=localName;
                    rootElement[2]=qName;
                    rootElement[3]=atts;
                } else {
                    insideRootElement = true;
                    // Retrieve logicsheets declared by top-level elements <xsp:logicsheet ...>
                    // And do not forward the startElement event
                    if ("xsp:logicsheet".equals(qName)) {
                        String href = atts.getValue("location");
                        try {
                            XSPMarkupLanguage.this.addLogicsheetToList(
                                language, href, this.resolver
                            );
                        } catch (ProcessingException pe) {
                            log.warn("XSPMarkupLanguage.startElement", pe);
                            throw new SAXException (pe);
                        } catch (IOException ioe) {
                            log.warn("XSPMarkupLanguage.startElement", ioe);
                            throw new SAXException (ioe);
                        }
                    } else {
                        // This element is not a <xsp:logicsheet> element, so finish
                        // by:
                        // * setting the 'fisnished' flag to true ;
                        // * refiring all the cached events ;
                        // * firing all the necessary event dealing with file dependencies
                        finished = true;

                        // Send SAX events 'startDocument'
                        super.startDocument();

                        // Send all prefix namespace
                        String [] prefixArray;
                        for (int i=0; i<startPrefix.size(); i++) {
                            prefixArray = (String []) startPrefix.get(i);
                            super.startPrefixMapping(prefixArray[0], prefixArray[1]);
                        }

                        // Send cached RootElement event
                        super.startElement(
                            (String) rootElement[0],
                            (String) rootElement[1],
                            (String) rootElement[2],
                            (Attributes) rootElement[3]
                        );

                        // Send cached characters
                        char[] ch = rootChars.toString().toCharArray();
                        super.characters( ch, 0, ch.length);

                        // Send the events dealing with dependencies.
                        // If some dependencies exist, then creates
                        // <xsp:dependency> elements
                        char[] locationChars;
                        Iterator iter = XSPMarkupLanguage.this.dependencies.iterator();
                        while(iter.hasNext()) {
                            super.startElement(
                                namespaceURI, "dependency",
                                "xsp:dependency", new AttributesImpl()
                            );
                            locationChars = ((String) iter.next()).toCharArray();
                            super.characters(locationChars, 0 , locationChars.length);
                            super.endElement(namespaceURI, "dependency", "xsp:dependency");
                        }

                        // And finally forward current Element.
                        super.startElement(namespaceURI, localName, qName, atts);
                    }
                }
            }
        }

        public void endElement (String namespaceURI, String localName,
                String qName) throws SAXException {
            if (finished) {
                // Forward the events
                super.endElement(namespaceURI, localName, qName);
            }
        }

        public void characters(char[] ch, int start, int length) throws SAXException {
            if (finished) {
                super.characters(ch, start, length);
            } else if(!insideRootElement) {
                // Caching the PCDATA for the root element
                rootChars.append(ch, start, length);
            }
        }

        public void startPrefixMapping(String prefix, String uri) throws SAXException {
            if(finished) {
                super.startPrefixMapping(prefix, uri);
            } else {
                String[] prefixArray = new String [2];
                prefixArray[0]= prefix;
                prefixArray[1]= uri;
                startPrefix.add(prefixArray);
            }
        }
    }
}
