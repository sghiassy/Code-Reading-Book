/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.pipeline;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.caching.*;
import org.apache.cocoon.components.sax.XMLDeserializer;
import org.apache.cocoon.components.sax.XMLSerializer;
import org.apache.cocoon.components.sax.XMLTeePipe;
import org.apache.cocoon.components.saxconnector.SAXConnector;
import org.apache.cocoon.components.store.Store;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.transformation.Transformer;
import org.apache.cocoon.xml.XMLConsumer;
import org.apache.cocoon.xml.XMLProducer;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * The CachingEventPipeline
 *
 * If all components of this event pipeline are cacheable then the whole
 * pipeline is also cacheable. If in this case the CacheableEventPipeline interface
 * is invoked (e.g. by the CachingStreamPipeline) the CachingEventPipeline
 * does not cache! (If it would cache, the response would be cached twice!)
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/22 00:17:11 $
 */
public class CachingEventPipeline
extends AbstractEventPipeline
implements Disposable, CacheableEventPipeline, Recyclable {

    /** The store for the cached sax events */
    private Store eventCache;

    private XMLProducer producer;
    private String generatorRole;
    private ArrayList transformerRoles = new ArrayList();
    private ArrayList notCacheableTransformers = new ArrayList();
    private Map validityObjects;
    private PipelineCacheKey pipelineCacheKey;
    private boolean setupFinished = false;
    /** Indicates if the whole pipeline is cacheable */
    private boolean cacheable = false;
    private int firstNotCacheableTransformerIndex;

    public void compose (ComponentManager manager)
    throws ComponentException {
        super.compose(manager);
        this.eventCache = (Store)this.manager.lookup(Store.ROLE);
    }

    /**
     * Set the generator.
     */
    public void setGenerator (String role, String source, Parameters param)
    throws Exception {
        super.setGenerator(role, source, param);
        this.generatorRole = role;
    }

    /**
     * Add a transformer.
     */
    public void addTransformer (String role, String source, Parameters param)
    throws Exception {
        super.addTransformer(role, source, param);
        this.transformerRoles.add(role);
    }

    /**
     * Generate the unique key.
     * This key is the <code>PipelineCacheKey</code> for the whole
     * EventPipeline.
     *
     * @param environment The current environment of the request.
     * @return The generated key or <code>null</code> if the pipeline
     *              is currently not cacheable as a whole.
     */
    public PipelineCacheKey generateKey(Environment environment)
    throws Exception {
        this.setup(environment);
        return (this.cacheable ? this.pipelineCacheKey : null);
    }

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
    public Map generateValidity(Environment environment)
    throws Exception {
        if (this.cacheable) {
            return this.validityObjects;
        }
        return null;
    }

    /**
     * The stream pipeline (or the content aggregator) calls this
     * method to tell the event pipeline if it must not cache the result
     */
    public void setStreamPipelineCaches(boolean flag) {
        // if we are cacheable and this method is called
        // reset the pipeline cache key to avoid duplicate caching
        if (flag) {
            this.pipelineCacheKey = null;
        }
    }

    public boolean process(Environment environment) throws Exception {
        this.setup(environment);

        // we cache if the pipelinecachekey is available
        XMLSerializer xmlSerializer = null;

        try {
            if (this.pipelineCacheKey != null) {
                // now we have the key to get the cached object
                CachedEventObject cachedObject = (CachedEventObject)this.eventCache.get(this.pipelineCacheKey);

                if (cachedObject != null) {
                    getLogger().debug("Found cached content for '" + environment.getURI() + "'.");
                    Iterator validityIterator = validityObjects.keySet().iterator();
                    ComponentCacheKey validityKey;
                    boolean valid = true;
                    while (validityIterator.hasNext() && valid) {
                        validityKey = (ComponentCacheKey)validityIterator.next();
                        valid = cachedObject.isValid(validityKey, (CacheValidity)validityObjects.get(validityKey));
                        if (getLogger().isDebugEnabled()) {
                            CacheValidity cachedValidity = cachedObject.getCacheValidity(validityKey);
                            getLogger().debug("Compared cached validity '" + cachedValidity +
                                "' with new validity '" + validityObjects.get(validityKey) +
                                "' : " + (valid ? "valid" : "changed"));
                        }
                    }
                    if (valid) {
                        getLogger().debug("Using valid cached content for '" + environment.getURI() + "'.");
                        // get all transformers which are not cacheable
                        int transformerSize = this.transformers.size();
                        while (this.firstNotCacheableTransformerIndex < transformerSize) {
                            this.notCacheableTransformers.add(this.transformers.get(this.firstNotCacheableTransformerIndex));
                            this.firstNotCacheableTransformerIndex++;
                        }

                        XMLDeserializer deserializer = null;
                        try {
                            deserializer = (XMLDeserializer)this.manager.lookup(XMLDeserializer.ROLE);
                            // connect the pipeline:
                            this.producer = deserializer;
                            this.connectPipeline(environment,
                                                 notCacheableTransformers,
                                                 null);
                            // execute the pipeline:
                            deserializer.deserialize(cachedObject.getSAXFragment());
                        } catch ( ProcessingException e ) {
                            throw e;
                        } catch ( Exception e ) {
                            throw new ProcessingException(
                                 "Failed to execute pipeline.",
                                  e
                            );
                        } finally {
                            if (deserializer != null)
                                this.manager.release((Component)deserializer);
                        }
                    } else {
                        getLogger().debug("Cached content is invalid for '" + environment.getURI() + "'.");
                        // remove invalid cached object
                        this.eventCache.remove(this.pipelineCacheKey);
                        cachedObject = null;
                    }
                }
                if (cachedObject == null) {
                    getLogger().debug("Caching content for further requests of '" + environment.getURI() + "'.");
                    xmlSerializer = (XMLSerializer)this.manager.lookup(XMLSerializer.ROLE);
                }
            }

            if (this.producer == null) {
                // the content was not cached/or is invalid
                this.producer = this.generator;
                this.connectPipeline(environment,
                                     this.transformers,
                                     xmlSerializer);
                // execute the pipeline:
                this.generator.generate();
                // did we have cacheable components?
                if (xmlSerializer != null) {
                    this.eventCache.store(this.pipelineCacheKey,
                        new CachedEventObject(this.validityObjects,
                        xmlSerializer.getSAXFragment()));
                }
            }
        } finally {
            if (xmlSerializer != null)
                this.manager.release((Component)xmlSerializer);
        }
        return true;
    }

    /**
     * Setup the evenet pipeline.
     * The components of the pipeline are checked if they are
     * Cacheable.
     */
    private void setup(Environment environment) throws Exception {
        if (this.setupFinished) {
            return;
        }
        if (!this.checkPipeline()) {
            throw new ProcessingException("Attempted to process incomplete pipeline.");
        }

        // set up all sitemap components
        this.setupPipeline(environment);

        this.firstNotCacheableTransformerIndex = 0;

        // is the generator cacheable?
        if (this.generator instanceof Cacheable) {

            long key = ((Cacheable)this.generator).generateKey();
            CacheValidity validity = ((Cacheable)this.generator).generateValidity();

            // final check, the current generator might not be cacheable
            if (key != 0 && validity != null) {
                ComponentCacheKey cck = new ComponentCacheKey(
                           ComponentCacheKey.ComponentType_Generator,
                           this.generatorRole,
                           key);
                this.validityObjects = new HashMap();
                this.validityObjects.put(cck, validity);
                this.pipelineCacheKey = new PipelineCacheKey();
                this.pipelineCacheKey.addKey(cck);

                // now testing transformers
                Transformer trans;
                ComponentCacheKey transCacheKey;
                int transformerSize = this.transformers.size();
                long transKey;
                CacheValidity transValidity;
                boolean testTrans = true;

                while (this.firstNotCacheableTransformerIndex < transformerSize
                           && testTrans) {
                    trans = (Transformer)this.transformers.get(this.firstNotCacheableTransformerIndex);
                    if (trans instanceof Cacheable) {
                        transKey = ((Cacheable)trans).generateKey();
                        transValidity = ((Cacheable)trans).generateValidity();
                        if (transKey != 0 && transValidity != null) {
                            transCacheKey = new ComponentCacheKey(
                                 ComponentCacheKey.ComponentType_Transformer,
                                 (String)this.transformerRoles.get(this.firstNotCacheableTransformerIndex),
                                 transKey);
                            this.pipelineCacheKey.addKey(transCacheKey);
                            this.validityObjects.put(transCacheKey, transValidity);
                        } else {
                            testTrans = false;
                        }
                    } else {
                        testTrans = false;
                    }
                    if (testTrans)
                        this.firstNotCacheableTransformerIndex++;
                }
                // all transformers are cacheable => pipeline is cacheable
                if (this.firstNotCacheableTransformerIndex == transformerSize)
                    this.cacheable = true;
            }
        }
        this.setupFinished = true;
    }

    /** Connect the pipeline.
     */
    private void connectPipeline(Environment   environment,
                                 ArrayList     usedTransformers,
                                 XMLSerializer xmlSerializer)
    throws ProcessingException {
        XMLProducer prev = this.producer;
        XMLConsumer next;

        boolean configuredSAXConnector = this.manager.hasComponent(SAXConnector.ROLE);

        try {
            int cacheableTransformerCount = this.firstNotCacheableTransformerIndex;

            Iterator itt = usedTransformers.iterator();
            while ( itt.hasNext() ) {
                if (configuredSAXConnector) {
                    // connect SAXConnector
                    SAXConnector connect = (SAXConnector) this.manager.lookup(SAXConnector.ROLE);
                    connect.setup(environment,environment.getObjectModel(),null,null);
                    this.connectors.add(connect);
                    next = connect;
                    prev.setConsumer(next);
                    prev = connect;
                }

                // Connect next component.
                next = (XMLConsumer) itt.next();
                if (xmlSerializer != null) {
                    if (cacheableTransformerCount == 0) {
                        next = new XMLTeePipe(next, xmlSerializer);
                        xmlSerializer = null;
                    } else {
                        cacheableTransformerCount--;
                    }
                }
                prev.setConsumer(next);
                prev = (XMLProducer) next;
            }

            if (configuredSAXConnector) {
                // insert SAXConnector
                SAXConnector connect = (SAXConnector) this.manager.lookup(SAXConnector.ROLE);
                connect.setup(environment,environment.getObjectModel(),null,null);
                this.connectors.add(connect);
                next = connect;
                prev.setConsumer(next);
                prev = connect;
            }

            // insert this consumer
            next = super.xmlConsumer;
            if (xmlSerializer != null) {
                next = new XMLTeePipe(next, xmlSerializer);
                xmlSerializer = null;
            }
            prev.setConsumer(next);

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
        getLogger().debug("Recycling of CachingEventPipeline");

        super.recycle();

        this.producer = null;
        this.generatorRole = null;
        this.transformerRoles.clear();
        this.notCacheableTransformers.clear();
        this.validityObjects = null;
        this.pipelineCacheKey = null;
        this.setupFinished = false;
        this.cacheable = false;
    }

    public void dispose() {
        this.manager.release((Component)this.eventCache);
        super.dispose();
    }
}
