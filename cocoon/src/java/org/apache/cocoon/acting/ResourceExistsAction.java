/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/**
 * This action simply checks to see if a given resource exists. It takes a
 * single parameter named 'url' and returns an empty map if it exists and
 * null otherwise. It has only been tested with context urls.
 *
 * @author <a href="mailto:balld@apache.org">Donald Ball</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $
 */
public class ResourceExistsAction extends ComposerAction implements ThreadSafe {

    public Map act(Redirector redirector, SourceResolver resolver, Map objectModel, String source, Parameters parameters) throws Exception {
        String urlstring = parameters.getParameter("url",null);
        Source src = null;
        try {
            src = resolver.resolve(urlstring);
            src.getInputStream();
        } catch (Exception e) {
            getLogger().debug("ResourceExistsAction: exception: ",e);
            return null;
        } finally {
            if (src != null) src.recycle();
        }
        return EMPTY_MAP;
    }

}
