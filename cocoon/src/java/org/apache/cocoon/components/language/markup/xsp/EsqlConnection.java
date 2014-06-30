/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup.xsp;
 
import java.sql.Connection;
import java.util.Properties;
import java.sql.SQLException;

/**
 * This is wrapper to extend a java.sql.Conncetion implementation
 * for easier paging / limit facilities
 *
 * @author <A HREF="mailto:tcurdt@dff.st">Torsten Curdt</A>
 * based on the orginal esql.xsl
 */ 

public class EsqlConnection implements Connection {
  public static final int LIMIT_METHOD_NOLIMIT = 0;
  public static final int LIMIT_METHOD_POSTGRESQL = 1;
  public static final int LIMIT_METHOD_MYSQL = 2;
  public static final int LIMIT_METHOD_JDBC = 3;

  public Connection connection = null;

  private String url = null;
  private Properties info = new Properties();
  private int limitMethod = LIMIT_METHOD_NOLIMIT;

  public Properties getInfo() {
    return(info);
  }

  public String getUrl() {
    return(url);
  }

  public void setUrl( String url ) {
    this.url = url;
  }

  public void setProperty( String name, Object value ) {
    info.put(name,value);
  }

  public void setUser(String user) {
    setProperty("user",user);
  }

  public void setPassword(String password) {
    setProperty("password",password);
  }

  public int getLimitMethod() {
    return(limitMethod);
  }

  public void setLimitMethod( String method ) {
    if ("".equals(method) || "auto".equals(method) ) {
      String jdbcSource;
      try {
        jdbcSource = connection.getMetaData().getURL();
      } catch (Exception e) {
        throw new RuntimeException("Error accessing connection metadata: "+e);
      }
      if (jdbcSource.startsWith("jdbc:postgresql:")) {
        limitMethod = LIMIT_METHOD_POSTGRESQL;
      } else if (jdbcSource.startsWith("jdbc:mysql:")) {
        limitMethod = LIMIT_METHOD_MYSQL;
      } else if (jdbcSource.startsWith("jdbc:sybase:")) {
        limitMethod = LIMIT_METHOD_JDBC;
      }
      else {
        throw new RuntimeException("Cannot guess limit method from jdbc url: " + method);
      }
    }
    else if ("postgresql".equals(method)) {
      limitMethod = LIMIT_METHOD_POSTGRESQL;
    } else if ("mysql".equals(method)) {
      limitMethod = LIMIT_METHOD_MYSQL;
    } else if ("jdbc".equals(method)) {
      limitMethod = LIMIT_METHOD_JDBC;
    } else {
      throw new RuntimeException("Unknown limit method: " + method);
    }
  }


  /* */ 

  public java.sql.Statement createStatement() throws SQLException {
    return(connection.createStatement());
  }

  public java.sql.Statement createStatement(int i1, int i2) throws SQLException {
    return(connection.createStatement(i1, i2));
  }

  public java.sql.PreparedStatement prepareStatement(String s) throws SQLException {
    return(connection.prepareStatement(s));
  }

  public java.sql.PreparedStatement prepareStatement(String s, int i1, int i2) throws SQLException {
    return(connection.prepareStatement(s, i1, i2));
  }


  public void close() throws SQLException {
    connection.close();
  }

  public void commit() throws SQLException {
    connection.commit();
  }

  public void rollback() throws SQLException {
    connection.rollback();
  }

  public boolean getAutoCommit() throws SQLException {
    return(connection.getAutoCommit());
  }

  public void setAutoCommit(boolean autocommit) throws SQLException {
    connection.setAutoCommit(autocommit);
  }

  public void setTransactionIsolation(int i)  throws SQLException {
    connection.setTransactionIsolation(i);
  }

  public int getTransactionIsolation()  throws SQLException {
    return(connection.getTransactionIsolation());
  }

  public String getCatalog()  throws SQLException {
    return(connection.getCatalog());
  }

  public java.sql.SQLWarning getWarnings()  throws SQLException {
    return(connection.getWarnings());
  }

  public java.util.Map getTypeMap() throws SQLException {
    return(connection.getTypeMap());
  }

  public boolean isClosed() throws SQLException {
    return(isClosed());
  }

  public java.sql.DatabaseMetaData getMetaData() throws SQLException {
    return(connection.getMetaData());
  }

  public void setCatalog(String s) throws SQLException {
    connection.setCatalog(s);
  }

  public void setTypeMap(java.util.Map m) throws SQLException {
    connection.setTypeMap(m);
  }

  public void setReadOnly(boolean b) throws SQLException {
    connection.setReadOnly(b);
  }

  public void clearWarnings() throws SQLException {
    connection.clearWarnings();
  }

  public boolean isReadOnly() throws SQLException {
    return(connection.isReadOnly());
  }

  public String nativeSQL(String s) throws SQLException {
    return(connection.nativeSQL(s));
  }

  public java.sql.CallableStatement prepareCall(String s) throws SQLException {
    return(connection.prepareCall(s));
  }

  public java.sql.CallableStatement prepareCall(String s, int i1, int i2) throws SQLException {
    return(connection.prepareCall(s,i1,i2));
  }

/*
    public void setHoldability(int holdability)
        throws SQLException
    {
        connection.setHoldability(holdability);
    }

    public int getHoldability()
        throws SQLException
    {
        return connection.getHoldability();
    }

    public java.sql.Savepoint setSavepoint()
        throws SQLException
    {
        return connection.setSavepoint();
    }

    public java.sql.Savepoint setSavepoint(String savepoint)
        throws SQLException
    {
        return connection.setSavepoint(savepoint);
    }

    public void rollback(java.sql.Savepoint savepoint)
        throws SQLException
    {
        connection.rollback(savepoint);
    }

    public void releaseSavepoint(java.sql.Savepoint savepoint)
        throws SQLException
    {
        connection.releaseSavepoint(savepoint);
    }

    public java.sql.Statement createStatement(int resulSetType,
                                           int resultSetConcurrency,
                                           int resultSetHoldability)
        throws SQLException
    {
        return connection.createStatement(resulSetType, resultSetConcurrency, resultSetHoldability);
    }

    public java.sql.PreparedStatement prepareStatement(String sql,
                                        int resulSetType,
                                        int resultSetConcurrency,
                                        int resultSetHoldability)
        throws SQLException
    {
        return connection.prepareStatement(sql, resulSetType, resultSetConcurrency, resultSetHoldability);
    }

    public java.sql.CallableStatement prepareCall(String sql,
                                        int resulSetType,
                                        int resultSetConcurrency,
                                        int resultSetHoldability)
        throws SQLException
    {
        return connection.prepareCall(sql, resulSetType, resultSetConcurrency, resultSetHoldability);
    }

    public java.sql.PreparedStatement prepareStatement(String sql,
                                        int autoGeneratedKeys)
        throws SQLException
    {
        return connection.prepareStatement(sql, autoGeneratedKeys);
    }

    public java.sql.PreparedStatement prepareStatement(String sql,
                                        int[] columnIndexes)
        throws SQLException
    {
        return prepareStatement(sql, columnIndexes);
    }

    public java.sql.PreparedStatement prepareStatement(String sql,
                                        String[] columnNames)
        throws SQLException
    {
        return prepareStatement(sql, columnNames);
    }
*/

}                                                                                                                      

