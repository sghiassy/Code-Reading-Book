/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting.ModularDatabaseAccess;

import java.util.SortedSet;
import java.util.Enumeration;

import org.apache.cocoon.environment.Request;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

/**
 * AttributeHelper specifies an interface for components that provide
 * access to individual attributes e.g. request parameters, request
 * attributes, session attributes &c.
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Id: AttributeHelper.java,v 1.1 2002/01/03 12:31:36 giacomo Exp $
 * */
public interface AttributeHelper extends Component {

    String ROLE = "org.apache.cocoon.acting.ModularDatabaseAccess.AttributeHelper";


    /** 
     * Standard access to an attribute's value. If more than one value
     * exists, the first is returned. If the value does not exist,
     * null is returned. To get all values, use {@link
     * getAttributeValues} or {@link getAttributeSet} and {@link
     * getValue} to get them one by one.
     * @param name a String that specifies what the caller thinks
     * would identify an attribute. This is mainly a fallback if no
     * modeConf is present.
     * @param modeConf column's mode configuration from resource
     * description. This argument is optional.
     * @param request the request object
     */
    Object getAttribute( String name, Configuration modeConf, Request request ) throws ConfigurationException;


    /**
     * Returns an Enumeration of String objects containing the names
     * of the attributes available. If no attributes are available,
     * the method returns an empty Enumeration.
     * @param modeConf column's mode configuration from resource
     * description. This argument is optional.
     * @param request the request object
     */
    Enumeration getAttributeNames( Configuration modeConf, Request request ) throws ConfigurationException;


    /**
     * Returns an array of String objects containing all of the values
     * the given attribute has, or null if the attribute does not
     * exist. As an alternative, {@link getAttributeSet} together with
     * {@link getValue} can be used to get the values one by one.
     * @param name a String that specifies what the caller thinks
     * would identify an attributes. This is mainly a fallback
     * if no modeConf is present.
     * @param modeConf column's mode configuration from resource
     * description. This argument is optional.
     * @param request the request object
     */
    Object[] getAttributeValues( String name, Configuration modeConf, Request request ) throws ConfigurationException;

}
