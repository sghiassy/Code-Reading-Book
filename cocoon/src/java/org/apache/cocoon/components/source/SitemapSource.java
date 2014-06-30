/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.source;

import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.Processor;
import org.apache.cocoon.caching.PipelineCacheKey;
import org.apache.cocoon.components.pipeline.CacheableEventPipeline;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.ModifiableSource;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.wrapper.EnvironmentWrapper;
import org.apache.cocoon.serialization.Serializer;
import org.apache.cocoon.util.HashUtil;
import org.apache.cocoon.xml.AbstractXMLConsumer;
import org.apache.cocoon.xml.ContentHandlerWrapper;
import org.apache.cocoon.xml.XMLConsumer;
import org.apache.cocoon.xml.XMLProducer;
import org.apache.log.Logger;
import org.xml.sax.ContentHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.ext.LexicalHandler;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Map;

/**
 * Description of a source which is defined by a pipeline.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public final class SitemapSource
extends AbstractXMLConsumer
implements ModifiableSource {

    /** The last modification date or 0 */
    private long lastModificationDate;

    /** The system id */
    private String systemId;

    /** The uri */
    private String uri;

    /** The current ComponentManager */
    private ComponentManager manager;

    /** The processor */
    private Processor processor;

    /** The environment */
    private EnvironmentWrapper environment;

    /** The prefix for the processing */
    private String prefix;

    /** The <code>EventPipeline</code> */
    private EventPipeline eventPipeline;

    /** The <code>StreamPipeline</code> */
    private StreamPipeline pipeline;

    /** The redirect <code>Source</code> */
    private Source redirectSource;

    /** The <code>SAXException</code> if unable to get resource */
    private ProcessingException exception;

    /** Do I need a refresh ? */
    private boolean needsRefresh;

    /**
     * Construct a new object
     */
    public SitemapSource(Environment      env,
                         ComponentManager manager,
                         Processor        sitemap,
                         String           uri,
                         Logger           logger)
    throws IOException, ProcessingException {

        this.manager = manager;
        this.setLogger(logger);
        // remove the protocol
        int protocolEnd = uri.indexOf(':');
        if (protocolEnd != -1) {
            uri = uri.substring(protocolEnd + 1);
        }

        // does the uri point to this sitemap or to the root sitemap?
        if (uri.startsWith("//")) {
            uri = uri.substring(2);
            Processor processor = null;
            try {
                processor = (Processor)this.manager.lookup(Processor.ROLE);
            } catch (ComponentException e) {
                throw new ProcessingException("Cannot get Processor instance", e);
            }
            this.prefix = ""; // start at the root
            this.processor = processor;
        } else if (uri.startsWith("/")) {
            this.prefix = null;
            uri = uri.substring(1);
            this.processor = sitemap;
        }

        // build the request uri which is relative to the context
        // create the queryString (if available)
        String requestURI = (this.prefix == null ? env.getURIPrefix() + uri : uri);
        String queryString = null;
        int queryStringPos = requestURI.indexOf('?');
        if (queryStringPos != -1) {
            queryString = requestURI.substring(queryStringPos+1);
            requestURI = requestURI.substring(0, queryStringPos);
        }
        Request request= (Request)env.getObjectModel().get(Constants.REQUEST_OBJECT);
        boolean isDefaultPort = "http".equalsIgnoreCase(request.getScheme()) && 80 == request.getServerPort();
        StringBuffer sysID = new StringBuffer(request.getScheme()).append("://")
                        .append(request.getServerName());
        
        if ( ! isDefaultPort ) {
            sysID.append(':').append(request.getServerPort());
        }
        sysID.append(request.getContextPath()).append('/').append(requestURI);
        queryStringPos = uri.indexOf('?');
        if (queryStringPos != -1) {
            uri = uri.substring(0, queryStringPos);
            sysID.append('?').append(queryString);
        }
        this.systemId = sysID.toString();
        this.environment = new EnvironmentWrapper(env, requestURI, queryString, logger);
        this.uri = uri;
        this.refresh();
    }

    /**
     * Get the last modification date of the source or 0 if it
     * is not possible to determine the date.
     */
    public long getLastModified() {
        if (this.needsRefresh) {
            this.refresh();
        }
        return this.lastModificationDate;
    }

    /**
     * Get the content length of the source or -1 if it
     * is not possible to determine the length.
     */
    public long getContentLength() {
        return -1;
    }

    /**
     * Return an <code>InputStream</code> object to read from the source.
     */
    public InputStream getInputStream()
    throws ProcessingException, IOException {
        if (this.needsRefresh) {
            this.refresh();
        }
        // VG: Why exception is not thrown in constructor?
        if (this.exception != null) {
            throw this.exception;
        }
        ComponentSelector serializerSelector = null;
        Serializer serializer = null;
        try {

            serializerSelector = (ComponentSelector) this.manager.lookup(Serializer.ROLE + "Selector");
            serializer = (Serializer)serializerSelector.select("xml");
            ByteArrayOutputStream os = new ByteArrayOutputStream();
            serializer.setOutputStream(os);

            this.toSAX(serializer);

            return new ByteArrayInputStream(os.toByteArray());
        } catch (ComponentException cme) {
            throw new ProcessingException("could not lookup pipeline components", cme);
        } catch (ProcessingException e) {
            throw e;
        } catch (Exception e) {
            throw new ProcessingException("Exception during processing of " + this.systemId, e);
        } finally {
            if (serializer != null) serializerSelector.release(serializer);
            if (serializerSelector != null) this.manager.release(serializerSelector);
            reset();
        }
    }

    /**
     * Return the unique identifer for this source
     */
    public String getSystemId() {
        return this.systemId;
    }

    /**
     * Refresh this object and update the last modified date
     * and content length.
     */
    public void refresh() {
        reset();
        try {
            // initialize the pipelines
            this.eventPipeline = (EventPipeline)this.manager.lookup(EventPipeline.ROLE);
            this.pipeline = (StreamPipeline)this.manager.lookup(StreamPipeline.ROLE);

            this.pipeline.setEventPipeline(eventPipeline);
            // set dummy consumer
            ((XMLProducer)eventPipeline).setConsumer(this);

            this.environment.setURI(this.prefix, this.uri);
            this.processor.process(this.environment, pipeline, eventPipeline);
            this.environment.changeToLastContext();
                String redirectURL = this.environment.getRedirectURL();
                if (redirectURL == null) {
                    if (this.eventPipeline instanceof CacheableEventPipeline) {
                        CacheableEventPipeline cep = (CacheableEventPipeline)this.eventPipeline;
                        PipelineCacheKey pck = cep.generateKey(this.environment);
                        Map validity = null;
                        if (pck != null) {
                            validity = cep.generateValidity(this.environment);
                            if (validity != null) {
                                // the event pipeline is cacheable
                                // now calculate a last modification date
                                String hashKey = pck.toString() + validity.toString();
                                this.lastModificationDate = HashUtil.hash(hashKey);
                            }
                        }
                    }
                } else {
                    if (redirectURL.indexOf(":") == -1) {
                        redirectURL = "cocoon:/" + redirectURL;
                    }
                    this.redirectSource = this.environment.resolve(redirectURL);
                    this.lastModificationDate = this.redirectSource.getLastModified();
                }
        } catch (ProcessingException e) {
            reset();
            this.exception = e;
        } catch (Exception e) {
            reset();
            this.exception = new ProcessingException("Could not get sitemap source "
                                                     + this.systemId, e);
        }
        this.needsRefresh = false;
    }

    /**
     * Return a new <code>InputSource</code> object
     */
    public InputSource getInputSource()
    throws ProcessingException, IOException {
        InputSource newObject = new InputSource(this.getInputStream());
        newObject.setSystemId(this.systemId);
        return newObject;
    }

    /**
     * Stream content to the content handler
     */
    public void toSAX(ContentHandler contentHandler)
        throws SAXException, ProcessingException
    {
        if (this.needsRefresh) {
            this.refresh();
        }
        if (this.exception != null) {
            throw this.exception;
        }
        try {
            XMLConsumer consumer;
            if (contentHandler instanceof XMLConsumer) {
                consumer = (XMLConsumer)contentHandler;
            } else if (contentHandler instanceof LexicalHandler) {
                consumer = new ContentHandlerWrapper(contentHandler, (LexicalHandler)contentHandler);
            } else {
                consumer = new ContentHandlerWrapper(contentHandler);
            }
            if (this.redirectSource != null) {
                this.redirectSource.toSAX(consumer);
            } else {
                ((XMLProducer)eventPipeline).setConsumer(consumer);
                eventPipeline.process(this.environment);
            }
        } catch (ComponentException cme) {
            throw new ProcessingException("Could not lookup pipeline components", cme);
        } catch (ProcessingException e) {
            // Preserve original exception
            throw e;
        } catch (Exception e) {
            throw new ProcessingException("Exception during processing of "
                                          + this.systemId, e);
        } finally {
            reset();
        }
    }

    private void reset() {
        if (this.eventPipeline != null) this.manager.release(this.eventPipeline);
        if (this.pipeline != null) this.manager.release(this.pipeline);
        this.eventPipeline = null;
        this.pipeline = null;
        this.lastModificationDate = 0;
        this.environment.reset();
        if (this.redirectSource != null) this.redirectSource.recycle();
        this.redirectSource = null;
        this.exception = null;
        this.needsRefresh = true;
    }

    public void recycle() {
        reset();
    }
}
