/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.source;

import org.apache.avalon.framework.component.Component;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Source;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;

/**
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version $Id: SourceHandler.java,v 1.1 2002/01/03 12:31:14 giacomo Exp $
 */
public interface SourceHandler extends Component {

    String ROLE = "org.apache.cocoon.components.source.SourceHandler";

    /**
     * Get a <code>Source</code> object.
     * @param environment This is optional.
     */
    Source getSource(Environment environment, String location)
    throws ProcessingException, MalformedURLException, IOException;

    /**
     * Get a <code>Source</code> object.
     * @param environment This is optional.
     */
    Source getSource(Environment environment, URL base, String location)
    throws ProcessingException, MalformedURLException, IOException;

    /**
     * Add a new source factory.
     * The factory is initialized by the handler, this means the
     * handler test for the Avalon interfaces <code>Composable</code>,
     * <code>Contextualizable</code> and <code>Loggable</code>.
     * When the handler is disposed it should also test the
     * <code>Disposable</code> interface.
     * If a factory with the protocol already exists it is
     * overridden by this new factory.
     */
    void addFactory(String protocol, SourceFactory factory)
    throws ProcessingException;

}
