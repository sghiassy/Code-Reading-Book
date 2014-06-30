/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.wrapper.EnvironmentWrapper;

import java.io.IOException;

/**
 * Wrapper for sitemap redirection
 *
 * @author <a href="mailto:cziegeler@sundn.de">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:20 $
 */
public class SitemapRedirector implements Redirector {
    private boolean hasRedirected = false;
    private Environment e;

    /**
     * Constructor with environment--so redirection happens as expected
     */
    public SitemapRedirector(Environment e) {
        this.e = e;
    }

    /**
     * Perform actual redirection
     */
    public void redirect(boolean sessionMode, String url) throws IOException {
        e.redirect(sessionMode, url);
        this.hasRedirected = true;
    }
    public void globalRedirect(boolean sessionMode, String url) throws IOException {
        if (e instanceof EnvironmentWrapper) {
          ((EnvironmentWrapper)e).globalRedirect(sessionMode,url);
        }
        else {
          e.redirect(sessionMode, url);
        }
        this.hasRedirected = true;
    }

    /**
     * Perform check on whether redirection has occured or not
     */
    public boolean hasRedirected() {
        return this.hasRedirected;
    }
}
