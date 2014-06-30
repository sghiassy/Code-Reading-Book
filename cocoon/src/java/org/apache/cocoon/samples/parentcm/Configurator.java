package org.apache.cocoon.samples.parentcm;

import org.apache.avalon.excalibur.naming.memory.MemoryInitialContextFactory;
import org.apache.avalon.framework.configuration.DefaultConfiguration;

import javax.naming.Context;
import javax.naming.InitialContext;
import java.util.Hashtable;

/**
 * This class sets up the configuration used by the ParentComponentManager sample.
 * The class also holds a reference to the initial context in which the configuration
 * is available.
 * <p>
 * The configuration is bound to <code>org/apache/cocoon/samples/parentcm/ParentCMConfiguration</code>.
 *
 * @author <a href="mailto:leo.sutic@inspireinfrastructure.com">Leo Sutic</a>
 */
public class Configurator  {

    /**
     * The Excalibur in-memory JNDI directory. Since the directory doesn't
     * provide any persistence we must keep a reference to the initial context
     * as a static member to avoid passing it around.
     */
    public static Context initialContext = null;

    static {
        try {
            //
            // Create a new role.
            //
            DefaultConfiguration config = new DefaultConfiguration("roles", "");
            DefaultConfiguration timeComponent = new DefaultConfiguration("role", "roles");
            timeComponent.addAttribute("name", Time.ROLE);
            timeComponent.addAttribute("default-class", TimeComponent.class.getName());
            timeComponent.addAttribute("shorthand", "samples-parentcm-time");
            config.addChild(timeComponent);

            //
            // Bind it - get an initial context.
            //
            Hashtable environment = new Hashtable();
            environment.put(Context.INITIAL_CONTEXT_FACTORY, MemoryInitialContextFactory.class.getName());
            initialContext = new InitialContext(environment);

            //
            // Create subcontexts and bind the configuration.
            //
            Context ctx = initialContext.createSubcontext("org");
            ctx = ctx.createSubcontext("apache");
            ctx = ctx.createSubcontext("cocoon");
            ctx = ctx.createSubcontext("samples");
            ctx = ctx.createSubcontext("parentcm");
            ctx.rebind("ParentCMConfiguration", config);
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }
    }
}

