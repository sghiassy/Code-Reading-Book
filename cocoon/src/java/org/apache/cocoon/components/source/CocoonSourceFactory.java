/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.source;

import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.Processor;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;

/**
 * This class implements the cocoon: protocol.
 * It cannot be configured like the other source factories
 * as it needs the current <code>Sitemap</code> as input.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:14 $
 */

public final class CocoonSourceFactory
extends AbstractLoggable
implements SourceFactory {

    /** The processor */
    private Processor    processor;

    /** The component manager */
    private ComponentManager  manager;

    public CocoonSourceFactory(Processor processor,
                               ComponentManager manager) {
        this.processor = processor;
        this.manager = manager;
    }

    /**
     * Resolve the source
     */
    public Source getSource(Environment environment, String location)
    throws ProcessingException, IOException, MalformedURLException {
        if (environment == null)
            throw new ProcessingException("CocoonSourceFactory: environment is required.");
        return new SitemapSource(environment,
                                 this.manager,
                                 this.processor,
                                 location,
                                 this.getLogger());
    }

    /**
     * Resolve the source
     */
    public Source getSource(Environment environment, URL base, String location)
    throws ProcessingException, IOException, MalformedURLException {
        if (environment == null)
            throw new ProcessingException("CocoonSourceFactory: environment is required.");
        return this.getSource(environment, base.toExternalForm() + location);
    }
}
