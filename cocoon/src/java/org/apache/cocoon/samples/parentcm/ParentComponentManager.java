package org.apache.cocoon.samples.parentcm;

import org.apache.avalon.excalibur.component.ExcaliburComponentManager;
import org.apache.avalon.excalibur.naming.memory.MemoryInitialContextFactory;
import org.apache.avalon.framework.activity.Initializable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.context.DefaultContext;
import org.apache.avalon.framework.logger.Loggable;
import org.apache.log.Logger;

import javax.naming.Context;
import java.util.Hashtable;

/**
 * A sample parent component manager. This manager will lookup the configuration object
 * given by the initialization parameter in JNDI, use it to configure an ExcaliburComponentManager
 * and delegate any requests to it.
 *
 * @author <a href="mailto:leo.sutic@inspireinfrastructure.com">Leo Sutic</a>
 */
public class ParentComponentManager implements ComponentManager, Loggable, Initializable {

    /**
     * Our logger.
     */
    private Logger logger;

    /**
     * The JNDI name where the component manager configuration can be found.
     */
    private final String jndiName;

    /**
     * The delegate that will be configured and provide the
     * functionality for this component manager.
     */
    private final ExcaliburComponentManager delegate;

    public ParentComponentManager(final String jndiName) {
        this.jndiName = jndiName;

        // Initialize it here so we can let it be final.
        this.delegate = new ExcaliburComponentManager();
    }

    public boolean hasComponent(final String role) {
        return delegate.hasComponent(role);
    }

    /**
     * Initializes the CM by looking up the configuration object and using it to
     * configure the delegate.
     */
    public void initialize() throws Exception {
        this.logger.debug("Looking up component manager configuration at : " + this.jndiName);

        Hashtable environment = new Hashtable();
        environment.put(Context.INITIAL_CONTEXT_FACTORY, MemoryInitialContextFactory.class.getName());

        //
        // Yes, this is cheating, but the Excalibur in-memory naming provider
        // is transient. That is, it doesn't store objects persistently and
        // is more like a HashMap.
        //
        // Should be:
        // Context initialContext = new InitialContext(environment);
        //
        Context initialContext = Configurator.initialContext;

        Configuration config = (Configuration) initialContext.lookup(this.jndiName);

        // We ignore the setRoleManager call, as ExcaliburComponentManager handles that
        // in configure().
        this.delegate.setLogger(logger);
        this.delegate.contextualize(new DefaultContext());
        this.delegate.configure(config);
        this.delegate.initialize();

        this.logger.debug("Component manager successfully initialized.");
    }

    public Component lookup(final String role) throws ComponentException {
        return this.delegate.lookup(role);
    }

    public void release(final Component component) {
        this.delegate.release(component);
    }

    public void setLogger(final Logger logger) {
        this.logger = logger;
    }
}

