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
import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.ConnectionResetException;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ResourceNotFoundException;

import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.ObjectModelHelper;
import org.apache.cocoon.components.notification.Notifying;
import org.apache.cocoon.components.notification.NotifyingBuilder;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.treeprocessor.AbstractParentProcessingNode;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ProcessingNode;
import org.apache.cocoon.treeprocessor.InvokeContext;

import java.util.*;

/**
 * 
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/15 11:10:54 $
 */

public class PipelineNode extends AbstractParentProcessingNode implements Composable {

    // TODO : handle a 'fail-hard' environment attribute
    // can be useful to stop off-line generation when there's an error
    
    private ProcessingNode[] children;
    
    private ProcessingNode error404;
    
    private ProcessingNode error500;

    private ComponentManager manager;
    
    private boolean internalOnly = false;
    
    /** Is it the last <pipeline> in the enclosing <pipelines> ? */
    private boolean isLast = false;
    
    /**
     * The component manager is used to create error pipelines
     */
    public void compose(ComponentManager manager) {
        this.manager = manager;
    }
    
    public void setChildren(ProcessingNode[] nodes)
    {
        this.children = nodes;
    }
    
    public void setLast(boolean isLast) {
        this.isLast = isLast;
    }
    
    public void set404Handler(ProcessingNode node)
    {
        this.error404 = node;
    }
    
    public void set500Handler(ProcessingNode node)
    {
        this.error500 = node;
    }
    
    public void setInternalOnly(boolean internalOnly) {
        this.internalOnly = internalOnly;
    }

    public final boolean invoke(Environment env, InvokeContext context)
      throws Exception {
        
        // Always fail on external resquests if internal only.
        if (this.internalOnly && !context.isInternalRequest()) {
            return false;
        }
        
        try {
            
            if (invokeNodes(children, env, context)) {
                return true;
            } else if (this.isLast) {
                String msg = "No pipeline matched request: " + env.getURIPrefix() + '/' + env.getURI();
                getLogger().info(msg);
                throw new ResourceNotFoundException(msg);
            } else {
                return false;
            }
            
        } catch (ConnectionResetException cre) {
            // Will be reported by CocoonServlet, rethrowing
            throw cre;

        } catch(ResourceNotFoundException rnfe) {
            getLogger().warn("Resource not found in pipeline at " + getLocation(), rnfe);
            
            if (error404 != null) {
                // There's a handler
                return invokeErrorHandler(error404, rnfe, env);
                
            } else {
                // No handler : propagate
                throw rnfe;
            }

        } catch(Exception e) {
            
            // Rethrow exception for internal requests
            if (error500 != null && !context.isInternalRequest()) {
                
                getLogger().warn("Error while processing pipeline at " + getLocation(), e);
                
                return invokeErrorHandler(error500, e, env);
                
            } else {
                // No handler : propagate
                
                getLogger().error("Error while processing pipeline at " + getLocation(), e);
                throw e;
            }
        }
    }
    
    private boolean invokeErrorHandler(ProcessingNode node, Exception ex, Environment env)
      throws Exception {

        InvokeContext errorContext = null;
        
        try {
            // Try to reset the response to avoid mixing already produced output
            // and error page.
            env.tryResetResponse();
            
            // Build a new context
            errorContext = new InvokeContext();
            errorContext.compose(this.manager);
            
            // Create a Notifying
            NotifyingBuilder notifyingBuilder= (NotifyingBuilder)this.manager.lookup(NotifyingBuilder.ROLE);
            Notifying currentNotifying = notifyingBuilder.build(this, ex);
            this.manager.release(notifyingBuilder);
            
            // Add it to the object model
            env.getObjectModel().put(Constants.NOTIFYING_OBJECT, currentNotifying );

            // notifying-generator is added in HandleErrorsNode
            return node.invoke(env, errorContext);
            
        } catch (Exception subEx) {
            getLogger().error("error notifier barfs", subEx);
            return false;
            
        } finally {
            if (errorContext != null) {
                errorContext.dispose();
            }
        }
    }
}
