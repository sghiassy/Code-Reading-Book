/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.transformation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.components.xpath.XPathProcessor;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.xml.IncludeXMLConsumer;
import org.apache.cocoon.xml.dom.DOMStreamer;
import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;

import java.io.*;
import java.net.MalformedURLException;
import java.util.Map;
import java.util.Stack;

/**
 * My first pass at an XInclude transformation. Currently it should set the base URI
 * from the SAX Locator's system id but allow it to be overridden by xml:base
 * elements as the XInclude spec mandates. It's also got some untested code
 * that should handle inclusion of text includes, but that method isn't called
 * by the SAX event FSM yet.
 *
 * @author <a href="mailto:balld@webslingerZ.com">Donald Ball</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/25 03:58:30 $ $Author: vgritsenko $
 */
public class XIncludeTransformer extends AbstractTransformer implements Composable, Recyclable, Disposable {

    private SourceResolver resolver;

    /** XPath Processor */
    private XPathProcessor processor = null;

    protected ComponentManager manager = null;

    public static final String XMLBASE_NAMESPACE_URI = "http://www.w3.org/XML/1998/namespace";
    public static final String XMLBASE_ATTRIBUTE = "base";

    public static final String XINCLUDE_NAMESPACE_URI = "http://www.w3.org/2001/XInclude";
    public static final String XINCLUDE_INCLUDE_ELEMENT = "include";
    public static final String XINCLUDE_INCLUDE_ELEMENT_HREF_ATTRIBUTE = "href";
    public static final String XINCLUDE_INCLUDE_ELEMENT_PARSE_ATTRIBUTE = "parse";

    protected Source base_xmlbase_uri = null;

    /** The current XMLBase URI. We start with an empty "dummy" URL. **/
    protected Source current_xmlbase_uri = null;

    /** This is a stack of xml:base attributes which belong to our ancestors **/
    protected Stack xmlbase_stack = new Stack();

    /** namespace uri of the last element which had an xml:base attribute **/
    protected String last_xmlbase_element_uri = "";

    protected Stack xmlbase_element_uri_stack = new Stack();

    /** name of the last element which had an xml:base attribute **/
    protected String last_xmlbase_element_name = "";

    protected Stack xmlbase_element_name_stack = new Stack();

    public void setup(SourceResolver resolver, Map objectModel,
                      String source, Parameters parameters)
            throws ProcessingException, SAXException, IOException {
        this.resolver = resolver;
    }

    public void compose(ComponentManager manager) {
        this.manager = manager;
        try {
            this.processor = (XPathProcessor)this.manager.lookup(XPathProcessor.ROLE);
        } catch (Exception e) {
            getLogger().error("cannot obtain XPathProcessor", e);
        }
    }

    public void startElement(String uri, String name, String raw, Attributes attr) throws SAXException {
        String value;
        if ((value = attr.getValue(XMLBASE_NAMESPACE_URI,XMLBASE_ATTRIBUTE)) != null) {
            try {
                startXMLBaseAttribute(uri,name,value);
            } catch (ProcessingException e) {
                getLogger().debug("Rethrowing exception", e);
                throw new SAXException(e);
            }
        }
        if (XINCLUDE_NAMESPACE_URI.equals(uri) && XINCLUDE_INCLUDE_ELEMENT.equals(name)) {
            String href = attr.getValue("",XINCLUDE_INCLUDE_ELEMENT_HREF_ATTRIBUTE);
            String parse = attr.getValue("",XINCLUDE_INCLUDE_ELEMENT_PARSE_ATTRIBUTE);

            if (null == parse) parse="xml";

            try {
                processXIncludeElement(href, parse);
            } catch (ProcessingException e) {
                getLogger().debug("Rethrowing exception", e);
                throw new SAXException(e);
            } catch (IOException e) {
                getLogger().debug("Rethrowing exception", e);
                throw new SAXException(e);
            }
            return;
        }
        super.startElement(uri,name,raw,attr);
    }

    public void endElement(String uri, String name, String raw) throws SAXException {
        if (last_xmlbase_element_uri.equals(uri) && last_xmlbase_element_name.equals(name)) {
            endXMLBaseAttribute();
        }
        if (uri != null && name != null && uri.equals(XINCLUDE_NAMESPACE_URI) && name.equals(XINCLUDE_INCLUDE_ELEMENT)) {
            return;
        }
        super.endElement(uri,name,raw);
    }

