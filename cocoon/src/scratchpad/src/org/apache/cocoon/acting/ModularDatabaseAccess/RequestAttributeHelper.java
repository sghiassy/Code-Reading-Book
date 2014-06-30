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
 * RequestAttributeHelper accesses request attributes. If the
 * attribute name contains an askerisk "*" this is considered a
 * wildcard and all attributes that would match this wildcard are
 * considered to be part of an array of that name for
 * getAttributeValues. Only one "*" is allowed.
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Id: RequestAttributeHelper.java,v 1.1 2002/01/03 12:31:36 giacomo Exp $ */
public class RequestAttributeHelper extends AbstractAttributeHelper implements ThreadSafe {
    
    public Object getAttribute( String name, Configuration modeConf, Request request )
        throws ConfigurationException {

        String pname = name;
        if ( modeConf != null ) {
            pname = modeConf.getAttribute( "parameter", pname );
        }
        return request.getAttribute( pname );
    }


    public Enumeration getAttributeNames( Configuration modeConf, Request request )
        throws ConfigurationException {

        return request.getAttributeNames();
    }


    public Object[] getAttributeValues( String name, Configuration modeConf, Request request )
        throws ConfigurationException {

        String wildcard = name;
        if ( modeConf != null ) {
            wildcard = modeConf.getAttribute( "parameter", wildcard );
        }
        int wildcardIndex = wildcard.indexOf( "*" );
        if ( wildcardIndex != -1 ) {
            // "*" contained in attribute name => combine all
            // attributes' values that match prefix, suffix
            
            // split the attribute's name so that the "*" could be
            // determined by looking at the attributes' names that
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
            Enumeration names = request.getAttributeNames();
            
            while (names.hasMoreElements()) {
                String pname = (String) names.nextElement();
                if ( pname.startsWith( prefix ) && pname.endsWith( suffix ) ) {
                    values.add( request.getAttribute( pname ) );
                }
            }
            
            return values.toArray();

        } else {
            // no "*" in attribute name => just return all values of
            // this one attribute. Make sure, it's an array.
            
            Object value = request.getAttribute( wildcard );
            if ( value != null && !value.getClass().isArray() ) {
                Object[] values = new Object[1];
                values[0] = value;
                return values;
            } else {
                return (Object[]) value;
            }
            
        }
    
    }
    


}
