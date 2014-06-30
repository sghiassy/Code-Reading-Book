/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment.commandline;

import org.apache.cocoon.Constants;
import org.apache.log.Logger;

import java.io.File;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.util.Map;

/**
 * This environment is used to save the requested file to disk.
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */

public class FileSavingEnvironment extends AbstractCommandLineEnvironment {

    public FileSavingEnvironment(String uri,
                                 File context,
                                 Map attributes,
                                 Map parameters,
                                 Map links,
                                 OutputStream stream,
                                 Logger log)
    throws MalformedURLException {
        super(uri, null, context, stream, log);
        this.getLogger().debug("FileSavingEnvironment: uri = " + uri);
        this.objectModel.put(Constants.LINK_OBJECT, links);
        this.objectModel.put(Constants.REQUEST_OBJECT, new CommandLineRequest(this, null, uri, null, attributes, parameters));
        this.objectModel.put(Constants.RESPONSE_OBJECT, new CommandLineResponse());
    }
}


