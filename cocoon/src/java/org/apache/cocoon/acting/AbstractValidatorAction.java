// $Id: AbstractValidatorAction.java,v 1.1 2002/01/03 12:31:07 giacomo Exp $
package org.apache.cocoon.acting;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.regexp.RE;
import org.apache.regexp.RESyntaxException;

import java.util.Map;

/**
 * Abstract implementation of action that needs to perform validation of
 * parameters (from session, from request, etc.). All `validator' actions
 * share the same description xml file. In such file every parameter is
 * described via its name, type and its constraints. One large description
 * file can be used among all validator actions, because each action should
 * explicitely specify which parameters to validate - through a sitemap
 * parameter.
 *
 * <pre>
 * &lt;root&gt;
*         &lt;parameter name="username" type="string" nullable="no"/&gt;
*         &lt;parameter name="role" type="string" nullable="no"/&gt;
*         &lt;parameter name="oldpassword" type="string" nullable="no"/&gt;
*         &lt;parameter name="newpassword" type="string" nullable="no"/&gt;
*         &lt;parameter name="renewpassword" type="string" nullable="no"/&gt;
*         &lt;parameter name="id" type="long" nullable="no"/&gt;
*         &lt;parameter name="sallary" type="double" nullable="no"/&gt;
*         &lt;parameter name="theme" type="string" nullable="yes" default="dflt"/&gt;
*         &lt;constraint-set name="is-logged-in"&gt;
*                 &lt;validate name="username"/&gt;
*                 &lt;validate name="role"/&gt;
*         &lt;/constraint-set&gt;
 *
*         &lt;constraint-set name="is-in-admin-role"&gt;
*                 &lt;validate name="username"/&gt;
*                 &lt;validate name="role" equals-to="admin"/&gt;
*         &lt;/constraint-set&gt;
 *
*         &lt;constraint-set name="new-passwords-match"&gt;
*                 &lt;validate name="oldpassword"/&gt;
*                 &lt;validate name="newpassword"/&gt;
*                 &lt;validate name="renewpassword" 
*                         equals-to-param="newpass"/&gt;
*         &lt;/constraint-set&gt;
 * &lt;/root&gt;
 * </pre>
 *
 * <h3>The types recognized by validator and their attributes</h3>
 * <table border="1">
*         <tr>
*                 <td><b>string</b></td><td>nullable="yes|no" default="str"</td>
*         </tr>
*         <tr>
*                 <td><b>long</b></td><td>nullable="yes|no" default="123123"</td>
*         </tr>
*         <tr>
*                 <td><b>double</b></td><td>nullable="yes|no" default="0.5"</td>
*         </tr>
 * </table>
 * Default value takes place only when specified parameter is nullable and
 * really is null or empty. Long numbers may be specified in decimal, hex or
 * octal values as accepted by java.Lang.decode (String s).
 *
 * <h3>Constraints</h3>
 * <table border="1">
 * <tr>
*     <td>matches-regex</td><td>POSIX regular expression</td>
 * </tr>
 * <tr>
*     <td>min-len</td><td>positive integer</td>
 * </tr>
 * <tr>
*     <td>max-len</td><td>positive integer</td>
 * </tr>
 * <tr>
*     <td>min</td><td>Double / Long</td>
 * </tr>
 * <tr>
*     <td>max</td><td>Double / Long</td>
 * </tr>
 * </table>
* Constraints can be defined globally for a parameter and can be overridden
* by redefinition in a constraint-set. Thus if e.g. a database field can take
* at maximum 200 character, this property can be set globally.
* 
*
* <h3>The attributes recognized in "constraint-set"</h3>
* <table>
* <tr>
*     <td>equals-to-param</td><td>parameter name</td>
* </tr>
* <tr>
*     <td>equals-to</td><td>string constant</td>
* </tr>
* </table>
* @author Martin Man &lt;Martin.Man@seznam.cz&gt;
* @author <a href="mailto:haul@informatik.tu-darmstadt.de">Christian Haul</a>
* @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $
*/
public abstract class AbstractValidatorAction
extends AbstractComplementaryConfigurableAction
implements Configurable
{
    /**
     * Try to validate given parameter.
     * @param name The name of the parameter to validate.
     * @param constraints Configuration of all constraints for this parameter
     * as taken from the description XML file.
     * @param conf Configuration of all parameters as taken from the
     * description XML file.
     * @param params The map of parameters.
     * @param is_string Indicates wheter given param to validate is string
     * (as taken from HTTP request for example) or wheteher it should be
     * regular instance of java.lang.Double, java.lang.Long, etc.
     * @return The validated parameter.
     */
    public ValidatorActionHelper validateParameter (String name, Configuration constraints, 
            Configuration[] conf, Map params, boolean is_string) {
        String type = null;
        int i = 0;

        getLogger().debug ("VALIDATOR: validating parameter: " + name);

        /* try to find matching param description in conf tree */
        try {
            boolean found = false;
            for (i = 0; i < conf.length; i ++) {
                if (name.equals (conf[i].getAttribute ("name"))) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                getLogger ().debug ("VALIDATOR: description for parameter "
                        + name + " not found");
                return null;
            }

            /* check parameter's type */
            type = conf[i].getAttribute ("type");
        } catch (Exception e) {
            getLogger ().debug ("VALIDATOR: no type specified for parameter "
                    + name);
            return null;
        }

        /*
         * Validation phase
         */
        if ("string".equals (type)) {
            return validateString (name, constraints, 
                    conf[i],params, is_string);
        } else if ("long".equals (type)) {
            return validateLong (name, constraints, 
                    conf[i], params, is_string);
        } else if ("double".equals (type)) {
            return validateDouble (name, constraints, 
                    conf[i], params, is_string);
        } else {
            getLogger().debug ("VALIDATOR: unknown type " + type
                    + " specified for parameter " + name);
        }
        return null;
    }

    /**
     * Validates nullability and default value for given parameter. If given
     * constraints are not null they are validated as well.
     */
    private ValidatorActionHelper validateString (String name, Configuration constraints,
            Configuration conf, Map params, boolean is_string) {
        Object param = params.get (name);
        String value = null;
        String dflt = getDefault (conf, constraints);
        boolean nullable = getNullable (conf, constraints);

        getLogger().debug ("VALIDATOR: validating string parameter "
                + name + " (encoded in a string: " + is_string + ")");
        value = getStringValue (name, conf, param, is_string);
        if (value == null) {
            getLogger().debug ("VALIDATOR: string parameter "
                    + name + " is null");
            if ( !nullable ) {
                return new ValidatorActionHelper ( value, 
                        ValidatorActionResult.ISNULL );
            } else { 
                value = dflt;
                return new ValidatorActionHelper (value);
            }
        }
        if (constraints != null) {
            String eq = constraints.getAttribute ("equals-to", "");
            String eqp = constraints.getAttribute ("equals-to-param", "");

            String regex = conf.getAttribute ("matches-regex", "");
            regex = constraints.getAttribute ( "matches-regex", regex);

            Long minlen = getAttributeAsLong (conf, "min-len", null);
            minlen = getAttributeAsLong (constraints, "min-len", minlen);

            Long maxlen = getAttributeAsLong (conf, "max-len", null);
            maxlen = getAttributeAsLong (constraints, "max-len", maxlen);

            // Validate whether param is equal to constant
            if (!"".equals (eq)) {
                getLogger().debug ("VALIDATOR: string parameter "
                        + name + " should be equal to " + eq);
                if (!value.equals (eq)) {
                    getLogger().debug ("VALIDATOR: and it is not");
                    return new ValidatorActionHelper ( value, ValidatorActionResult.NOMATCH);
                }
            }

            // Validate whether param is equal to another param
            // FIXME: take default value of param being compared with into
            // account?
            if (!"".equals (eqp)) {
                getLogger().debug ("VALIDATOR: string parameter "
                        + name + " should be equal to " + params.get (eqp));
                if (!value.equals (params.get (eqp))) {
                    getLogger().debug ("VALIDATOR: and it is not");
                    return new ValidatorActionHelper ( value, ValidatorActionResult.NOMATCH);
                }
            }

            // Validate whether param length is at least of minimum length
            if (minlen != null) {
                getLogger().debug ("VALIDATOR: string parameter "
                        + name + " should be at least " + minlen + " characters long");
                if ( value.length() < minlen.longValue() ) {
                    getLogger().debug ("VALIDATOR: and it is shorter (" +
                            value.length() + ")" );
                    return new ValidatorActionHelper ( value, ValidatorActionResult.TOOSMALL);
                }
            }

            // Validate whether param length is at most of maximum length
            if (maxlen != null) {
                getLogger().debug ("VALIDATOR: string parameter "
                        + name + " should be at most " + maxlen + " characters long");

                if ( value.length() > maxlen.longValue() ) {
                    getLogger().debug ("VALIDATOR: and it is longer (" +
                            value.length() + ")" );
                    return new ValidatorActionHelper ( value, ValidatorActionResult.TOOLARGE);
                }
            }

            // Validate wheter param matches regular expression
            if (!"".equals (regex)) {
                getLogger().debug ("VALIDATOR: string parameter " + name +
                        " should match regexp \"" + regex + "\"" );
                try {
                    RE r = new RE ( regex );
                    if ( !r.match(value) ) {
                        getLogger().debug("VALIDATOR: and it does not match");
                        return new ValidatorActionHelper ( value, ValidatorActionResult.NOMATCH);
                    };
                } catch ( RESyntaxException rese ) {
                    getLogger().error ("VALIDATOR: string parameter " + name +
                            " regex error ", rese);
                    return new ValidatorActionHelper ( value, ValidatorActionResult.NOMATCH);
                }
            }

        }
        return new ValidatorActionHelper(value);
    }

    /**
     * Validates nullability and default value for given parameter. If given
     * constraints are not null they are validated as well.
     */
    private ValidatorActionHelper validateLong (String name, Configuration constraints,
            Configuration conf, Map params, boolean is_string) {
        Object param = params.get (name);
        boolean nullable = getNullable (conf, constraints);
        Long value = null;
        Long dflt = null;
        {
            String tmp = getDefault (conf, constraints);
            if ( tmp != null ) dflt = Long.decode(tmp);
        }

        getLogger().debug ("VALIDATOR: validating long parameter "
                + name + " (encoded in a string: " + is_string + ")");
        value = getLongValue (name, conf, param, is_string);
        if (value == null) {
            getLogger().debug ("VALIDATOR: long parameter "
                    + name + " is null");
            if (!nullable) {
                return new ValidatorActionHelper ( value, 
                        ValidatorActionResult.ISNULL);
            } else {
                value = dflt;
                return new ValidatorActionHelper (value);
            }
        }
        if (constraints != null) {
            Long eq = getAttributeAsLong (constraints, "equals-to", null);
            String eqp = constraints.getAttribute ("equals-to-param", "");

            Long min = getAttributeAsLong (conf, "min", null);
            min = getAttributeAsLong ( constraints, "min", min);

            Long max = getAttributeAsLong (conf, "max",null);
            max = getAttributeAsLong (constraints, "max", max);

            // Validate whether param is equal to constant
            if (eq != null) {
                getLogger().debug ("VALIDATOR: long parameter "
                        + name + " should be equal to " + eq);

                if (!value.equals(eq)) {
                    getLogger().debug ("VALIDATOR: and it is not");
                    return new ValidatorActionHelper ( value, ValidatorActionResult.NOMATCH);
                }
            }

            // Validate whether param is equal to another param
            // FIXME: take default value of param being compared with into
            // account?
            if (!"".equals (eqp)) {
                getLogger().debug ("VALIDATOR: long parameter "
                        + name + " should be equal to " + params.get (eqp));
                // Request parameter is stored as string. 
                // Need to convert it beforehand.
                try {
                    Long _eqp = new Long ( Long.parseLong((String) params.get(eqp)) );
                    if (!value.equals (_eqp)) {
                        getLogger().debug ("VALIDATOR: and it is not");
                        return new ValidatorActionHelper ( value, ValidatorActionResult.NOMATCH);
                    }
                } catch ( NumberFormatException nfe ) {
                    getLogger().debug("VALIDATOR: long parameter "+ name +": "+eqp+" is no long", nfe);
                    return new ValidatorActionHelper ( value, ValidatorActionResult.NOMATCH);
                }
            }

            // Validate wheter param is at least min
            if (min != null) {
                getLogger().debug ("VALIDATOR: long parameter "
                        + name + " should be at least " + min);

                if (min.compareTo(value)>0) {
                    getLogger().debug ("VALIDATOR: and it is not");
                    return new ValidatorActionHelper ( value, ValidatorActionResult.TOOSMALL);
                }
            }

            // Validate wheter param is at most max
            if (max != null) {
                getLogger().debug ("VALIDATOR: long parameter "
                        + name + " should be at most " + max);
                if (max.compareTo(value)<0) {
                    getLogger().debug ("VALIDATOR: and it is not");
                    return new ValidatorActionHelper ( value, ValidatorActionResult.TOOLARGE);
                }
            }
        }
        return new ValidatorActionHelper(value);
    }

    /**
     * Validates nullability and default value for given parameter. If given
     * constraints are not null they are validated as well.
     */
    private ValidatorActionHelper validateDouble (String name, Configuration constraints,
            Configuration conf, Map params, boolean is_string) {
        Object param = params.get (name);
        boolean nullable = getNullable (conf, constraints);
        Double value = null;
        Double dflt = null;
        { 
            String tmp = getDefault (conf, constraints); 
            if ( tmp!=null ) dflt = Double.valueOf(tmp);
        }

        getLogger().debug ("VALIDATOR: validating double parameter "
                + name + " (encoded in a string: " + is_string + ")");
        value = getDoubleValue (name, conf, param, is_string);
        if (value == null) {
            getLogger().debug ("VALIDATOR: double parameter "
                    + name + " is null");
            if (!nullable) {
                return new ValidatorActionHelper ( value, 
                        ValidatorActionResult.ISNULL);
            } else {
                value = dflt;
                return new ValidatorActionHelper (value);
            }
        }
        if (constraints != null) {
            Double eq = getAttributeAsDouble (constraints, "equals-to", null);
            String eqp = constraints.getAttribute ("equals-to-param", "");

            Double min = getAttributeAsDouble (conf, "min", null);
            min = getAttributeAsDouble ( constraints, "min", min);

            Double max = getAttributeAsDouble (conf, "max", null);
            max = getAttributeAsDouble (constraints, "max", max);

            // Validate whether param is equal to constant
            if (eq != null) {
                getLogger().debug ("VALIDATOR: Double parameter "
                        + name + " should be equal to " + eq);

                if (!value.equals (eq)) {
                    getLogger().debug ("VALIDATOR: and it is not");
                    return new ValidatorActionHelper ( value, ValidatorActionResult.NOMATCH);
                }
            }

            // Validate whether param is equal to another param
            // FIXME: take default value of param being compared with into
            // account?
            if (!"".equals (eqp)) {
                getLogger().debug ("VALIDATOR: Double parameter "
                        + name + " should be equal to " + params.get (eqp));
                // Request parameter is stored as string. 
                // Need to convert it beforehand.
                try {
                    Double _eqp = new Double ( Double.parseDouble((String) params.get(eqp)));
                    if (!value.equals (_eqp)) {
                        getLogger().debug ("VALIDATOR: and it is not");
                        return new ValidatorActionHelper ( value, ValidatorActionResult.NOMATCH);
                    }
                } catch ( NumberFormatException nfe ) {
                    getLogger().debug("VALIDATOR: Double parameter "+ name +": "+eqp+" is no double", nfe);
                    return new ValidatorActionHelper ( value, ValidatorActionResult.NOMATCH);
                }
            }

            // Validate wheter param is at least min
            if (min != null) {
                getLogger().debug ("VALIDATOR: Double parameter "
                        + name + " should be at least " + min);
                if (0>value.compareTo(min)) {
                    getLogger().debug ("VALIDATOR: and it is not");
                    return new ValidatorActionHelper ( value, ValidatorActionResult.TOOSMALL);
                }
            }

            // Validate wheter param is at most max
            if (max != null) {
                getLogger().debug ("VALIDATOR: Double parameter "
                        + name + " should be at most " + max);
                if (0<value.compareTo(max)) {
                    getLogger().debug ("VALIDATOR: and it is not");
                    return new ValidatorActionHelper ( value, ValidatorActionResult.TOOLARGE);
                }
            }
        }
        return new ValidatorActionHelper(value);
    }

    /**
     * Returns the parsed Double value.
     *
     * FIXME: is the name parameter needed? It is not used
     */
    private Double getDoubleValue (String name,
            Configuration conf, Object param, boolean is_string) {
        Double value = null;

        /* convert param to double */
        if (is_string) {
            String tmp = (String)param;
            if (tmp != null && "".equals (tmp.trim ())) {
                tmp = null;
            }
            try {
                value = Double.valueOf (tmp);
            } catch (Exception e) {
                value = null;
            }
        } else {
            try {
                value = (Double)param;
            } catch (Exception e) {
                value = null;
            }
        }
        return value;
    }

    /**
     * Returns the parsed Double value.
     *
     * FIXME:
     * Is the name parameter needed? it is not used.  */
    private Long getLongValue (String name,
            Configuration conf, Object param, boolean is_string) {
        Long value = null;

        /* convert param to long */
        if (is_string) {
            String tmp = (String)param;
            if (tmp != null && "".equals (tmp.trim ())) {
                tmp = null;
            }
            try {
                value = Long.decode (tmp);
            } catch (Exception e) {
                value = null;
            }
        } else {
            try {
                value = (Long)param;
            } catch (Exception e) {
                value = null;
            }
        }
        return value;
    }

    /**
     * Checks whether param is nullable, and returns it, otherwise it
     * returns the parsed Double value. 
     *
     * FIXME:
     * Is the "is_string" parameter really needed?
     * Is the "name" parameter really needed?
     * Neither are used */
    private String getStringValue (String name,
            Configuration conf, Object param, boolean is_string) {
        String value = null;

        /* convert param to string */
        try {
            value = (String)param;
            if (value != null && "".equals (value.trim ())) {
                value = null;
            }
        } catch (Exception e) {
            value = null;
        }
        return value;
    }

    /**
     * Returns the value of 'nullable' attribute from given configuration or
     * from given constraints, value present in constrints takes precedence,
     * false when attribute is not present in either of them.
     */
    private boolean getNullable (Configuration conf, Configuration cons) {
        /* check nullability */
        try {
            String tmp = cons.getAttribute ("nullable", "no");
            if ("yes".equals (tmp) || "true".equals (tmp)) {
                return true;
            }
        } catch (Exception e) {
            try {
                String tmp = conf.getAttribute ("nullable", "no");
                if ("yes".equals (tmp) || "true".equals (tmp)) {
                    return true;
                }
            } catch (Exception e1) {
            }
        }
        return false;
    }

    /**
     * Returns the default value from given configuration or constraints.
     * Value present in constraints takes precedence, null is returned when no
     * default attribute is present in eiher of them.
     */
    private String getDefault (Configuration conf, Configuration cons) {
        String dflt = null;
        try {
            dflt = cons.getAttribute ("default", "");
            if ("".equals (dflt.trim ())) {
                return null;
            }
        } catch (Exception e) {
            try {
                dflt = conf.getAttribute ("default", "");
                if ("".equals (dflt.trim ())) {
                    return null;
                }
            } catch (Exception e1) {
                return null;
            }
        }
        return dflt;
    }

    /**
     * Replacement for Avalon's Configuration.getAttributeAsLong
     * because that one doesn't take <code>Long</code> but long and
     * thus won't take <code>null</code> as parameter value for
     * default.
     *
     * @param conf Configuration
     * @param name Parameter's name
     * @param dflt Default value
     * @return Parameter's value in <code>configuration</code> or
     * <code>dflt</code> if parameter is not set or couldn't be
     * converted to a <code>Long</code>
     * @see org.apache.avalon.Configuration.getParameterAsLong */

    private Long getAttributeAsLong (Configuration conf, String name, Long dflt) {
        Long value = null;
        try {
            value = new Long(conf.getAttributeAsLong(name));
        } catch (ConfigurationException e) {
            value = dflt;
        }
        return value;
    }

    /**
     * Addition to Avalon's Configuration.getAttributeAsFloat
     * because that one does only deal with <code>float</code>.
     *
     * @param conf Configuration
     * @param name Parameter's name
     * @param dflt Default value
     * @return Parameter's value in <code>configuration</code> or
     * <code>dflt</code> if parameter is not set or couldn't be
     * converted to a <code>Double</code>
     * @see org.apache.avalon.Configuration.getParameterAsFloat
     */

    private Double getAttributeAsDouble (Configuration conf, String name, Double dflt) {
        Double value = null;
        String tmp = null;
        try {
            tmp = conf.getAttribute(name);
            try {
                value = new Double ( Double.parseDouble(tmp) );
            } catch ( NumberFormatException nfe ) {
                value = null;
            }
        } catch (ConfigurationException e) {
            value = dflt;
        }
        return value;
    }

}

// $Id: AbstractValidatorAction.java,v 1.1 2002/01/03 12:31:07 giacomo Exp $
// vim: set et ts=4 sw=4:
