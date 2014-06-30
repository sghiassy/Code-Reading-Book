/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

/**
 * A CacheValidity object contains all information for one pipeline component
 * to check if it is still valid.<br>
 * For example, the FileGenerator stores only the timestamp for the read
 * xml file in this container.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:08 $
 */
public interface CacheValidity extends java.io.Serializable {

    /**
     * Check if the component is still valid.
     * This is only true, if the incoming CacheValidity is of the same
     * type and has the same values.
     */
    boolean isValid(CacheValidity validity);
}
