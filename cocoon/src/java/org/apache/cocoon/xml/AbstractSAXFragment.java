/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.xml;

import org.apache.cocoon.xml.dom.DOMBuilder;
import org.w3c.dom.Node;

/**
 * Abstract implementation of {@link XMLFragment} for objects that are more easily represented
 * as SAX events.
 * <br/>
 * The toDOM() method is implemented by piping in a <code>DOMBuilder</code> the results
 * of <code>toSAX()</code> that must be implemented by concrete subclasses.
 * 
 * @author <a href="mailto:sylvain.wallez@anyware-tech.com">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:22 $
 */

public abstract class AbstractSAXFragment implements XMLFragment {

    /**
     * Appends children representing the object's state to the given node by using
     * the results of <code>toSAX()</code>.
     */
    public void toDOM(Node node) throws Exception
    {
        DOMBuilder builder = new DOMBuilder(node);
        this.toSAX(builder);
    }
}
