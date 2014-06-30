/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

import java.util.Collections;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.Iterator;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.SourceResolver;

/**
 * This action simply checks to see if a given request parameter
 * exists. It takes an arbitrary number of default parameters to check
 * named 'parameter-name'. Non-default parameters need to be separated
 * by spaces and passed as value of a sitemap parameter named
 * 'parameters'. The action returns a map with all parameters if all
 * of them exist and null otherwise. Parameter names can only be added
 * to this list but no default parameters can be overridden by
 * specific ones.
 *
 * <p>This action is very closely related to @link{RequestParamAction}
 * and @link{FormValidatorAction}. However this action is considerably
 * simpler in that it tests only for existence of a parameter and it
 * doesn't need a descriptor. Besides it doesn't propagate all request
 * parameters to the sitemap but only those that are marked as
 * required.</p> <p> One special feature is, however, that parameters
 * can contain <strong>one</strong> wildcard ("*"). It will be
 * checked, whether all parameters with a wildcard have the same
 * matches. E.g. "id_* name_*" enforces, that if "id_1" exists,
 * "name_1" must also exist and vice versa.</p>
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $ */
public class RequestParameterExistsAction extends AbstractConfigurableAction 
{

    protected class StringParts {
	String prefix = null;
	String pstfix = null;
	int count = 0;

	public StringParts ( String pre, String post ) {
	    prefix = pre;
	    pstfix = post;
	}
    }
				 


    public Map act( Redirector redirector, 
		    SourceResolver resolver, 
		    Map objectModel, 
		    String source, 
		    Parameters parameters
		    ) 
	throws Exception 
    {
        Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
        if (request == null) {
          getLogger().error("RequestInfoAction: no request object!");
          return(null);
        }
        HashMap results = new HashMap();
	HashMap items = new HashMap();
	int wildcards = 0;

	// check default parameters for existence
	getLogger().debug("checking default parameters");
	Iterator reqParams = settings.values().iterator();
	while ( reqParams.hasNext() ) {
	    String paramName = (String) reqParams.next();
	    StringParts sp = splitParameter( paramName );
	    if ( sp != null ) {
		// wildcard: special care required (deferred until later)
		items.put( new Integer( wildcards++ ), sp );
		getLogger().debug("(default) deferring "+paramName);
	    } else {
		String paramValue = request.getParameter( paramName );
		if ( paramValue == null ) {
		    return null;
		}
		results.put( paramName, paramValue );
	    }
	}

	// check parameters for existence
	getLogger().debug("checking sitemap parameters");
	String params = parameters.getParameter( "parameters", null) ;
	if ( params != null ) {
	    StringTokenizer st = new StringTokenizer( params );
	    while ( st.hasMoreTokens() ) {
		String paramName = st.nextToken();
		StringParts sp = splitParameter( paramName );
		if ( sp != null ) {
		    // wildcard: special care required (deferred until later)
		    items.put(new Integer(wildcards++), sp );
		    getLogger().debug("deferring "+paramName);
		} else {

		    String paramValue = request.getParameter( paramName );
		    if ( paramValue == null ) {
			return null;
		    }
		    results.put( paramName, paramValue );
		}
	    }
	}
	
	if ( wildcards != 0 ) {
	    // special care for parameters with wildcard
	    //
	    getLogger().debug("deferred checking for parameters: "+wildcards);

	    // first one
	    //
	    getLogger().debug(" checking first");
	    HashMap values = new HashMap();
	    StringParts sp1 = (StringParts) items.get( new Integer( 0 ) );
	    getLogger().debug("  Parameter is ["+sp1.prefix+" * "+sp1.pstfix+"] ");
	    Enumeration requestParams = request.getParameterNames();
	    Boolean dummy = new Boolean( true );
	    while ( requestParams.hasMoreElements() ) {
		String paramName = (String) requestParams.nextElement();
		String match = getMatch( paramName, sp1 );
		if ( match != null ) {
		    getLogger().debug("  value is >"+match+"< "+ sp1.prefix.length()+" "+ paramName.length()+ " " +sp1.pstfix.length());
		    values.put( match, dummy );
		    sp1.count++;
		    getLogger().debug("   Parameter "+sp1.prefix+"*"+sp1.pstfix+" matches "+paramName+" ("+sp1.count+" so far)");

		    String paramValue = request.getParameter( paramName );
		    if ( paramValue == null ) {
			return null;
		    }
		    results.put( paramName, paramValue );
		}
	    }

	    if ( sp1.count == 0 ) {
		getLogger().debug("   Parameter "+sp1.prefix+"*"+sp1.pstfix+" matches "+sp1.count);
		return null;
	    }

	    
	    // all other ones
	    //
	    getLogger().debug(" checking others");
	    requestParams = request.getParameterNames();
	    while ( requestParams.hasMoreElements() ) {
		String paramName = (String) requestParams.nextElement();
		getLogger().debug("  checking request parameter "+paramName);
		for ( int i = wildcards-1; i > 0; i-- ) {
		    getLogger().debug("   checking against "+i);
		    StringParts sp = (StringParts) items.get( new Integer( i ) );
		    String match = getMatch( paramName, sp );
		    getLogger().debug("   Parameter is ["+sp.prefix+" * "+sp.pstfix+"] ");
		    if ( match != null ) {
			getLogger().debug("   Parameter "+sp.prefix+"*"+sp.pstfix+" matches "+paramName+" ("+sp.count+" so far)");
			if ( values.containsKey( match ) ) {
			    sp.count++;
			    getLogger().debug("   "+paramName+" (verified)");
			    
			    String paramValue = request.getParameter( paramName );
			    if ( paramValue == null ) {
				return null;
			    }
			    results.put( paramName, paramValue );

			} else {
			    getLogger().debug("Match " + match + "not found for "
					      + sp1.prefix + "*" + sp1.pstfix + " but for "
					      + sp.prefix + "*" + sp.pstfix);
			    return null;
			}
		    }
		}
	    }

	    // since we enforce that only matches are counted, that exist for
	    // the first parameter as well, check if every parameter has an 
	    // equal number of matches.
	    //
	    getLogger().debug("checking number of matches");
	    for ( int i = wildcards-1; i > 0; i-- ) {
		StringParts sp = (StringParts) items.get( new Integer( i ) );
		if ( sp.count != sp1.count ) {
		    getLogger().debug("Found " + sp.count + " matches for "
				      + sp.prefix + "*" + sp.pstfix + " but expected "
				      + sp1.count);
		    return null;
		} else {
		    getLogger().debug("Found " + sp.count + " matches for "
				      + sp.prefix + "*" + sp.pstfix + " as expected");
		}
	    }
		
	}
	

        return Collections.unmodifiableMap(results);
    }


    /** 
     * Find first "*" in a String and split it into the substring
     * before and after the "*". Returns null if no "*" is present.
     */
    protected StringParts splitParameter( String paramName ) 
    {
	int idx = paramName.indexOf("*");
	if ( idx != -1 ) {
	    return new StringParts(paramName.substring(0,idx), paramName.substring(idx+1));
	} else {
	    return null;
	}
    }

    /** 
     * If a String matches a StringPart spec, return the substring
     * between the specified prefix and postfix. Returns null if it
     * doesn't match. 
     */
    protected String getMatch( String paramName,
			       StringParts sp
			       )
    {
	if ( paramName.startsWith( sp.prefix ) && paramName.endsWith( sp.pstfix ) ) {
	    return paramName.substring( sp.prefix.length(), ( paramName.length() - sp.pstfix.length() ) );
	} else {
	    return null;
	}
    }

}
