/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor.sitemap;

import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

import org.apache.cocoon.matching.Matcher;
import org.apache.cocoon.matching.PreparableMatcher;
import org.apache.cocoon.treeprocessor.AbstractParentProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.MapStackResolver;
import org.apache.cocoon.treeprocessor.ProcessingNode;
import org.apache.cocoon.treeprocessor.LinkedProcessingNodeBuilder;
import org.apache.cocoon.treeprocessor.SimpleSelectorProcessingNode;

import java.util.*;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:54 $
 */

public class MatchNodeBuilder extends AbstractParentProcessingNodeBuilder
  implements Composable {

    private static final String SELECTOR_ROLE = Matcher.ROLE + "Selector";

    private ComponentManager manager;

    public void compose(ComponentManager manager) {
        this.manager = manager;
    }

    public ProcessingNode buildNode(Configuration config) throws Exception {
        
        String pattern = config.getAttribute("pattern", null);
        
        String type = this.treeBuilder.getTypeForStatement(config, SELECTOR_ROLE);

        // Get the type and class for this matcher
        ComponentsSelector selector = (ComponentsSelector)this.manager.lookup(SELECTOR_ROLE);
        
        // Find matcher class
        Matcher matcher = (Matcher)selector.select(type);
        Class clazz = matcher.getClass();
        selector.release(matcher);
    
        // PreparableMatcher are only prepared if pattern doesn't need request-time resolution.
        boolean preparable =
            PreparableMatcher.class.isAssignableFrom(clazz) &&
            !MapStackResolver.needsResolve(pattern);
        
        // Instanciate appropriate node
        SimpleSelectorProcessingNode node;
        
        if (preparable) {
            node = new PreparableMatchNode(type, pattern);
        } else {
            node = new MatchNode(type, pattern);
        }
        
        this.treeBuilder.setupNode(node, config);
        
        // Get all children
        ProcessingNode[] children = buildChildNodes(config);
        
        if (children.length == 0) {
            String msg = "There must be at least one child in match at " + config.getLocation();
            getLogger().error(msg);
            throw new ConfigurationException(msg);
        }
       
        node.setChildren(children);
        
        return node;
    }
}
