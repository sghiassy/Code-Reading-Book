/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup.xsp;
 
import java.sql.Connection;
import java.sql.Statement;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;

/**
 * This helper class takes care of contstructing queries
 * and cursor positioning (paging) for all different kinds
 * of databases
 *
 * @author <A HREF="mailto:tcurdt@dff.st">Torsten Curdt</A>
 * based on the orginal esql.xsl
 */
 
public class EsqlQuery {
  private Connection connection = null;
  private Statement statement = null;
  private PreparedStatement preparedStatement = null;
  private ResultSet resultSet = null;
  private ResultSetMetaData resultSetMetaData = null;
  private boolean hasResultSet = false;
  private int position = -1;
  private int maxRows = -1;
  private int skipRows = 0;
  private boolean keepgoing = true; 
  private java.util.Hashtable groupingVars = new java.util.Hashtable();

  private String query;
  private int limitMethod;

  public EsqlQuery( EsqlConnection connection, String query ) {
    this.connection = connection;
    this.limitMethod = connection.getLimitMethod();
    this.query = query;
  }

  public int getSkipRows() {
    return(skipRows);
  }

  public void setSkipRows( int i ) {
    this.skipRows = i;
  }

  public int getMaxRows() {
    return(maxRows);
  }

  public void setMaxRows( int i ) {
    this.maxRows = i;
  }

  public int getCurrentRow() {
    return(position);
  }

  public String getQueryString() {
    switch(limitMethod) {
      case EsqlConnection.LIMIT_METHOD_POSTGRESQL:
        if (skipRows > 0) {
          if (maxRows > -1) {
            return(new StringBuffer(query).append(" LIMIT ").append(maxRows).append(",").append(skipRows).toString());
          }
          else {
            return(new StringBuffer(query).append(" OFFSET ").append(skipRows).toString());
          }
        }
        else {
          if (maxRows > -1) {
            return(new StringBuffer(query).append(" LIMIT ").append(maxRows).toString());
          }
          else {
            return(query);
          }
        }
      case EsqlConnection.LIMIT_METHOD_MYSQL:
        if (skipRows > 0) {
          if (maxRows > -1) {
            return(new StringBuffer(query).append(" LIMIT ").append(skipRows).append(",").append(maxRows).toString());
          }
          else {
            throw new RuntimeException("MySQL does not support a skip of rows only");
          }
        }
        else {
          if (maxRows > -1) {
            return(new StringBuffer(query).append(" LIMIT ").append(maxRows).toString());
          }
          else {
            return(query);
          }
        }
      default:
        return(query);
    }
  }

  public PreparedStatement prepareStatement() throws SQLException {
    switch(limitMethod) {
      case EsqlConnection.LIMIT_METHOD_JDBC:
        preparedStatement = connection.prepareStatement( getQueryString(), ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
        break;
      default:
        preparedStatement = connection.prepareStatement( getQueryString() );
    };
    statement = preparedStatement; 
    return(preparedStatement);
  }

  public PreparedStatement getPreparedStatement() {
    return(preparedStatement);
  }

  public ResultSet getResultSet() {
    return(resultSet);
  }

  public boolean nextRow() throws SQLException {
    position++;
    return(resultSet.next());
  }

  public boolean keepGoing() {
    return(keepgoing);
  }

  public void setKeepGoing( boolean still ) {
    keepgoing = still;
  }

  public Object setGroupingVar( String key, Object value) {
    return groupingVars.put(key,value);
  } 
 
  public Object getGroupingVar( String key) {
    return groupingVars.get(key);
  } 

  public ResultSetMetaData getResultSetMetaData() {
    return(resultSetMetaData);
  }

  public Statement createStatement() throws SQLException {
    switch(limitMethod) {
      case EsqlConnection.LIMIT_METHOD_JDBC:
        statement = connection.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
        break;
      default:
        statement = connection.createStatement();
    };
    return(statement);
  }

  public Statement getStatement() {
    return(statement);
  }

  public boolean getMoreResults() throws SQLException {
    return(statement.getMoreResults());
  }

  public boolean execute() throws SQLException {
    if (preparedStatement != null) {
      hasResultSet = preparedStatement.execute();
    }
    else {
      hasResultSet = statement.execute( getQueryString() );
    }
    return(hasResultSet);
  }

  public boolean hasResultSet() {
    return(this.hasResultSet);
  }

  public int rowCount() throws SQLException {
    switch(limitMethod) {
      case EsqlConnection.LIMIT_METHOD_JDBC:
          // TC: I'm not quite sure if synchronized is really necessary since
          // the XSP page will be poolable
          synchronized (resultSet) {
            int currentRow = resultSet.getRow();
            resultSet.last();
            int count = resultSet.getRow();
            if (currentRow > 0) {
              resultSet.absolute(currentRow);
            }
            else {
              resultSet.first();
             resultSet.relative(-1);
            }
            return(count);
          }
      default:
        // select count(*)
        throw new RuntimeException("not yet implemented");
    };
  }

  public void getResultRows() throws SQLException {
    resultSet = statement.getResultSet();
    resultSetMetaData = resultSet.getMetaData();
   
    if (skipRows > 0) {
      switch(limitMethod) {
        case EsqlConnection.LIMIT_METHOD_POSTGRESQL:
          // in clause
          position = skipRows;
          break;
        case EsqlConnection.LIMIT_METHOD_MYSQL:
          // in clause
          position = skipRows;
          break;
        case EsqlConnection.LIMIT_METHOD_JDBC:
          resultSet.absolute(skipRows);
          position = skipRows;
          break;
        default:
          while (resultSet.next()) {
            position++;
            if (position == skipRows) {
               break;
            }
          }
      };
    }
  }
}
