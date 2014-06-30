// $Id: SessionValidatorAction.java,v 1.1 2002/01/03 12:31:07 giacomo Exp $
package org.apache.cocoon.acting;

import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Session;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.util.Tokenizer;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;


/**
 * This is the action used to validate Session parameters (attributes).
 * The parameters are described via the external xml
 * file (its format is defined in AbstractValidatorAction).
 *
 * <h3>Variant 1</h3>
 * <pre>
 * &lt;map:act type="session-validator"&gt;
 *         &lt;parameter name="descriptor" value="context://descriptor.xml"&gt;
 *         &lt;parameter name="validate" value="username,password"&gt;
 * &lt;/map:act&gt;
 * </pre>
 * The list of parameters to be validated is specified as a comma separated
 * list of their names. descriptor.xml can therefore be used among many
 * various actions.
 *
 * <h3>Variant 2</h3>
 * <pre>
 * <pre>
 * &lt;map:act type="session-validator"&gt;
 *         &lt;parameter name="descriptor" value="context://descriptor.xml"&gt;
 *         &lt;parameter name="validate-set" value="is-logged-in"&gt;
 * &lt;/map:act&gt;
 * </pre>
 *
 * The parameter "validate-set" tells to take a given "constraint-set"
 * from description
 * file and test all parameters against given criteria. This variant is more
 * powerful, more aspect oriented and more flexibile than the previous one,
 * becuase it allows the comparsion constructs, etc. See
 * AbstractValidatorAction documentation.
 *
 * This action returns null when validation fails, otherwise it provides
 * all validated parameters to the sitemap via {name} expression.
 *
 * @author Martin Man &lt;Martin.Man@seznam.cz&gt;
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $
 */
public class SessionValidatorAction extends AbstractValidatorAction implements ThreadSafe
{
    /**
     * Main invocation routine.
     */
    public Map act (Redirector redirector, SourceResolver resolver, Map objectModel, String src,
            Parameters parameters) throws Exception {
        Request req = (Request)
            objectModel.get (Constants.REQUEST_OBJECT);

        if (req == null) {
            getLogger ().debug ("SESSIONVALIDATOR: no request object");
            return null;
        }

        /* check session validity */
        Session session = req.getSession (false);
        if (session == null) {
            getLogger ().debug ("SESSIONVALIDATOR: no session object");
            return null;
        }

    // read global parameter settings
    boolean reloadable = Constants.DESCRIPTOR_RELOADABLE_DEFAULT;
    if (this.settings.containsKey("reloadable"))
        reloadable = Boolean.getBoolean((String) this.settings.get("reloadable"));

        try {
            Configuration conf = this.getConfiguration (
                    parameters.getParameter ("descriptor", (String) this.settings.get("descriptor")), resolver,
            parameters.getParameterAsBoolean("reloadable",reloadable));

            String valsetstr = parameters.getParameter ( "validate-set", (String) settings.get("validate-set") );
            String valstr = parameters.getParameter ( "validate", (String) settings.get("validate") );

            Configuration[] desc = conf.getChildren ("parameter");
            Configuration[] csets = conf.getChildren ("constraint-set");
            HashMap actionMap = new HashMap ();
            /*
             * old obsoleted method
             */
            if (valstr != null && !"".equals (valstr.trim ())) {
                getLogger ().debug ("SESSIONVALIDATOR: validating parameters "
                        + "as specified via 'validate' parameter");
                /* get list of params to be validated */
                String[] rparams = Tokenizer.tokenize (valstr, ",", false);

                /* perform actuall validation */
                ValidatorActionHelper result = null;
                String name = null;
                HashMap params = new HashMap (rparams.length);
                /* put required params into hash */
                for (int i = 0; i < rparams.length; i ++) {
                    name = rparams[i];
                    if (name == null || "".equals (name.trim ())) {
                        getLogger ().debug ("SESSIONVALIDATOR: "
                        + "wrong syntax of the 'validate' parameter");
                        return null;
                    }
                    name = name.trim ();
                    params.put (name, session.getAttribute (name));
                }
                for (int i = 0; i < rparams.length; i ++) {
                    name = rparams[i].trim ();
                    result = validateParameter (name, null, desc,
                            params, false);
                    if (!result.isOK()) {
                        getLogger().debug ("SESSIONVALIDATOR: "
                                + "validation failed for parameter " + name);
                        return null;
                    }
                    session.setAttribute (name, result.getObject());
                    actionMap.put (name, result.getObject());
                }
            }
            /*
             * new set-based method
             */
            if (valsetstr != null && !"".equals (valsetstr.trim ())) {
                getLogger ().debug ("SESSIONVALIDATOR: validating parameters "
                        + "from given constraint-set " + valsetstr);
                Configuration cset = null;
                String setname = null;
                int j = 0;
                boolean found = false;
                for (j = 0; j < csets.length; j ++) {
                    setname = csets[j].getAttribute ("name", "");
                    if (valsetstr.trim().equals (setname.trim ())) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    getLogger ().debug ("SESSIONVALIDATOR: given set "
                            + valsetstr
                            + " does not exist in a description file");
                    return null;
                }
                cset = csets[j];
                /* get the list of params to be validated */
                Configuration[] set = cset.getChildren ("validate");

                /* perform actuall validation */
                ValidatorActionHelper result = null;
                String name = null;
                HashMap params = new HashMap (set.length);
                getLogger ().debug ("SESSIONVALIDATOR: given set "
                        + valsetstr
                        + " contains " + set.length + " rules");
                /* put required params into hash */
                for (int i = 0; i < set.length; i ++) {
                    name = set[i].getAttribute ("name", "");
                    if ("".equals (name.trim ())) {
                        getLogger ().debug ("SESSIONVALIDATOR: wrong syntax "
                                + " of 'validate' children nr. " + i);
                        return null;
                    }
                    name = name.trim ();
                    params.put (name, session.getAttribute (name));
                }
                for (int i = 0; i < set.length; i ++) {
                    name = set[i].getAttribute ("name", null);
                    result = validateParameter (name, set[i],
                            desc, params, false);
                    if (!result.isOK()) {
                        getLogger().debug ("SESSIONVALIDATOR: "
                                + "validation failed for parameter " + name);
                        return null;
                    }
                    session.setAttribute (name, result.getObject());
                    actionMap.put (name, result.getObject());
                }
            }
            getLogger().debug ("SESSIONVALIDATOR: all session "
                    + "params validated");
            return Collections.unmodifiableMap (actionMap);
        } catch (Exception e) {
            getLogger().debug ("exception: ", e);
        }
        return null;
    }
}

// $Id: SessionValidatorAction.java,v 1.1 2002/01/03 12:31:07 giacomo Exp $
// vim: set et ts=4 sw=4:
