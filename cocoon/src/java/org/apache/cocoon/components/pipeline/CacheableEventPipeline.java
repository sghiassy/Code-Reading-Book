/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.pipeline;

import org.apache.cocoon.caching.PipelineCacheKey;
import org.apache.cocoon.environment.Environment;

import java.util.Map;

/**
 * This marker interface declares an <code>EventPipeline</code> as cacheable.
 *
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public interface CacheableEventPipeline {

    /**
     * Generate the unique key.
     * This key is the <code>PipelineCacheKey</code> for the whole
     * EventPipeline.
     *
     * @param environment The current environment of the request.
     * @return The generated key or <code>null</code> if the pipeline
     *              is currently not cacheable as a whole.
     */
    PipelineCacheKey generateKey(Environment environment) throws Exception;

    /**
     * Generate the validity object.
     * Before this method can be invoked the generateKey() method
     * must be invoked.
     *
     * @param environment The current environment of the request.
     * @return The generated validity objects for the whole pipeline
     *         or <code>null</code> if the pipeline is currently
     *         not cacheable.
     */
    Map generateValidity(Environment environment) throws Exception;

    /**
     * The stream pipeline (or the content aggregator) calls this
     * method to tell the event pipeline if it must not cache the result
     */
    void setStreamPipelineCaches(boolean flag);
}
