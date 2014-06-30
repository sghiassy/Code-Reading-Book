/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.transformation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.xml.IncludeXMLConsumer;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.IOException;
import java.util.Map;

/**
 * This transformer triggers for the element <code>include</code> in the
 * namespace "http://apache.org/cocoon/include/1.0".
 * The <code>src</code> attribute contains the url which points to
 * an xml resource which is include instead of the element.
 * With the attributes <code>element</code>, <code>ns</code> and
 * <code>prefix</code> it is possible to specify an element
 * which surrounds the included content.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/07 13:09:02 $ $Author: cziegeler $
 */
public class CIncludeTransformer extends AbstractTransformer
implements Recyclable, Composable {

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
    public final void compose(final ComponentManager manager) {
        this.manager = manager;
    }

    /**
     * Recycle the component
     */
    public void recycle() {
        super.recycle();
        this.sourceResolver = null;
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

    protected void processCIncludeElement(String src, String element, String ns, String prefix)
    throws SAXException {

        if (element == null) element="";
        if (ns == null) ns="";
        if (prefix == null) prefix="";

        getLogger().debug("Processing CInclude element: src=" + src
                          + ", element=" + element
                          + ", ns=" + ns
                          + ", prefix=" + prefix);

        IncludeXMLConsumer consumer = new IncludeXMLConsumer(this);

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
            source.toSAX(consumer);
        } catch (IOException e) {
            getLogger().error("CIncludeTransformer", e);
            throw new SAXException("CIncludeTransformer could not read resource", e);
        } catch (ProcessingException e){
            getLogger().error("Could not stream input", e);
            throw new SAXException("Exception in CIncludeTransformer",e);
        } finally {
            source.recycle();
        }

        if (!"".equals(element)) {
            super.endElement(ns, element, (!ns.equals("") && !prefix.equals("") ? prefix+":"+element : element));
            if (!ns.equals("")) {
                super.endPrefixMapping(prefix);
            }
        }
    }
}
