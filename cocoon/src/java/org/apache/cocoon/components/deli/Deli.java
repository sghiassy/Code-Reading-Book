/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.deli;

import org.apache.avalon.framework.component.Component;
import org.apache.cocoon.environment.Request;
import org.w3c.dom.Document;
import java.io.IOException;

/**
 * A component for providing CC/PP and UAProf support using the DELI
 * library.
 *
 * @author <a href="mailto:marbut@hplb.hpl.hp.com">Mark H. Butler</a>
 * @version CVS $ $ $Date: 2002/01/15 07:25:04 $
 */

public interface Deli extends Component {

    String ROLE = "org.apache.cocoon.components.deli.Deli";

    /**
     * Convert a profile stored as a vector of profile attributes
     *  to a DOM tree.
     *
     *@param	theRequest	The Request.
     *@return	The DOM tree.
     */
    Document getUACapabilities(Request theRequest)
    throws IOException, Exception;
}

