/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.transformation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.caching.CacheValidity;
import org.apache.cocoon.caching.Cacheable;
import org.apache.cocoon.caching.IncludeCacheValidity;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.xml.IncludeXMLConsumer;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.IOException;
import java.util.Map;

/**
 * <p>This transformer triggers for the element <code>include</code> in the
 * namespace "http://apache.org/cocoon/include/1.0".
 * The <code>src</code> attribute contains the url which points to
 * an xml resource which is include instead of the element.
 * With the attributes <code>element</code>, <code>ns</code> and
 * <code>prefix</code> it is possible to specify an element
 * which surrounds the included content.</p>
 *
 * <p>Validity of cached pipelines is calculated not by comparing old and new
 * IncludeCacheValidity objects (as in AggregatedCacheValidity) but by comparing
 * timestamps. Validity object of cached pipeline contain two lists: source urls
 * and timestamps. When it comes to checking validity of cached pipeline we know
 * that generation/transformation steps before CIncludeTransformer are valid (otherwise
 * we would have had discarded cached pipeline already) so source url list
 * of new validity will be the same as of old one. Only timestamps have to be
 * recalculated and compared.</p>
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @author <a href="mailto:maciejka@tiger.com.pl">Maciek Kaminski</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/25 03:58:30 $ $Author: vgritsenko $
 */
public class CachingCIncludeTransformer extends AbstractTransformer
implements Recyclable, Composable, Cacheable {

    public static final String CINCLUDE_NAMESPACE_URI = "http://apache.org/cocoon/include/1.0";
    public static final String CINCLUDE_INCLUDE_ELEMENT = "include";
    public static final String CINCLUDE_INCLUDE_ELEMENT_SRC_ATTRIBUTE = "src";
    public static final String CINCLUDE_INCLUDE_ELEMENT_ELEMENT_ATTRIBUTE = "element";
    public static final String CINCLUDE_INCLUDE_ELEMENT_NS_ATTRIBUTE = "ns";
    public static final String CINCLUDE_INCLUDE_ELEMENT_PREFIX_ATTRIBUTE = "prefix";

    /** The <code>SourceResolver</code> */
    protected SourceResolver sourceResolver;

    /** The current <code>ComponentManager</code>. */
    protected ComponentManager manager = null;

    /** The current <code>IncludeCacheValidity</code>. */
    protected IncludeCacheValidity currentCacheValidity;

    /** The current <code>IncludeXMLConsumer</code> that ommits start and endDocument events. */
    protected IncludeXMLConsumer consumer;

    /**
     * Setup the component.
     */
    public void setup(SourceResolver resolver, Map objectModel,
                      String source, Parameters parameters)
    throws ProcessingException, SAXException, IOException {
        this.sourceResolver = resolver;
    }

    /**
     * Composable Interface
     */
    public final void compose(final ComponentManager manager) throws ComponentException {
        this.manager = manager;
    }

    /**
     * Recycle the component
     */
    public void recycle() {
        super.recycle();
        this.sourceResolver = null;
        this.currentCacheValidity = null;
    }

    public void startElement(String uri, String name, String raw, Attributes attr)
    throws SAXException {
        if (uri != null && name != null
            && uri.equals(CINCLUDE_NAMESPACE_URI)
            && name.equals(CINCLUDE_INCLUDE_ELEMENT)) {

            this.processCIncludeElement(attr.getValue("",CINCLUDE_INCLUDE_ELEMENT_SRC_ATTRIBUTE),
                                        attr.getValue("",CINCLUDE_INCLUDE_ELEMENT_ELEMENT_ATTRIBUTE),
                                        attr.getValue("",CINCLUDE_INCLUDE_ELEMENT_NS_ATTRIBUTE),
                                        attr.getValue("",CINCLUDE_INCLUDE_ELEMENT_PREFIX_ATTRIBUTE));

        } else {
            super.startElement(uri, name, raw, attr);
        }
    }

    public void endElement(String uri, String name, String raw) throws SAXException {
        if (uri != null && name != null
            && uri.equals(CINCLUDE_NAMESPACE_URI)
            && name.equals(CINCLUDE_INCLUDE_ELEMENT)) {
            return;
        }
        super.endElement(uri, name, raw);
    }

    public void endDocument()
    throws SAXException {
        super.endDocument();
        if(currentCacheValidity != null) {
            currentCacheValidity.setIsNew2False();
        }
    }

    protected void processCIncludeElement(String src, String element, String ns, String prefix)
    throws SAXException {

        if (element == null) element="";
        if (ns == null) ns="";
        if (prefix == null) prefix="";

        getLogger().debug("Processing CInclude element: src=" + src
                          + ", element=" + element
                          + ", ns=" + ns
                          + ", prefix=" + prefix);

        // complete validity information
        if(currentCacheValidity != null ) {
            Source temp = null;
            try {
                temp = sourceResolver.resolve(src);
                currentCacheValidity.add(src, temp.getLastModified());
                getLogger().debug("currentCacheValidity: " + currentCacheValidity);
            } catch (Exception e) {
                getLogger().error("CachingCIncludeTransformer could not resolve resource:" + src,  e);
                throw new SAXException("CachingCIncludeTransformer could not resolve resource", e);
            } finally {
                if (temp != null) temp.recycle();
            }
        }

        if (!"".equals(element)) {
            AttributesImpl attrs = new AttributesImpl();
            if (!ns.equals("")) {
                super.startPrefixMapping(prefix, ns);
            }
            super.startElement(ns,
                               element,
                               (!ns.equals("") && !prefix.equals("") ? prefix+":"+element : element),
                               attrs);
        }

        Source source = null;
        try {
            source = this.sourceResolver.resolve(src);
            source.toSAX(getConsumer());
        } catch (Exception e) {
            getLogger().error("CachingCIncludeTransformer", e);
            throw new SAXException("CachingCIncludeTransformer could not read resource", e);
        } finally {
            if (source != null) source.recycle();
        }

        if (!"".equals(element)) {
            super.endElement(ns, element, (!ns.equals("") && !prefix.equals("") ? prefix+":"+element : element));
            if (!ns.equals("")) {
                super.endPrefixMapping(prefix);
            }
        }
    }

    /**
     * Generate the unique key.
     * This key must be unique inside the space of this component.
     * CachingCIncludeTransformer always generates the same key since which documents
     * are included depends only on former generation/transformation stages.
     *
     * @return The generated key hashes the src
     */

    public long generateKey() {
            return 1;
    }

    /**
     * Generate the validity object.
     * CachingCIncludeTransformer generates "empty" IncludeCacheValidity
     * and completes it with validity data during transformation.
     * See processCIncludeElement method.
     *
     * @return The generated validity object or <code>null</code> if the
     *         component is currently not cacheable.
     */

    public CacheValidity generateValidity() {

        getLogger().debug("generateValidity");
        try {
            currentCacheValidity = new IncludeCacheValidity(sourceResolver);
            return currentCacheValidity;
        } catch (RuntimeException e) {
            getLogger().error("CachingCIncludeTransformer: could not generateKey", e);
            return null;
        }
    }

    protected IncludeXMLConsumer getConsumer() {
        if(consumer == null) {
            consumer = new IncludeXMLConsumer(this);
        }
        return consumer;
    }
}
