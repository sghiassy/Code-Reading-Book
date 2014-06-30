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
import java.sql.Clob;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Struct;
import java.sql.Types;

import java.io.InputStream;
import java.io.BufferedInputStream;

import org.apache.cocoon.util.HashMap;

import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

import org.apache.cocoon.environment.Request;

/**
 * Selects a record from a database. The action can select one or more
 * tables, and can select from more than one row of a table at a
 * time. The form descriptor semantics for this are still in a bit of
 * a state of flux.
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:35 $
 */
public class ModularDatabaseSelectAction extends ModularDatabaseAction {

    /**
     * determine which mode to use as default mode
     * here: SELECT
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

                queryData = new CacheHelper( keys.length, keys.length );
                fillModes( keys, true, defaultModeNames, modeTypes, queryData );
                fillModes( values, false, defaultModeNames, modeTypes, queryData );
                
                StringBuffer queryBuffer = new StringBuffer("SELECT ");
                
                queryBuffer.append(table.getAttribute("name")).append(" WHERE ");
                for (int i = 0; i < queryData.columns.length; i++) {
                    if ( !queryData.columns[i].isKey ) {
                        if ( i > 0 ) {
                            queryBuffer.append(", ");
                        }
                        queryBuffer
                            .append( queryData.columns[i].columnConf.getAttribute( "name" ) );
                    }
                }
                
                queryBuffer.append(" FROM ").append(table.getAttribute("name")).append(" WHERE ");
                for (int i = 0; i < queryData.columns.length; i++) {
                    if ( queryData.columns[i].isKey ) {
                        if ( i > 0 ) {
                            queryBuffer.append(" AND ");
                        }
                        queryBuffer
                            .append( queryData.columns[i].columnConf.getAttribute( "name" ) )
                            .append( "= ?");
                    }
                }
                
                queryData.queryString = queryBuffer.toString();
                
                this.cachedQueryData.put( lookUpKey, queryData );
            }
        }
        
        return queryData;
    }
    
    
    /**
     * Fetch all values for all key columns that are needed to do the
     * database operation.
     */
    protected Object[][] getColumnValues( Configuration tableConf, CacheHelper queryData, Request request )
        throws ConfigurationException, ComponentException {

        Object[][] columnValues = new Object[ queryData.columns.length ][];
        for ( int i = 0; i < queryData.columns.length; i++ ){
            if ( queryData.columns[i].isKey ) {
                columnValues[i] = this.getColumnValue( tableConf, queryData.columns[i], request );
            } else {
                // columnValues[i] = new Object[1]; // this should not be needed
            }
        }
        return columnValues;
    }


    /**
     * set all necessary ?s and execute the query
     */
    protected void processRow ( Request request, Connection conn, PreparedStatement statement, Configuration table,
                                CacheHelper queryData, Object[][] columnValues, int rowIndex, Map results )
        throws SQLException, ConfigurationException, Exception {
    
        int currentIndex = 1;

        // ordering is different for SELECT just needs keys
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
        // retrieve values
        ResultSet resultset = statement.getResultSet();
        rowIndex = -1;
        while ( resultset.next() ){
            if ( ! ( rowIndex == -1 && resultset.isLast() ) ) {
                rowIndex++;
            }
            for (int i = 0; i < queryData.columns.length; i++) {
                if ( !queryData.columns[i].isKey ) {
                    Object value = this.getColumn( resultset, queryData.columns[i].columnConf );
                    String param = getOutputName( table, queryData.columns[i].columnConf, rowIndex );
                    this.setRequestAttribute( request, param, value );
                }
            }
        }
    }
    
    
    /**
     * Get the Statement column so that the results are mapped correctly.
     * (this has been copied from AbstractDatabaseAction and modified slightly)
     */
    protected Object getColumn(ResultSet set, Configuration column ) throws Exception {
        
        Integer type = (Integer) AbstractDatabaseAction.typeConstants.get(column.getAttribute("type"));
        String dbcol = column.getAttribute("name");
        Object value = null;
        
        switch (type.intValue()) {
        case Types.CLOB:
            Clob dbClob = set.getClob(dbcol);
            int length = (int) dbClob.length();
            InputStream asciiStream = new BufferedInputStream(dbClob.getAsciiStream());
            byte[] buffer = new byte[length];
            asciiStream.read(buffer);
            String str = new String(buffer);
            asciiStream.close();
            value = str;
            break;
        case Types.BIGINT:
            value = set.getBigDecimal(dbcol);
            break;
        case Types.TINYINT:
            value = new Byte(set.getByte(dbcol));
            break;
        case Types.VARCHAR:
            value  = set.getString(dbcol);
            break;
        case Types.DATE:
            value = set.getDate(dbcol);
            break;
        case Types.DOUBLE:
            value = new Double(set.getDouble(dbcol));
            break;
        case Types.FLOAT:
            value = new Float(set.getFloat(dbcol));
            break;
        case Types.INTEGER:
            value = new Integer(set.getInt(dbcol));
            break;
        case Types.NUMERIC:
            value = new Long(set.getLong(dbcol));
            break;
        case Types.SMALLINT:
            value = new Short(set.getShort(dbcol));
            break;
        case Types.TIME:
            value = set.getTime(dbcol);
            break;
        case Types.TIMESTAMP:
            value = set.getTimestamp(dbcol);
            break;
        case Types.ARRAY:
            value = set.getArray(dbcol); // new Integer(set.getInt(dbcol));
            break;
        case Types.BIT:
            value = new Integer(set.getInt(dbcol));
            break;
        case Types.CHAR:
            value = new Integer(set.getInt(dbcol));
            break;
        case Types.STRUCT:
            value = (Struct) set.getObject(dbcol);
            break;
        case Types.OTHER:
            value = set.getObject(dbcol);
            break;
            
        default:
            // The blob types have to be requested separately, via a Reader.
            value = "";
            break;
        }
        
        return value;
    }


}
