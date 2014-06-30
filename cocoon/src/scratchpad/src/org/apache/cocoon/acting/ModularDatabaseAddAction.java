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

import org.apache.cocoon.acting.ModularDatabaseAccess.AutoIncrementHelper;

/**
 * Adds record in a database. The action can update one or more tables,
 * and can add more than one row to a table at a time. The form descriptor
 * semantics for this are still in a bit of a state of flux. 
 *
 * The difference to the other Database*Actions is, that it uses
 * additional components for reading and writing parameters. In
 * addition the descriptor format has changed to accomodate the new
 * features.
 *
 * This action is heavily based upon the DatabaseAddAction and relies
 * on the AbstractDatabaseAction.
 *
 * Note: Component management will move to sitemap so that other
 * components can utilize the helper components.
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:35 $
 */
public class ModularDatabaseAddAction extends ModularDatabaseAction {


    /**
     * set all necessary ?s and execute the query
     */
    protected void processRow ( Request request, Connection conn, PreparedStatement statement,
                                Configuration table, CacheHelper queryData, Object[][] columnValues,
                                int rowIndex, Map results ) 
        throws SQLException, ConfigurationException, Exception {
    
        int currentIndex = 1;
        for (int i = 0; i < queryData.columns.length; i++) {
            Column col = queryData.columns[i];
            if ( col.isAutoIncrement && col.isKey ) {
                currentIndex += setKeyAuto( table, col, currentIndex, rowIndex, 
                                            conn, statement, request, results );
            } else {
                this.setColumn( statement, currentIndex, request, col.columnConf, 
                                getOutputName( table, col.columnConf, rowIndex ),
                                columnValues[ i ][ ( col.isSet ? rowIndex : 0 ) ], 
                                rowIndex);
                currentIndex++;
            }
        }
        statement.execute();
        // get resulting ids for autoincrement columns
        for (int i = 0; i < queryData.columns.length; i++) {
            if ( queryData.columns[i].isAutoIncrement && queryData.columns[i].isKey ) {
                storeKeyValue( table, queryData.columns[i], rowIndex, 
                               conn, statement, request, results );
            }
        }
    }


    /**
     * determine which mode to use as default mode
     * here: INSERT
     * highly specific to operation INSERT / UPDATE / DELETE / SELECT
     */
    protected String selectMode ( boolean isAutoIncrement, HashMap modes ) {

        if ( isAutoIncrement )
            return (String) modes.get( MODE_AUTOINCR );
        else 
            return (String) modes.get( MODE_OTHERS );
    }


    /**
     * determine whether autoincrement columns should be honoured by
     * this operation. This is usually snsible only for INSERTs.
     */
    protected boolean honourAutoIncrement() { return true; }


    /**
     * Fetch all values for all columns that are needed to do the
     * database operation.
     */
    protected Object[][] getColumnValues( Configuration tableConf, CacheHelper queryData,
                                          Request request ) 
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
                Configuration[] values = table.getChild("values").getChildren("value");
                Configuration[] keys = table.getChild("keys").getChildren("key");

                queryData = new CacheHelper( keys.length, keys.length + values.length );
                fillModes( keys,   true,  defaultModeNames, modeTypes, queryData );
                fillModes( values, false, defaultModeNames, modeTypes, queryData );
                
                StringBuffer queryBuffer = new StringBuffer("INSERT INTO ");
                StringBuffer valueBuffer = new StringBuffer(") VALUES (");
                AutoIncrementHelper dah;
                
                queryBuffer.append(table.getAttribute("name"));
                queryBuffer.append(" (");
                int actualColumns = 0;

                for (int i = 0; i < queryData.columns.length; i++) {
                    if ( actualColumns > 0 ) {
                        queryBuffer.append( ", " );
                        valueBuffer.append( ", " );
                    }
                    if ( queryData.columns[i].isKey && queryData.columns[i].isAutoIncrement ) {
                        dah = (AutoIncrementHelper) modeMapping.select( queryData.columns[i].mode );
                        if ( dah.includeInQuery() ) {
                            actualColumns++;
                            queryBuffer.append( queryData.columns[i].columnConf.getAttribute( "name" ) );
                            if ( dah.includeAsValue() ) {
                                valueBuffer.append( "?" );
                            } else {
                                valueBuffer.append( 
                                                   dah.getSubquery( table, queryData.columns[i].columnConf, 
                                                                    queryData.columns[i].modeConf ) );
                            }
                        }
                        modeMapping.release( dah );
                    } else {
                        actualColumns++;
                        queryBuffer.append( queryData.columns[i].columnConf.getAttribute( "name" ) );
                        valueBuffer.append( "?" );
                    }
                }
                
                valueBuffer.append(")");
                queryBuffer.append(valueBuffer);
                
                queryData.queryString = queryBuffer.toString();
                
                this.cachedQueryData.put( lookUpKey, queryData );
            }
        }
        
        return queryData;
    }
    

}
