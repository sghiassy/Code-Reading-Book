/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.component.ComponentManager;

import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;

import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.sitemap.PatternException;

import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.AbstractProcessingNode;
import org.apache.cocoon.treeprocessor.TreeProcessor;
import org.apache.cocoon.treeprocessor.InvokeContext;

import java.util.*;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.component.ComponentException;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:37 $
 */

public class MountNode extends AbstractProcessingNode implements Composable {
    
    /** The 'uri-prefix' attribute */
    private MapStackResolver prefix;
    
    /** The 'src' attribute */
    private MapStackResolver source;
    
    /** Processors for sources */
    private Map processors = new HashMap();
    
    /** The processor for this node */
    private TreeProcessor parentProcessor;
    
    /** The language for the mounted processor */
    private String language;
    
    /** The component manager to be used by the mounted processor */
    private ComponentManager manager;
    
    public MountNode(String prefix, String source, String language, TreeProcessor parentProcessor)
      throws PatternException {
        this.prefix = MapStackResolver.getResolver(prefix);
        this.source = MapStackResolver.getResolver(source);
        this.language = language;
        this.parentProcessor = parentProcessor;
    }

    public void compose(ComponentManager manager) throws ComponentException {
        this.manager = manager;
    }
    
    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        List mapStack = context.getMapStack();
        
        String resolvedSource = this.source.resolve(mapStack);
        TreeProcessor processor = (TreeProcessor)processors.get(resolvedSource);
        if (processor == null) {
            processor = getProcessor(resolvedSource);
        }
        
        String resolvedPrefix = this.prefix.resolve(mapStack);

        String oldPrefix = env.getURIPrefix();
        String oldURI    = env.getURI();
        try {
            env.changeContext(resolvedPrefix, resolvedSource);

            if (context.isInternalRequest()) {
                // Propagate pipelines
                return processor.process(env, context.getStreamPipeline(), context.getEventPipeline());
            } else {
                // Processor will create its own pipelines
                return processor.process(env);
            }
            
        } finally {
            // Restore context
            env.setContext(oldPrefix, oldURI);
            
            // Recompose pipelines which may have been recomposed by subsitemap
            context.recompose(this.manager);
        }
    }
    
    private synchronized TreeProcessor getProcessor(String source) throws Exception {
        TreeProcessor processor = (TreeProcessor)processors.get(source);
        
        if (processor == null) {
            
            processor = this.parentProcessor.createChildProcessor(this.manager, this.language);

            processors.put(source, processor);
        }
        
        return processor;
    }
    
    public void dispose() {
        Iterator iter = this.processors.values().iterator();
        while(iter.hasNext()) {
            ((TreeProcessor)iter.next()).dispose();
        }
    }
}