    public void setDocumentLocator(Locator locator) {
        try {
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("setDocumentLocator called " + locator.getSystemId());
            }

            base_xmlbase_uri = this.resolver.resolve(locator.getSystemId());

            // If url ends with .xxx then truncate to dir
            if (base_xmlbase_uri.getSystemId().lastIndexOf('.') > base_xmlbase_uri.getSystemId().lastIndexOf('/')) {
               String uri = base_xmlbase_uri.getSystemId().substring(0,base_xmlbase_uri.getSystemId().lastIndexOf('/')+1);
               base_xmlbase_uri.recycle();
               base_xmlbase_uri = null;
               base_xmlbase_uri = this.resolver.resolve(uri);
            }

            if (current_xmlbase_uri == null) {
               current_xmlbase_uri = base_xmlbase_uri;
            }

        } catch (Exception e) {
            getLogger().debug("Exception ignored", e);
        }
        super.setDocumentLocator(locator);
    }

    protected void startXMLBaseAttribute(String uri, String name, String value) throws ProcessingException {
        String urlLoc = value;

        if (! urlLoc.endsWith("/")) {
            urlLoc += "/";
        }

        if (getLogger().isDebugEnabled()) {
            getLogger().debug("XMLBase = " + urlLoc);
        }

        if (current_xmlbase_uri != null) {
            xmlbase_stack.push(current_xmlbase_uri);
        }

        try {
            current_xmlbase_uri = this.resolver.resolve(urlLoc);
    
            xmlbase_element_uri_stack.push(last_xmlbase_element_uri);
            last_xmlbase_element_uri = uri;
    
            xmlbase_element_name_stack.push(last_xmlbase_element_name);
            last_xmlbase_element_name = name;
        } catch (ProcessingException e) {
            throw e;
        } catch (Exception e) {
            throw new ProcessingException("Could not resolve '" + urlLoc + "'", e);
        }
    }

    protected void endXMLBaseAttribute() {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("XMLBase ended");
        }

        if (xmlbase_stack.size() > 0) {
            current_xmlbase_uri = (Source)xmlbase_stack.pop();
        } else {
            current_xmlbase_uri = base_xmlbase_uri;
        }
        last_xmlbase_element_uri = (String)xmlbase_element_uri_stack.pop();
        last_xmlbase_element_name = (String)xmlbase_element_name_stack.pop();
    }

    protected void processXIncludeElement(String href, String parse) throws SAXException,ProcessingException,IOException {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Processing XInclude element: href="+href+", parse="+parse);
            if(current_xmlbase_uri == null)
                getLogger().debug("Base URI: null");
            else
                getLogger().debug("Base URI: " + current_xmlbase_uri.getSystemId());
        }

        Source url = null;
        String suffix;
        try {
            int index = href.indexOf('#');
            if (index < 0) {
                if(current_xmlbase_uri == null)
                    url = this.resolver.resolve(href);
                else
                    url = this.resolver.resolve(current_xmlbase_uri.getSystemId() + href);
                suffix = "";
            } else {
                if(current_xmlbase_uri == null)
                    url = this.resolver.resolve(href.substring(0,index));
                else
                    url = this.resolver.resolve(current_xmlbase_uri.getSystemId() + href.substring(0,index));
                suffix = href.substring(index+1);
            }
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("URL: "+url+"\nSuffix: "+suffix);
            }
        
            if (parse.equals("text")) {
                getLogger().debug("Parse type is text");
                InputStream input = url.getInputStream();
                Reader reader = new BufferedReader(new InputStreamReader(input));
                int read;
                char ary[] = new char[1024];
                if (reader != null) {
                    while ((read = reader.read(ary)) != -1) {
                        super.characters(ary,0,read);
                    }
                    reader.close();
                }
            } else if (parse.equals("xml")) {
                getLogger().debug("Parse type is XML");
                Parser parser = null;
                try {
                    parser = (Parser)manager.lookup(Parser.ROLE);
        
                    InputSource input = url.getInputSource();
        
                    if (suffix.startsWith("xpointer(") && suffix.endsWith(")")) {
                        String xpath = suffix.substring(9,suffix.length()-1);
                        getLogger().debug("XPath is "+xpath);
                        Document document = parser.parseDocument(input);
                        NodeList list = processor.selectNodeList(document,xpath);
                        DOMStreamer streamer = new DOMStreamer(super.contentHandler,super.lexicalHandler);
                        int length = list.getLength();
                        for (int i=0; i<length; i++) {
                            streamer.stream(list.item(i));
                        }
                    } else {
                        IncludeXMLConsumer xinclude_handler = new IncludeXMLConsumer(super.contentHandler,super.lexicalHandler);
                        xinclude_handler.setLogger(getLogger());
                        parser.setConsumer(xinclude_handler);
                        parser.parse(input);
                    }
                } catch(SAXException e) {
                    getLogger().error("Error in processXIncludeElement", e);
                    throw e;
                } catch(ProcessingException e) {
                    getLogger().error("Error in processXIncludeElement", e);
                    throw e;
                } catch(MalformedURLException e) {
                    getLogger().error("Error in processXIncludeElement", e);
                    throw e;
                } catch(IOException e) {
                    getLogger().error("Error in processXIncludeElement", e);
                    throw e;
                } catch(ComponentException e) {
                    getLogger().error("Error in processXIncludeElement", e);
                    throw new SAXException(e);
                } finally {
                    if(parser != null) this.manager.release(parser);
                }
            }
        } finally {
            if (url != null) url.recycle();
        }
    }

    public void recycle()
    {
        // Reset all variables to initial state.
        this.resolver = null;
        if (base_xmlbase_uri != null) base_xmlbase_uri.recycle();
        base_xmlbase_uri = null;
        if (current_xmlbase_uri != null) current_xmlbase_uri.recycle();
        current_xmlbase_uri = null;
        xmlbase_stack = new Stack();
        last_xmlbase_element_uri = "";
        xmlbase_element_uri_stack = new Stack();
        last_xmlbase_element_name = "";
        xmlbase_element_name_stack = new Stack();
        super.recycle();
    }

    public void dispose()
    {
        this.manager.release((Component)this.processor);
    }
}
