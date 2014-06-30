/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup;

import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.logger.AbstractLoggable;

import org.apache.cocoon.components.xslt.XSLTProcessor;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.ProcessingException;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.XMLFilter;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.XMLFilterImpl;

import javax.xml.transform.sax.TransformerHandler;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

/**
 * A code-generation logicsheet. This class is actually a wrapper for
 * a "standard" XSLT stylesheet stored as <code>trax.Templates</code>
 * object.  Though this will change shortly: a new markup language
 * will be used for logicsheet authoring; logicsheets written in this
 * language will be transformed into an equivalent XSLT stylesheet
 * anyway... This class should probably be based on an interface...
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/26 02:12:33 $
 */
public class Logicsheet extends AbstractLoggable
{
    /**
     * The Source object for this logicsheet.
     */
    private Source source;

    /**
     * the template namespace's list
     */
    protected Map namespaces = new HashMap();

    /**
     * The {@link org.apache.cocoon.components.xslt.XSLTProcessor} component.
     */
    private XSLTProcessor xsltProcessor;

    /**
     * The ComponentManager of this instance.
     */
    private ComponentManager manager;


    public Logicsheet(Source source, ComponentManager manager, SourceResolver resolver)
        throws ProcessingException
    {
        this.source = source;
        this.manager = manager;
        try {
            this.xsltProcessor = (XSLTProcessor)manager.lookup(XSLTProcessor.ROLE);
            this.xsltProcessor.setSourceResolver(resolver);
        } catch (ComponentException e) {
            throw new ProcessingException("Could not obtain XSLT processor", e);
        }
    }

    public Logicsheet(String systemId, ComponentManager manager, SourceResolver resolver)
        throws SAXException, IOException, ProcessingException
    {
        this(resolver.resolve(systemId), manager, resolver);
    }

    public String getSystemId()
    {
        return source.getSystemId();
    }

    /**
     * This will return the list of namespaces in this logicsheet.
     */
    public Map getNamespaces() throws ProcessingException
    {
        // Force the parsing of the Source or, if nothing changed,
        // return the old content of namespaces.
        getTransformerHandler();
        return namespaces;
    }

    /**
     * Obtain the TransformerHandler object that will perform the
     * transformation associated with this logicsheet.
     *
     * @return a <code>TransformerHandler</code> value
     */
    public TransformerHandler getTransformerHandler() throws ProcessingException
    {
        try {
            // If the Source object is not changed, the
            // getTransformerHandler() of XSLTProcessor will simply return
            // the old template object. If the Source is unchanged, the
            // namespaces are not modified either.
            XMLFilter saveNSFilter = new SaveNamespaceFilter(namespaces);
            return xsltProcessor.getTransformerHandler(source, saveNSFilter);
        } finally {
            // Release used resources
            source.recycle();
        }
    }

    /**
     * This filter listen for source SAX events, and register the declared
     * namespaces into a <code>Map</code> object.
     *
     * @see org.xml.sax.XMLFilter
     * @see org.xml.sax.ContentHandler
     */
    protected class SaveNamespaceFilter extends XMLFilterImpl {
        private Map originalNamepaces;

        /**
         * The contructor needs an initialized <code>Map</code> object where it
         * can store the found namespace declarations.
         * @param originalNamepaces a initialized <code>Map</code> instance.
         */
        public SaveNamespaceFilter(Map originalNamepaces) {
            this.originalNamepaces = originalNamepaces;
        }

        public void setParent(XMLReader reader) {
            super.setParent(reader);
            reader.setContentHandler(this);
        }

        public void startDocument() throws SAXException {
            super.startDocument();
        }

        public void startPrefixMapping(String prefix, String uri)
            throws SAXException
        {
            originalNamepaces.put(prefix,uri);
            super.startPrefixMapping(prefix, uri);
        }

        public void startElement (String namespaceURI, String localName,
                                  String qName, Attributes atts)
            throws SAXException
        {
            super.startElement(namespaceURI, localName, qName, atts);
        }
    }
}
