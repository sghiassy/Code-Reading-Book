/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.xml.xlink;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

/**
 * This class extends the XLink semantic capabilities to understand those
 * elements that are have default linking semantics associated.
 *
 * This class reacts on 'href' and 'src' attributes and is able to understand
 * the semantics of XHTML/WML/SMIL/SVG and all the rest of the languages that
 * use either XLink of the above attributes.
 *
 * NOTE: this class is clearly a hack and is not future compatible, but
 * since many XML formats to date are not compatible with the XLink semantics
 * this is what we have to do to live in the bleeding edge. Once there will
 * be a way to remove this, that will be a happy day for XML and for Cocoon too.
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:23 $
 */

public abstract class ExtendedXLinkPipe extends XLinkPipe {

    public void startElement(String uri, String name, String raw, Attributes attr) throws SAXException {
        if (uri == null) {
            uri = "";
        }
        String href = attr.getValue(uri, "href");
        if (href != null) {
            simpleLink(href, null, null, null, null, null, uri, name, raw, attr);
            return;
        }

        String src = attr.getValue(uri, "src");
        if (src != null) {
            simpleLink(src, null, null, null, null, null, uri, name, raw, attr);
            return;
        }

        String background = attr.getValue(uri, "background");
        if (background != null) {
            simpleLink(background, null, null, null, null, null, uri, name, raw, attr);
            return;
        }
        super.startElement(uri, name, raw, attr);
    }

    public void simpleLink(String href, String role, String arcrole, String title, String show, String actuate, String uri, String name, String raw, Attributes attr) throws SAXException {
        AttributesImpl newattr = new AttributesImpl(attr);
        int hrefIndex = attr.getIndex(uri, "href");
        if (hrefIndex > -1) newattr.setValue(hrefIndex, href);
        int srcIndex = attr.getIndex(uri, "src");
        if (srcIndex > -1) newattr.setValue(srcIndex, href);
        int backgroundIndex = attr.getIndex(uri, "background");
        if (backgroundIndex > -1) newattr.setValue(backgroundIndex, href);
        super.startElement(uri, name, raw, newattr);
    }
}

