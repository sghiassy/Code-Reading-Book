/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import org.apache.avalon.framework.component.Component;

import java.io.IOException;
import java.io.OutputStream;

/**
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:20 $
 */
public interface SitemapOutputComponent extends Component {
    /**
     * Set the <code>OutputStream</code> where the requested resource should
     * be serialized.
     */
    void setOutputStream(OutputStream out) throws IOException;

    /**
     * Get the mime-type of the output of this <code>Component</code>.
     */
    String getMimeType();

    /**
     * Test if the component wants to set the content length
     */
    boolean shouldSetContentLength();
}
