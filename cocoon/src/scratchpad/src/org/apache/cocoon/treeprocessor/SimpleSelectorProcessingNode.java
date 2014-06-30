/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.treeprocessor;

import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.treeprocessor.AbstractParentProcessingNode;
import org.apache.cocoon.treeprocessor.ProcessingNode;

/**
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/15 11:10:52 $
 */

public abstract class SimpleSelectorProcessingNode extends SimpleParentProcessingNode {

    /** The node component name (e.g. action name, selector name, etc) */
    protected String componentName;
    
    /** Selector where to get components from */
    protected ComponentSelector selector;

    public SimpleSelectorProcessingNode(String componentName) {
        this.componentName = componentName;
    }
    
    public void setSelector(ComponentSelector selector) throws ComponentException {
        this.selector = selector;
    }
        
    /**
     * Tests is the component designated by this node using the selector and component name
     * is <code>ThreadSafe</code>, and return it if true.
     * <p>
     * Note : this method must be called <i>after</i> <code>setSelector()</code>.
     */
    protected Component getThreadSafeComponent() throws ComponentException {
        return getThreadSafeComponent(this.componentName);
    }

    /**
     * Tests is the component designated by this node using the selector and component name
     * is <code>ThreadSafe</code>, and return it if true.
     * <p>
     * Note : this method must be called <i>after</i> <code>setSelector()</code>.
     */
    protected Component getThreadSafeComponent(String name) throws ComponentException {
        Component component = this.selector.select(name);
        if (component instanceof ThreadSafe) {
            return component;
        } else {
            this.selector.release(component);
            return null;
        }
    }
}
