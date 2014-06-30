/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.pipeline;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.Recomposable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.environment.Environment;

/** A <code>StreamPipeline</code> either
 * <ul>
 *  <li>collects a <code>Reader</code> and let it produce a character stream</li>
 *  <li>or connects an <code>EventPipeline</code> with a
 *  <code>Serializer</code> and let them produce the byte stream</li>
 * </UL>
 * <p>
 * A <code>StreamPipeline</code> is <code>Recomposable</code> since the
 * <code>ComponentManager</code> used to get the reader or serializer
 * depends on the pipeline assembly engine where they are defined (i.e.
 * a given sitemap file).
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public interface StreamPipeline extends Component, Recomposable, Recyclable {

    String ROLE = "org.apache.cocoon.components.pipeline.StreamPipeline";

    /**
     * Process the given <code>Environment</code> producing the output
     */
    boolean process(Environment environment)
    throws Exception;

    /**
     * Set the <code>EventPipeline</code>
     */
    void setEventPipeline (EventPipeline eventPipeline) throws Exception;

    /**
     * Get the <code>EventPipeline</code>
     */
    EventPipeline getEventPipeline ();

    /**
     * Set the reader for this pipeline
     */
    void setReader (String role, String source, Parameters param) throws Exception;

    /**
     * Set the reader for this pipeline
     */
    void setReader (String role, String source, Parameters param, String mimeType) throws Exception;

    /**
     * Set the serializer for this pipeline
     */
    void setSerializer (String role, String source, Parameters param) throws Exception;

    /**
     * Set the serializer for this pipeline
     */
    void setSerializer (String role, String source, Parameters param, String mimeType) throws Exception;
}
