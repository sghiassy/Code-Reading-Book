/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting.ModularDatabaseAccess;

import java.util.Enumeration;
import java.util.List;
import java.util.LinkedList;
import org.apache.cocoon.environment.Request;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.thread.ThreadSafe;

/**
 * RequestParameterHelper accesses request parameters. If the
 * parameter name contains an askerisk "*" this is considered a
 * wildcard and all parameters that would match this wildcard are
 * considered to be part of an array of that name for
 * getAttributeValues. Only one "*" is allowed. Wildcard matches take
 * precedence over real arrays. In that case only the first value of
 * such array is returned.
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Id: RequestParameterHelper.java,v 1.1 2002/01/03 12:31:36 giacomo Exp $ */
public class RequestParameterHelper extends AbstractAttributeHelper implements ThreadSafe {

    public Object getAttribute( String name, Configuration modeConf, Request request ) throws ConfigurationException {

        String pname = name;
        if ( modeConf != null ) {
            pname = modeConf.getAttribute( "parameter", pname );
        }
        return request.getParameter( pname );
    }


    public Enumeration getAttributeNames( Configuration modeConf, Request request ) throws ConfigurationException {

        return request.getParameterNames();
    }


    public Object[] getAttributeValues( String name, Configuration modeConf, Request request ) 
        throws ConfigurationException {
        
        String wildcard = name;
        if ( modeConf != null ) {
            wildcard = modeConf.getAttribute( "parameter", wildcard );
        }
        int wildcardIndex = wildcard.indexOf( "*" );
        if ( wildcardIndex != -1 ) {
            // "*" contained in parameter name => combine all
            // parameters' values that match prefix, suffix
            
            // split the parameter's name so that the "*" could be
            // determined by looking at the parameters' names that
            // start with the prefix and end with the suffix
            //
            String prefix = wildcard.substring( 0, wildcardIndex );
            String suffix;
            if ( wildcard.length() >= wildcardIndex + 1 ) {
                suffix = wildcard.substring( wildcardIndex + 1 );
            } else {
                suffix = "";
            }
            List values = new LinkedList();
            Enumeration names = request.getParameterNames();
            
            while (names.hasMoreElements()) {
                String pname = (String) names.nextElement();
                if ( pname.startsWith( prefix ) && pname.endsWith( suffix ) ) {
                    values.add( request.getParameter( pname ) );
                }
            }
            
            return values.toArray();
            
        } else {
            // no "*" in parameter name => just return all values of
            // this one parameter.
            
            return request.getParameterValues( wildcard );
            
        }
        
    }
    
}
