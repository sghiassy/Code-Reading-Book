/*
 * Copyright (C) The Apache Software Foundation. All rights reserved.
 *
 * This software is published under the terms of the Apache Software License
 * version 1.1, a copy of which has been included with this distribution in
 * the LICENSE file.
 */
package org.apache.cocoon.components;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.activity.Initializable;
import org.apache.avalon.framework.activity.Startable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.logger.Loggable;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.avalon.excalibur.component.RoleManageable;
import org.apache.avalon.excalibur.component.RoleManager;
import org.apache.avalon.excalibur.logger.LogKitManager;
import org.apache.avalon.excalibur.logger.LogKitManageable;

import org.apache.log.Logger;

/**
 * Utility class for setting up Avalon components. Similar to Excalibur's
 * <code>DefaultComponentFactory</code>, but on existing objects.
 * <p>
 * To be moved to Avalon ?
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/15 11:10:52 $
 */
 
// FIXME : need to handle also LogEnabled.

public class LifecycleHelper
{
    /** The Logger for the component
     */
    private Logger                  m_logger;

    /** The Context for the component
     */
    private Context                 m_context;

    /** The component manager for this component.
     */
    private ComponentManager        m_componentManager;

    /** The configuration for this component.
     */
    private Configuration           m_configuration;

    /** The RoleManager for child ComponentSelectors
     */
    private RoleManager             m_roles;

    /** The LogKitManager for child ComponentSelectors
     */
    private LogKitManager           m_logkit;

    /**
     * Construct a new <code>LifecycleHelper</code> that can be used repeatedly to
     * setup several components. <b>Note</b> : if a parameter is <code>null</code>,
     * the corresponding method isn't called (e.g. if <code>configuration</code> is
     * <code>null</code>, <code>configure()</code> isn't called).
     *
     * @param logger the <code>Logger</code> to pass to <code>Loggable</code>s, unless there is
     *        a <code>LogKitManager</code> and the configuration specifies a logger name.
     * @param context the <code>Context</code> to pass to <code>Contexutalizable</code>s.
     * @param componentManager the component manager to pass to <code>Composable</code>s.
     * @param roles the <code>RoleManager</code> to pass to <code>DefaultComponentSelector</code>s.
     * @param configuration the <code>Configuration</code> object to pass to new instances.
     */
    public LifecycleHelper( final Logger logger,
            final Context context,
            final ComponentManager componentManager,
            final RoleManager roles,
            final LogKitManager logkit,
            final Configuration configuration )
    {
        m_logger = logger;
        m_context = context;
        m_componentManager = componentManager;
        m_roles = roles;
        m_logkit = logkit;
        m_configuration = configuration;
    }

    /**
     * Setup a component, including initialization and start.
     *
     * @param component the component to setup.
     * @return the component passed in, to allow function chaining.
     * @throws Exception if something went wrong.
     */
    public Object setupComponent(Object component)
        throws Exception
    {
        return setupComponent(component, true);
    }

    /**
     * Setup a component, and optionnaly initializes (if it's <code>Initializable</code>)
     * and starts it (if it's <code>Startable</code>).
     *
     * @param component the component to setup.
     * @param initializeAndStart if true, <code>intialize()</code> and <code>start()</code>
     *        will be called.
     * @return the component passed in, to allow function chaining.
     * @throws Exception if something went wrong.
     */
    public Object setupComponent( Object component, boolean initializeAndStart )
        throws Exception
    {
        return setupComponent( component,
                m_logger,
                m_context,
                m_componentManager,
                m_roles,
                m_logkit,
                m_configuration,
                initializeAndStart );
    }

    /**
     * Static equivalent to {@link #setupComponent(Object)}, to be used when there's only one
     * component to setup.
     */
    public static Object setupComponent( final Object component,
            final Logger logger,
            final Context context,
            final ComponentManager componentManager,
            final RoleManager roles,
            final LogKitManager logkit,
            final Configuration configuration )
        throws Exception
    {
        return setupComponent( component,
                logger,
                context,
                componentManager,
                roles,
                logkit,
                configuration,
                true );
    }

    /**
     * Static equivalent to {@link #setupComponent(Object, boolean)}, to be used when there's only one
     * component to setup.
     */
    public static Object setupComponent( final Object component,
            final Logger logger,
            final Context context,
            final ComponentManager componentManager,
            final RoleManager roles,
            final LogKitManager logkit,
            final Configuration configuration,
            final boolean initializeAndStart )
        throws Exception
    {
        if( component instanceof Loggable )
        {
            Logger usedLogger;
            if( null == logkit )
            {
                usedLogger = logger;
            }
            else if( configuration == null )
            {
                usedLogger = logger;
            }
            else
            {
                final String loggerName = configuration.getAttribute( "logger", null );
                if( null == loggerName )
                {
                    // No logger attribute available, using standard logger
                    usedLogger = logger;
                }
                else
                {
                    usedLogger = logkit.getLogger( loggerName );
                }
            }
            
            ((Loggable)component).setLogger( usedLogger );
        }

        if( null != context && component instanceof Contextualizable )
        {
            ((Contextualizable)component).contextualize( context );
        }

        if( null != componentManager && component instanceof Composable )
        {
            ((Composable)component).compose( componentManager );
        }

        if( null != roles && component instanceof RoleManageable )
        {
            ((RoleManageable)component).setRoleManager( roles );
        }

        if( null != logkit && component instanceof LogKitManageable )
        {
            ((LogKitManageable)component).setLogKitManager( logkit );
        }

        if( null != configuration && component instanceof Configurable )
        {
            ((Configurable)component).configure( configuration );
        }

        if( null != configuration && component instanceof Parameterizable )
        {
            ((Parameterizable)component).
                parameterize( Parameters.fromConfiguration( configuration ) );
        }

        if( initializeAndStart && component instanceof Initializable )
        {
            ((Initializable)component).initialize();
        }

        if( initializeAndStart && component instanceof Startable )
        {
            ((Startable)component).start();
        }

        return component;
    }

    /**
     * Decomission a component, by stopping (if it's <code>Startable</code>) and
     * disposing (if it's <code>Disposable</code>) a component.
     */
    public static final void decommission( final Object component )
        throws Exception
    {
        if( component instanceof Startable )
        {
            ((Startable)component).stop();
        }

        dispose( component );
    }
    
    /**
     * Dispose a component if it's <code>Disposable</code>. Otherwhise, do nothing.
     */
    public static final void dispose( final Object component )
    {
        if ( component instanceof Disposable )
        {
            ((Disposable)component).dispose();
        }
    }
}
