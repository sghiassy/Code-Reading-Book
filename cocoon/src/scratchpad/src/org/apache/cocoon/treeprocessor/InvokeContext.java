/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Recomposable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.environment.Environment;

import java.util.*;

/**
 * The invocation context of <code>ProcessingNode</code>s.
 * <p>
 * This class serves two purposes :
 * <ul><li>Avoid explicit enumeration of all needed parameters in
 *         {@link ProcessingNode#invoke(Environment, InvokeContext)},
 *         thus allowing easier addition of new parameters,
 *     <li>Hold pipelines, and provide "just in time" lookup for them.
 * </ul>
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public class InvokeContext implements Recomposable, Disposable {
    
    private List mapStack = new ArrayList();
    
    private StreamPipeline streamPipeline;
    
    private EventPipeline eventPipeline;
    
    private boolean isInternalRequest;
    
    /** The current component manager, as set by the last call to compose() or recompose() */
    private ComponentManager currentManager;
    
    /** The component manager that was used to get the pipelines */
    private ComponentManager pipelinesManager;
    
    /**
     * Create an <code>InvokeContext</code> without existing pipelines. This also means
     * the current request is external.
     */
    public InvokeContext() {
        this.isInternalRequest = false;
    }
    
    /**
     * Create an <code>InvokeContext</code> with existing pipelines. This also means
     * the current request is internal.
     */
    public InvokeContext(StreamPipeline pipeline, EventPipeline eventPipeline) {
        this.isInternalRequest = true;
        this.streamPipeline = pipeline;
        this.eventPipeline = eventPipeline;
    }
    
    public void compose(ComponentManager manager) throws ComponentException {
        this.currentManager = manager;
    }

    public void recompose(ComponentManager manager) throws ComponentException {

        this.currentManager = manager;
        
        // Recompose pipelines, if any.
        if (this.streamPipeline != null) {
            this.streamPipeline.recompose(manager);
            this.eventPipeline.recompose(manager);
        }
    }

    /**
     * Get the <code>EventPipeline</code>. If none already exists, a set of new
     * pipelines is looked up.
     */
    public final EventPipeline getEventPipeline() throws Exception {
        if (this.eventPipeline == null) {
            setupPipelines();
        }
        return this.eventPipeline;
    }
    
    /**
     * Get the <code>StreamPipeline</code>. If none already exists, a set of new
     * pipelines is looked up.
     */
    public final StreamPipeline getStreamPipeline() throws Exception {
        if (this.streamPipeline == null) {
            setupPipelines();
        }
        return this.streamPipeline;
    }
    
    private final void setupPipelines() throws Exception {
        // Keep current manager for proper release
        this.pipelinesManager = this.currentManager;
        
        // Lookup pipelines
        this.eventPipeline  = (EventPipeline)this.pipelinesManager.lookup(EventPipeline.ROLE);
        this.streamPipeline = (StreamPipeline)this.pipelinesManager.lookup(StreamPipeline.ROLE);
        this.streamPipeline.setEventPipeline(this.eventPipeline);
        
        // Immediately recompose them to the current manager : pipelines are created
        // by a parent of the current manager, but need to select component in this one,
        // and not its parent.
        this.eventPipeline.recompose(this.pipelinesManager);
        this.streamPipeline.recompose(this.pipelinesManager);
    }

    /**
     * Is this an internal request ?
     */
    public final boolean isInternalRequest() {
        return this.isInternalRequest;
    }

    /**
     * Get the current Map stack used to resolve expressions.
     */
    public final List getMapStack() {
        return this.mapStack;
    }
    
    /**
     * Push a Map on top of the current Map stack.
     */
    public final void pushMap(Map map) {
        mapStack.add(map);
        // FIXME : dump map contents to logger
    }
    
    /**
     * Pop the topmost element of the current Map stack.
     */
    public final void popMap() {
        mapStack.remove(mapStack.size() - 1);
    }

    /**
     * Release the pipelines, if any, if they were looked up by this context.
     */
    public void dispose() {
        // Release pipelines, if any
        if (!this.isInternalRequest && this.pipelinesManager != null) {
            
            this.pipelinesManager.release(this.eventPipeline);
            this.pipelinesManager.release(this.streamPipeline);
            
            this.pipelinesManager = null;
        }
    }
}
