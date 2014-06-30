/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon;

/**
 * This Exception is thrown every time there is a problem in finding
 * a resource.
 *
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:06 $
 */
public class ResourceNotFoundException extends ProcessingException {

    /**
     * Construct a new <code>ResourceNotFoundException</code> instance.
     */
    public ResourceNotFoundException(String message) {
        super(message, null);
    }

    /**
     * Construct a new <code>ResourceNotFoundException</code> that references
     * a parent Exception.
     */
    public ResourceNotFoundException(String message, Throwable t) {
        super(message, t);
    }
}
