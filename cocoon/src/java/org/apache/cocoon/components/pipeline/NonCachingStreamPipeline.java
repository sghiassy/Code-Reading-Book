/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.pipeline;

import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Environment;

import java.io.IOException;

/** A <CODE>ResourcePipeline</CODE> either
 * <UL>
 *  <LI>collects a <CODE>Reader</CODE> and let it process</LI>
 *  <LI>or connects a <CODE>EventPipeline</CODE> with a
 *  <CODE>Serializer</CODE> and let them produce the requested
 * resource
 * </UL>
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public final class NonCachingStreamPipeline extends AbstractStreamPipeline {

    /** Setup pipeline components.
     */
    protected void setupPipeline(Environment environment)
    throws ProcessingException {
        try {
            this.serializer.setOutputStream(environment.getOutputStream());
        } catch (IOException e) {
            throw new ProcessingException(
                "Could not setup resource pipeline.",
                e
            );
        }
        super.setupPipeline(environment);
    }

    /**
     * Recycle this component
     */
    public void recycle() {
        getLogger().debug("Recycling of NonCachingStreamPipeline");
        super.recycle();
    }
}
