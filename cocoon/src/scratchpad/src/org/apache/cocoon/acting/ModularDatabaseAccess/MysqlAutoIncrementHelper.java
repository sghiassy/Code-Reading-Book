/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting.ModularDatabaseAccess;

import java.lang.Integer;
import java.sql.PreparedStatement;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.sql.SQLException;
import org.apache.cocoon.environment.Request;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.thread.ThreadSafe;

/**
 * Abstraction layer to encapsulate different DBMS behaviour for autoincrement columns.
 *
 * Here: MYSQL AUTO_INCREMENT columns
 * {@link http://www.mysql.com}
 *
 * @author <a href="mailto:phantom@stserv.hcf.jhu.edu">Tim Myers</a>
 * @version CVS $Id: MysqlAutoIncrementHelper.java,v 1.1 2002/01/03 12:31:36 giacomo Exp $
 */
public class MysqlAutoIncrementHelper implements AutoIncrementHelper, ThreadSafe {
    
    public Object getPostValue( Configuration tableConf, Configuration columnConf, Configuration modeConf, 
                                Connection conn, Statement stmt, Request request )  throws SQLException, ConfigurationException {

        Integer id = null;
        /*
          // if mysql did support callable statements ...  i'm not sure what what would go here, maybe:

          CallableStatement callStmt = conn.prepareCall("? = {CALL LAST_INSERT_ID()}");
          callStmt.registerOutParameter(1, Types.INTEGER);
          ResultSet resultSet = callStmt.executeQuery();
        */
        
        PreparedStatement pstmt = conn.prepareStatement("SELECT LAST_INSERT_ID()");
        ResultSet resultSet = pstmt.executeQuery();
        while ( resultSet.next() ) {
            id = new Integer(resultSet.getInt(1));
        }
        resultSet.close();
        
        return id;
    }
    

    public boolean includeInQuery() { return false; }


    public boolean includeAsValue() { return false; }


    public Object getPreValue( Configuration tableConf, Configuration columnConf, Configuration modeConf, 
                               Connection conn, Request request ) throws SQLException, ConfigurationException {

        return null;
    }


    public String getSubquery( Configuration tableConf, Configuration columnConf, Configuration modeConf ) 
        throws ConfigurationException {

        return null;
    }
}

