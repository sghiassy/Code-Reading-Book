/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment;

import java.io.IOException;

/**
 * Interface for an redirector abstraction
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:15 $
 */

public interface Redirector {

    /**
     * Redirect to the given URL
     */
    void redirect(boolean sessionmode, String url) throws IOException;
    void globalRedirect(boolean sessionmode, String url) throws IOException;
}

