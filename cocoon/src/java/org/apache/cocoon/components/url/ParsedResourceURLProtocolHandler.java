/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.url;

import org.apache.batik.util.AbstractParsedURLProtocolHandler;
import org.apache.batik.util.ParsedURL;
import org.apache.batik.util.ParsedURLData;

/**
 * Provide an extension to Batik to handle the "resource:" protocol.  This class
 * uses the <code>Thread.getContextClassLoader()</code> classloader to get resources.
 * It is safe to use this URL with multiple Cocoon webapps running.
 *
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @version $Id: ParsedResourceURLProtocolHandler.java,v 1.1 2002/01/03 12:31:15 giacomo Exp $
 */
public class ParsedResourceURLProtocolHandler extends AbstractParsedURLProtocolHandler {

    /**
     * Create a new instance, this doesn't do much beyond register the type of
     * protocol we handle.
     */
    public ParsedResourceURLProtocolHandler() {
        super("resource");
    }

    /**
     * Getbase.getPath() the ParsedURLData for the context.  Absolute URIs are specified like
     * "resource://".
     */
    public ParsedURLData parseURL(String uri) {
        ParsedURLData urldata = null;
        String path = uri.substring("resource:/".length());
        urldata = new ParsedURLData(Thread.currentThread().getContextClassLoader().getResource(path));

        if ("file".equals(urldata.protocol)) {
            urldata.host = null;
            urldata.port = -1;
        } else if (null == urldata.host) {
            urldata.port = -1;
        } else if (urldata.port < 0) {
            urldata.host = null;
        }

        return urldata;
    }

    /**
     * The build the relative URL.  Relative URIs are specified like "resource:".
     */
    public ParsedURLData parseURL(ParsedURL base, String uri) {
        StringBuffer newURI = new StringBuffer("resource://");
        newURI.append(base.getPath());

        if ( !newURI.toString().endsWith("/") ) {
            newURI.append("/");
        }

        newURI.append(uri.substring("resource:".length()));

        return this.parseURL(newURI.toString());
    }
}
