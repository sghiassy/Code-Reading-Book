/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.sax;

import org.apache.cocoon.xml.XMLConsumer;

/**
 * This interfaces identifies classes that serialize XML data, receiving
 * notification of SAX events.
 * <br>
 * It's beyond the scope of this interface to specify the format for
 * the serialized data.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:13 $
 */
public interface XMLSerializer extends XMLConsumer {

    String ROLE = "org.apache.cocoon.components.sax.XMLSerializer";
    /**
     * Get the serialized xml data
     *
     * @return The serialized xml data.
     */
    Object getSAXFragment();
}
