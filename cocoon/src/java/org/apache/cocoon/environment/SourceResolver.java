/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment;

import org.apache.cocoon.ProcessingException;
import org.xml.sax.SAXException;

import java.io.IOException;

/**
 * Base interface for resolving a source by system identifiers.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:15 $
 */

public interface SourceResolver {

    /**
     * Resolve the source.
     * @param systemID This is either a system identifier
     * (<code>java.net.URL</code> or a local file.
     */
    Source resolve(String systemID)
    throws ProcessingException, SAXException, IOException;
}

