/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.pipeline;

import org.apache.avalon.excalibur.pool.Recyclable;

/**
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public class NonCachingEventPipeline extends AbstractEventPipeline implements Recyclable {

    public void recycle() {
        getLogger().debug("Recycling of NonCachingEventPipeline");
        super.recycle();
    }
}
