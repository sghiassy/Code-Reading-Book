/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

/*
 * Package definition
 */
package org.apache.cocoon.acting;

/*
 * Standard imports
 */
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.SourceResolver;

import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;

/**
 * This action makes some request details available to the sitemap via parameter
 * substitution.
 *
 * {context}      - is the context path of the servlet (usually "/cocoon")
 * {requestURI}   - is the requested URI without parameters
 * {requestQuery} - is the query string like "?param1=test" if there is one
 *
 * Additionlly all request parameters can be made available for use in the sitemap.
 * if the parameter "parameters" is set to true.
 * (A variable is created for each request parameter (only if it doesn't exist)
 * with the same name as the parameter itself)
 *
 * Default values can be set for request parameters, by including sitemap parameters
 * named "default.<parameter-name>".
 *
 * Sitemap definition:
 *
 * <pre>
 * &lt;map:action name="request" src="org.apache.cocoon.acting.RequestParamAction"/&gt;
 * </pre>
 *
 * <p>
 *
 * Example use:
 *
 * <pre>
 * &lt;map:match pattern="some-resource"&gt;
 *  &lt;map:act type="request"&gt;
 *     &lt;map:parameter name="parameters" value="true"/&gt;
 *     &lt;map:parameter name="default.dest" value="invalid-destination.html"/&gt;
 *     &lt;map:redirect-to uri="{context}/somewhereelse/{dest}"/&gt;
 *  &lt;/map:act&gt;
 * &lt;/map:match&gt;
 * </pre>
 *
 * Redirection is only one example, another use:
 *
 * <pre>
 * &lt;map:match pattern="some-resource"&gt;
 *  &lt;map:act type="request"&gt;
 *     &lt;map:parameter name="parameters" value="true"/&gt;
 *     &lt;map:generate src="users/menu-{id}.xml"/&gt;
 *  &lt;/map:act&gt;
 *  &lt;map:transform src="menus/personalisation.xsl"/&gt;
 *  &lt;map:serialize/&gt;
 * &lt;/map:match&gt;
 * </pre>
 *
 * etc, etc.
 *
 * @author <a href="mailto:Marcus.Crafter@osa.de">Marcus Crafter</a>
 * @author <a href="mailto:tcurdt@dff.st">Torsten Curdt</a>
 * @version CVS $Revision: 1.1 $
 */
public class RequestParamAction extends ComposerAction implements ThreadSafe {

    public final static String MAP_URI         = "requestURI";
    public final static String MAP_QUERY       = "requestQuery";
    public final static String MAP_CONTEXTPATH = "context";
 
    public final static String PARAM_PARAMETERS = "parameters";
    public final static String PARAM_DEFAULT_PREFIX = "default.";

 
    public Map act( Redirector redirector, SourceResolver resolver, Map objectModel, String source, Parameters param )
        throws Exception
    {
 
        Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
        if (request == null) {
          getLogger().error("RequestInfoAction: no request object!");
          return(null);
        }
 
        Map map = new HashMap();
 
        map.put(MAP_URI, request.getRequestURI());
 
        String query = request.getQueryString();
        if (query != null && query.length() > 0){
          map.put(MAP_QUERY, "?" + query);
        }
        else{
          map.put(MAP_QUERY, "");
        }
 
        map.put(MAP_CONTEXTPATH, request.getContextPath());
 
        if ("true".equalsIgnoreCase(param.getParameter(PARAM_PARAMETERS, null))){
          Enumeration e = request.getParameterNames();
          while(e.hasMoreElements()){
            String name = (String) e.nextElement();
            String value = request.getParameter(name);

            if (value != null && !map.containsKey(name)){
              map.put(name, value);
            }
          }
	  
	  String[] paramNames = param.getNames();
          for (int i=0; i< paramNames.length; i++) {
	      if (paramNames[i].startsWith(PARAM_DEFAULT_PREFIX) &&
		  (request.getParameter(paramNames[i].substring(PARAM_DEFAULT_PREFIX.length())) == null)) {
		  map.put(paramNames[i].substring(PARAM_DEFAULT_PREFIX.length()), param.getParameter(paramNames[i]));
	      }
	  }


        }
 
        return(map);
    }
}
