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
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import org.apache.cocoon.environment.Request;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.thread.ThreadSafe;
import java.lang.Integer;
import java.util.Map;
import java.util.HashMap;
import java.util.SortedSet;

/**
 * Abstraction layer to encapsulate different DBMS behaviour for
 * autoincrement columns.
 *
 * Here: manual mode The new value is determined by doing a "select
 * max(column)+1 from table" query. With transactions and correct
 * isolation levels, this should to the trick almost everywhere.
 *
 * Note however, that the above query does not prevent a parallel
 * transaction to try to insert a row with the same ID since it
 * requires only shared locks. C.f. "Phantom Problem"
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Id: ManualAutoIncrementHelper.java,v 1.1 2002/01/03 12:31:36 giacomo Exp $
 */
public class ManualAutoIncrementHelper extends AbstractAutoIncrementHelper implements ThreadSafe {

    private Map selectStatements = new HashMap();
    

    public Object getPostValue( Configuration tableConf, Configuration columnConf, Configuration modenConf,
                                Connection conn, Statement stmt, Request request ) 
        throws SQLException, ConfigurationException {

        return null;
    }
    
    public boolean includeInQuery( ) { return true; }

   
    public boolean includeAsValue( ) { return true; }


    public Object getPreValue( Configuration tableConf, Configuration columnConf, Configuration modeConf,
                               Connection conn, Request request ) 
        throws SQLException, ConfigurationException {

        /** Set the key value using SELECT MAX(keyname)+1 **/
        String tableName = tableConf.getAttribute("name","");
        String selectQuery = this.getSelectQuery(tableName, columnConf);
        PreparedStatement select_statement = conn.prepareStatement(selectQuery);
        ResultSet set = select_statement.executeQuery();
        set.next();
        int maxid = set.getInt("maxid");
        set.close();
        select_statement.close();
        if (getLogger().isDebugEnabled())
            getLogger().debug("autoincrementValue " + (maxid+1));
        return new Integer(maxid + 1);
    }


    public String getSubquery( Configuration tableConf, Configuration columnConf, Configuration modeConf ) 
        throws ConfigurationException {
        
        return null;
    }


    /**
     * Set the String representation of the MaxID lookup statement.  This is
     * mapped to the Configuration object itself, so if it doesn't exist,
     * it will be created.
     */
    protected final synchronized void setSelectQuery( String tableName, Configuration entry ) throws ConfigurationException {

        StringBuffer queryBuffer = new StringBuffer("SELECT max(");
        queryBuffer.append(entry.getAttribute("name"));
        queryBuffer.append(") AS maxid FROM ");
        queryBuffer.append(tableName);
        
        this.selectStatements.put(entry, queryBuffer.toString());
    }
    

    protected final synchronized String getSelectQuery( String tableName, Configuration entry ) throws ConfigurationException {

        String result = (String) this.selectStatements.get(entry);
        if (result == null) {
            setSelectQuery(tableName, entry);
            result = (String) this.selectStatements.get(entry);
        }
        return result;
    }

}
