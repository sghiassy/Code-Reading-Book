/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.environment.Environment;

/**
 * This class is a wrapper around the real processor (the <code>Cocoon</code> class).
 * It is necessary to avoid infinite dispose loops
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:06 $
 */
public final class ProcessorWrapper
implements Processor, Component, Disposable, ThreadSafe {

    private Processor processor;

    public void dispose() {
        this.processor = null;
    }

    public ProcessorWrapper(Processor processor) {
        this.processor = processor;
    }

    /**
     * Process the given <code>Environment</code> producing the output
     */
    public boolean process(Environment environment)
    throws Exception {
        return this.processor.process(environment);
    }

    /**
     * Process the given <code>Environment</code> to assemble
     * a <code>StreamPipeline</code> and an <code>EventPipeline</code>.
     */
    public boolean process(Environment environment,
                           StreamPipeline pipeline,
                           EventPipeline eventPipeline)
    throws Exception {
        return this.processor.process(environment, pipeline, eventPipeline);
    }
}
