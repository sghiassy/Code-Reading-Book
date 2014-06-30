/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.xml.dom;

import org.w3c.dom.Document;

/**
 * This interface identifies classes producing instances of DOM
 * <code>Document</code> objects.
 *
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:23 $
 */
public interface DOMFactory {
    /**
     * Create a new Document object.
     */
    Document newDocument();

    /**
     * Create a new Document object with a specified DOCTYPE.
     */
    Document newDocument(String name);

    /**
     * Create a new Document object with a specified DOCTYPE, public ID and
     * system ID.
     */
    Document newDocument(String name, String publicId, String systemId);
}
