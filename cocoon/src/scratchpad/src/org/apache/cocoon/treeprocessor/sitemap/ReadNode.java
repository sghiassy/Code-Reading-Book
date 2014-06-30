/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.sitemap.PatternException;

import org.apache.cocoon.treeprocessor.AbstractProcessingNode;
import org.apache.cocoon.treeprocessor.InvokeContext;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ParameterizableProcessingNode;

import java.util.List;
import java.util.Map;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class ReadNode extends AbstractProcessingNode implements ParameterizableProcessingNode {

    private String readerName;
    
    private MapStackResolver source;
    
    private String mimeType;
    
    private int statusCode;
    
    private Map parameters;
    
    /**
     * Build a <code>SerializerNode</code> having a name, a mime-type and a status code (HTTP codes).
     *
     * @param name the name of the serializer to use.
     * @param mimeType the mime-type, or <code>null</code> not specified.
     * @param statusCode the HTTP response status code, or <code>-1</code> if not specified.
     */
    public ReadNode(String name, String source, String mimeType, int statusCode) throws PatternException {
        this.readerName = name;
        this.source = MapStackResolver.getResolver(source);
        this.mimeType = mimeType;
        this.statusCode = statusCode;
    }
    
    public void setParameters(Map parameterMap) {
        this.parameters = parameterMap;
    }

    public final boolean invoke(Environment env,  InvokeContext context)
      throws Exception {
        
        List mapStack = context.getMapStack();
        
        StreamPipeline pipeline = context.getStreamPipeline();
        
        if (this.mimeType == null) {
            // No mime-type set on node
            pipeline.setReader(
                this.readerName,
                source.resolve(mapStack),
                MapStackResolver.buildParameters(this.parameters, mapStack)
            );
            
        } else {
            // mime-type set on node
            pipeline.setReader(
                this.readerName,
                source.resolve(mapStack),
                MapStackResolver.buildParameters(this.parameters, mapStack),
                this.mimeType
            );
        }
        
        // Set status code if there is one
        if (this.statusCode >= 0) {
            env.setStatus(this.statusCode);
        }
        
        if (! context.isInternalRequest()) {
            // Process pipeline
            return pipeline.process(env);
            
        } else {
            // Return true : pipeline is finished.
            return true;
        }
    }
}
