/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment;

import org.apache.cocoon.components.source.SourceHandler;

import java.io.IOException;
import java.io.OutputStream;
import java.net.URL;
import java.util.Enumeration;
import java.util.Map;

/**
 * Base interface for an environment abstraction
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 07:42:02 $
 */

public interface Environment extends SourceResolver {

    /**
     * Get the <code>SourceHandler</code> for the current request
     */
    SourceHandler getSourceHandler();

    /**
     * Set the <code>SourceHandler</code> for the current request
     */
    void setSourceHandler(SourceHandler sourceHandler);

    /**
     * Get the URI to process. The prefix is stripped off.
     */
    String getURI();

    /**
     * Get the prefix of the URI in progress.
     */
    String getURIPrefix();

    /**
     * Get the Root Context
     */
    URL getRootContext();

    /**
     * Get current context
     */
    URL getContext();

    /**
     * Get the view to process
     */
    String getView();

    /**
     * Get the action to process
     */
    String getAction();

    /**
     * Set the context. This is similar to changeContext()
     * except that it is absolute.
     */
    void setContext(String prefix, String uri);

    /**
     * Change the context from uriprefix to context
     */
    void changeContext(String uriprefix, String context) throws Exception;

    /**
     * Redirect to the given URL
     */
    void redirect(boolean sessionmode, String url) throws IOException;

    /**
     * Set the content type of the generated resource
     */
    void setContentType(String mimeType);

    /**
     * Get the content type of the resource
     */
    String getContentType();

    /**
     * Set the length of the generated content
     */
    void setContentLength(int length);

    /**
     * Set the response status code
     */
    void setStatus(int statusCode);

    /**
     * Get the output stream where to write the generated resource.
     */
    OutputStream getOutputStream() throws IOException;

    /**
     * Get the underlying object model
     */
    Map getObjectModel();

    /**
     * Check if the response has been modified since the same
     * "resource" was requested.
     * The caller has to test if it is really the same "resource"
     * which is requested.
     * @result true if the response is modified or if the
     *         environment is not able to test it
     */
    boolean isResponseModified(long lastModified);

    /**
     * Mark the response as not modified.
     */
    void setResponseIsNotModified();

    /**
     * Binds an object to this environment, using the name specified. This allows
     * the pipeline assembly engine to store for its own use objects that souldn't
     * be exposed to other components (generators, selectors, etc) and therefore
     * cannot be put in the object model.
     * <p>
     * If an object of the same name is already bound, the object is replaced.
     *
     * @param name  the name to which the object is bound
     * @param value the object to be bound
     */
    void setAttribute(String name, Object value);

    /**
     * Returns the object bound with the specified name, or <code>null</code>
     * if no object is bound under the name.
     *
     * @param name                a string specifying the name of the object
     * @return                        the object with the specified name
     */
    Object getAttribute(String name);

    /**
     * Removes the object bound with the specified name from
     * this environment. If the environment does not have an object
     * bound with the specified name, this method does nothing.
     *
     * @param name the name of the object to remove
     */
    void removeAttribute(String name);

    /**
     * Returns an <code>Enumeration</code> of <code>String</code> objects
     * containing the names of all the objects bound to this environment.
     *
     * @return an <code>Enumeration</code> of <code>String</code>s.
     */
    Enumeration getAttributeNames();

    /**
     * Reset the response if possible. This allows error handlers to have
     * a higher chance to produce clean output if the pipeline that raised
     * the error has already output some data.
     *
     * @return true if the response was successfully reset
    */
    boolean tryResetResponse();

}

