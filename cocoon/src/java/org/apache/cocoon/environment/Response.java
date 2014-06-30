/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.environment;

import java.util.Locale;

/**
 * Defines an interface to provide client response information .
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:15 $
 *
 */

public interface Response {

    /**
     * Returns the name of the charset used for
     * the MIME body sent in this response.
     *
     * <p>If no charset has been assigned, it is implicitly
     * set to <code>ISO-8859-1</code> (<code>Latin-1</code>).
     *
     * <p>See RFC 2047 (http://ds.internic.net/rfc/rfc2045.txt)
     * for more information about character encoding and MIME.
     *
     * @return                a <code>String</code> specifying the
     *                        name of the charset, for
     *                        example, <code>ISO-8859-1</code>
     *
     */

    String getCharacterEncoding();

    /**
     * Sets the locale of the response, setting the headers (including the
     * Content-Type's charset) as appropriate.  By default, the response locale
     * is the default locale for the server.
     *
     * @param loc  the locale of the response
     *
     * @see                 #getLocale
     *
     */

    void setLocale(Locale loc);

    /**
     * Returns the locale assigned to the response.
     *
     *
     * @see                 #setLocale
     *
     */

    Locale getLocale();

    /**
     * Constructs a cookie with a specified name and value.
     *
     * <p>The name must conform to RFC 2109. That means it can contain
     * only ASCII alphanumeric characters and cannot contain commas,
     * semicolons, or white space or begin with a $ character. The cookie's
     * name cannot be changed after creation.
     *
     * <p>The value can be anything the server chooses to send. Its
     * value is probably of interest only to the server. The cookie's
     * value can be changed after creation with the
     * <code>setValue</code> method.
     *
     * <p>By default, cookies are created according to the Netscape
     * cookie specification. The version can be changed with the
     * <code>setVersion</code> method.
     *
     *
     * @param name                         a <code>String</code> specifying the name of the cookie
     *
     * @param value                        a <code>String</code> specifying the value of the cookie
     *
     * @throws IllegalArgumentException        if the cookie name contains illegal characters
     *                                        (for example, a comma, space, or semicolon)
     *                                        or it is one of the tokens reserved for use
     *                                        by the cookie protocol
     *
     */
    Cookie createCookie(String name, String value);

    /**
     * Adds the specified cookie to the response.  This method can be called
     * multiple times to set more than one cookie.
     *
     * @param cookie the Cookie to return to the client
     *
     */

    void addCookie(Cookie cookie);

    /**
     * Returns a boolean indicating whether the named response header
     * has already been set.
     *
     * @param        name        the header name
     * @return                <code>true</code> if the named response header
     *                        has already been set;
     *                         <code>false</code> otherwise
     */

    boolean containsHeader(String name);

    /**
     * Encodes the specified URL by including the session ID in it,
     * or, if encoding is not needed, returns the URL unchanged.
     * The implementation of this method includes the logic to
     * determine whether the session ID needs to be encoded in the URL.
     * For example, if the browser supports cookies, or session
     * tracking is turned off, URL encoding is unnecessary.
     *
     * <p>For robust session tracking, all URLs emitted by a servlet
     * should be run through this
     * method.  Otherwise, URL rewriting cannot be used with browsers
     * which do not support cookies.
     *
     * @param        url        the url to be encoded.
     * @return                the encoded URL if encoding is needed;
     *                         the unchanged URL otherwise.
     */

    String encodeURL(String url);

    /**
     *
     * Sets a response header with the given name and
     * date-value.  The date is specified in terms of
     * milliseconds since the epoch.  If the header had already
     * been set, the new value overwrites the previous one.  The
     * <code>containsHeader</code> method can be used to test for the
     * presence of a header before setting its value.
     *
     * @param        name        the name of the header to set
     * @param        value        the assigned date value
     *
     * @see #containsHeader
     * @see #addDateHeader
     */

    void setDateHeader(String name, long date);

    /**
     *
     * Adds a response header with the given name and
     * date-value.  The date is specified in terms of
     * milliseconds since the epoch.  This method allows response headers
     * to have multiple values.
     *
     * @param        name        the name of the header to set
     * @param        value        the additional date value
     *
     * @see #setDateHeader
     */

    void addDateHeader(String name, long date);

    /**
     *
     * Sets a response header with the given name and value.
     * If the header had already been set, the new value overwrites the
     * previous one.  The <code>containsHeader</code> method can be
     * used to test for the presence of a header before setting its
     * value.
     *
     * @param        name        the name of the header
     * @param        value        the header value
     *
     * @see #containsHeader
     * @see #addHeader
     */

    void setHeader(String name, String value);

    /**
     * Adds a response header with the given name and value.
     * This method allows response headers to have multiple values.
     *
     * @param        name        the name of the header
     * @param        value        the additional header value
     *
     * @see #setHeader
     */

    void addHeader(String name, String value);
}



