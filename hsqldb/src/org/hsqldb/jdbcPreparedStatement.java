/*
 * jdbcPreparedStatement.java
 *
 * Copyright (c) 2001, The HSQL Development Group
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 *
 * Neither the name of the HSQL Development Group nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This package is based on HypersonicSQL, originally developed by Thomas Mueller.
 *
 */
package org.hsqldb;

import java.sql.*;
import java.math.BigDecimal;
import java.io.*;
import java.util.*;

/**
 * <P><font color="#009900">
 * In HSQL Database Engine, the class jdbcPreparedStatement implements both the
 * PreparedStatement and the CallableStatement interfaces.
 * <P>
 * CallableStatement is used to execute SQL stored procedures,<BR>
 * PreparedStatement is used to execute SQL statement with parameters.
 * <P>
 * There are currently no stored procedures in HSQL Database Engine.
 * The reason is that stored procedures are currently extremely
 * proprietary. If a stored procedure was made for one database type,
 * it will not work in a database of another vendor.
 * <P>
 * The functions for working with OUT parameters are not implemented
 * because there are no statements that returns OUT parameters. All
 * statements in HSQL Database Engine returns a ResultSet or a Update Count.
 * <P>
 * For other databases it may be required to use OUT parameters
 * when calling stored procedures or vendor specific statements.
 * </font><P>
 * A PreparedStatement represents a precompiled SQL statement.
 * <P>A SQL statement is pre-compiled and stored in a
 * PreparedStatement object. This object can then be used to
 * execute this statement multiple times.
 *
 * <P><B>Note:</B> The setXXX methods for setting IN parameter values
 * must specify types that are compatible with the defined SQL type of
 * the input parameter. For instance, if the IN parameter has SQL type
 * Integer, then the method <code>setInt</code> should be used.
 *
 * <p>If arbitrary parameter type conversions are required, the method
 * <code>setObject</code> should be used with a target SQL type.
 * <br>
 * Example of setting a parameter; <code>con</code> is an active connection
 * <pre><code>
 * PreparedStatement pstmt = con.prepareStatement("UPDATE EMPLOYEES
 * SET SALARY = ? WHERE ID = ?");
 * pstmt.setBigDecimal(1, 153833.00)
 * pstmt.setInt(2, 110592)
 * </code></pre>
 *
 * <P>
 * The interface CallableStatement is used to execute SQL
 * stored procedures.  JDBC provides a stored procedure
 * SQL escape that allows stored procedures to be called in a standard
 * way for all RDBMSs. This escape syntax has one form that includes
 * a result parameter and one that does not. If used, the result
 * parameter must be registered as an OUT parameter. The other parameters
 * can be used for input, output or both. Parameters are referred to
 * sequentially, by number. The first parameter is 1.
 * <P>
 * <blockquote><pre>
 * {?= call &lt;procedure-name&gt;[&lt;arg1&gt;,&lt;arg2&gt;, ...]}
 * {call &lt;procedure-name&gt;[&lt;arg1&gt;,&lt;arg2&gt;, ...]}
 * </pre></blockquote>
 * <P>
 * IN parameter values are set using the set methods inherited from
 * {@link PreparedStatement}.  The type of all OUT parameters must be
 * registered prior to executing the stored procedure; their values
 * are retrieved after execution via the <code>get</code> methods provided here.
 * <P>
 * A <code>CallableStatement</code> can return one {@link ResultSet} or
 * multiple <code>ResultSet</code> objets.  Multiple
 * <code>ResultSet</code> objects are handled using operations
 * inherited from {@link Statement}.
 * <P>
 * For maximum portability, a call's <code>ResultSet</code> objects and
 * update counts should be processed prior to getting the values of output
 * parameters.
 *
 * @see jdbcResultSet
 */
