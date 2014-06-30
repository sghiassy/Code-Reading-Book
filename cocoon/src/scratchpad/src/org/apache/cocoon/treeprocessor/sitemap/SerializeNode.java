/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.components.pipeline.StreamPipeline;

import org.apache.cocoon.sitemap.PatternException;

import org.apache.cocoon.treeprocessor.AbstractProcessingNode;
import org.apache.cocoon.treeprocessor.InvokeContext;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.*;
/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.4 $ $Date: 2002/01/30 14:35:07 $
 */

public class SerializeNode extends AbstractProcessingNode {

    private String serializerName;
    
    private String mimeType;
    
    private int statusCode;
    
    private Map views;
    
    /**
     * Build a <code>SerializerNode</code> having a name, a mime-type and a status code (HTTP codes).
     *
     * @param name the name of the serializer to use.
     * @param mimeType the mime-type, or <code>null</code> not specified.
     * @param statusCode the HTTP response status code, or <code>-1</code> if not specified.
     */
    public SerializeNode(String name, String mimeType, int statusCode) throws PatternException {
        this.serializerName = name;
        this.mimeType = mimeType;
        this.statusCode = statusCode;
    }
    
    public void setViews(Map views) {
        this.views = views;
    }
    
    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        // Check view
        if (this.views != null) {
            String cocoonView = env.getView();
            if (cocoonView != null) {
                
                // Get view node
                ProcessingNode viewNode = (ProcessingNode)this.views.get(cocoonView);
                
                if (viewNode != null) {
                    if (getLogger().isInfoEnabled()) {
                        getLogger().info("Jumping to view " + cocoonView + " from serializer at " + this.getLocation());
                    }
                    return viewNode.invoke(env, context);
                }
            }
        }

        // Perform link translation if requested
        if (env.getObjectModel().containsKey(Constants.LINK_OBJECT)) {
            context.getEventPipeline().addTransformer(
                "!link-translator!", null, Parameters.EMPTY_PARAMETERS
            );
        }

        StreamPipeline pipeline = context.getStreamPipeline();
        
        if (this.mimeType == null) {
            // No mime-type set on node
            pipeline.setSerializer(
                this.serializerName,
                null,
                Parameters.EMPTY_PARAMETERS // No parameters on serializers
            );
            
        } else {
            // mime-type set on node
            pipeline.setSerializer(
                this.serializerName,
                null,
                Parameters.EMPTY_PARAMETERS,
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
