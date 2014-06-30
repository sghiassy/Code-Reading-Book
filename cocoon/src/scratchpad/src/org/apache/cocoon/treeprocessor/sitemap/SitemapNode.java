/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;

import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;

import org.apache.cocoon.sitemap.SitemapRedirector;

import org.apache.cocoon.treeprocessor.AbstractParentProcessingNode;
import org.apache.cocoon.treeprocessor.EnvironmentSourceResolver;
import org.apache.cocoon.treeprocessor.ProcessingNode;
import org.apache.cocoon.treeprocessor.InvokeContext;

import java.util.*;

/**
 * The root node of a sitemap.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:37 $
 */

public class SitemapNode extends AbstractParentProcessingNode implements Composable {

    private static final String REDIRECTOR_ATTR = "sitemap:redirector";
    
    protected ComponentManager manager;
    
    protected ProcessingNode pipelines;
    
    protected ProcessingNode[] otherNodes;
    
    public static SitemapRedirector getRedirector(Environment env) {
        return (SitemapRedirector)env.getAttribute(REDIRECTOR_ATTR);
    }
    
    /**
     * Keep the component manager used everywhere in the tree so that we can
     * cleanly dispose it.
     */
    public void compose(ComponentManager manager) {
        this.manager = manager;
    }
    
    /**
     * Set the pipeline nodes that will process the environment.
     */
    public void setPipelines(ProcessingNode pipelines) {
        this.pipelines = pipelines;
    }
    
    /**
     * Set the non-pipeline nodes (views & resources), which are held to properly
     * <code>dispose()</code> them.
     */
    public void setOtherNodes(ProcessingNode[] otherNodes) {
        this.otherNodes = otherNodes;
    }

    /**
     * Process the environment. Also adds an <code>EnvironmentSourceResolver</code>
     * and a <code>Redirector</code> in the object model. The previous resolver and
     * redirector, if any, are restored before return.
     */
    public boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        // Recompose context (and pipelines) to the local component manager
        context.recompose(this.manager);

        // Create the source resolver relative to the environment.
        
        // Design note : creating the SourceResolver could also have been done
        // in TreeProcessor.invoke(), but doing it here ensures we use the local
        // component manager used by all other nodes.
        EnvironmentSourceResolver resolver = new EnvironmentSourceResolver(this.manager, env);
        SitemapRedirector redirector = new SitemapRedirector(env);
        
        Map objectModel = env.getObjectModel();
        
        Object oldResolver = objectModel.get(OBJECT_SOURCE_RESOLVER);
        Object oldRedirector = env.getAttribute(REDIRECTOR_ATTR);

        objectModel.put(OBJECT_SOURCE_RESOLVER, resolver);
        env.setAttribute(REDIRECTOR_ATTR, redirector);

        try {
            // FIXME : is there any useful information that can be passed as top-level parameters,
            //         such as the URI of the mount point ?
            
            return this.pipelines.invoke(env, context);
            
        } finally {

            // Restore old redirector and resolver
            env.setAttribute(REDIRECTOR_ATTR, oldRedirector);
            objectModel.put(OBJECT_SOURCE_RESOLVER, oldResolver);
            
            // Dispose the resolver
            resolver.dispose();
        }
    }
    
    /**
     * Dispose the component manager.
     */
    public void dispose() {
        if (this.manager instanceof Disposable) {
            ((Disposable)this.manager).dispose();
        }
    }
}