public class jdbcPreparedStatement extends jdbcStatement
    implements PreparedStatement, CallableStatement {
    private String sSql;
    private Vector vParameter;

    /**
     * Executes the SQL query in this <code>PreparedStatement</code> object
     * and returns the result set generated by the query.
     *
     * @return a ResultSet that contains the data produced by the
     * query; never null
     * @exception SQLException if a database access error occurs
     */
    public ResultSet executeQuery() throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return super.executeQuery(build());
    }

    /**
     * Executes the SQL INSERT, UPDATE or DELETE statement
     * in this <code>PreparedStatement</code> object.
     * In addition,
     * SQL statements that return nothing, such as SQL DDL statements,
     * can be executed.
     *
     * @return either the row count for INSERT, UPDATE or DELETE statements;
     * or 0 for SQL statements that return nothing
     * @exception SQLException if a database access error occurs
     */
    public int executeUpdate() throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return super.executeUpdate(build());
    }

    /**
     * Sets the designated parameter to SQL NULL.
     *
     * <P><B>Note:</B> You must specify the parameter's SQL type.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param sqlType the SQL type code defined in java.sql.Types
     * @exception SQLException if a database access error occurs
     */
    public void setNull(int parameterIndex, int sqlType) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	setNull(parameterIndex);
    }

    /**
     * Sets the designated parameter to a Java boolean value.  The driver
     * converts this to an SQL BIT value when it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setBoolean(int parameterIndex,
			   boolean x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	setParameter(parameterIndex, x ? "TRUE" : "FALSE");
    }

    /**
     * Sets the designated parameter to a Java byte value.  The driver
     * converts this to an SQL TINYINT value when it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setByte(int parameterIndex, byte x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	setParameter(parameterIndex, "" + x);
    }

    /**
     * Sets the designated parameter to a Java short value.  The driver
     * converts this to an SQL SMALLINT value when it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setShort(int parameterIndex, short x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	setParameter(parameterIndex, "" + x);
    }

    /**
     * Sets the designated parameter to a Java int value.  The driver
     * converts this to an SQL INTEGER value when it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setInt(int parameterIndex, int x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	setParameter(parameterIndex, "" + x);
    }

    /**
     * Sets the designated parameter to a Java long value.  The driver
     * converts this to an SQL BIGINT value when it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setLong(int parameterIndex, long x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	setParameter(parameterIndex, "" + x);
    }

    /**
     * Sets the designated parameter to a Java float value.  The driver
     * converts this to an SQL FLOAT value when it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setFloat(int parameterIndex, float x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	setParameter(parameterIndex, "" + x);
    }

    /**
     * Sets the designated parameter to a Java double value.  The driver
     * converts this to an SQL DOUBLE value when it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setDouble(int parameterIndex, double x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	setParameter(parameterIndex, "" + x);
    }

    /**
     * Sets the designated parameter to a java.lang.BigDecimal value.
     * The driver converts this to an SQL NUMERIC value when
     * it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setBigDecimal(int parameterIndex,
			      BigDecimal x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (x == null) {
	    setNull(parameterIndex);
	} else {
	    setParameter(parameterIndex, x.toString());
	}
    }

    /**
     * Sets the designated parameter to a Java String value.  The driver
     * converts this to an SQL VARCHAR or LONGVARCHAR value (depending on the
     * argument's size relative to the driver's limits on VARCHARs) when it sends
     * it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setString(int parameterIndex, String x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (x == null) {
	    setNull(parameterIndex);
	} else {
	    setParameter(parameterIndex, Column.createString(x));
	}
    }

    /**
     * Sets the designated parameter to a Java array of bytes.  The driver
     * converts this to an SQL VARBINARY or LONGVARBINARY (depending on the
     * argument's size relative to the driver's limits on VARBINARYs)
     * when it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setBytes(int parameterIndex, byte[] x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (x == null) {
	    setNull(parameterIndex);
	} else {
	    setParameter(parameterIndex,
			 "'" + ByteArray.createString(x) + "'");
	}
    }

    /**
     * Sets the designated parameter to a java.sql.Date value.  The driver
     * converts this to an SQL DATE value when it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setDate(int parameterIndex,
			java.sql.Date x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (x == null) {
	    setNull(parameterIndex);
	} else {
	    setParameter(parameterIndex, "'" + x.toString() + "'");
	}
    }

    /**
     * Sets the designated parameter to a java.sql.Time value.  The driver
     * converts this to an SQL TIME value when it sends it to the database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setTime(int parameterIndex, Time x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (x == null) {
	    setNull(parameterIndex);
	} else {
	    setParameter(parameterIndex, "'" + x.toString() + "'");
	}
    }

    /**
     * Sets the designated parameter to a java.sql.Timestamp value.  The driver
     * converts this to an SQL TIMESTAMP value when it sends it to the
     * database.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setTimestamp(int parameterIndex,
			     Timestamp x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (x == null) {
	    setNull(parameterIndex);
	} else {
	    setParameter(parameterIndex, "'" + x.toString() + "'");
	}
    }

    /**
     * Sets the designated parameter to the given input stream, which will have
     * the specified number of bytes.
     * When a very large ASCII value is input to a LONGVARCHAR
     * parameter, it may be more practical to send it via a
     * java.io.InputStream. JDBC will read the data from the stream
     * as needed, until it reaches end-of-file.  The JDBC driver will
     * do any necessary conversion from ASCII to the database char format.
     *
     * <P><B>Note:</B> This stream object can either be a standard
     * Java stream object or your own subclass that implements the
     * standard interface.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the Java input stream that contains the ASCII parameter value
     * @param length the number of bytes in the stream
     * @exception SQLException if a database access error occurs
     */
    public void setAsciiStream(int parameterIndex, InputStream x,
			       int length) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (x == null) {
	    setNull(parameterIndex);
	} else {
	    setString(parameterIndex, StringConverter.InputStreamToString(x));
	}
    }

    /**
     * Sets the designated parameter to the given input stream, which will have
     * the specified number of bytes.
     * When a very large UNICODE value is input to a LONGVARCHAR
     * parameter, it may be more practical to send it via a
     * java.io.InputStream. JDBC will read the data from the stream
     * as needed, until it reaches end-of-file.  The JDBC driver will
     * do any necessary conversion from UNICODE to the database char format.
     * The byte format of the Unicode stream must be Java UTF-8, as
     * defined in the Java Virtual Machine Specification.
     *
     * <P><B>Note:</B> This stream object can either be a standard
     * Java stream object or your own subclass that implements the
     * standard interface.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the java input stream which contains the
     * UNICODE parameter value
     * @param length the number of bytes in the stream
     * @exception SQLException if a database access error occurs
     * @deprecated
     */
    public void setUnicodeStream(int paramIndex, InputStream x,
				 int length) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	setAsciiStream(paramIndex, x, length);
    }

    /**
     * Sets the designated parameter to the given input stream, which will have
     * the specified number of bytes.
     * When a very large binary value is input to a LONGVARBINARY
     * parameter, it may be more practical to send it via a
     * java.io.InputStream. JDBC will read the data from the stream
     * as needed, until it reaches end-of-file.
     *
     * <P><B>Note:</B> This stream object can either be a standard
     * Java stream object or your own subclass that implements the
     * standard interface.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the java input stream which contains the binary parameter value
     * @param length the number of bytes in the stream
     * @exception SQLException if a database access error occurs
     */
    public void setBinaryStream(int parameterIndex, InputStream x,
				int length) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	byte b[] =
	    new byte[length];    // todo: is this correct? what if length=0?

	try {
	    x.read(b, 0, length);
	    x.close();
	} catch (IOException e) {
	    throw Trace.error(Trace.INPUTSTREAM_ERROR, e.getMessage());
	}

	setBytes(parameterIndex, b);
    }

    /**
     * Clears the current parameter values immediately.
     * <P>In general, parameter values remain in force for repeated use of a
     * Statement. Setting a parameter value automatically clears its
     * previous value.  However, in some cases it is useful to immediately
     * release the resources used by the current parameter values; this can
     * be done by calling clearParameters.
     */
    public void clearParameters() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	vParameter.removeAllElements();
    }

    /**
     * <p>Sets the value of a parameter using an object. The second
     * argument must be an object type; for integral values, the
     * java.lang equivalent objects should be used.
     *
     * <p>The given Java object will be converted to the targetSqlType
     * before being sent to the database.
     *
     * If the object has a custom mapping (is of a class implementing SQLData),
     * the JDBC driver should call its method <code>writeSQL</code> to write it
     * to the SQL data stream.
     * If, on the other hand, the object is of a class implementing
     * Ref, Blob, Clob, Struct,
     * or Array, the driver should pass it to the database as a value of the
     * corresponding SQL type.
     *
     * <p>Note that this method may be used to pass datatabase-
     * specific abstract data types.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the object containing the input parameter value
     * @param targetSqlType the SQL type (as defined in java.sql.Types) to be
     * sent to the database. The scale argument may further qualify this type.
     * @param scale for java.sql.Types.DECIMAL or java.sql.Types.NUMERIC types,
     * this is the number of digits after the decimal point.  For all other
     * types, this value will be ignored.
     * @exception SQLException if a database access error occurs
     */
    public void setObject(int i, Object x, int type,
			  int scale) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	setObject(i, x, type);
    }

    /**
     * Sets the value of the designated parameter with the given object.
     * This method is like setObject above, except that it assumes a scale
     * of zero.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the object containing the input parameter value
     * @param targetSqlType the SQL type (as defined in java.sql.Types) to be
     * sent to the database
     * @exception SQLException if a database access error occurs
     */
    public void setObject(int parameterIndex, Object x,
			  int type) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (x == null) {
	    setNull(parameterIndex);

	    return;
	}

	x = Column.convertObject(x, type);

	setObjectInType(parameterIndex, x, type);
    }

    /**
     * <p>Sets the value of a parameter using an object; use the
     * java.lang equivalent objects for integral values.
     *
     * <p>The JDBC specification specifies a standard mapping from
     * Java Object types to SQL types.  The given argument java object
     * will be converted to the corresponding SQL type before being
     * sent to the database.
     *
     * <p>Note that this method may be used to pass datatabase-
     * specific abstract data types, by using a Driver-specific Java
     * type.
     *
     * If the object is of a class implementing SQLData,
     * the JDBC driver should call its method <code>writeSQL</code> to write it
     * to the SQL data stream.
     * If, on the other hand, the object is of a class implementing
     * Ref, Blob, Clob, Struct,
     * or Array, then the driver should pass it to the database as a value of the
     * corresponding SQL type.
     *
     * This method throws an exception if there is an ambiguity, for example, if
     * the object is of a class implementing more than one of those interfaces.
     *
     * <P><font color="#009900">
     * In HSQL Database Engine, this function checks if the object is of one of the
     * Java SQL Types (Integer, String, Double,...) and if yes, the function will
     * call the apropriate setXXX function. If an object should be inserted
     * with data type 'Object' (OTHER), then the function
     * setObject(i,x,Types.OTHER) should be called.
     * </font><P>
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the object containing the input parameter value
     * @exception SQLException if a database access error occurs
     */
    public void setObject(int parameterIndex, Object x) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (x == null) {
	    setNull(parameterIndex);

	    return;
	}

	int type = Types.OTHER;

	if (x instanceof String) {
	    type = Types.VARCHAR;
	} else if (x instanceof BigDecimal) {
	    type = Types.NUMERIC;
	} else if (x instanceof Integer) {
	    type = Types.INTEGER;
	} else if (x instanceof Long) {
	    type = Types.BIGINT;
	} else if (x instanceof Float) {
	    type = Types.REAL;
	} else if (x instanceof Double) {
	    type = Types.DOUBLE;
	} else if (x instanceof byte[]) {
	    type = Types.BINARY;
	} else if (x instanceof java.sql.Date) {
	    type = Types.DATE;
	} else if (x instanceof Time) {
	    type = Types.TIME;
	} else if (x instanceof Timestamp) {
	    type = Types.TIMESTAMP;
	} else if (x instanceof Boolean) {
	    type = Types.BIT;
	} else if (x instanceof Byte) {
	    type = Types.TINYINT;
	} else if (x instanceof Short) {
	    type = Types.SMALLINT;
	}

	setObjectInType(parameterIndex, x, type);
    }

    /**
     * Executes any kind of SQL statement.
     * Some prepared statements return multiple results; the execute
     * method handles these complex statements as well as the simpler
     * form of statements handled by executeQuery and executeUpdate.
     *
     * @exception SQLException if a database access error occurs
     * @see Statement#execute
     */
    public boolean execute() throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return super.execute(build());
    }

    /**
     * Method declaration
     *
     *
     * @param i
     * @param s
     */
    private void setParameter(int i, String s) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (vParameter.size() < i) {
	    vParameter.setSize(i);
	}

	vParameter.setElementAt(s, --i);
    }

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     */
    private String getParameter(int i) {
	if (i >= vParameter.size()) {
	    return null;
	}

	return (String) vParameter.elementAt(i);
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    private String build() {
	if (vParameter.isEmpty()) {
	    return sSql;
	}

	StringBuffer s = new StringBuffer();
	int	     i = 0, l = sSql.length();
	boolean      bSkip = false, bSkip2 = false;

	for (int j = 0; j < l; j++) {
	    char c = sSql.charAt(j);

	    if (c == '?' &&!bSkip &&!bSkip2) {
		s.append(getParameter(i++));
	    } else {
		if (c == '\'' &&!bSkip2) {
		    bSkip = !bSkip;
		} else if (c == '"' &&!bSkip) {
		    bSkip2 = !bSkip2;
		}

		s.append(c);
	    }
	}

	return s.toString();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     */
    private void setNull(int parameterIndex) {
	setParameter(parameterIndex, "NULL");
    }

    // implementation of CallableStatement

    /**
     * Registers the OUT parameter in ordinal position
     * <code>parameterIndex</code> to the JDBC type
     * <code>sqlType</code>.  All OUT parameters must be registered
     * before a stored procedure is executed.
     * <p>
     * The JDBC type specified by <code>sqlType</code> for an OUT
     * parameter determines the Java type that must be used
     * in the <code>get</code> method to read the value of that parameter.
     * <p>
     * If the JDBC type expected to be returned to this output parameter
     * is specific to this particular database, <code>sqlType</code>
     * should be <code>java.sql.Types.OTHER</code>.  The method
     * {@link #getObject} retrieves the value.
     * @param parameterIndex the first parameter is 1, the second is 2,
     * and so on
     * @param sqlType the JDBC type code defined by <code>java.sql.Types</code>.
     * If the parameter is of type Numeric or Decimal, the version of
     * <code>registerOutParameter</code> that accepts a scale value
     * should be used.
     * @exception SQLException if a database access error occurs
     */
    public void registerOutParameter(int parameterIndex,
				     int sqlType) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Registers the parameter in ordinal position
     * <code>parameterIndex</code> to be of JDBC type
     * <code>sqlType</code>.  This method must be called
     * before a stored procedure is executed.
     * <p>
     * The JDBC type specified by <code>sqlType</code> for an OUT
     * parameter determines the Java type that must be used
     * in the <code>get</code> method to read the value of that parameter.
     * <p>
     * This version of <code>registerOutParameter</code> should be
     * used when the parameter is of JDBC type <code>NUMERIC</code>
     * or <code>DECIMAL</code>.
     * @param parameterIndex the first parameter is 1, the second is 2,
     * and so on
     * @param sqlType SQL type code defined by <code>java.sql.Types</code>.
     * @param scale the desired number of digits to the right of the
     * decimal point.  It must be greater than or equal to zero.
     * @exception SQLException if a database access error occurs
     */
    public void registerOutParameter(int parameterIndex, int sqlType,
				     int scale) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Indicates whether or not the last OUT parameter read had the value of
     * SQL NULL.  Note that this method should be called only after
     * calling the <code>get</code> method; otherwise, there is no value to use
     * in determining whether it is <code>null</code> or not.
     * @return <code>true</code> if the last parameter read was SQL NULL;
     * <code>false</code> otherwise.
     * @exception SQLException if a database access error occurs
     */
    public boolean wasNull() throws SQLException {
	throw getNotSupported();
    }

    /**
     * Retrieves the value of a JDBC <code>CHAR</code>, <code>VARCHAR</code>,
     * or <code>LONGVARCHAR</code> parameter as a <code>String</code> in
     * the Java programming language.
     * <p>
     * For the fixed-length type JDBC CHAR, the <code>String</code> object
     * returned has exactly the same value the JDBC CHAR value had in the
     * database, including any padding added by the database.
     * @param parameterIndex the first parameter is 1, the second is 2,
     * and so on
     * @return the parameter value. If the value is SQL NULL, the result
     * is <code>null</code>.
     * @exception SQLException if a database access error occurs
     */
    public String getString(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Gets the value of a JDBC BIT parameter as a <code>boolean</code>
     * in the Java programming language.
     * @param parameterIndex the first parameter is 1, the second is 2,
     * and so on
     * @return the parameter value.  If the value is SQL NULL, the result
     * is <code>false</code>.
     * @exception SQLException if a database access error occurs
     */
    public boolean getBoolean(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public byte getByte(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public short getShort(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public int getInt(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public long getLong(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public float getFloat(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public double getDouble(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     * @param scale
     *
     * @return
     *
     * @throws SQLException
     */
    public BigDecimal getBigDecimal(int parameterIndex,
				    int scale) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public byte[] getBytes(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public java.sql.Date getDate(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public Time getTime(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public Timestamp getTimestamp(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
    public Object getObject(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     *
     * @return
     *
     * @throws SQLException
     */
//#ifdef JAVA2

    public BigDecimal getBigDecimal(int parameterIndex) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2
    /**
     * Method declaration
     *
     *
     * @param i
     * @param map
     *
     * @return
     *
     * @throws SQLException
     */
//#ifdef JAVA2

    public Object getObject(int i, Map map) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Sets a REF(&lt;structured-type&gt;) parameter.
     *
     * @param i the first parameter is 1, the second is 2, ...
     * @param x an object representing data of an SQL REF Type
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public Ref getRef(int i) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2
    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     *
     * @throws SQLException
     */
//#ifdef JAVA2

    public Blob getBlob(int i) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2
    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     *
     * @throws SQLException
     */
//#ifdef JAVA2

    public Clob getClob(int i) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2
    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     *
     * @throws SQLException
     */
//#ifdef JAVA2

    public Array getArray(int i) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2
    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     * @param cal
     *
     * @return
     *
     * @throws SQLException
     */
//#ifdef JAVA2

    public java.sql.Date getDate(int parameterIndex,
				 Calendar cal) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2
    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     * @param cal
     *
     * @return
     *
     * @throws SQLException
     */
//#ifdef JAVA2

    public Time getTime(int parameterIndex,
			Calendar cal) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2
    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     * @param cal
     *
     * @return
     *
     * @throws SQLException
     */
//#ifdef JAVA2

    public Timestamp getTimestamp(int parameterIndex,
				  Calendar cal) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2
    /**
     * Method declaration
     *
     *
     * @param paramIndex
     * @param sqlType
     * @param typeName
     *
     * @throws SQLException
     */
//#ifdef JAVA2

    public void registerOutParameter(int paramIndex, int sqlType,
				     String typeName) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Adds a set of parameters to the batch.
     *
     * @exception SQLException if a database access error occurs
     * @see Statement#addBatch
     */

//#ifdef JAVA2
    public void addBatch() throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Sets the designated parameter to a java.sql.Date value,
     * using the given <code>Calendar</code> object.  The driver uses
     * the <code>Calendar</code> object to construct an SQL DATE,
     * which the driver then sends to the database.  With a
     * a <code>Calendar</code> object, the driver can calculate the date
     * taking into account a custom timezone and locale.  If no
     * <code>Calendar</code> object is specified, the driver uses the default
     * timezone and locale.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @param cal the <code>Calendar</code> object the driver will use
     * to construct the date
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public void setDate(int parameterIndex, java.sql.Date x,
			Calendar cal) throws SQLException {
	setDate(parameterIndex, x);
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Sets the designated parameter to a java.sql.Time value,
     * using the given <code>Calendar</code> object.  The driver uses
     * the <code>Calendar</code> object to construct an SQL TIME,
     * which the driver then sends to the database.  With a
     * a <code>Calendar</code> object, the driver can calculate the time
     * taking into account a custom timezone and locale.  If no
     * <code>Calendar</code> object is specified, the driver uses the default
     * timezone and locale.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @param cal the <code>Calendar</code> object the driver will use
     * to construct the time
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public void setTime(int parameterIndex, Time x,
			Calendar cal) throws SQLException {
	setTime(parameterIndex, x);
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Sets the designated parameter to a java.sql.Timestamp value,
     * using the given <code>Calendar</code> object.  The driver uses
     * the <code>Calendar</code> object to construct an SQL TIMESTAMP,
     * which the driver then sends to the database.  With a
     * a <code>Calendar</code> object, the driver can calculate the timestamp
     * taking into account a custom timezone and locale.  If no
     * <code>Calendar</code> object is specified, the driver uses the default
     * timezone and locale.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param x the parameter value
     * @param cal the <code>Calendar</code> object the driver will use
     * to construct the timestamp
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public void setTimestamp(int parameterIndex, Timestamp x,
			     Calendar cal) throws SQLException {
	setTimestamp(parameterIndex, x);
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Sets the designated parameter to SQL NULL.  This version of setNull should
     * be used for user-named types and REF type parameters.  Examples
     * of user-named types include: STRUCT, DISTINCT, JAVA_OBJECT, and
     * named array types.
     *
     * <P><B>Note:</B> To be portable, applications must give the
     * SQL type code and the fully-qualified SQL type name when specifying
     * a NULL user-defined or REF parameter.  In the case of a user-named type
     * the name is the type name of the parameter itself.  For a REF
     * parameter the name is the type name of the referenced type.  If
     * a JDBC driver does not need the type code or type name information,
     * it may ignore it.
     *
     * Although it is intended for user-named and Ref parameters,
     * this method may be used to set a null parameter of any JDBC type.
     * If the parameter does not have a user-named or REF type, the given
     * typeName is ignored.
     *
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param sqlType a value from java.sql.Types
     * @param typeName the fully-qualified name of an SQL user-named type,
     * ignored if the parameter is not a user-named type or REF
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public void setNull(int parameterIndex, int sqlType,
			String typeName) throws SQLException {
	setNull(parameterIndex, sqlType);
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Sets the designated parameter to the given <code>Reader</code>
     * object, which is the given number of characters long.
     * When a very large UNICODE value is input to a LONGVARCHAR
     * parameter, it may be more practical to send it via a
     * java.io.Reader. JDBC will read the data from the stream
     * as needed, until it reaches end-of-file.  The JDBC driver will
     * do any necessary conversion from UNICODE to the database char format.
     *
     * <P><B>Note:</B> This stream object can either be a standard
     * Java stream object or your own subclass that implements the
     * standard interface.
     *
     * @param parameterIndex the first parameter is 1, the second is 2, ...
     * @param reader the java reader which contains the UNICODE data
     * @param length the number of characters in the stream
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public void setCharacterStream(int parameterIndex, Reader reader,
				   int length) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Sets a REF(&lt;structured-type&gt;) parameter.
     *
     * @param i the first parameter is 1, the second is 2, ...
     * @param x an object representing data of an SQL REF Type
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public void setRef(int i, Ref x) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Sets a BLOB parameter.
     *
     * @param i the first parameter is 1, the second is 2, ...
     * @param x an object representing a BLOB
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public void setBlob(int i, Blob x) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Sets a CLOB parameter.
     *
     * @param i the first parameter is 1, the second is 2, ...
     * @param x an object representing a CLOB
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public void setClob(int i, Clob x) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Sets an Array parameter.
     *
     * @param i the first parameter is 1, the second is 2, ...
     * @param x an object representing an SQL array
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public void setArray(int i, Array x) throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Gets the number, types and properties of a ResultSet's columns.
     *
     * @return the description of a ResultSet's columns
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public ResultSetMetaData getMetaData() throws SQLException {
	throw getNotSupported();
    }

//#endif JAVA2

    /**
     * Constructor declaration
     *
     *
     * @param c
     * @param s
     */
    jdbcPreparedStatement(jdbcConnection c, String s) {
	super(c);

	sSql = s;
	vParameter = new Vector();
    }

    /**
     * Method declaration
     *
     *
     * @param parameterIndex
     * @param x
     * @param type
     *
     * @throws SQLException
     */
    private void setObjectInType(int parameterIndex, Object x,
				 int type) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (x == null) {
	    setNull(parameterIndex);

	    return;
	}

	switch (type) {

	case Types.BIT:
	    setBoolean(parameterIndex, ((Boolean) x).booleanValue());

	    break;

	case Types.TINYINT:
	    setByte(parameterIndex, ((Byte) x).byteValue());

	    break;

	case Types.SMALLINT:
	    setShort(parameterIndex, ((Short) x).shortValue());

	    break;

	case Types.INTEGER:
	    setInt(parameterIndex, ((Integer) x).intValue());

	    break;

	case Types.BIGINT:
	    setLong(parameterIndex, ((Long) x).longValue());

	    break;

	case Types.REAL:
	    setFloat(parameterIndex, ((Float) x).floatValue());

	    break;

	case Types.FLOAT:

	case Types.DOUBLE:
	    setDouble(parameterIndex, ((Double) x).doubleValue());

	    break;

	case Types.NUMERIC:
	    setBigDecimal(parameterIndex, (BigDecimal) x);

	    break;

	case Types.CHAR:

	case Types.VARCHAR:

	case Types.LONGVARCHAR:
	    setString(parameterIndex, (String) x);

	    break;

	case Types.DATE:
	    setDate(parameterIndex, (java.sql.Date) x);

	    break;

	case Types.TIME:
	    setTime(parameterIndex, (Time) x);

	    break;

	case Types.TIMESTAMP:
	    setTimestamp(parameterIndex, (Timestamp) x);

	    break;

	case Types.BINARY:

	case Types.VARBINARY:

// fredt@users.sourceforge.net 20010707
// bug 425495 by emorning@users.sourceforge.net
// to allow class cast when SQL type is used with setObject() for prepareStatement()
	case Types.LONGVARBINARY:
            if (x instanceof ByteArray)
                x= ((ByteArray)x).byteValue();
	    setBytes(parameterIndex, (byte[]) x);

	    break;

	case Types.OTHER:
	    setParameter(parameterIndex,
			 "'" + ByteArray.serializeToString(x) + "'");

	    break;

	default:
	    setParameter(parameterIndex, x.toString());

	    break;
	}
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    private SQLException getNotSupported() {
	return Trace.error(Trace.FUNCTION_NOT_SUPPORTED);
    }

}
