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
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Session;
import org.apache.cocoon.environment.SourceResolver;

import java.util.HashMap;
import java.util.Map;

/**
 * Store the session's current state in a session attribute.
 *
 * <p> To keep track of the state of a user's session, a string is
 * stored in a session attribute in order to allow to chose between
 * different pipelines in the sitemap accordingly.</p>
 *
 * <p> For added flexibility it is possible to use sub states as
 * well. For this declare your own name for the session state
 * attribute and give the number of sublevels plus the level to
 * modify. (This is <b>one</b> based!) Sub states below the current
 * one are removed from the session so that the default sub state will
 * be reentered when the user returns. If you don't like this
 * behaviour and prefer independent sub states, use this action
 * several times with different attribute names rather than sub
 * levels. </p>
 *
 * <p><b>Global and local paramters:</b></p> 
 *
 * <table border="1"> 
 *   <tr>
 *     <td><code>state-key-prefix</code></td>
 *     <td>String that identifies the attribute that stores the session state in the
 *     	 session object. When sublevels are used, this is a prefix ie. the
 *     	 number of the level is appended to the prefix. Example prefix is
 *     	 "<code>__sessionState</code>", sub-levels is 2, attributes
 *     	 "<code>__sessionState1</code>", "<code>__sessionState2</code>", and
 *     	 "<code>__sessionState3</code>" will be used to store the
 *     	 information.
 *     </td>
 *   </tr> 
 *   <tr>
 *     <td><code>new-state</code></td>
 *     <td>String that identifies the current state</td>
 *   </tr>
 *   <tr>
 *     <td><code>sub-levels</code></td>
 *     <td>Number of sub levels to  use</td>
 *   </tr> 
 *   <tr>
 *     <td><code>state-level</code></td>
 *     <td>Sub level to modify, this is <b>one</b> based</td>
 *   </tr> 
 * </table>
 *
 * @see org.apache.cocoon.matching.WildcardSessionAttributeMatcherFactory
 * @see org.apache.cocoon.selection.SessionAttributeSelectorFactory
 *
 * @author <a href="mailto:haul@informatik.tu-darmstadt.de">Christian Haul</a>
 * @version CVS $Id: SessionStateAction.java,v 1.1 2002/01/03 12:31:07 giacomo Exp $ */
public class SessionStateAction extends AbstractConfigurableAction implements ThreadSafe {

    protected String statekey = "org.apache.cocoon.SessionState";
    protected String newstate = null;
    protected int sublevels = 0;
    protected int mylevel = 0;

    /**
     * Configures the Action.
     */
    public void configure(Configuration conf) throws ConfigurationException 
    {
	super.configure(conf);

	if (settings.containsKey("state-key-prefix"))
	    statekey = (String) settings.get("state-key-prefix");
	if (settings.containsKey("new-state"))
	    newstate = (String) settings.get("new-state");
	if (settings.containsKey("sub-levels"))
	    sublevels = Integer.parseInt((String) settings.get("sub-levels"));
	if (settings.containsKey("state-level"))
	    mylevel = Integer.parseInt((String) settings.get("state-level"));
    }


    public Map 	act (Redirector redirector, 
		     SourceResolver resolver, 
		     Map objectModel, 
		     String src, 
		     Parameters par) throws Exception 
    {
        Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);

	// read local settings
	newstate = par.getParameter("new-state", newstate);
	statekey = par.getParameter("state-key", statekey);
	sublevels = par.getParameterAsInteger("sublevels", sublevels);
	mylevel = par.getParameterAsInteger("state-level", mylevel);

	if (newstate == null ) {
	    getLogger().error("new-state is null");
	    return null;
	}


        if (request != null) {
           Session session = request.getSession (false);
	    
            if (session != null && request.isRequestedSessionIdValid()) {
		String oldstate = null;
		if (sublevels == 0) {
		    oldstate = (String) session.getAttribute(statekey);
		    session.setAttribute(statekey, newstate);
		    getLogger().debug(statekey+"="+newstate);

		} else { // sublevels != 0
		    oldstate = (String) session.getAttribute( statekey + Integer.toString(mylevel) );
		    for (int i=mylevel+1; i<=sublevels; i++) {
			session.removeAttribute( statekey+Integer.toString(i) );
			getLogger().debug("remove "+statekey+Integer.toString(i));
		    }
		    session.setAttribute( statekey + Integer.toString(mylevel), newstate);
		    getLogger().debug(statekey+Integer.toString(mylevel)+"="+newstate);
		}
		getLogger().debug("transition " + oldstate + " -> " + newstate);
		
		HashMap map = new HashMap(1);
		map.put("newstate", newstate);
		return map;
		
	    } else {
                getLogger().warn("A session object was not present or no longer valid");
		return null;
            }
        } else {
	    getLogger().warn("No request object");
	    return null;
	}
	
    }
}



