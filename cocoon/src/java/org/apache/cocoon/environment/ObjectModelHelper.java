/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment;

import java.util.Map;

/**
 * A set of constants and methods to access the content of the object model.
 * <p>
 * The object model is a <code>Map</code> used to pass information about the
 * calling environment to the sitemap and its components (matchers, actions,
 * transformers, etc).
 * <p>
 * This class provides accessors only for the objects in the object model that are
 * common to every environment and which can thus be used safely. Some environments
 * provide additional objects, but they are not described here and accessing them
 * should be done in due cause since this ties the application to that particular
 * environment.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:15 $
 */

public final class ObjectModelHelper {
    
    // FIXME : Constants copied from cocoon.Constants where they should be deprecated
    // LINK_OBJECT should also be moved to CommandLineEnvironment
    
    /** Key for the environment {@link Request} in the object model. */
    public final static String REQUEST_OBJECT  = "request";
    
    /** Key for the environment {@link Response} in the object model. */
    public final static String RESPONSE_OBJECT = "response";
    
    /** Key for the environment {@link Context} in the object model. */
    public final static String CONTEXT_OBJECT  = "context";
    
    private ObjectModelHelper() {
        // Forbid instantiation
    }
    
    public static final Request getRequest(Map objectModel) {
        return (Request)objectModel.get(REQUEST_OBJECT);
    }
    
    public static final Response getResponse(Map objectModel) {
        return (Response)objectModel.get(RESPONSE_OBJECT);
    }
    
    public static final Context getContext(Map objectModel) {
        return (Context)objectModel.get(CONTEXT_OBJECT);
    }
}
