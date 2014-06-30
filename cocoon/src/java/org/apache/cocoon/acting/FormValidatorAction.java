// $Id: FormValidatorAction.java,v 1.1 2002/01/03 12:31:07 giacomo Exp $
package org.apache.cocoon.acting;

import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.util.Tokenizer;

import java.util.HashMap;
import java.util.Map;

/**
 * This is the action used to validate Request parameters.
 * The parameters are described via the external xml
 * file (its format is defined in AbstractValidatorAction).
 * 
 * <h3>Variant 1</h3>
 * <pre>
 * &lt;map:act type="form-validator"&gt;
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
 * &lt;map:act type="form-validator"&gt;
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
 * In addition a request attribute <code>org.apache.cocoon.acting.FormValidatorAction.results</code>
 * contains the validation results in both cases and make it available
 * to XSPs. Mind you that redirections create new request objects and thus
 * the result is not available for the target page.
 *
 * @author Martin Man &lt;Martin.Man@seznam.cz&gt;
 * @author <a href="mailto:haul@informatik.tu-darmstadt.de">Christian Haul</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $
 */
public class FormValidatorAction extends AbstractValidatorAction implements ThreadSafe
{
    /**
     * Main invocation routine.
     */
    public Map act (Redirector redirector, SourceResolver resolver, Map objectModel, String src,
            Parameters parameters) throws Exception {
        Request req = (Request)
            objectModel.get (Constants.REQUEST_OBJECT);

        /* check request validity */
        if (req == null) {
            getLogger().debug ("FORMVALIDATOR: no request object");
            return null;
        }

	// read global parameter settings
	boolean reloadable = Constants.DESCRIPTOR_RELOADABLE_DEFAULT;
	if (this.settings.containsKey("reloadable"))
	    reloadable = Boolean.getBoolean((String) this.settings.get("reloadable"));
	String constraints = (String) this.settings.get("constraint-set");
	// read local settings
        try {
            Configuration conf = this.getConfiguration (
                    parameters.getParameter ("descriptor", (String) this.settings.get("descriptor")), resolver, 
		    parameters.getParameterAsBoolean("reloadable", reloadable));
            String valstr = parameters.getParameter ("validate", (String) settings.get("validate",""));
            String valsetstr = parameters.getParameter ("validate-set", (String) settings.get("validate-set",""));
            Configuration[] desc = conf.getChildren ("parameter");
            Configuration[] csets = conf.getChildren ("constraint-set");
            HashMap actionMap = new HashMap ();
            HashMap resultMap = new HashMap ();
	    boolean allOK = true;
            /* 
             * old obsoleted method
             */
            if (!"".equals (valstr.trim ())) {
                getLogger().debug ("FORMVALIDATOR: validating parameters "
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
                        getLogger().debug ("FORMVALIDATOR: "
                        + "wrong syntax of the 'validate' parameter");
                        return null;
                    }
                    name = name.trim ();
                    params.put (name, req.getParameter (name));
                }
                for (int i = 0; i < rparams.length; i ++) {
                    name = rparams[i].trim ();
                    result = validateParameter (name, null, desc,
                            params, true);
                    if (!result.isOK()) {
                        getLogger().debug ("FORMVALIDATOR: "
                                + "validation failed for parameter " + name);
                        allOK = false;
                    }
                    actionMap.put (name, result.getObject());
                    resultMap.put (name, result.getResult());
                }
            }
            /* 
             * new set-based method
             */
            if (!"".equals (valsetstr.trim ())) {
                getLogger().debug ("FORMVALIDATOR: validating parameters "
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
                    getLogger().debug ("FORMVALIDATOR: given set "
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
                getLogger().debug ("FORMVALIDATOR: given set "
                        + valsetstr 
                        + " contains " + set.length + " rules");
                /* put required params into hash */
                for (int i = 0; i < set.length; i ++) {
                    name = set[i].getAttribute ("name", "");
                    if ("".equals (name.trim ())) {
                        getLogger().debug ("FORMVALIDATOR: wrong syntax "
                                + " of 'validate' children nr. " + i);
                        return null;
                    }
                    name = name.trim ();
                    params.put (name, req.getParameter (name));
                }
                for (int i = 0; i < set.length; i ++) {
                    name = set[i].getAttribute ("name", null);
                    result = validateParameter (name, set[i], 
                            desc, params, true);
                    if (!result.isOK()) {
                        getLogger().debug ("FORMVALIDATOR: "
                                + "validation failed for parameter " + name);
                        allOK = false;
                    }
                    actionMap.put (name, result.getObject());
                    resultMap.put (name, result.getResult());
                }
            }
            if (!allOK) {
                // if any validation failed return an empty map
                actionMap = null;
                getLogger().debug ("FORMVALIDATOR: all form "
                                   + "params validated. An error occurred.");
            } else {
                getLogger().debug ("FORMVALIDATOR: all form "
                                   + "params successfully validated");
            }
            // store validation results in request attribute
            req.setAttribute(Constants.XSP_FORMVALIDATOR_PATH, resultMap);
            //return Collections.unmodifiableMap (actionMap);
            return actionMap;
        } catch (Exception e) {
            getLogger().debug ("exception: ", e);
        }
        return null;
    }
}

// $Id: FormValidatorAction.java,v 1.1 2002/01/03 12:31:07 giacomo Exp $
// vim: set et ts=4 sw=4:
