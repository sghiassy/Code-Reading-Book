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

import java.io.*;
import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Map;

/**
 * This environment is sample the links of the resource.
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/22 00:17:12 $
 */

public class LinkSamplingEnvironment extends AbstractCommandLineEnvironment
{

    private boolean skip = false;

    public LinkSamplingEnvironment( String uri,
                                    File contextFile,
                                    Map attributes,
                                    Map parameters,
                                    Logger log )
    throws MalformedURLException, IOException
    {
        super( uri, Constants.LINK_VIEW, contextFile, new ByteArrayOutputStream(), log );
        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug( "LinkSamplingEnvironment: uri = " + uri );
        }
        this.objectModel.put( Constants.REQUEST_OBJECT, new CommandLineRequest( this, null, uri, null, attributes, parameters ) );
        this.objectModel.put( Constants.RESPONSE_OBJECT, new CommandLineResponse() );
    }

    /**
     * Set the ContentType
     */
    public void setContentType( String contentType )
    {
        if ( ! Constants.LINK_CONTENT_TYPE.equals( contentType ) )
        {
            this.skip = true;
        }
    }

    /**
     * Indicates if other links are present.
     */
    public Collection getLinks() throws IOException
    {
        ArrayList list = new ArrayList();
        if ( !skip )
        {
            BufferedReader buffer = new BufferedReader(
                                        new InputStreamReader(
                                            new ByteArrayInputStream(
                                                ( ( ByteArrayOutputStream ) super.stream ).toByteArray()
                                            )
                                        )
                                    );
            while ( true )
            {
                String line = buffer.readLine();
                if ( line == null )
                    break;
                list.add( line );
            }
        }
        return list;
    }
}
