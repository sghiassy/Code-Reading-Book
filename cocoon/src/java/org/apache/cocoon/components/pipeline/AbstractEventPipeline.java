/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.pipeline;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.saxconnector.SAXConnector;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.generation.Generator;
import org.apache.cocoon.transformation.Transformer;
import org.apache.cocoon.xml.AbstractXMLProducer;
import org.apache.cocoon.xml.XMLConsumer;
import org.apache.cocoon.xml.XMLProducer;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;

/**
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @author <a href="mailto:cziegeler@Carsten Ziegeler">Carsten Ziegeler</a>
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @author <a href="mailto:proyal@managingpartners.com">Peter Royal</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public abstract class AbstractEventPipeline
  extends AbstractXMLProducer
  implements EventPipeline, Disposable {

    // Generator stuff
    protected Generator generator;
    protected Parameters generatorParam;
    protected String generatorSource;
    protected ComponentSelector generatorSelector;
    
    // Transformer stuff
    protected ArrayList transformers = new ArrayList();
    protected ArrayList transformerParams = new ArrayList();
    protected ArrayList transformerSources = new ArrayList();
    protected ArrayList transformerSelectors = new ArrayList();
    
    protected ArrayList connectors = new ArrayList();

    /** the component manager set with compose() */
    protected ComponentManager manager;
    
    /** the component manager set with compose() and recompose() */
    protected ComponentManager newManager;

    public void compose (ComponentManager manager)
    throws ComponentException {
        this.manager = manager;
        this.newManager = manager;
    }

    public void recompose (ComponentManager manager)
    throws ComponentException {
        this.newManager = manager;
    }

    /**
     * @deprecated The NotifyingGenerator gets the Notificable from the objectModel
     */
    public void setGenerator (String role, String source, Parameters param, Exception e)
    throws Exception {
        this.setGenerator (role, source, param);
        if (this.generator instanceof org.apache.cocoon.sitemap.ErrorNotifier) {
            ((org.apache.cocoon.sitemap.ErrorNotifier)this.generator).setThrowable(e);
        }
    }

    public void setGenerator (String role, String source, Parameters param)
    throws Exception {
        if (this.generator != null) {
            throw new ProcessingException ("Generator already set. You can only select one Generator (" + role + ")");
        }
        this.generatorSelector = (ComponentSelector) this.newManager.lookup(Generator.ROLE + "Selector");     
        this.generator = (Generator) this.generatorSelector.select(role);
        this.generatorSource = source;
        this.generatorParam = param;
    }

    public Generator getGenerator() {
        return this.generator;
    }

    public void addTransformer (String role, String source, Parameters param)
    throws Exception {
        ComponentSelector selector = (ComponentSelector) this.newManager.lookup(Transformer.ROLE + "Selector");
        this.transformerSelectors.add(selector);
        this.transformers.add((Transformer)selector.select(role));
        this.transformerSources.add(source);
        this.transformerParams.add(param);
    }

    public boolean process(Environment environment) throws Exception {
        if ( !checkPipeline() ) {
            throw new ProcessingException("Attempted to process incomplete pipeline.");
        }

        setupPipeline(environment);
        connectPipeline(environment);

        // execute the pipeline:
        try {
            this.generator.generate();
        } catch ( Exception e ) {
            throw e;
        }
        return true;
    }

    /** Sanity check the non-reader pipeline.
     * @return true if the pipeline is 'sane', false otherwise.
     */
    protected boolean checkPipeline() {
        if ( this.generator == null ) {
            return false;
        }

        Iterator itt = this.transformers.iterator();
        while ( itt.hasNext() ) {
            if ( itt.next() == null) {
                return false;
            }
        }

        if (super.xmlConsumer == null &&
            super.contentHandler == null &&
            super.lexicalHandler == null) {
            return false;
        }
        return true;
    }

    /** Setup pipeline components.
     */
    protected void setupPipeline(Environment environment)
    throws ProcessingException {
        try {
            // setup the generator
            this.generator.setup(
                environment,
                environment.getObjectModel(),
                generatorSource,
                generatorParam
            );

            Iterator transformerItt = this.transformers.iterator();
            Iterator transformerSourceItt = this.transformerSources.iterator();
            Iterator transformerParamItt = this.transformerParams.iterator();

            while ( transformerItt.hasNext() ) {
                Transformer trans = (Transformer)transformerItt.next();
                trans.setup(
                    environment,
                    environment.getObjectModel(),
                    (String)transformerSourceItt.next(),
                    (Parameters)transformerParamItt.next()
                );
            }
        } catch (SAXException e) {
            throw new ProcessingException(
                "Could not setup pipeline.",
                e
            );
        } catch (IOException e) {
            throw new ProcessingException(
                "Could not setup pipeline.",
                e
            );
        }


    }

    /** Connect the pipeline.
     */
    protected void connectPipeline(Environment environment) throws ProcessingException {
        XMLProducer prev = (XMLProducer)this.generator;
        XMLConsumer next;
        
        // Looked up in manager and not newManager (it's not a sitemap component)
        boolean configuredSAXConnector = this.manager.hasComponent(SAXConnector.ROLE);

        try {
            Iterator itt = this.transformers.iterator();
            while ( itt.hasNext() ) {
                if (configuredSAXConnector) {
                    // connect SAXConnector
                    SAXConnector connect = (SAXConnector) this.manager.lookup(SAXConnector.ROLE);
                    connect.setup(environment,environment.getObjectModel(), null, null);
                    this.connectors.add(connect);
                    next = connect;
                    prev.setConsumer(next);
                    prev = connect;
                }
                // Connect next component.
                Transformer trans = (Transformer) itt.next();
                next = trans;
                prev.setConsumer(next);
                prev = trans;
            }

            if (configuredSAXConnector) {
                // insert SAXConnector
                SAXConnector connect = (SAXConnector) this.manager.lookup(SAXConnector.ROLE);
                connect.setup(environment,environment.getObjectModel(), null, null);
                this.connectors.add(connect);
                next = connect;
                prev.setConsumer(next);
                prev = connect;
            }
            // insert this consumer
            prev.setConsumer(super.xmlConsumer);
        } catch ( IOException e ) {
            throw new ProcessingException(
                "Could not connect pipeline.",
                e
            );
        } catch ( SAXException e ) {
            throw new ProcessingException(
                "Could not connect pipeline.",
                e
            );
        } catch ( ComponentException e ) {
            throw new ProcessingException(
                "Could not connect pipeline.",
                e
            );
        }

    }

    public void recycle() {
        super.recycle();

        try {
            // Release generator.
            if ( generatorSelector != null ) {
                generatorSelector.release(this.generator);
            }

            // Release transformers
            int size = this.transformerSelectors.size();
            for (int i = 0; i < size; i++)
            {
                ((ComponentSelector)this.transformerSelectors.get(i)).release(
                    (Component)this.transformers.get(i)
                );
            }

            // Release connectors
            Iterator itc = this.connectors.iterator();
            while ( itc.hasNext() ) {
                this.manager.release((Component) itc.next());
            }
        } catch ( Exception e ) {
            getLogger().warn(
                "Failed to release components from event pipeline.",
                e
            );
        } finally {
    
            // FIXME : in theory selectors should be released on their respective managers
            // but this causes no harm since they're ThreadSafe
            this.generatorSelector = null;
            this.generator = null;
            this.transformerSelectors.clear();
            this.transformers.clear();
            this.transformerParams.clear();
            this.transformerSources.clear();
            this.connectors.clear();
        }
    }
    
    public void dispose() {
        // Nothing here
    }
}

