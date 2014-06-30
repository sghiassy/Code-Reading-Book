/*
 * jdbcConnection.java
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
import java.util.*;
import java.io.*;
import java.net.*;

/**
 * <P>A connection (session) with a specific
 * database. Within the context of a Connection, SQL statements are
 * executed and results are returned.
 *
 * <P>A Connection's database is able to provide information
 * describing its tables, its supported SQL grammar, its stored
 * procedures, the capabilities of this connection, and so on. This
 * information is obtained with the <code>getMetaData</code> method.
 *
 * <P><B>Note:</B> By default the Connection automatically commits
 * changes after executing each statement. If auto commit has been
 * disabled, an explicit commit must be done or database changes will
 * not be saved.
 * <P><font color="#009900">
 * To connect to a HSQL database, the following code may be used:
 * <pre>
 * Class.forName("org.hsqldb.jdbcDriver");
 * Connection c=DriverManager.getConnection(url,user,password);
 * </pre>
 * For HSQL, the url must start with jdbc:hsqldb.
 * The url for an In-Memory database is <code>jdbc:hsqldb:.</code>
 * The url for a standalone database is jdbc:hsqldb:name
 * where name is the filename of the database, including path. For example:
 * <code>jdbc:hsqldb:test</code> will connect to a database called
 * 'test', with the files test.properties, test.data and test.script.
 * For more information about the internal file structure of HSQL,
 * please read the documentation, page 'Files'.
 * </font><P>
 * @see jdbcStatement
 * @see jdbcResultSet
 * @see jdbcDatabaseMetaData
 */
public class jdbcConnection implements Connection {
    private boolean	     bClosed;
    private boolean	     bAutoCommit;
    private String	     sDatabaseName;
    private final static int HTTP = 0, STANDALONE = 1, INTERNAL = 2, HSQL = 3;
    private int		     iType;
    private Database	 dDatabase;
    private Channel	     cChannel;
    Socket		    	 sSocket;
    DataOutputStream	 dOutput;
    DataInputStream	     dInput;
    public static final int DEFAULT_HSQL_PORT = 9001;

    private static Hashtable tDatabase = new Hashtable();
    private static Hashtable iUsageCount = new Hashtable();

    /**
     * Creates a <code>Statement</code> object for sending
     * SQL statements to the database.
     * SQL statements without parameters are normally
     * executed using Statement objects.
     *
     * @return a new Statement object
     */
    public Statement createStatement() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (bClosed) {
	    return null;
	}

