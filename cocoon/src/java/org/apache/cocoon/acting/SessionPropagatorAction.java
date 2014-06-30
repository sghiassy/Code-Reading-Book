// $Id: SessionPropagatorAction.java,v 1.1 2002/01/03 12:31:07 giacomo Exp $
package org.apache.cocoon.acting;

import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Session;
import org.apache.cocoon.environment.SourceResolver;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/**
 * This is the action used to propagate parameters into session. It
 * simply propagates given expression to the session. If session does not
 * exist, action fails. Additionaly it will make all propagated values
 * available via returned Map.
 *
 * <pre>
 * &lt;map:act type="session-propagator"&gt;
 *      &lt;paramater name="example" value="{example}"&gt;
 *      &lt;paramater name="example1" value="xxx"&gt;
 * &lt;/map:act&gt;
 * </pre>
 *
 * @author Martin Man &lt;Martin.Man@seznam.cz&gt;
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $
 */
public class SessionPropagatorAction extends AbstractConfigurableAction implements ThreadSafe
{

    private static Object[] defaults = {};
    
    public void configure(Configuration conf) throws ConfigurationException {
        if (conf != null) {
	    String[] names = {};
	    super.configure(conf);
	    defaults=(settings.keySet()).toArray();
	};
    }

    /**
     * Main invocation routine.
     */
    public Map act (Redirector redirector, SourceResolver resolver, Map objectModel, String src,
            Parameters parameters) throws Exception {
        Request req = (Request)
            objectModel.get (Constants.REQUEST_OBJECT);
        HashMap actionMap = new HashMap ();

	HashMap isDone = new HashMap();
	Integer dummy = new Integer(1);

        if (req == null) {
            getLogger ().debug ("SESSIONPROPAGATOR: no request object");
            return null;
        }

        /* check session validity */
        Session session = req.getSession (false);
        if (session == null) {
            getLogger ().debug ("SESSIONPROPAGATOR: no session object");
            return null;
        }

        try {

	    String[] names = parameters.getNames();

	    // parameters
	    for (int i=0; i<names.length; i++) {
		String sessionParamName = names[i];
                if (sessionParamName == null ||
		    "".equals (sessionParamName.trim ()))
                    return null;
		isDone.put(sessionParamName, dummy);
		String value = parameters.getParameter(sessionParamName);
                getLogger().debug ("SESSIONPROPAGATOR: propagating value "
                        + value
                        + " to session attribute "
                        + sessionParamName);
                session.setAttribute (sessionParamName, value);
                actionMap.put (sessionParamName, value);
	    }
        
	    // defaults, that are not overridden
	    for (int i=0; i<defaults.length; i++) {
		if (! isDone.containsKey(defaults[i])) {
		    String sessionParamName = (String) defaults[i];
		    if (sessionParamName == null ||
			"".equals (sessionParamName.trim ()))
			return null;
		    isDone.put(sessionParamName, dummy);
		    String value = parameters.getParameter(sessionParamName);
		    getLogger().debug ("SESSIONPROPAGATOR: propagating value "
				       + value
				       + " to session attribute "
				       + sessionParamName);
		    session.setAttribute (sessionParamName, value);
		    actionMap.put (sessionParamName, value);
		}
	    }
            getLogger().debug ("SESSIONPROPAGATOR: all params propagated "
                    + "to session");
            return Collections.unmodifiableMap (actionMap);
        } catch (Exception e) {
            getLogger().debug ("exception: ", e);
        }
        return null;
    }
}

// $Id: SessionPropagatorAction.java,v 1.1 2002/01/03 12:31:07 giacomo Exp $
// vim: set et ts=4 sw=4:

