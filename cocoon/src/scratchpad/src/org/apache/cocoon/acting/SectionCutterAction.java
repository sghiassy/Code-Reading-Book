/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.acting.ConfigurableComposerAction;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Session;
import org.apache.cocoon.environment.SourceResolver;

import java.util.Collections;
import java.util.Dictionary;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Map;
import java.util.Vector;

/**
 *  An action designed to set any number of variables, based on the current site
 *  section. The action matches the request uri against a configurable set of
 *  regular expressions (note: currently not implemented. Checking the beggining
 *  of the URI). When an expression matches, the action will set the configured
 *  variable in the Map.
 *
 * @author     <a href="mailto:sergio.carvalho@acm.org?Subject=SectionCutterAction">
 *      Sergio Carvalho</a>
 * @version    CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:35 $
 */
public class SectionCutterAction extends ConfigurableComposerAction implements ThreadSafe {

    Vector sections = new Vector();

    /**
     *  Description of the Method
     *
     * @param  conf                        Description of Parameter
     * @exception  ConfigurationException  Description of Exception
     */
    public void configure(Configuration conf)
        throws ConfigurationException {
        try {
            Configuration[] sectionConfigurations;
            sectionConfigurations = conf.getChildren("section");

            for (int i = 0; i < sectionConfigurations.length; i++) {
                try {
                    if (getLogger().isDebugEnabled()) {
                        getLogger().debug("Creating one section");
                    }
                    sections.add(new Section(sectionConfigurations[i]));
                } catch (Exception e) {
                    getLogger().error("Failed configuring section", e);
                    if (getLogger().isDebugEnabled()) {
                        // In production, try to continue. Assume that one rotten section config can't stop the whole app.
                        // When debug is enabled, scream, screech and grind to a halt.
                        throw (e);
                    }
                }
            }
        } catch (Exception e) {
            throw new ConfigurationException("Cannot configure action", e);
        }
    }


    /**
     *  A simple Action that logs if the <code>Session</code> object has been
     *  created
     *
     * @param  redirector     Description of Parameter
     * @param  resolver       Description of Parameter
     * @param  objectModel    Description of Parameter
     * @param  src            Description of Parameter
     * @param  par            Description of Parameter
     * @return                Description of the Returned Value
     * @exception  Exception  Description of Exception
     */
    public Map act(Redirector redirector, SourceResolver resolver, Map objectModel, String src, Parameters par)
        throws Exception {
        Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
        Map results = new HashMap();
        if (request != null) {
            boolean hasMatched = false;

            if (getLogger().isDebugEnabled()) {
                getLogger().debug("Matching against '" + request.getSitemapURI() + "'");
            }
            for (Enumeration sectionsEnum = sections.elements(); sectionsEnum.hasMoreElements() && !hasMatched; ) {
                Section section = (Section) sectionsEnum.nextElement();
                if (section.matches(request.getSitemapURI())) {
                    if (getLogger().isDebugEnabled()) {
                        getLogger().debug("Matched '" + section.matchExpression + "'");
                    }

                    section.fillMap(results);
                    hasMatched = true;
                }
            }
        } else {
            getLogger().warn("Request was null");
        }

        return Collections.unmodifiableMap(results);
    }


    /**
     *  Description of the Class
     *
     * @author     subzero
     * @version
     */
    class Section extends Object {
        String matchExpression;
        Dictionary mapVars = new Hashtable();


        /**
         *  Constructor for the Section object
         *
         * @param  conf           Description of Parameter
         * @exception  Exception  Description of Exception
         */
        public Section(Configuration conf)
            throws Exception {
            matchExpression = conf.getAttribute("pattern");

            Configuration[] variables;
            variables = conf.getChildren("set-var");

            for (int i = 0; i < variables.length; i++) {
                mapVars.put(variables[i].getAttribute("name"), variables[i].getAttribute("value"));
            }
        }


        /**
         *  Description of the Method
         *
         * @param  expression  Description of Parameter
         * @return             Description of the Returned Value
         */
        public boolean matches(String expression) {
            return expression.startsWith(matchExpression);
        }


        /**
         *  Description of the Method
         *
         * @param  map  Description of Parameter
         */
        public void fillMap(Map map) {
            for (Enumeration keys = mapVars.keys(); keys.hasMoreElements(); ) {
                Object key = keys.nextElement();
                Object value = mapVars.get(key);

                map.put(key, value);
            }
        }
    }
}