	return new jdbcStatement(this);
    }

    /**
     * Creates a <code>PreparedStatement</code> object for sending
     * parameterized SQL statements to the database.
     *
     * A SQL statement with or without IN parameters can be stored in
     * a PreparedStatement object. This object can then be used to execute
     * this statement multiple times.
     * <P><font color="#009900">
     * In HSQL, the statement is not sent to the database until
     * the <code>PreparedStatement</code> is executed. Other drivers may send
     * the statment to the database when this function is called. This has
     * no direct effect on users; however, it does affect which method throws
     * certain SQLExceptions.
     * </font><P>
     * @param sql a SQL statement that may contain one or more '?' IN
     * parameter placeholders
     * @return a new PreparedStatement object containing the
     * pre-compiled statement
     */
    public PreparedStatement prepareStatement(String sql) {
	if (Trace.TRACE) {
	    Trace.trace(sql);
	}

	if (bClosed) {
	    return null;
	}

	return new jdbcPreparedStatement(this, sql);
    }

    /**
     * Creates a <code>CallableStatement</code> object for calling
     * database stored procedures.
     * The CallableStatement provides
     * methods for setting up its IN and OUT parameters, and
     * methods for executing the call to a stored procedure.
     *
     * <P><B>Note:</B> This method is optimized for handling stored
     * procedure call statements.
     * <P><font color="#009900">
     * HSQL does not send the call
     * statement to the database when the method <code>prepareCall</code>
     * is done; it waits until the CallableStatement is executed. This has
     * no direct effect on users; however, it does affect which method
     * throws certain SQLExceptions.
     * </font><P>
     * @param sql a SQL statement that may contain one or more '?'
     * parameter placeholders. Typically this  statement is a JDBC
     * function call escape string.
     * @return a new CallableStatement object containing the
     * pre-compiled SQL statement
     */
    public CallableStatement prepareCall(String sql) {
	if (Trace.TRACE) {
	    Trace.trace(sql);
	}

	if (bClosed) {
	    return null;
	}

	return new jdbcPreparedStatement(this, sql);
    }

    /**
     * Converts the given SQL statement into the system's native SQL grammar.
     * <P><font color="#009900">
     * HSQL does convert the JDBC sql grammar into its system's
     * native SQL grammar prior to sending it; this method returns the
     * native form of the statement that the driver would have sent.
     * </font><P>
     * @param sql a SQL statement that may contain one or more '?'
     * parameter placeholders
     * @return the native form of this statement
     */
    public String nativeSQL(String sql) {
	if (sql.indexOf('{') == -1) {
	    return sql;
	}

	char    s[] = sql.toCharArray();
	boolean changed = false;
	int     state = 0;
	int     len = s.length;

	for (int i = 0; i < len; i++) {
	    char c = s[i];

	    switch (state) {

	    case 0:    // normal
		if (c == '\'') {
		    state = 1;
		} else if (c == '"') {
		    state = 2;
		} else if (c == '{') {
		    s[i] = ' ';
		    changed = true;

		    String sub = sql.substring(i + 1).toUpperCase();

		    if (sub.startsWith("?=")) {
			i += 2;
		    } else if (sub.startsWith("CALL")) {
			i += 4;
		    } else if (sub.startsWith("ESCAPE")) {
			i += 6;
		    }

		    state = 3;
		}

		break;

	    case 1:    // inside ' '

	    case 5:    // inside { } and ' '
		if (c == '\'') {
		    state -= 1;
		}

		break;

	    case 2:    // inside " "

	    case 6:    // inside { } and " "
		if (c == '"') {
		    state -= 2;
		}

		break;

	    case 3:    // inside { } before whitespace
		if (c == ' ') {
		    state = 4;
		} else {
		    s[i] = ' ';
		    changed = true;
		}

		break;

	    case 4:    // inside { } after whitespace
		if (c == '\'') {
		    state = 5;
		} else if (c == '"') {
		    state = 6;
		} else if (c == '}') {
		    s[i] = ' ';
		    changed = true;
		    state = 0;
		}
	    }
	}

	if (changed) {
	    sql = new String(s);

	    if (Trace.TRACE) {
		Trace.trace(s + " > " + sql);
	    }
	}

	return sql;
    }

    /**
     * Sets this connection's auto-commit mode.
     * If a connection is in auto-commit mode, then all its SQL
     * statements will be executed and committed as individual
     * transactions.  Otherwise, its SQL statements are grouped into
     * transactions that are terminated by a call to either
     * the method <code>commit</code> or the method <code>rollback</code>.
     * By default, new connections are in auto-commit
     * mode.
     *
     * The commit occurs when the statement completes or the next
     * execute occurs, whichever comes first. In the case of
     * statements returning a ResultSet, the statement completes when
     * the last row of the ResultSet has been retrieved or the
     * ResultSet has been closed. In advanced cases, a single
     * statement may return multiple results as well as output
     * parameter values. In these cases the commit occurs when all results and
     * output parameter values have been retrieved.
     *
     * @param autoCommit true enables auto-commit; false disables
     * auto-commit.
     * @exception SQLException if a database access error occurs
     */
    public void setAutoCommit(boolean autoCommit) throws SQLException {
	bAutoCommit = autoCommit;

	execute("SET AUTOCOMMIT " + (bAutoCommit ? "TRUE" : "FALSE"));
    }

    /**
     * Gets the current auto-commit state.
     *
     * @return the current state of auto-commit mode
     * @see #setAutoCommit
     */
    public boolean getAutoCommit() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return bAutoCommit;
    }

    /**
     * Makes all changes made since the previous
     * commit/rollback permanent and releases any database locks
     * currently held by the Connection. This method should be
     * used only when auto-commit mode has been disabled.
     *
     * @exception SQLException if a database access error occurs
     * @see #setAutoCommit
     */
    public void commit() throws SQLException {
	execute("COMMIT");
    }

    /**
     * Drops all changes made since the previous
     * commit/rollback and releases any database locks currently held
     * by this Connection. This method should be used only when auto-
     * commit has been disabled.
     *
     * @exception SQLException if a database access error occurs
     * @see #setAutoCommit
     */
    public void rollback() throws SQLException {
	execute("ROLLBACK");
    }

    /**
     * Releases a Connection's database and JDBC resources
     * immediately instead of waiting for
     * them to be automatically released.
     *
     * <P><B>Note:</B> A Connection is automatically closed when it is
     * garbage collected.
     *
     * @exception SQLException if a database access error occurs
     */
    public void close() throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	if (bClosed) {
	    return;
	}

	if ((iType == STANDALONE) || (iType == INTERNAL)) {
	    closeStandalone();
	} else {
	    execute("DISCONNECT");
	}

	bClosed = true;
    }

    /**
     * Tests to see if a Connection is closed.
     *
     * @return true if the connection is closed; false if it's still open
     */
    public boolean isClosed() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return bClosed;
    }

    /**
     * Gets the metadata regarding this connection's database.
     * A Connection's database is able to provide information
     * describing its tables, its supported SQL grammar, its stored
     * procedures, the capabilities of this connection, and so on. This
     * information is made available through a DatabaseMetaData
     * object.
     *
     * @return a DatabaseMetaData object for this Connection
     */
    public DatabaseMetaData getMetaData() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return new jdbcDatabaseMetaData(this);
    }

    /**
     * Puts this connection in read-only mode as a hint to enable
     * database optimizations.
     *
     * <P><B>Note:</B> This method should not be called while in the
     * middle of a transaction.
     * <P><font color="#009900">
     * HSQL will in this case commit the transaction automatically.
     * <P>
     * In HSQL, additionally the whole database can be put in
     * read-only mode by manually adding the line 'readonly=true' to the
     * .properties file. All connections are then automatically readonly.
     * The database files will then be opened in readonly mode, and it is
     * thus possible to create a CD with this database.
     * </font><P>
     * @param readOnly true enables read-only mode; false disables
     * read-only mode.
     * @exception SQLException if a database access error occurs
     */
    public void setReadOnly(boolean readonly) throws SQLException {
	execute("SET READONLY " + (readonly ? "TRUE" : "FALSE"));
    }

    /**
     * Tests to see if the connection is in read-only mode.
     *
     * @return true if connection is read-only and false otherwise
     * @exception SQLException if a database access error occurs
     */
    public boolean isReadOnly() throws SQLException {
	String    s =
	    "SELECT * FROM SYSTEM_CONNECTIONINFO WHERE KEY='READONLY'";
	ResultSet r = execute(s);

	r.next();

	return r.getString(2).equals("TRUE");
    }

    /**
     * Sets a catalog name in order to select
     * a subspace of this Connection's database in which to work.
     * <P><font color="#009900">
     * HSQL does not support catalogs and ignores
     * this request.
     * </font><P>
     */
    public void setCatalog(String catalog) {
	if (Trace.TRACE) {
	    Trace.trace(catalog);
	}
    }

    /**
     * Returns the Connection's current catalog name.
     *
     * @return the current catalog name or null
     */
    public String getCatalog() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return null;
    }

    /**
     * Attempts to change the transaction
     * isolation level to the one given.
     * The constants defined in the interface <code>Connection</code>
     * are the possible transaction isolation levels.
     *
     * <P><B>Note:</B> This method should not be called while
     * in the middle of a transaction.
     * <P><font color="#009900">
     * HSQL currently ignores this call.
     * </font><P>
     * @param level one of the TRANSACTION_* isolation values with the
     * exception of TRANSACTION_NONE; some databases may not support
     * other values
     * @see DatabaseMetaData#supportsTransactionIsolationLevel
     */
    public void setTransactionIsolation(int level) {
	if (Trace.TRACE) {
	    Trace.trace(level);
	}
    }

    /**
     * Gets this Connection's current transaction isolation level.
     *
     * @return the current TRANSACTION_* mode value
     */
    public int getTransactionIsolation() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return Connection.TRANSACTION_READ_UNCOMMITTED;
    }

    /**
     * Returns the first warning reported by calls on this Connection.
     * <P><font color="#009900">
     * HSQL never produces warnings and returns always null.
     * </font><P>
     * @return the first SQLWarning or null
     */
    public SQLWarning getWarnings() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return null;
    }

    /**
     * Clears all warnings reported for this <code>Connection</code> object.
     * After a call to this method, the method <code>getWarnings</code>
     * returns null until a new warning is
     * reported for this Connection.
     */
    public void clearWarnings() {
	if (Trace.TRACE) {
	    Trace.trace();
	}
    }

    /**
     * JDBC 2.0
     *
     * Creates a <code>Statement</code> object that will generate
     * <code>ResultSet</code> objects with the given type and concurrency.
     * This method is the same as the <code>createStatement</code> method
     * above, but it allows the default result set
     * type and result set concurrency type to be overridden.
     * <P><font color="#009900">
     * HSQL currently supports type TYPE_FORWARD_ONLY and
     * concurrency CONCUR_READ_ONLY.
     * </font><P>
     * @param resultSetType a result set type; see ResultSet.TYPE_XXX
     * @param resultSetConcurrency a concurrency type; see ResultSet.CONCUR_XXX
     * @return a new Statement object
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public Statement createStatement(int type,
				     int concurrency) throws SQLException {
	return createStatement();
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Creates a <code>PreparedStatement</code> object that will generate
     * <code>ResultSet</code> objects with the given type and concurrency.
     * This method is the same as the <code>prepareStatement</code> method
     * above, but it allows the default result set
     * type and result set concurrency type to be overridden.
     * <P><font color="#009900">
     * HSQL currently supports type TYPE_FORWARD_ONLY and
     * concurrency CONCUR_READ_ONLY.
     * </font><P>
     * @param resultSetType a result set type; see ResultSet.TYPE_XXX
     * @param resultSetConcurrency a concurrency type; see ResultSet.CONCUR_XXX
     * @return a new PreparedStatement object containing the
     * pre-compiled SQL statement
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public PreparedStatement prepareStatement(String sql, int type,
					      int concurrency) throws SQLException {
	return prepareStatement(sql);
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Creates a <code>CallableStatement</code> object that will generate
     * <code>ResultSet</code> objects with the given type and concurrency.
     * This method is the same as the <code>prepareCall</code> method
     * above, but it allows the default result set
     * type and result set concurrency type to be overridden.
     * <P><font color="#009900">
     * HSQL currently supports type TYPE_FORWARD_ONLY and
     * concurrency CONCUR_READ_ONLY.
     * </font><P>
     * @param resultSetType a result set type; see ResultSet.TYPE_XXX
     * @param resultSetConcurrency a concurrency type; see ResultSet.CONCUR_XXX
     * @return a new CallableStatement object containing the
     * pre-compiled SQL statement
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public CallableStatement prepareCall(String sql, int resultSetType,
					 int resultSetConcurrency) throws SQLException {
	return prepareCall(sql);
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Gets the type map object associated with this connection.
     * Unless the application has added an entry to the type map,
     * the map returned will be empty.
     *
     * @return the <code>java.util.Map</code> object associated
     * with this <code>Connection</code> object
     */

