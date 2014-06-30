/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting.ModularDatabaseAccess;

import java.lang.Integer;
import java.util.SortedSet;
import java.sql.Connection;
import java.sql.Statement;
import java.sql.SQLException;
import org.apache.cocoon.environment.Request;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.thread.ThreadSafe;
import com.informix.jdbc.IfxStatement;

/**
 * Abstraction layer to encapsulate different DBMS behaviour for autoincrement columns.
 *
 * Here: Informix IUS 9.21 SERIAL columns
 * (need another one for SERIAL8 ones!)
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Id: IfxSerialAutoIncrementHelper.java,v 1.1 2002/01/03 12:31:36 giacomo Exp $
 */
public class IfxSerialAutoIncrementHelper implements AutoIncrementHelper, ThreadSafe {
    
    public Object getPostValue( Configuration tableConf, Configuration columnConf, Configuration modeConf, 
                                Connection conn, Statement stmt, Request request ) 
        throws SQLException, ConfigurationException {

        return new Integer(((com.informix.jdbc.IfxStatement) stmt).getSerial());
    };
    

    public boolean includeInQuery() { return false; };


    public boolean includeAsValue() { return false; };


    public Object getPreValue( Configuration tableConf, Configuration columnConf, Configuration modeConf, 
                               Connection conn, Request request ) throws SQLException, ConfigurationException {

        return null;
    };

    public String getSubquery( Configuration tableConf, Configuration columnConf, Configuration modeConf ) 
        throws ConfigurationException {

        return null;
    };

}
