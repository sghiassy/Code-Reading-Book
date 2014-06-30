/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.pipeline;

import org.apache.avalon.framework.component.ComponentSelector;

/**
 * A <code>ComponentSelector</code> for output components used by a {@link StreamPipeline}.
 * This selector is able to associate a MIME type to a hint.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/15 11:10:51 $
 */

public interface OutputComponentSelector extends ComponentSelector {

    /**
     * Get the MIME type for a given hint.
     *
     * @param hint the component hint
     * @return the MIME type for this hint, or <code>null</code>.
     */
    String getMimeTypeForHint(Object hint);
}