//#ifdef JAVA2
    public Map getTypeMap() throws SQLException {

	// todo
	return new HashMap(0);
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Installs the given type map as the type map for
     * this connection.  The type map will be used for the
     * custom mapping of SQL structured types and distinct types.
     *
     * @param the <code>java.util.Map</code> object to install
     * as the replacement for this <code>Connection</code>
     * object's default type map
     */

//#ifdef JAVA2
    public void setTypeMap(Map map) throws SQLException {
	throw Trace.error(Trace.FUNCTION_NOT_SUPPORTED);
    }

//#endif JAVA2

    /**
     * Constructor declaration
     *
     *
     * @param s
     * @param user
     * @param password
     */
    jdbcConnection(String s, String user,
		   String password) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace(s);
	}

	bAutoCommit = true;
	sDatabaseName = s;

	if (s.toUpperCase().startsWith("HTTP://")) {
	    iType = HTTP;

	    openHTTP(user, password);
	} else if (s.toUpperCase().startsWith("HSQL://")) {
	    iType = HSQL;

	    openHSQL(user, password);
	} else {
	    iType = STANDALONE;

	    openStandalone(user, password);
	}
    }

    /**
     * Constructor declaration
     *
     *
     * @param c
     */
    jdbcConnection(Channel c) throws SQLException {
	iType = INTERNAL;
	cChannel = c;
	dDatabase = c.getDatabase();
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     *
     * @throws SQLException
     */
    jdbcResultSet execute(String s) throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace(s);
	}

	Trace.check(!bClosed, Trace.CONNECTION_IS_CLOSED);

	if (iType == HTTP) {
	    return executeHTTP(s);
	} else if (iType == HSQL) {
	    return executeHSQL(s);
	} else {
	    // internal and standalone
	    return executeStandalone(s);
	}
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean usesLocalFiles() {

	// Standalone connections do, HTTP connections not
	return iType != HTTP;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    String getName() {
	return sDatabaseName;
    }

    // HTTP
    private String      sConnect, sUser, sPassword;
    final static String ENCODING = "8859_1";

    /**
     * Method declaration
     *
     *
     * @param user
     * @param password
     *
     * @throws SQLException
     *
     * @see
     */
    private void openHTTP(String user, String password) throws SQLException {
	sConnect = sDatabaseName;
	sUser = user;
	sPassword = password;

	executeHTTP(" ");
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     *
     * @throws SQLException
     */
    private synchronized jdbcResultSet executeHTTP(String s)
	    throws SQLException {
	byte result[];

	try {
	    URL    url = new URL(sConnect);
	    String p = StringConverter.unicodeToHexString(sUser);

	    p += "+" + StringConverter.unicodeToHexString(sPassword);
	    p += "+" + StringConverter.unicodeToHexString(s);

	    URLConnection c = url.openConnection();

	    c.setDoOutput(true);

	    OutputStream os = c.getOutputStream();

	    os.write(p.getBytes(ENCODING));
	    os.close();
	    c.connect();

	    InputStream		is = (InputStream) c.getContent();
	    BufferedInputStream in = new BufferedInputStream(is);
	    int			len = c.getContentLength();

	    result = new byte[len];

	    for (int i = 0; i < len; i++) {
		int r = in.read();

		result[i] = (byte) r;
	    }
	} catch (Exception e) {
	    throw Trace.error(Trace.CONNECTION_IS_BROKEN, e.getMessage());
	}

	return new jdbcResultSet(new Result(result));
    }

    /**
     * Method declaration
     *
     *
     * @param user
     * @param password
     *
     * @throws SQLException
     */
    private void openHSQL(String user, String password) throws SQLException {

    sConnect=sDatabaseName.substring(7);
    sUser=user;
    sPassword=password;
    reconnectHSQL();

	sUser = user;
	sPassword = password;

	reconnectHSQL();
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    private void reconnectHSQL() throws SQLException {
	try {
        StringTokenizer st = new StringTokenizer(sConnect, ":" );
        String host = st.hasMoreTokens() ? st.nextToken() : "";
        int port = st.hasMoreTokens() ? Integer.parseInt( st.nextToken() ) : DEFAULT_HSQL_PORT;
        sSocket = new Socket( host, port );

	    sSocket.setTcpNoDelay(true);

	    dOutput =
		new DataOutputStream(new BufferedOutputStream(sSocket.getOutputStream()));
	    dInput =
		new DataInputStream(new BufferedInputStream(sSocket.getInputStream()));

	    dOutput.writeUTF(sUser);
	    dOutput.writeUTF(sPassword);
	    dOutput.flush();
	} catch (Exception e) {
	    throw Trace.error(Trace.CONNECTION_IS_BROKEN, e.getMessage());
	}
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     *
     * @throws SQLException
     */
    private synchronized jdbcResultSet executeHSQL(String s)
	    throws SQLException {
	byte result[];

	try {
	    dOutput.writeUTF(s);
	    dOutput.flush();

	    int len = dInput.readInt();

	    result = new byte[len];

	    int p = 0;

	    while (true) {
		int l = dInput.read(result, p, len);

		if (l == len) {
		    break;
		} else {
		    len -= l;
		    p += l;
		}
	    }
	} catch (Exception e) {
	    throw Trace.error(Trace.CONNECTION_IS_BROKEN, e.getMessage());
	}

	return new jdbcResultSet(new Result(result));
    }

    /**
     * Method declaration
     *
     *
     * @param user
     * @param password
     *
     * @throws SQLException
     */
    private synchronized void openStandalone(String user, String password)
	    throws SQLException {
	dDatabase = (Database) tDatabase.get(sDatabaseName);

	int usage;

	if (dDatabase == null) {
	    dDatabase = new Database(sDatabaseName);

	    tDatabase.put(sDatabaseName, dDatabase);

	    usage = 1;
	} else {
	    usage = 1 + ((Integer) iUsageCount.get(sDatabaseName)).intValue();
	}

	iUsageCount.put(sDatabaseName, new Integer(usage));

	cChannel = dDatabase.connect(user, password);
    }

    /**
     * Method declaration
     *
     */
    public void finalize() {
	try {
	    close();
	} catch (SQLException e) {}
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    private synchronized void closeStandalone() throws SQLException {
	Integer i = (Integer) iUsageCount.get(sDatabaseName);

	if (i == null) {

	    // It was already closed - ignore it.
	    return;
	}

	int usage = i.intValue() - 1;

	if (usage == 0) {
	    iUsageCount.remove(sDatabaseName);
	    tDatabase.remove(sDatabaseName);

	    if (!dDatabase.isShutdown()) {
		execute("SHUTDOWN");
	    }

	    dDatabase = null;
	    cChannel = null;
	} else {
	    iUsageCount.put(sDatabaseName, new Integer(usage));
	    execute("DISCONNECT");
	}
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     *
     * @throws SQLException
     */
    private jdbcResultSet executeStandalone(String s) throws SQLException {
	return new jdbcResultSet(dDatabase.execute(s, cChannel));
    }

}
