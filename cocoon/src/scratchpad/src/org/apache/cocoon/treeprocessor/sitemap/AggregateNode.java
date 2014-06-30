/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.environment.Environment;

import org.apache.cocoon.sitemap.PatternException;
import org.apache.cocoon.sitemap.ContentAggregator;

import org.apache.cocoon.treeprocessor.AbstractProcessingNode;
import org.apache.cocoon.treeprocessor.InvokeContext;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ProcessingNode;

import java.util.*;

/**
 *
 * View-handling in aggregation :
 * <ul>
 * <li>map:aggregate can have a label, but doesn't match view from-position="first" like generators
 * </li>
 * <li>each map:part can have a label
 * </li>
 * <li>parts labels are checked first for a matching view, then the aggregate label is checked
 * </li>
 * </ul>
 * For more info on aggregation and view, see the mail archive
 * <a href="http://marc.theaimsgroup.com/?l=xml-cocoon-dev&m=100525751417953">here</a> or 
 * <a href="http://marc.theaimsgroup.com/?l=xml-cocoon-dev&m=100517130418424">here</a>.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/16 10:54:12 $
 */

public class AggregateNode extends AbstractProcessingNode {
    
    private Part[] parts;
    private String element;
    private String nsURI;
    private String nsPrefix;
    
    /** Views for each part */
    private Map[]partViews;
    
    /** View for map:aggregate */
    private Map views;
    
    public AggregateNode(String element, String nsURI, String nsPrefix) {
        this.element = element;
        this.nsURI = nsURI;
        this.nsPrefix = nsPrefix;
    }
    
    public void setParts(Part[] parts) {
        this.parts = parts;
    }
    
    public void setViews(Map views, Map[]partViews) {
        this.views = views;
        this.partViews = partViews;
    }
    
    public boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        boolean infoEnabled = getLogger().isInfoEnabled();
        
        List mapStack = context.getMapStack();

        // Setup aggregator
        EventPipeline eventPipeline = context.getEventPipeline();
        
        eventPipeline.setGenerator("!content-aggregator!", null, Parameters.EMPTY_PARAMETERS);

        ContentAggregator aggregator = (ContentAggregator)eventPipeline.getGenerator();
        aggregator.setRootElement(this.element, this.nsURI, this.nsPrefix);
        
        // Get actual parts, potentially filtered by the view
        Part[] actualParts;
        String cocoonView = env.getView();
        if (cocoonView == null) {
            // Keep all parts
            actualParts = this.parts;

        } else {
            // Keep only those parts that are in the view
            actualParts = new Part[this.parts.length];
            for (int i = 0; i < actualParts.length; i++) {
                if (this.partViews[i] != null && this.partViews[i].containsKey(cocoonView)) {
                    // this part is in the view
                    actualParts[i] = this.parts[i];
                } else {
                    if (infoEnabled) {
                        getLogger().info("Aggregate part " + (i+1) + " is not in the '" +
                            cocoonView + "' view, at " + this.getLocation());
                    }
                }
            }
        }

        // Add parts
        for (int i = 0; i < actualParts.length; i++) {
            Part part = actualParts[i];
            if (part != null) {
                // FIXME : update ContentAggregator to accept boolean for stripRoot
                aggregator.addPart(part.source.resolve(mapStack),
                    part.element, part.nsURI, String.valueOf(part.stripRoot), part.nsPrefix
                );
            }
        }
        
        // Check aggregate-level view
        if (cocoonView != null && this.views != null) {
            ProcessingNode viewNode = (ProcessingNode)this.views.get(cocoonView);
            if (viewNode != null) {
                if (infoEnabled) {
                    getLogger().info("Jumping to view '" + cocoonView + "' from aggregate at " + this.getLocation());
                }
                return viewNode.invoke(env, context);
            }
        }
        
        // Return false to continue sitemap invocation
        return false;
    }
    
    public static class Part {
        public Part(String source, String element, String nsURI, String nsPrefix, boolean stripRoot)
          throws PatternException {
            this.source = MapStackResolver.getResolver(source);
            this.element = element;
            this.nsURI = nsURI;
            this.nsPrefix = nsPrefix;
            this.stripRoot = stripRoot;
        }

        private MapStackResolver source;
        private String element;
        private String nsURI;
        private String nsPrefix;
        private boolean stripRoot;
    }
}
