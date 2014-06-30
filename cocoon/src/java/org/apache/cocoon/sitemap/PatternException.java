/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import org.apache.avalon.framework.CascadingException;

/**
 * This exception is thrown by a <code>URIMatcher</code> or by a
 * <code>URITranslator</code> when there's something wrong with the matching or
 * translation patterns.
 *
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:20 $
 */
public class PatternException extends CascadingException {

    /**
     * Construct a new <code>PatternException</code> instance.
     */
    public PatternException(String message) {
        super(message, null);
    }

    /**
     * Creates a new <code>PatternException</code> instance.
     *
     * @param ex an <code>Exception</code> value
     */
    public PatternException(Exception ex) {
        super(ex.getMessage(), ex);
    }
 
    /**
     * Construct a new <code>PatternException</code> that references
     * a parent Exception.
     */
    public PatternException(String message, Throwable t) {
        super(message, t);
    }
}
