/*
 * Copyright (C) The Apache Software Foundation. All rights reserved.
 *
 * This software is published under the terms of the Apache Software License
 * version 1.1, a copy of which has been included  with this distribution in
 * the LICENSE file.
 */
package org.apache.cocoon.components;

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Stack;
import org.apache.avalon.excalibur.component.ExcaliburComponentManager;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Recomposable;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;

import org.xml.sax.SAXException;

/**
 * Cocoon Component Manager.
 * This manager extends the <code>ExcaliburComponentManager</code>
 * by a special lifecycle handling for a RequestLifecycleComponent.
 *
 * @author <a href="mailto:cziegeler@org.com">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/28 13:29:16 $
 */
public final class CocoonComponentManager
extends ExcaliburComponentManager
{
    /** The environment information */
    private static InheritableThreadLocal environmentStack = new InheritableThreadLocal();

    /** Create the ComponentManager */
    public CocoonComponentManager()
    {
        super( null, Thread.currentThread().getContextClassLoader() );
    }

    /** Create the ComponentManager with a Classloader */
    public CocoonComponentManager( final ClassLoader loader )
    {
        super( null, loader );
    }

    /** Create the ComponentManager with a Classloader and parent ComponentManager */
    public CocoonComponentManager( final ComponentManager manager, final ClassLoader loader )
    {
        super(manager, loader);
    }

    /** Create the ComponentManager with a parent ComponentManager */
    public CocoonComponentManager(final ComponentManager manager)
    {
        super(manager);
    }

    /**
     * This hook must be called by the sitemap each time a sitemap is entered
     */
    public static void enterEnvironment(SourceResolver resolver, Map objectModel) {
        if (environmentStack.get() == null) {
            environmentStack.set(new Stack());
        }
        final Stack stack = (Stack)environmentStack.get();
        stack.push(new Object[] {resolver, objectModel, new HashMap(5)});
    }

    /**
     * This hook must be called by the sitemap each time a sitemap is left
     */
    public static void leaveEnvironment() {
        final Stack stack = (Stack)environmentStack.get();
        if ( null != stack && !stack.empty()) {
            final Object[] objects = (Object[])stack.pop();
            final Map components = (Map)objects[2];
            final Iterator iter = components.values().iterator();
            while (iter.hasNext()) {
                final Object[] o = (Object[])iter.next();
                final Component component = (Component)o[0];
                ((ComponentManager)o[1]).release( component );
            }
        }
    }

    /**
     * Return an instance of a component based on a Role.  The Role is usually the Interface's
     * Fully Qualified Name(FQN)--unless there are multiple Components for the same Role.  In that
     * case, the Role's FQN is appended with "Selector", and we return a ComponentSelector.
     */
    public Component lookup( final String role )
    throws ComponentException {
        final Stack stack = (Stack)environmentStack.get();
        if ( null != stack && !stack.empty()) {
            final Object[] objects = (Object[])stack.peek();
            final Map components = (Map)objects[2];
            final Object[] o = (Object[])components.get(role);
            if ( null != o  ) {
                final Component component = (Component) o[0];
                if ( null != component ) {
                    return component;
                }
            }
        }

        final Component component = super.lookup( role );
        if ( null != component && component instanceof RequestLifecycleComponent) {
            if ( stack == null || stack.empty() ) {
                throw new ComponentException("ComponentManager has no Environment Stack.");
            }
            final Object[] objects = (Object[]) stack.peek();
            final Map components = (Map)objects[2];
            try {
                if (component instanceof Recomposable) {
                    ((Recomposable) component).recompose(this);
                }
                ((RequestLifecycleComponent) component).setup( (SourceResolver)objects[0],
                                                               (Map)objects[1]);
            } catch (Exception local) {
                throw new ComponentException("Exception during setup of RequestLifecycleComponent with role '"+role+"'", local);
            }
            components.put( role, new Object[] {component, this});
        }
        return component;
    }

    /**
     * Release a Component.  This implementation makes sure it has a handle on the propper
     * ComponentHandler, and let's the ComponentHandler take care of the actual work.
     */
    public void release( final Component component )
    {
        if( null == component )
        {
            return;
        }

        if ( component instanceof RequestLifecycleComponent) {
            return;
        }
        super.release( component);
    }

}
