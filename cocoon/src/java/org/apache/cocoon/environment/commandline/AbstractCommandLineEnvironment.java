/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment.commandline;

import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.AbstractEnvironment;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Session;
import org.apache.cocoon.environment.Source;
import org.apache.log.Logger;
import org.xml.sax.SAXException;

import java.io.File;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.MalformedURLException;

/**
 * This environment is used to save the requested file to disk.
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/25 03:46:35 $
 */

public abstract class AbstractCommandLineEnvironment
extends AbstractEnvironment
implements Redirector {

    protected String contentType;
    protected OutputStream stream;
    protected int contentLength;

    public AbstractCommandLineEnvironment(String uri,
                                          String view,
                                          File context,
                                          OutputStream stream,
                                          Logger log)
    throws MalformedURLException {
        super(uri, view, context);
        this.setLogger(log);
        this.stream = stream;
    }

    /**
     * Redirect the client to a new URL
     */
    public void redirect(boolean sessionmode, String newURL)
    throws IOException {
        if (sessionmode) {
            CommandLineSession.getSession(true);
        }

        // fix all urls created with request.getScheme()+... etc.
        if (newURL.startsWith("cli:/")) {
            int pos = newURL.indexOf('/', 6);
            newURL = newURL.substring(pos+1);
        }

        // fix all relative urls to use to cocoon: protocol
        if (newURL.indexOf(":") == -1) {
            newURL = "cocoon:/" + newURL;
        }

        // FIXME: this is a hack for the links view
        if (newURL.startsWith("cocoon:")
            && this.getView() != null
            && this.getView().equals(Constants.LINK_VIEW)) {

            // as the internal cocoon protocol is used the last
            // serializer is removed from it! And therefore
            // the LinkSerializer is not used.
            // so we create one without Avalon...
            org.apache.cocoon.serialization.LinkSerializer ls =
                new org.apache.cocoon.serialization.LinkSerializer();
            ls.setOutputStream(this.stream);

            Source redirectSource = null;
            try {
                redirectSource = this.resolve(newURL);
                redirectSource.toSAX(ls);
            } catch (SAXException se) {
                throw new IOException("SAXException: " + se);
            } catch (ProcessingException pe) {
                throw new IOException("ProcessingException: " + pe);
            } finally {
                if (redirectSource != null) redirectSource.recycle();
            }
        } else {
            Source redirectSource = null;
            try {
                redirectSource = this.resolve(newURL);
                InputStream is = redirectSource.getInputStream();
                byte[] buffer = new byte[8192];
                int length = -1;

                while ((length = is.read(buffer)) > -1) {
                    this.stream.write(buffer, 0, length);
                }
            } catch (SAXException se) {
                throw new IOException("SAXException: " + se);
            } catch (ProcessingException pe) {
                throw new IOException("ProcessingException: " + pe);
            } finally {
                if (redirectSource != null) redirectSource.recycle();
            }
        }
    }

    /**
     * Set the ContentType
     */
    public void setContentType(String contentType) {
        this.contentType = contentType;
    }

    /**
     * Set the ContentLength
     */
    public void setContentLength(int contentLength) {
        this.contentLength = contentLength;
    }

    /**
     * Get the ContentType
     */
    public String getContentType() {
        return this.contentType;
    }

    /**
     * Get the OutputStream
     */
    public OutputStream getOutputStream() throws IOException {
        return this.stream;
    }
}


