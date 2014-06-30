/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.reading;

import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.sitemap.SitemapModelComponent;
import org.apache.cocoon.sitemap.SitemapOutputComponent;
import org.xml.sax.SAXException;

import java.io.IOException;

/**
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/04 12:06:46 $
 */
public interface Reader extends SitemapModelComponent, SitemapOutputComponent {

    String ROLE = "org.apache.cocoon.reading.Reader";

    /**
     * Generate the response.
     */
    void generate()
    throws IOException, SAXException, ProcessingException;

    /**
     * @return the time the read source was last modified or 0 if it is not
     *         possible to detect
     */
    long getLastModified();
}
