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

/**
 * This interface indicates an XLinkHandler that uses the same
 * event driven design patterns that SAX enforces.
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:23 $
 */
public interface XLinkHandler  {

    void simpleLink(String href, String role, String arcrole, String title, String show, String actuate, String uri, String name, String raw, Attributes attr) throws SAXException;

    void startExtendedLink(String role, String title, String uri, String name, String raw, Attributes attr) throws SAXException;

    void endExtendedLink(String uri, String name, String raw) throws SAXException;

    void startLocator(String href, String role, String title, String label, String uri, String name, String raw, Attributes attr) throws SAXException;

    void endLocator(String uri, String name, String raw) throws SAXException;

    void startArc(String arcrole, String title, String show, String actuate, String from, String to, String uri, String name, String raw, Attributes attr) throws SAXException;

    void endArc(String uri, String name, String raw) throws SAXException;

    void linkResource(String role, String title, String label, String uri, String name, String raw, Attributes attr) throws SAXException;

    void linkTitle(String uri, String name, String raw, Attributes attr) throws SAXException;

}

