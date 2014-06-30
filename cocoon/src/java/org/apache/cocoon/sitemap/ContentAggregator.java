/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.caching.AggregatedCacheValidity;
import org.apache.cocoon.caching.CacheValidity;
import org.apache.cocoon.caching.Cacheable;
import org.apache.cocoon.caching.TimeStampCacheValidity;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.generation.Generator;
import org.apache.cocoon.util.HashUtil;
import org.apache.cocoon.xml.ContentHandlerWrapper;
import org.apache.cocoon.xml.XMLConsumer;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;

/**
 * @author <a href="mailto:giacomo@apache.org">Giacomo Pati</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Id: ContentAggregator.java,v 1.2 2002/01/22 00:17:13 vgritsenko Exp $
 */
public class ContentAggregator
extends ContentHandlerWrapper
implements Generator, Cacheable, Recyclable {

    /** the root element of the aggregated content */
    protected Element rootElement;

    /** the parts */
    protected ArrayList parts = new ArrayList();

    /** Empty Attributes */
    private AttributesImpl emptyAttrs = new AttributesImpl();

    /** Indicates the position in the stack of the root element of the aggregated content */
    private int rootElementIndex;

    /** The element used for the current part */
    protected Element currentElement;

    /** This object holds the part parts :) */
    protected final class Part {
        public String uri;
        public Element element;
        public Source source;
        boolean stripRootElement;

        public Part(String uri, Element element, String stripRoot) {
            this.uri = uri;
            this.element = element;
            this.stripRootElement = (stripRoot.equals("yes") || stripRoot.equals("true"));
        }
    }

    /** This object holds an element definition */
    protected final class Element {
        public String namespace;
        public String prefix;
        public String name;

        public Element(String name, String namespace, String prefix) {
            this.namespace = namespace;
            this.prefix = prefix;
            this.name = name;
        }
    }

    /**
     * generates the content
     */
    public void generate()
    throws IOException, SAXException, ProcessingException {
        if (this.getLogger().isDebugEnabled()) {
            getLogger().debug("ContentAggregator: generating aggregated content");
        }
        this.contentHandler.startDocument();
        this.startElem(this.rootElement);
        try {
            for (int i = 0; i < this.parts.size(); i++) {
                final Part part = (Part)this.parts.get(i);
                this.rootElementIndex = (part.stripRootElement ? -1 : 0);
                if (part.element != null) {
                    this.currentElement = part.element;
                    this.startElem(part.element);
                } else {
                    this.currentElement = this.rootElement;
                }

                try {
                    part.source.toSAX(this);
                } finally {
                    if (part.element != null) {
                        this.endElem(part.element);
                    }
                }
            }
        } finally {
            this.endElem(this.rootElement);
            this.contentHandler.endDocument();
        }
        getLogger().debug("ContentAggregator: finished aggregating content");
    }

    /**
     * Generate the unique key.
     * This key must be unique inside the space of this component.
     *
     * @return The generated key hashes the src
     */
    public long generateKey() {
        try {
            long key = HashUtil.hash("CA(" +
                                     this.rootElement.prefix +
                                     ':' +
                                     this.rootElement.name +
                                     '<' + this.rootElement.namespace + ">)");
            Source current;
            for (int i = 0; i < this.parts.size(); i++) {
                final Part part = (Part)this.parts.get(i);
                current = part.source;
                if (current.getLastModified() == 0) {
                        return 0;
                } else {
                    if (part.element == null) {
                        key += HashUtil.hash("P=" +
                                         part.stripRootElement + ':' +
                                         current.getSystemId() + ';');
                    } else {
                        key += HashUtil.hash("P=" +
                                         part.element.prefix +
                                         ':' +
                                         part.element.name +
                                         '<' + part.element.namespace + ">:" +
                                         part.stripRootElement + ':' +
                                         current.getSystemId() + ';');
                    }
                }
            }
            return key;
        } catch (Exception e) {
            getLogger().error("ContentAggregator: could not generateKey", e);
            return 0;
        }
    }

    /**
     * Generate the validity object.
     *
     * @return The generated validity object or <code>null</code> if the
     *         component is currently not cacheable.
     */
    public CacheValidity generateValidity() {
        try {
            AggregatedCacheValidity v = new AggregatedCacheValidity();
            Source current;
            for (int i = 0; i < this.parts.size(); i++) {
                current = ((Part)this.parts.get(i)).source;
                if (current.getLastModified() == 0) {
                        return null;
                } else {
                    v.add(new TimeStampCacheValidity(current.getLastModified()));
                }
            }
            return v;
        } catch (Exception e) {
            getLogger().error("ContentAggregator: could not generateKey", e);
            return null;
        }
    }

    /**
     * Set the root element. Please make sure that the parameters are not null!
     */
    public void setRootElement(String element, String namespace, String prefix) {
        this.rootElement = new Element(element,
                                       namespace,
                                       prefix);
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("ContentAggregator: root element='" + element +
                                   "' ns='" + namespace + "' prefix='" + prefix + "'");
        }
    }

    /**
     * Add a part. Please make sure that the parameters are not null!
     */
    public void addPart(String uri,
                        String element,
                        String namespace,
                        String stripRootElement,
                        String prefix) {
        Element elem = null;
        if (!element.equals("")) {
            if (namespace.equals("")) {
                elem = new Element(element,
                                   this.rootElement.namespace,
                                   this.rootElement.prefix);
            } else {
                elem = new Element(element,
                                   namespace,
                                   prefix);
            }
        }
        this.parts.add(new Part(uri,
                                elem,
                                stripRootElement));
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("ContentAggregator: part uri='" + uri +
                                   "' element='" + element + "' ns='" + namespace +
                                   "' stripRootElement='" + stripRootElement + "' prefix='" + prefix + "'");
        }
    }

    /**
     * Set the <code>XMLConsumer</code> that will receive XML data.
     * <br>
     * This method will simply call <code>setContentHandler(consumer)</code>
     * and <code>setLexicalHandler(consumer)</code>.
     */
    public void setConsumer(XMLConsumer consumer) {
        this.setContentHandler(consumer);
        this.setLexicalHandler(consumer);
    }

    /**
     * Recycle the producer by removing references
     */
    public void recycle() {
        super.recycle();
        this.rootElement = null;
        for (Iterator i = this.parts.iterator(); i.hasNext();)
            ((Part)i.next()).source.recycle();
        this.parts.clear();
        this.currentElement = null;
    }

    /**
     * Set the <code>SourceResolver</code>, object model <code>Map</code>,
     * the source and sitemap <code>Parameters</code> used to process the request.
     */
    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters par)
    throws ProcessingException, SAXException, IOException {
        // get the Source for each part
        for(int i=0; i<this.parts.size();i++) {
            final Part current = (Part)this.parts.get(i);
            current.source = resolver.resolve(current.uri);
        }
    }

    /**
     * Private method generating startElement event for the aggregated parts
     * and the root element
     */
    private void startElem(Element element)
    throws SAXException {
        final String qname = (element.prefix.equals("")) ? element.name : element.prefix + ':' + element.name;
        if (!element.namespace.equals("")) {
            this.contentHandler.startPrefixMapping(element.prefix, element.namespace);
        }
        this.contentHandler.startElement(element.namespace, element.name, qname, this.emptyAttrs);
    }

    /**
     * Private method generating endElement event for the aggregated parts
     * and the root element
     */
    private void endElem(Element element) throws SAXException {
        final String qname = (element.prefix.equals("")) ? element.name : element.prefix + ':' + element.name;
        this.contentHandler.endElement(element.namespace, element.name, qname);
        if (!element.namespace.equals("")) {
            this.contentHandler.endPrefixMapping(element.prefix);
        }
    }

    /**
     * Ignore start and end document events
     */
    public void startDocument() throws SAXException {
    }

    /**
     * Ignore start and end document events
     */
    public void endDocument() throws SAXException {
    }

    /**
     * Override startElement() event to add namespace and prefix
     */
    public void startElement(String namespaceURI, String localName, String raw, Attributes atts)
    throws SAXException {
        this.rootElementIndex++;
        if (this.rootElementIndex == 0) {
            getLogger().debug("ContentAggregator: skipping root element start event.");
            return;
        }
        if (namespaceURI == null || namespaceURI.equals("")) {
            final String qname = this.currentElement.prefix.equals("") ? localName : this.currentElement.prefix + ':' + localName;
            this.contentHandler.startElement(this.currentElement.namespace, localName, qname, atts);
        } else {
            this.contentHandler.startElement(namespaceURI, localName, raw, atts);
        }
    }

    /**
     * Override startElement() event to add namespace and prefix
     */
    public void endElement(String namespaceURI, String localName, String raw) throws SAXException {
        this.rootElementIndex--;
        if (this.rootElementIndex == -1) {
            getLogger().debug("ContentAggregator: skipping root element end event.");
            return;
        }
        if (namespaceURI == null || namespaceURI.equals("")) {
            final String qname = this.currentElement.prefix.equals("") ? localName : this.currentElement.prefix + ':' + localName;
            this.contentHandler.endElement(this.currentElement.namespace, localName, qname);
        } else {
            this.contentHandler.endElement(namespaceURI, localName, raw);
        }
    }
}
