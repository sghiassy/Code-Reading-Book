/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.xml;

import org.w3c.dom.Node;

/**
 * This interface must be implemented by classes willing
 * to provide an XML representation of their current state.
 * <br/>
 * This interface exists in both Cocoon 1 and Cocoon 2 and to ensure
 * a minimal compatibility between the two versions.
 * <br/>
 * Cocoon 2 only objects can implement the SAX-only <code>XMLizable</code>
 * interface.
 *
 * @author <a href="mailto:sylvain.wallez@anyware-tech.com">Sylvain Wallez</a>
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a> for the original XObject class
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:22 $
 */

public interface XMLFragment extends XMLizable {
// Now inherited from XMLizable
//    /**
//     * Generates SAX events representing the object's state
//     * for the given content handler.
//     */
//    void toSAX(ContentHandler handler) throws SAXException;

    /**
     * Appends children representing the object's state to the given node.
     */
    void toDOM(Node node) throws Exception;
}
