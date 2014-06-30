/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting.ModularDatabaseAccess;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.SQLException;
import org.apache.cocoon.environment.Request;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

/**
 * Abstraction layer to encapsulate different DBMS behaviour for key
 * attribute columns.
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Id: AutoIncrementHelper.java,v 1.1 2002/01/03 12:31:36 giacomo Exp $ 
 * */
public interface AutoIncrementHelper extends Component {

    String ROLE = "org.apache.cocoon.acting.ModularDatabaseAccess.AutoIncrementHelper";

    
    /**
     * Return key attribute value of last inserted row.
     *
     * @param name a String that specifies what the caller thinks
     * would identify a set of parameters. This is mainly a fallback
     * if no modeConf is present.

     * @param tableConf Table's configuration from resource description.
     * @param columnConf column's configuration from resource description.
     * @param mdoeConf this mode's configuration from resource description.
     * @param conn Connection
     * @param stmt Statement that was executed to insert the last row.
     * @param request The request object
     * @return value representing the last key value value.
     * */
    Object getPostValue( Configuration tableConf, Configuration columnConf, Configuration modeConf, 
                         Connection conn, Statement stmt, Request request ) throws SQLException, ConfigurationException;

    
    /**
     * Boolean whether the key attribute column needs to be included
     * in the insert query.
     *
     * @return true if the column is needed, false if the column
     * should be skipped.  
     * */
    boolean includeInQuery( );


    /**
     * Boolean whether the key attribute needs to be included in the
     * insert query as an attribute value (no subquery).
     *
     * @return true if a value is needed, false if a subquery
     * expression is used or the column is skipped altogether.
     * */
    boolean includeAsValue( );


    /**
     * Provide the value for the key attribute column.
     *
     * If a value for the key value column is needed (i.e. the column
     * is not skipped), this value is computed here.
     *
     * @param tableConf Table's configuration from resource description.
     * @param columnConf column's configuration from resource description.
     * @param mdoeConf this mode's configuration from resource description.
     * @param conn Connection
     * @param request The request object
     * @param idx In case of multiple rows to be inserted, index to the desired row
     * @return exact value for key attribute column 
     * */
    Object getPreValue( Configuration tableConf, Configuration columnConf, Configuration modeConf, 
                        Connection conn, Request request ) throws SQLException, ConfigurationException;


    /**
     * Provide subquery string for the key attribute column.
     *
     * If a value for the autoincrement column is needed (i.e. the
     * column is not skipped), and the value can be determined through
     * a nested subquery, this function provides the subquery as a
     * string.
     *
     * @return subquery string for autoincrement column.
     */
    String getSubquery( Configuration tableConf, Configuration columnConf, Configuration modeConf ) throws ConfigurationException;

}
