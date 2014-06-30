/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

import java.util.Map;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import org.apache.cocoon.util.HashMap;

import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

import org.apache.cocoon.environment.Request;

/**
 * Updates a record in a database. The action can update one or more
 * tables, and can update more than one row to a table at a time. The
 * form descriptor semantics for this are still in a bit of a state of
 * flux. 
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:35 $
 */
public class ModularDatabaseUpdateAction extends ModularDatabaseAction {

    /**
     * determine which mode to use as default mode
     * here: UPDATE
     * highly specific to operation INSERT / UPDATE / DELETE / SELECT
     */
    protected String selectMode ( boolean isAutoIncrement, HashMap modes ) {

        return (String) modes.get( MODE_OTHERS );
    }


    /**
     * determine whether autoincrement columns should be honoured by
     * this operation. This is usually snsible only for INSERTs.
     */
    protected boolean honourAutoIncrement() { return false; }


    /**
     * Fetch all values for all columns that are needed to do the
     * database operation.
     */
    protected Object[][] getColumnValues( Configuration tableConf, CacheHelper queryData, Request request )
        throws ConfigurationException, ComponentException {

        Object[][] columnValues = new Object[ queryData.columns.length ][];
        for ( int i = 0; i < queryData.columns.length; i++ ){
            columnValues[i] = this.getColumnValue( tableConf, queryData.columns[i], request );
        }
        return columnValues;
    }


    /**
     * Get the String representation of the PreparedStatement.  This is
     * mapped to the Configuration object itself, so if it doesn't exist,
     * it will be created.
     *
     * @param table the table's configuration object
     * @return the insert query as a string
     */
    protected CacheHelper getQuery( Configuration table, HashMap modeTypes, HashMap defaultModeNames ) 
        throws ConfigurationException, ComponentException {

        LookUpKey lookUpKey = new LookUpKey( table, modeTypes );
        CacheHelper queryData = null;
        synchronized( this.cachedQueryData ) {
            queryData = (CacheHelper) this.cachedQueryData.get( lookUpKey );
            if (queryData == null) {
                Configuration[] keys = table.getChild("keys").getChildren("key");
                Configuration[] values = table.getChild("values").getChildren("value");
                
                queryData = new CacheHelper( keys.length, keys.length + values.length );
                fillModes( keys,   true,  defaultModeNames, modeTypes, queryData );
                fillModes( values, false, defaultModeNames, modeTypes, queryData );
                
                StringBuffer queryBuffer = new StringBuffer("UPDATE ");
                queryBuffer.append(table.getAttribute("name")).append(" SET ");
                
                
                int cols = 0;
                for (int i = 0; i < queryData.columns.length; i++) {
                    if ( !queryData.columns[i].isKey ) {
                        if ( cols > 0 ) {
                            queryBuffer.append(", ");
                        }
                        cols++;
                        queryBuffer
                            .append( queryData.columns[i].columnConf.getAttribute( "name" ) )
                            .append( "= ?" );
                    }
                }
                queryBuffer.append(" WHERE ");
                for (int i = 0; i < queryData.columns.length; i++) {
                    if ( queryData.columns[i].isKey ) {
                        if ( i > 0 ) {
                            queryBuffer.append(" AND ");
                        }
                        queryBuffer
                            .append( queryData.columns[i].columnConf.getAttribute( "name" ) )
                            .append( "= ?" );
                    }
                }
                
                queryData.queryString = queryBuffer.toString();
                
                this.cachedQueryData.put( lookUpKey, queryData );
            }
        }
        
        return queryData;
    }
    
    
    /**
     * set all necessary ?s and execute the query
     */
    protected void processRow ( Request request, Connection conn, PreparedStatement statement, Configuration table,
                                CacheHelper queryData, Object[][] columnValues, int rowIndex, Map results )
        throws SQLException, ConfigurationException, Exception {

    
        int currentIndex = 1;

        // ordering is different for UPDATE than for INSERT: values, keys
        for (int i = 0; i < queryData.columns.length; i++) {
            Column col = queryData.columns[i];
            if ( !col.isKey ) {
                this.setColumn( statement, currentIndex, request, col.columnConf, 
                                getOutputName( table, col.columnConf, rowIndex ),
                                columnValues[ i ][ ( col.isSet ? rowIndex : 0 ) ], 
                                rowIndex);
                currentIndex++;
            }
        }
        for (int i = 0; i < queryData.columns.length; i++) {
            Column col = queryData.columns[i];
            if ( col.isKey ) {
                this.setColumn( statement, currentIndex, request, col.columnConf, 
                                getOutputName( table, col.columnConf, rowIndex ),
                                columnValues[ i ][ ( col.isSet ? rowIndex : 0 ) ], 
                                rowIndex);
                currentIndex++;
            }
        }
        statement.execute();
    }
    
}
