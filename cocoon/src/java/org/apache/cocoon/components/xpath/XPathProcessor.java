/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.xpath;

import org.apache.avalon.framework.component.Component;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * This is the interface of the XPath processor in Cocoon.
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/07 14:08:41 $ $Author: cziegeler $
 */
public interface XPathProcessor
extends Component {
    /**
     * The role implemented by an <code>XSLTProcessor</code>.
     */
    String ROLE = "org.apache.cocoon.components.xpath.XPathProcessor";

    /**
     * Use an XPath string to select a single node. XPath namespace
     * prefixes are resolved from the context node, which may not
     * be what you want (see the next method).
     *
     * @param contextNode The node to start searching from.
     * @param str A valid XPath string.
     * @return The first node found that matches the XPath, or null.
     */
    Node selectSingleNode(Node contextNode, String str);

    /**
     *  Use an XPath string to select a nodelist.
     *  XPath namespace prefixes are resolved from the contextNode.
     *
     *  @param contextNode The node to start searching from.
     *  @param str A valid XPath string.
     *  @return A List, should never be null.
     */
    NodeList selectNodeList(Node contextNode, String str);
}
