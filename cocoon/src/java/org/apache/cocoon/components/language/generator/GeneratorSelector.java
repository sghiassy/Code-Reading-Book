/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.generator;

import org.apache.avalon.excalibur.component.ComponentHandler;
import org.apache.avalon.excalibur.component.ExcaliburComponentSelector;
import org.apache.avalon.excalibur.component.RoleManager;
import org.apache.avalon.excalibur.logger.LogKitManager;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.configuration.DefaultConfiguration;
import org.apache.avalon.framework.context.Context;
import org.apache.cocoon.Constants;
import org.apache.cocoon.components.classloader.ClassLoaderManager;

import java.io.File;
import java.util.*;

/**
 * This interface is the common base of all Compiled Components.  This
 * includes Sitemaps and XSP Pages
 *
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:09 $
 */
public class GeneratorSelector extends ExcaliburComponentSelector implements Disposable {

    public static String ROLE = "org.apache.cocoon.components.language.generator.ServerPages";

    private ClassLoaderManager classManager;

    /** The component manager */
    protected ComponentManager manager;

    private LogKitManager logKitManager;

    protected Context context;

    protected RoleManager roles;

    protected Map componentHandlers = new HashMap();

    /** Dynamic component handlers mapping. */
    private Map componentMapping = new HashMap();

    public void contextualize(Context context) {
        super.contextualize(context);
        this.context = context;
    }

    public void setRoleManager(RoleManager roleMgr) {
        super.setRoleManager(roleMgr);
        this.roles = roleMgr;
    }

    public void setLogKitManager(LogKitManager logKitMgr) {
        super.setLogKitManager(logKitMgr);
        this.logKitManager = logKitMgr;
    }

    public void compose (ComponentManager manager) throws ComponentException {
        super.compose(manager);
        this.manager = manager;

        try {
            this.classManager = (ClassLoaderManager) manager.lookup(ClassLoaderManager.ROLE);
        } catch (ComponentException cme) {
            throw new ComponentException("GeneratorSelector", cme);
        }

        try {
            this.classManager.addDirectory((File) this.m_context.get(Constants.CONTEXT_WORK_DIR));
        } catch (Exception e) {
            throw new ComponentException("Could not add repository to ClassLoaderManager", e);
        }
    }

    public Component select(Object hint) throws ComponentException {
        try {
            return super.select(hint);
        } catch (Exception e) {
            // if it isn't loaded, it may already be compiled...
            try {
                ComponentHandler handler = (ComponentHandler) this.componentHandlers.get(hint);
                Component component = (Component) handler.get();
                componentMapping.put(component, handler);
                return component;
            } catch (Exception ce) {
                getLogger().debug("Could not access component for hint: " + hint);
                throw new ComponentException("Could not access component for hint: " + hint, null);
            }
        }
    }

    public void release(Component component) {
        ComponentHandler handler = (ComponentHandler)componentMapping.get(component);
        if (handler != null) {
            try {
                handler.put(component);
            } catch (Exception e) {
                getLogger().error("Error trying to release component", e);
            }
        } else {
            super.release(component);
        }
    }

    private void addGenerator(Object hint) throws Exception {
        Class generator;
        String className = hint.toString().replace(File.separatorChar, '.');
        generator = this.classManager.loadClass(className);

        this.addGenerator(this.manager, hint, generator);
    }

    protected void addGenerator(ComponentManager newManager, Object hint, Class generator) throws Exception {
        try
        {
            final ComponentHandler handler =
                ComponentHandler.getComponentHandler( generator,
                                                      new DefaultConfiguration("", "GeneratorSelector"),
                                                      newManager,
                                                      this.context,
                                                      this.roles,
                                                      this.logKitManager );

            handler.setLogger( getLogger() );
            handler.initialize();
            this.componentHandlers.put( hint, handler );
            getLogger().debug( "Adding " + generator.getName() + " for " + hint.toString() );
        }
        catch( final Exception e )
        {
            getLogger().error("Could not set up Component for hint: " + hint, e);
            throw e;
        }
    }

    protected void removeGenerator(Object hint) {
        ComponentHandler handler = (ComponentHandler) this.componentHandlers.remove(hint);
        handler.dispose();
        this.classManager.reinstantiate();
        getLogger().debug( "Removing " + handler.getClass().getName() + " for " + hint.toString());
    }

    public void dispose() {
        this.manager.release((Component)this.classManager);

        synchronized(this)
        {
            Iterator keys = this.componentHandlers.keySet().iterator();
            List keyList = new ArrayList();

            while( keys.hasNext() )
            {
                Object key = keys.next();
                ComponentHandler handler =
                    (ComponentHandler)this.componentHandlers.get( key );

                handler.dispose();

                keyList.add( key );
            }

            keys = keyList.iterator();

            while( keys.hasNext() )
            {
                this.componentHandlers.remove( keys.next() );
            }

            keyList.clear();
        }

        super.dispose();
    }
}
