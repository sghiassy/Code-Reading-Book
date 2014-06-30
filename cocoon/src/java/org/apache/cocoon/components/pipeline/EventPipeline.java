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
import org.apache.cocoon.generation.Generator;
import org.apache.cocoon.xml.XMLProducer;

/**
 * A pipeline that produces SAX events using a <code>Generator</code> and
 * zero or more <code>Transformer</code>s.
 * <p>
 * An <code>EventPipeline</code> is <code>Recomposable</code> since the
 * <code>ComponentManager</code> used to get the generator and transformers
 * depends on the pipeline assembly engine where they are defined (i.e. a given
 * sitemap file).
 * 
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public interface EventPipeline extends XMLProducer, Component, Recomposable, Recyclable {

    String ROLE = "org.apache.cocoon.components.pipeline.EventPipeline";

    /**
     * Process the given <code>Environment</code>, producing the output.
     */
    boolean process(Environment environment) throws Exception;
    
    /**
     * Set the generator that will be used as the initial step in the pipeline.
     * The generator role is given : the actual <code>Generator</code> is fetched
     * from the latest <code>ComponentManager</code> given by <code>compose()</code>
     * or <code>recompose()</code>.
     *
     * @param role the generator role in the component manager.
     * @param source the source where to produce XML from, or <code>null</code> if no
     *        source is given.
     * @param param the parameters for the generator.
     * @throws Exception if the generator couldn't be obtained.
     */
    void setGenerator (String role, String source, Parameters param) throws Exception;
    
    /**
     * Same as {@link #setGenerator(String, String, Parameters)} with an additional
     * <code>Exception</code> parameter. This is used for setting the special
     * {link org.apache.cocoon.sitemap.ErrorNotifier} generator used to generate
     * input for error pages.
     *
     * @param role the generator role in the component manager.
     * @param source the source where to produce XML from (e.g. xml file), or
     *        <code>null</code> if no source is given.
     * @param param the parameters for the generator.
     * @param e the exception to generate.
     * @throws Exception if the generator couldn't be obtained.
     * @see org.apache.cocoon.sitemap.ErrorNotifier
     */
    void setGenerator (String role, String source, Parameters param, Exception e) throws Exception;
    
    /**
     * Get the generator used by this pipeline. Access to the generator is needed for
     * content aggregation since the aggregation generator is fed by the pipeline engine
     * with the different parts.
     *
     * @return the generator
     */
    Generator getGenerator ();
    
    /**
     * Add a transformer at the end of the pipeline.
     * The transformer role is given : the actual <code>Transformer</code> is fetched
     * from the latest <code>ComponentManager</code> given by <code>compose()</code>
     * or <code>recompose()</code>.
     *
     * @param role the transformer role in the component manager.
     * @param source the source used to setup the transformer (e.g. XSL file), or
     *        <code>null</code> if no source is given.
     * @param param the parameters for the transfomer.
     * @throws Exception if the generator couldn't be obtained.
     */
    void addTransformer (String role, String source, Parameters param) throws Exception;
}
