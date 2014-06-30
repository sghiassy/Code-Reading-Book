/*
 * jdbcDatabaseMetaData.java
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

/**
 * Comprehensive information about the database as a whole.
 *
 * <P>Many of the methods here return lists of information in
 * the form of <code>ResultSet</code> objects.
 * You can use the normal ResultSet methods such as getString and getInt
 * to retrieve the data from these ResultSets. If a given form of
 * metadata is not available, these methods should throw an SQLException.
 *
 * <P>Some of these methods take arguments that are String patterns.  These
 * arguments all have names such as fooPattern.  Within a pattern String, "%"
 * means match any substring of 0 or more characters, and "_" means match
 * any one character. Only metadata entries matching the search pattern
 * are returned. If a search pattern argument is set to a null ref,
 * that argument's criteria will be dropped from the search.
 *
 * <P>An <code>SQLException</code> will be thrown if a driver does not support
 * a meta data method.  In the case of methods that return a ResultSet,
 * either a ResultSet (which may be empty) is returned or a
 * SQLException is thrown.
 */
public class jdbcDatabaseMetaData implements DatabaseMetaData {
    private jdbcConnection cConnection;

    /**
     * Constructor declaration
     *
     *
     * @param c
     */
    jdbcDatabaseMetaData(jdbcConnection c) {
	cConnection = c;
    }

    /**
     * Can all the procedures returned by getProcedures be called by the
     * current user?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean allProceduresAreCallable() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Can all the tables returned by getTable be SELECTed by the
     * current user?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean allTablesAreSelectable() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * What's the url for this database?
     *
     * @return the url
     */
    public String getURL() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return "jdbc:hsqldb:" + cConnection.getName();
    }

    /**
     * What's our user name as known to the database?
     *
     * @return our database user name
     * @exception SQLException if a database access error occurs
     */
    public String getUserName() throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	ResultSet r = executeSelect("SYSTEM_CONNECTIONINFO", "KEY='USER'");

	r.next();

	return r.getString(2);
    }

    /**
     * Is the database in read-only mode?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     * @exception SQLException if a database access error occurs
     */
    public boolean isReadOnly() throws SQLException {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return cConnection.isReadOnly();
    }

    /**
     * Are NULL values sorted high?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean nullsAreSortedHigh() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Are NULL values sorted low?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean nullsAreSortedLow() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Are NULL values sorted at the start regardless of sort order?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean nullsAreSortedAtStart() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Are NULL values sorted at the end regardless of sort order?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean nullsAreSortedAtEnd() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * What's the name of this database product?
     *
     * @return database product name
     */
    public String getDatabaseProductName() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return jdbcDriver.PRODUCT;
    }

    /**
     * What's the version of this database product?
     *
     * @return database version
     */
    public String getDatabaseProductVersion() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return jdbcDriver.VERSION;
    }

    /**
     * What's the name of this JDBC driver?
     *
     * @return JDBC driver name
     */
    public String getDriverName() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return jdbcDriver.PRODUCT + " Driver";
    }

    /**
     * What's the version of this JDBC driver?
     *
     * @return JDBC driver version
     */
    public String getDriverVersion() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return jdbcDriver.VERSION;
    }

    /**
     * What's this JDBC driver's major version number?
     *
     * @return JDBC driver major version
     */
    public int getDriverMajorVersion() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return jdbcDriver.MAJOR;
    }

    /**
     * What's this JDBC driver's minor version number?
     *
     * @return JDBC driver minor version number
     */
    public int getDriverMinorVersion() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return jdbcDriver.MINOR;
    }

    /**
     * Does the database store tables in a local file?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean usesLocalFiles() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return cConnection.usesLocalFiles();
    }

    /**
     * Does the database use a file for each table?
     *
     * @return true if the database uses a local file for each table
     */
    public boolean usesLocalFilePerTable() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Does the database treat mixed case unquoted SQL identifiers as
     * case sensitive and as a result store them in mixed case?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver will
     * always return false.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsMixedCaseIdentifiers() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Does the database treat mixed case unquoted SQL identifiers as
     * case insensitive and store them in upper case?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean storesUpperCaseIdentifiers() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Does the database treat mixed case unquoted SQL identifiers as
     * case insensitive and store them in lower case?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean storesLowerCaseIdentifiers() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Does the database treat mixed case unquoted SQL identifiers as
     * case insensitive and store them in mixed case?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean storesMixedCaseIdentifiers() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Does the database treat mixed case quoted SQL identifiers as
     * case sensitive and as a result store them in mixed case?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver will
     * always return true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsMixedCaseQuotedIdentifiers() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;

	// InterBase (iscdrv32.dll) returns false
    }

    /**
     * Does the database treat mixed case quoted SQL identifiers as
     * case insensitive and store them in upper case?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean storesUpperCaseQuotedIdentifiers() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Does the database treat mixed case quoted SQL identifiers as
     * case insensitive and store them in lower case?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean storesLowerCaseQuotedIdentifiers() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Does the database treat mixed case quoted SQL identifiers as
     * case insensitive and store them in mixed case?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean storesMixedCaseQuotedIdentifiers() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;

	// No: as case sensitive.
    }

    /**
     * What's the string used to quote SQL identifiers?
     * This returns a space " " if identifier quoting isn't supported.
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup>
     * driver always uses a double quote character.
     *
     * @return the quoting string
     */
    public String getIdentifierQuoteString() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return "\"";

	// InterBase (iscdrv32.dll) returns ""
    }

    /**
     * Gets a comma-separated list of all a database's SQL keywords
     * that are NOT also SQL92 keywords.
     *
     * @return the list
     */
    public String getSQLKeywords() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return "";
    }

    /**
     * Gets a comma-separated list of math functions.  These are the
     * X/Open CLI math function names used in the JDBC function escape
     * clause.
     *
     * @return the list
     */
    public String getNumericFunctions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return getList(Library.sNumeric);
    }

    /**
     * Gets a comma-separated list of string functions.  These are the
     * X/Open CLI string function names used in the JDBC function escape
     * clause.
     *
     * @return the list
     */
    public String getStringFunctions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return getList(Library.sString);
    }

    /**
     * Gets a comma-separated list of system functions.  These are the
     * X/Open CLI system function names used in the JDBC function escape
     * clause.
     *
     * @return the list
     */
    public String getSystemFunctions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return getList(Library.sSystem);
    }

    /**
     * Gets a comma-separated list of time and date functions.
     *
     * @return the list
     */
    public String getTimeDateFunctions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return getList(Library.sTimeDate);
    }

    /**
     * Gets the string that can be used to escape wildcard characters.
     * This is the string that can be used to escape '_' or '%' in
     * the string pattern style catalog search parameters.
     *
     * <P>The '_' character represents any single character.
     * <P>The '%' character represents any sequence of zero or
     * more characters.
     *
     * @return the string used to escape wildcard characters
     */
    public String getSearchStringEscape() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return "\\";
    }

    /**
     * Gets all the "extra" characters that can be used in unquoted
     * identifier names (those beyond a-z, A-Z, 0-9 and _).
     *
     * @return the string containing the extra characters
     */
    public String getExtraNameCharacters() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return "";
    }

    /**
     * Is "ALTER TABLE" with add column supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsAlterTableWithAddColumn() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Is "ALTER TABLE" with drop column supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsAlterTableWithDropColumn() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Is column aliasing supported?
     *
     * <P>If so, the SQL AS clause can be used to provide names for
     * computed columns or to provide alias names for columns as
     * required.
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver
     * always returns true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsColumnAliasing() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;

	// InterBase (iscdrv32.dll) returns false
    }

    /**
     * Are concatenations between NULL and non-NULL values NULL?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver always
     * returns true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean nullPlusNonNullIsNull() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;

	// Access (odbcjt32.dll) returns false
    }

    /**
     * Is the CONVERT function between SQL types supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsConvert() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Is CONVERT between the given SQL types supported?
     * <P><font color="#009900">
     * In HSQL Database Engine, conversions are made over Strings, so
     * everything theoretically is possible.
     * </font><P>
     * @param fromType the type to convert from
     * @param toType the type to convert to
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsConvert(int fromType, int toType) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Are table correlation names supported?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver
     * always returns true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsTableCorrelationNames() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * If table correlation names are supported, are they restricted
     * to be different from the names of the tables?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsDifferentTableCorrelationNames() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Are expressions in "ORDER BY" lists supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsExpressionsInOrderBy() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Can an "ORDER BY" clause use columns not in the SELECT statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsOrderByUnrelated() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Is some form of "GROUP BY" clause supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsGroupBy() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Can a "GROUP BY" clause use columns not in the SELECT?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsGroupByUnrelated() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Can a "GROUP BY" clause add columns not in the SELECT
     * provided it specifies all the columns in the SELECT?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsGroupByBeyondSelect() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Is the escape character in "LIKE" clauses supported?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver
     * always returns true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsLikeEscapeClause() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Are multiple ResultSets from a single execute supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsMultipleResultSets() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can we have multiple transactions open at once (on different
     * connections)?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsMultipleTransactions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Can columns be defined as non-nullable?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver
     * always returns true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsNonNullableColumns() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Is the ODBC Minimum SQL grammar supported?
     *
     * All JDBC Compliant<sup><font size=-2>TM</font></sup> drivers
     * must return true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsMinimumSQLGrammar() {
	if (Trace.TRACE) {
	    Trace.trace();

	    // todo
	}

	return false;
    }

    /**
     * Is the ODBC Core SQL grammar supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsCoreSQLGrammar() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Is the ODBC Extended SQL grammar supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsExtendedSQLGrammar() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Is the ANSI92 entry level SQL grammar supported?
     *
     * All JDBC Compliant<sup><font size=-2>TM</font></sup> drivers must
     * return true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsANSI92EntryLevelSQL() {
	if (Trace.TRACE) {
	    Trace.trace();

	    // todo
	}

	return false;
    }

    /**
     * Is the ANSI92 intermediate SQL grammar supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsANSI92IntermediateSQL() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Is the ANSI92 full SQL grammar supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsANSI92FullSQL() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Is the SQL Integrity Enhancement Facility supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsIntegrityEnhancementFacility() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Is some form of outer join supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsOuterJoins() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Are full nested outer joins supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsFullOuterJoins() {
	if (Trace.TRACE) {
	    Trace.trace();

	    // todo
	}

	return false;
    }

    /**
     * Is there limited support for outer joins?  (This will be true
     * if supportFullOuterJoins is true.)
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsLimitedOuterJoins() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * What's the database vendor's preferred term for "schema"?
     *
     * @return the vendor term
     */
    public String getSchemaTerm() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return "";
    }

    /**
     * What's the database vendor's preferred term for "procedure"?
     *
     * @return the vendor term
     */
    public String getProcedureTerm() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return "";
    }

    /**
     * What's the database vendor's preferred term for "catalog"?
     *
     * @return the vendor term
     */
    public String getCatalogTerm() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return "";
    }

    /**
     * Does a catalog appear at the start of a qualified table name?
     * (Otherwise it appears at the end)
     *
     * @return true if it appears at the start
     */
    public boolean isCatalogAtStart() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * What's the separator between catalog and table name?
     *
     * @return the separator string
     */
    public String getCatalogSeparator() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return "";
    }

    /**
     * Can a schema name be used in a data manipulation statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsSchemasInDataManipulation() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can a schema name be used in a procedure call statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsSchemasInProcedureCalls() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can a schema name be used in a table definition statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsSchemasInTableDefinitions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can a schema name be used in an index definition statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsSchemasInIndexDefinitions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can a schema name be used in a privilege definition statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsSchemasInPrivilegeDefinitions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can a catalog name be used in a data manipulation statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsCatalogsInDataManipulation() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can a catalog name be used in a procedure call statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsCatalogsInProcedureCalls() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can a catalog name be used in a table definition statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsCatalogsInTableDefinitions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can a catalog name be used in an index definition statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsCatalogsInIndexDefinitions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can a catalog name be used in a privilege definition statement?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsCatalogsInPrivilegeDefinitions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Is positioned DELETE supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsPositionedDelete() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Is positioned UPDATE supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsPositionedUpdate() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Is SELECT for UPDATE supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsSelectForUpdate() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Are stored procedure calls using the stored procedure escape
     * syntax supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsStoredProcedures() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Are subqueries in comparison expressions supported?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver
     * always returns true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsSubqueriesInComparisons() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Are subqueries in 'exists' expressions supported?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver
     * always returns true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsSubqueriesInExists() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Are subqueries in 'in' statements supported?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver
     * always returns true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsSubqueriesInIns() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Are subqueries in quantified expressions supported?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver
     * always returns true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsSubqueriesInQuantifieds() {
	if (Trace.TRACE) {
	    Trace.trace();

	    // todo: check if this is correct
	}

	return true;
    }

    /**
     * Are correlated subqueries supported?
     *
     * A JDBC Compliant<sup><font size=-2>TM</font></sup> driver
     * always returns true.
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsCorrelatedSubqueries() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Is SQL UNION supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsUnion() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Is SQL UNION ALL supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsUnionAll() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Can cursors remain open across commits?
     *
     * @return <code>true</code> if cursors always remain open;
     * <code>false</code> if they might not remain open
     */
    public boolean supportsOpenCursorsAcrossCommit() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can cursors remain open across rollbacks?
     *
     * @return <code>true</code> if cursors always remain open;
     * <code>false</code> if they might not remain open
     */
    public boolean supportsOpenCursorsAcrossRollback() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Can statements remain open across commits?
     *
     * @return <code>true</code> if statements always remain open;
     * <code>false</code> if they might not remain open
     */
    public boolean supportsOpenStatementsAcrossCommit() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Can statements remain open across rollbacks?
     *
     * @return <code>true</code> if statements always remain open;
     * <code>false</code> if they might not remain open
     */
    public boolean supportsOpenStatementsAcrossRollback() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * How many hex characters can you have in an inline binary literal?
     *
     * @return max binary literal length in hex characters;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxBinaryLiteralLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the max length for a character literal?
     *
     * @return max literal length;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxCharLiteralLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the limit on column name length?
     *
     * @return max column name length;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxColumnNameLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum number of columns in a "GROUP BY" clause?
     *
     * @return max number of columns;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxColumnsInGroupBy() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum number of columns allowed in an index?
     *
     * @return max number of columns;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxColumnsInIndex() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum number of columns in an "ORDER BY" clause?
     *
     * @return max number of columns;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxColumnsInOrderBy() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum number of columns in a "SELECT" list?
     *
     * @return max number of columns;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxColumnsInSelect() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum number of columns in a table?
     *
     * @return max number of columns;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxColumnsInTable() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * How many active connections can we have at a time to this database?
     *
     * @return max number of active connections;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxConnections() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum cursor name length?
     *
     * @return max cursor name length in bytes;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxCursorNameLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum length of an index (in bytes)?
     *
     * @return max index length in bytes;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxIndexLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum length allowed for a schema name?
     *
     * @return max name length in bytes;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxSchemaNameLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum length of a procedure name?
     *
     * @return max name length in bytes;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxProcedureNameLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum length of a catalog name?
     *
     * @return max name length in bytes;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxCatalogNameLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum length of a single row?
     *
     * @return max row size in bytes;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxRowSize() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * Did getMaxRowSize() include LONGVARCHAR and LONGVARBINARY
     * blobs?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean doesMaxRowSizeIncludeBlobs() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * What's the maximum length of a SQL statement?
     *
     * @return max length in bytes;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxStatementLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * How many active statements can we have open at one time to this
     * database?
     *
     * @return the maximum number of statements that can be open at one time;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxStatements() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum length of a table name?
     *
     * @return max name length in bytes;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxTableNameLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum number of tables in a SELECT statement?
     *
     * @return the maximum number of tables allowed in a SELECT statement;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxTablesInSelect() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the maximum length of a user name?
     *
     * @return max user name length  in bytes;
     * a result of zero means that there is no limit or the limit is not known
     */
    public int getMaxUserNameLength() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return 0;
    }

    /**
     * What's the database's default transaction isolation level?  The
     * values are defined in <code>java.sql.Connection</code>.
     *
     * @return the default isolation level
     * @see Connection
     */
    public int getDefaultTransactionIsolation() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return Connection.TRANSACTION_READ_UNCOMMITTED;
    }

    /**
     * Are transactions supported? If not, invoking the method
     * <code>commit</code> is a noop and the
     * isolation level is TRANSACTION_NONE.
     *
     * @return <code>true</code> if transactions are supported; <code>false</code> otherwise
     */
    public boolean supportsTransactions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Does this database support the given transaction isolation level?
     *
     * @param level the values are defined in <code>java.sql.Connection</code>
     * @return <code>true</code> if so; <code>false</code> otherwise
     * @see Connection
     */
    public boolean supportsTransactionIsolationLevel(int level) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return level == Connection.TRANSACTION_READ_UNCOMMITTED;
    }

    /**
     * Are both data definition and data manipulation statements
     * within a transaction supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsDataDefinitionAndDataManipulationTransactions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Are only data manipulation statements within a transaction
     * supported?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean supportsDataManipulationTransactionsOnly() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Does a data definition statement within a transaction force the
     * transaction to commit?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean dataDefinitionCausesTransactionCommit() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return true;
    }

    /**
     * Is a data definition statement within a transaction ignored?
     *
     * @return <code>true</code> if so; <code>false</code> otherwise
     */
    public boolean dataDefinitionIgnoredInTransactions() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

    /**
     * Gets a description of the stored procedures available in a
     * catalog.
     *
     * <P>Only procedure descriptions matching the schema and
     * procedure name criteria are returned.  They are ordered by
     * PROCEDURE_SCHEM, and PROCEDURE_NAME.
     *
     * <P>Each procedure description has the the following columns:
     * <OL>
     * <LI><B>PROCEDURE_CAT</B> String => procedure catalog (may be null)
     * <LI><B>PROCEDURE_SCHEM</B> String => procedure schema (may be null)
     * <LI><B>PROCEDURE_NAME</B> String => procedure name
     * <LI> reserved for future use
     * <LI> reserved for future use
     * <LI> reserved for future use
     * <LI><B>REMARKS</B> String => explanatory comment on the procedure
     * <LI><B>PROCEDURE_TYPE</B> short => kind of procedure:
     * <UL>
     * <LI> procedureResultUnknown - May return a result
     * <LI> procedureNoResult - Does not return a result
     * <LI> procedureReturnsResult - Returns a result
     * </UL>
     * </OL>
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schemaPattern a schema name pattern; "" retrieves those
     * without a schema
     * @param procedureNamePattern a procedure name pattern
     * @return ResultSet - each row is a procedure description
     * @exception SQLException if a database access error occurs
     * @see #getSearchStringEscape
     */
    public ResultSet getProcedures(String catalog, String schemaPattern,
				   String procedureNamePattern) throws SQLException {

	// currently no records for any pattern
	return executeSelect("SYSTEM_PROCEDURES", null);
    }

    /**
     * Gets a description of a catalog's stored procedure parameters
     * and result columns.
     *
     * <P>Only descriptions matching the schema, procedure and
     * parameter name criteria are returned.  They are ordered by
     * PROCEDURE_SCHEM and PROCEDURE_NAME. Within this, the return value,
     * if any, is first. Next are the parameter descriptions in call
     * order. The column descriptions follow in column number order.
     *
     * <P>Each row in the ResultSet is a parameter description or
     * column description with the following fields:
     * <OL>
     * <LI><B>PROCEDURE_CAT</B> String => procedure catalog (may be null)
     * <LI><B>PROCEDURE_SCHEM</B> String => procedure schema (may be null)
     * <LI><B>PROCEDURE_NAME</B> String => procedure name
     * <LI><B>COLUMN_NAME</B> String => column/parameter name
     * <LI><B>COLUMN_TYPE</B> Short => kind of column/parameter:
     * <UL>
     * <LI> procedureColumnUnknown - nobody knows
     * <LI> procedureColumnIn - IN parameter
     * <LI> procedureColumnInOut - INOUT parameter
     * <LI> procedureColumnOut - OUT parameter
     * <LI> procedureColumnReturn - procedure return value
     * <LI> procedureColumnResult - result column in ResultSet
     * </UL>
     * <LI><B>DATA_TYPE</B> short => SQL type from java.sql.Types
     * <LI><B>TYPE_NAME</B> String => SQL type name, for a UDT type the
     * type name is fully qualified
     * <LI><B>PRECISION</B> int => precision
     * <LI><B>LENGTH</B> int => length in bytes of data
     * <LI><B>SCALE</B> short => scale
     * <LI><B>RADIX</B> short => radix
     * <LI><B>NULLABLE</B> short => can it contain NULL?
     * <UL>
     * <LI> procedureNoNulls - does not allow NULL values
     * <LI> procedureNullable - allows NULL values
     * <LI> procedureNullableUnknown - nullability unknown
     * </UL>
     * <LI><B>REMARKS</B> String => comment describing parameter/column
     * </OL>
     *
     * <P><B>Note:</B> Some databases may not return the column
     * descriptions for a procedure. Additional columns beyond
     * REMARKS can be defined by the database.
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schemaPattern a schema name pattern; "" retrieves those
     * without a schema
     * @param procedureNamePattern a procedure name pattern
     * @param columnNamePattern a column name pattern
     * @return ResultSet - each row describes a stored procedure parameter or
     * column
     * @exception SQLException if a database access error occurs
     * @see #getSearchStringEscape
     */
    public ResultSet getProcedureColumns(String catalog,
					 String schemaPattern,
					 String procedureNamePattern,
					 String columnNamePattern) throws SQLException {

	// currently no records for any pattern
	return executeSelect("SYSTEM_PROCEDURECOLUMNS", null);
    }

    /**
     * Gets a description of tables available in a catalog.
     *
     * <P>Only table descriptions matching the catalog, schema, table
     * name and type criteria are returned.  They are ordered by
     * TABLE_TYPE, TABLE_SCHEM and TABLE_NAME.
     *
     * <P>Each table description has the following columns:
     * <OL>
     * <LI><B>TABLE_CAT</B> String => table catalog (may be null)
     * <LI><B>TABLE_SCHEM</B> String => table schema (may be null)
     * <LI><B>TABLE_NAME</B> String => table name
     * <LI><B>TABLE_TYPE</B> String => table type.  Typical types are "TABLE",
     * "VIEW",	"SYSTEM TABLE", "GLOBAL TEMPORARY",
     * "LOCAL TEMPORARY", "ALIAS", "SYNONYM".
     * <LI><B>REMARKS</B> String => explanatory comment on the table
     * </OL>
     *
     * <P><B>Note:</B> Some databases may not return information for
     * all tables.
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schemaPattern a schema name pattern; "" retrieves those
     * without a schema
     * @param tableNamePattern a table name pattern
     * @param types a list of table types to include; null returns all types
     * @return ResultSet - each row is a table description
     * @exception SQLException if a database access error occurs
     * @see #getSearchStringEscape
     */
    public ResultSet getTables(String catalog, String schemaPattern,
			       String tableNamePattern,
			       String[] types) throws SQLException {
	String table = "SYSTEM_TABLES";
	String where = null;

	if (tableNamePattern != null) {
	    where = "TABLE_NAME LIKE '" + tableNamePattern.toUpperCase()
		    + "'";
	}

	// todo: types pattern
	return executeSelect(table, where);
    }

    /**
     * Gets the schema names available in this database.  The results
     * are ordered by schema name.
     *
     * <P>The schema column is:
     * <OL>
     * <LI><B>TABLE_SCHEM</B> String => schema name
     * </OL>
     *
     * @return ResultSet - each row has a single String column that is a
     * schema name
     * @exception SQLException if a database access error occurs
     */
    public ResultSet getSchemas() throws SQLException {
	return executeSelect("SYSTEM_SCHEMAS", null);
    }

    /**
     * Gets the catalog names available in this database.  The results
     * are ordered by catalog name.
     *
     * <P>The catalog column is:
     * <OL>
     * <LI><B>TABLE_CAT</B> String => catalog name
     * </OL>
     *
     * @return ResultSet - each row has a single String column that is a
     * catalog name
     * @exception SQLException if a database access error occurs
     */
    public ResultSet getCatalogs() throws SQLException {
	return executeSelect("SYSTEM_CATALOGS", null);
    }

    /**
     * Gets the table types available in this database.  The results
     * are ordered by table type.
     *
     * <P>The table type is:
     * <OL>
     * <LI><B>TABLE_TYPE</B> String => table type.  Typical types are "TABLE",
     * "VIEW",	"SYSTEM TABLE", "GLOBAL TEMPORARY",
     * "LOCAL TEMPORARY", "ALIAS", "SYNONYM".
     * </OL>
     *
     * @return ResultSet - each row has a single String column that is a
     * table type
     * @exception SQLException if a database access error occurs
     */
    public ResultSet getTableTypes() throws SQLException {
	return executeSelect("SYSTEM_TABLETYPES", null);
    }

    /**
     * Gets a description of table columns available in
     * the specified catalog.
     *
     * <P>Only column descriptions matching the catalog, schema, table
     * and column name criteria are returned.  They are ordered by
     * TABLE_SCHEM, TABLE_NAME and ORDINAL_POSITION.
     *
     * <P>Each column description has the following columns:
     * <OL>
     * <LI><B>TABLE_CAT</B> String => table catalog (may be null)
     * <LI><B>TABLE_SCHEM</B> String => table schema (may be null)
     * <LI><B>TABLE_NAME</B> String => table name
     * <LI><B>COLUMN_NAME</B> String => column name
     * <LI><B>DATA_TYPE</B> short => SQL type from java.sql.Types
     * <LI><B>TYPE_NAME</B> String => Data source dependent type name,
     * for a UDT the type name is fully qualified
     * <LI><B>COLUMN_SIZE</B> int => column size.  For char or date
     * types this is the maximum number of characters, for numeric or
     * decimal types this is precision.
     * <LI><B>BUFFER_LENGTH</B> is not used.
     * <LI><B>DECIMAL_DIGITS</B> int => the number of fractional digits
     * <LI><B>NUM_PREC_RADIX</B> int => Radix (typically either 10 or 2)
     * <LI><B>NULLABLE</B> int => is NULL allowed?
     * <UL>
     * <LI> columnNoNulls - might not allow NULL values
     * <LI> columnNullable - definitely allows NULL values
     * <LI> columnNullableUnknown - nullability unknown
     * </UL>
     * <LI><B>REMARKS</B> String => comment describing column (may be null)
     * <LI><B>COLUMN_DEF</B> String => default value (may be null)
     * <LI><B>SQL_DATA_TYPE</B> int => unused
     * <LI><B>SQL_DATETIME_SUB</B> int => unused
     * <LI><B>CHAR_OCTET_LENGTH</B> int => for char types the
     * maximum number of bytes in the column
     * <LI><B>ORDINAL_POSITION</B> int	=> index of column in table
     * (starting at 1)
     * <LI><B>IS_NULLABLE</B> String => "NO" means column definitely
     * does not allow NULL values; "YES" means the column might
     * allow NULL values.  An empty string means nobody knows.
     * </OL>
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schemaPattern a schema name pattern; "" retrieves those
     * without a schema
     * @param tableNamePattern a table name pattern
     * @param columnNamePattern a column name pattern
     * @return ResultSet - each row is a column description
     * @exception SQLException if a database access error occurs
     * @see #getSearchStringEscape
     */
    public ResultSet getColumns(String catalog, String schemaPattern,
				String tableNamePattern,
				String columnNamePattern) throws SQLException {
	String table = "SYSTEM_COLUMNS";
	String where = null;

	if (tableNamePattern != null) {
	    where = "TABLE_NAME LIKE '" + tableNamePattern.toUpperCase()
		    + "'";
	}

	if (columnNamePattern != null) {
	    where = addWhere(where,
			     "COLUMN_NAME LIKE '"
			     + columnNamePattern.toUpperCase() + "'");
	}

	return executeSelect(table, where);
    }

    /**
     * Gets a description of the access rights for a table's columns.
     *
     * <P>Only privileges matching the column name criteria are
     * returned.  They are ordered by COLUMN_NAME and PRIVILEGE.
     *
     * <P>Each privilige description has the following columns:
     * <OL>
     * <LI><B>TABLE_CAT</B> String => table catalog (may be null)
     * <LI><B>TABLE_SCHEM</B> String => table schema (may be null)
     * <LI><B>TABLE_NAME</B> String => table name
     * <LI><B>COLUMN_NAME</B> String => column name
     * <LI><B>GRANTOR</B> => grantor of access (may be null)
     * <LI><B>GRANTEE</B> String => grantee of access
     * <LI><B>PRIVILEGE</B> String => name of access (SELECT,
     * INSERT, UPDATE, REFRENCES, ...)
     * <LI><B>IS_GRANTABLE</B> String => "YES" if grantee is permitted
     * to grant to others; "NO" if not; null if unknown
     * </OL>
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schema a schema name; "" retrieves those without a schema
     * @param table a table name
     * @param columnNamePattern a column name pattern
     * @return ResultSet - each row is a column privilege description
     * @exception SQLException if a database access error occurs
     * @see #getSearchStringEscape
     */
    public ResultSet getColumnPrivileges(String catalog, String schema,
					 String tableP,
					 String columnNamePattern) throws SQLException {
	String table = "SYSTEM_COLUMNPRIVILEGES";
	String where = null;

	if (tableP != null) {

	    // other databases may not support table as pattern
	    where = "TABLE_NAME LIKE '" + tableP.toUpperCase() + "'";
	}

	if (columnNamePattern != null) {
	    where = addWhere(where,
			     "COLUMN_NAME LIKE '"
			     + columnNamePattern.toUpperCase() + "'");
	}

	return executeSelect(table, where);
    }

    /**
     * Gets a description of the access rights for each table available
     * in a catalog. Note that a table privilege applies to one or
     * more columns in the table. It would be wrong to assume that
     * this priviledge applies to all columns (this may be true for
     * some systems but is not true for all.)
     *
     * <P>Only privileges matching the schema and table name
     * criteria are returned.  They are ordered by TABLE_SCHEM,
     * TABLE_NAME, and PRIVILEGE.
     *
     * <P>Each privilige description has the following columns:
     * <OL>
     * <LI><B>TABLE_CAT</B> String => table catalog (may be null)
     * <LI><B>TABLE_SCHEM</B> String => table schema (may be null)
     * <LI><B>TABLE_NAME</B> String => table name
     * <LI><B>GRANTOR</B> => grantor of access (may be null)
     * <LI><B>GRANTEE</B> String => grantee of access
     * <LI><B>PRIVILEGE</B> String => name of access (SELECT,
     * INSERT, UPDATE, REFRENCES, ...)
     * <LI><B>IS_GRANTABLE</B> String => "YES" if grantee is permitted
     * to grant to others; "NO" if not; null if unknown
     * </OL>
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schemaPattern a schema name pattern; "" retrieves those
     * without a schema
     * @param tableNamePattern a table name pattern
     * @return ResultSet - each row is a table privilege description
     * @exception SQLException if a database access error occurs
     * @see #getSearchStringEscape
     */
    public ResultSet getTablePrivileges(String catalog, String schemaPattern,
					String tableNamePattern) throws SQLException {
	String table = "SYSTEM_TABLEPRIVILEGES";
	String where = null;

	if (tableNamePattern != null) {
	    where = "TABLE_NAME LIKE '" + tableNamePattern.toUpperCase()
		    + "'";
	}

	return executeSelect(table, where);
    }

    /**
     * Gets a description of a table's optimal set of columns that
     * uniquely identifies a row. They are ordered by SCOPE.
     *
     * <P>Each column description has the following columns:
     * <OL>
     * <LI><B>SCOPE</B> short => actual scope of result
     * <UL>
     * <LI> bestRowTemporary - very temporary, while using row
     * <LI> bestRowTransaction - valid for remainder of current transaction
     * <LI> bestRowSession - valid for remainder of current session
     * </UL>
     * <LI><B>COLUMN_NAME</B> String => column name
     * <LI><B>DATA_TYPE</B> short => SQL data type from java.sql.Types
     * <LI><B>TYPE_NAME</B> String => Data source dependent type name,
     * for a UDT the type name is fully qualified
     * <LI><B>COLUMN_SIZE</B> int => precision
     * <LI><B>BUFFER_LENGTH</B> int => not used
     * <LI><B>DECIMAL_DIGITS</B> short	 => scale
     * <LI><B>PSEUDO_COLUMN</B> short => is this a pseudo column
     * like an Oracle ROWID
     * <UL>
     * <LI> bestRowUnknown - may or may not be pseudo column
     * <LI> bestRowNotPseudo - is NOT a pseudo column
     * <LI> bestRowPseudo - is a pseudo column
     * </UL>
     * </OL>
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schema a schema name; "" retrieves those without a schema
     * @param table a table name
     * @param scope the scope of interest; use same values as SCOPE
     * @param nullable include columns that are nullable?
     * @return ResultSet - each row is a column description
     * @exception SQLException if a database access error occurs
     */
    public ResultSet getBestRowIdentifier(String catalog, String schema,
					  String table, int scope,
					  boolean nullable) throws SQLException {
	return executeSelect("SYSTEM_BESTROWIDENTIFIER", null);
    }

    /**
     * Gets a description of a table's columns that are automatically
     * updated when any value in a row is updated.  They are
     * unordered.
     *
     * <P>Each column description has the following columns:
     * <OL>
     * <LI><B>SCOPE</B> short => is not used
     * <LI><B>COLUMN_NAME</B> String => column name
     * <LI><B>DATA_TYPE</B> short => SQL data type from java.sql.Types
     * <LI><B>TYPE_NAME</B> String => Data source dependent type name
     * <LI><B>COLUMN_SIZE</B> int => precision
     * <LI><B>BUFFER_LENGTH</B> int => length of column value in bytes
     * <LI><B>DECIMAL_DIGITS</B> short	 => scale
     * <LI><B>PSEUDO_COLUMN</B> short => is this a pseudo column
     * like an Oracle ROWID
     * <UL>
     * <LI> versionColumnUnknown - may or may not be pseudo column
     * <LI> versionColumnNotPseudo - is NOT a pseudo column
     * <LI> versionColumnPseudo - is a pseudo column
     * </UL>
     * </OL>
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schema a schema name; "" retrieves those without a schema
     * @param table a table name
     * @return ResultSet - each row is a column description
     * @exception SQLException if a database access error occurs
     */
    public ResultSet getVersionColumns(String catalog, String schema,
				       String table) throws SQLException {
	return executeSelect("SYSTEM_VERSIONCOLUMNS", null);
    }

    /**
     * Gets a description of a table's primary key columns.  They
     * are ordered by COLUMN_NAME.
     *
     * <P>Each primary key column description has the following columns:
     * <OL>
     * <LI><B>TABLE_CAT</B> String => table catalog (may be null)
     * <LI><B>TABLE_SCHEM</B> String => table schema (may be null)
     * <LI><B>TABLE_NAME</B> String => table name
     * <LI><B>COLUMN_NAME</B> String => column name
     * <LI><B>KEY_SEQ</B> short => sequence number within primary key
     * <LI><B>PK_NAME</B> String => primary key name (may be null)
     * </OL>
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schema a schema name; "" retrieves those
     * without a schema
     * @param table a table name
     * @return ResultSet - each row is a primary key column description
     * @exception SQLException if a database access error occurs
     */
    public ResultSet getPrimaryKeys(String catalog, String schema,
				    String tableP) throws SQLException {
	String table = "SYSTEM_PRIMARYKEYS";
	String where = null;

	if (tableP != null) {

	    // other databases may not support table as pattern
	    where = "TABLE_NAME LIKE '" + tableP.toUpperCase() + "'";
	}

	return executeSelect(table, where);
    }

    /**
     * Gets a description of the primary key columns that are
     * referenced by a table's foreign key columns (the primary keys
     * imported by a table).  They are ordered by PKTABLE_CAT,
     * PKTABLE_SCHEM, PKTABLE_NAME, and KEY_SEQ.
     *
     * <P>Each primary key column description has the following columns:
     * <OL>
     * <LI><B>PKTABLE_CAT</B> String => primary key table catalog
     * being imported (may be null)
     * <LI><B>PKTABLE_SCHEM</B> String => primary key table schema
     * being imported (may be null)
     * <LI><B>PKTABLE_NAME</B> String => primary key table name
     * being imported
     * <LI><B>PKCOLUMN_NAME</B> String => primary key column name
     * being imported
     * <LI><B>FKTABLE_CAT</B> String => foreign key table catalog (may be null)
     * <LI><B>FKTABLE_SCHEM</B> String => foreign key table schema (may be null)
     * <LI><B>FKTABLE_NAME</B> String => foreign key table name
     * <LI><B>FKCOLUMN_NAME</B> String => foreign key column name
     * <LI><B>KEY_SEQ</B> short => sequence number within foreign key
     * <LI><B>UPDATE_RULE</B> short => What happens to
     * foreign key when primary is updated:
     * <UL>
     * <LI> importedNoAction - do not allow update of primary
     * key if it has been imported
     * <LI> importedKeyCascade - change imported key to agree
     * with primary key update
     * <LI> importedKeySetNull - change imported key to NULL if
     * its primary key has been updated
     * <LI> importedKeySetDefault - change imported key to default values
     * if its primary key has been updated
     * <LI> importedKeyRestrict - same as importedKeyNoAction
     * (for ODBC 2.x compatibility)
     * </UL>
     * <LI><B>DELETE_RULE</B> short => What happens to
     * the foreign key when primary is deleted.
     * <UL>
     * <LI> importedKeyNoAction - do not allow delete of primary
     * key if it has been imported
     * <LI> importedKeyCascade - delete rows that import a deleted key
     * <LI> importedKeySetNull - change imported key to NULL if
     * its primary key has been deleted
     * <LI> importedKeyRestrict - same as importedKeyNoAction
     * (for ODBC 2.x compatibility)
     * <LI> importedKeySetDefault - change imported key to default if
     * its primary key has been deleted
     * </UL>
     * <LI><B>FK_NAME</B> String => foreign key name (may be null)
     * <LI><B>PK_NAME</B> String => primary key name (may be null)
     * <LI><B>DEFERRABILITY</B> short => can the evaluation of foreign key
     * constraints be deferred until commit
     * <UL>
     * <LI> importedKeyInitiallyDeferred - see SQL92 for definition
     * <LI> importedKeyInitiallyImmediate - see SQL92 for definition
     * <LI> importedKeyNotDeferrable - see SQL92 for definition
     * </UL>
     * </OL>
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schema a schema name; "" retrieves those
     * without a schema
     * @param table a table name
     * @return ResultSet - each row is a primary key column description
     * @exception SQLException if a database access error occurs
     * @see #getExportedKeys
     */
    public ResultSet getImportedKeys(String catalog, String schema,
				     String table) throws SQLException {
	return executeSelect("SYSTEM_IMPORTEDKEYS", null);
    }

    /**
     * Gets a description of the foreign key columns that reference a
     * table's primary key columns (the foreign keys exported by a
     * table).  They are ordered by FKTABLE_CAT, FKTABLE_SCHEM,
     * FKTABLE_NAME, and KEY_SEQ.
     *
     * <P>Each foreign key column description has the following columns:
     * <OL>
     * <LI><B>PKTABLE_CAT</B> String => primary key table catalog (may be null)
     * <LI><B>PKTABLE_SCHEM</B> String => primary key table schema (may be null)
     * <LI><B>PKTABLE_NAME</B> String => primary key table name
     * <LI><B>PKCOLUMN_NAME</B> String => primary key column name
     * <LI><B>FKTABLE_CAT</B> String => foreign key table catalog (may be null)
     * being exported (may be null)
     * <LI><B>FKTABLE_SCHEM</B> String => foreign key table schema (may be null)
     * being exported (may be null)
     * <LI><B>FKTABLE_NAME</B> String => foreign key table name
     * being exported
     * <LI><B>FKCOLUMN_NAME</B> String => foreign key column name
     * being exported
     * <LI><B>KEY_SEQ</B> short => sequence number within foreign key
     * <LI><B>UPDATE_RULE</B> short => What happens to
     * foreign key when primary is updated:
     * <UL>
     * <LI> importedNoAction - do not allow update of primary
     * key if it has been imported
     * <LI> importedKeyCascade - change imported key to agree
     * with primary key update
     * <LI> importedKeySetNull - change imported key to NULL if
     * its primary key has been updated
     * <LI> importedKeySetDefault - change imported key to default values
     * if its primary key has been updated
     * <LI> importedKeyRestrict - same as importedKeyNoAction
     * (for ODBC 2.x compatibility)
     * </UL>
     * <LI><B>DELETE_RULE</B> short => What happens to
     * the foreign key when primary is deleted.
     * <UL>
     * <LI> importedKeyNoAction - do not allow delete of primary
     * key if it has been imported
     * <LI> importedKeyCascade - delete rows that import a deleted key
     * <LI> importedKeySetNull - change imported key to NULL if
     * its primary key has been deleted
     * <LI> importedKeyRestrict - same as importedKeyNoAction
     * (for ODBC 2.x compatibility)
     * <LI> importedKeySetDefault - change imported key to default if
     * its primary key has been deleted
     * </UL>
     * <LI><B>FK_NAME</B> String => foreign key name (may be null)
     * <LI><B>PK_NAME</B> String => primary key name (may be null)
     * <LI><B>DEFERRABILITY</B> short => can the evaluation of foreign key
     * constraints be deferred until commit
     * <UL>
     * <LI> importedKeyInitiallyDeferred - see SQL92 for definition
     * <LI> importedKeyInitiallyImmediate - see SQL92 for definition
     * <LI> importedKeyNotDeferrable - see SQL92 for definition
     * </UL>
     * </OL>
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schema a schema name; "" retrieves those
     * without a schema
     * @param table a table name
     * @return ResultSet - each row is a foreign key column description
     * @exception SQLException if a database access error occurs
     * @see #getImportedKeys
     */
    public ResultSet getExportedKeys(String catalog, String schema,
				     String table) throws SQLException {
	return executeSelect("SYSTEM_EXPORTEDKEYS", null);
    }

    /**
     * Gets a description of the foreign key columns in the foreign key
     * table that reference the primary key columns of the primary key
     * table (describe how one table imports another's key.) This
     * should normally return a single foreign key/primary key pair
     * (most tables only import a foreign key from a table once.)  They
     * are ordered by FKTABLE_CAT, FKTABLE_SCHEM, FKTABLE_NAME, and
     * KEY_SEQ.
     *
     * <P>Each foreign key column description has the following columns:
     * <OL>
     * <LI><B>PKTABLE_CAT</B> String => primary key table catalog (may be null)
     * <LI><B>PKTABLE_SCHEM</B> String => primary key table schema (may be null)
     * <LI><B>PKTABLE_NAME</B> String => primary key table name
     * <LI><B>PKCOLUMN_NAME</B> String => primary key column name
     * <LI><B>FKTABLE_CAT</B> String => foreign key table catalog (may be null)
     * being exported (may be null)
     * <LI><B>FKTABLE_SCHEM</B> String => foreign key table schema (may be null)
     * being exported (may be null)
     * <LI><B>FKTABLE_NAME</B> String => foreign key table name
     * being exported
     * <LI><B>FKCOLUMN_NAME</B> String => foreign key column name
     * being exported
     * <LI><B>KEY_SEQ</B> short => sequence number within foreign key
     * <LI><B>UPDATE_RULE</B> short => What happens to
     * foreign key when primary is updated:
     * <UL>
     * <LI> importedNoAction - do not allow update of primary
     * key if it has been imported
     * <LI> importedKeyCascade - change imported key to agree
     * with primary key update
     * <LI> importedKeySetNull - change imported key to NULL if
     * its primary key has been updated
     * <LI> importedKeySetDefault - change imported key to default values
     * if its primary key has been updated
     * <LI> importedKeyRestrict - same as importedKeyNoAction
     * (for ODBC 2.x compatibility)
     * </UL>
     * <LI><B>DELETE_RULE</B> short => What happens to
     * the foreign key when primary is deleted.
     * <UL>
     * <LI> importedKeyNoAction - do not allow delete of primary
     * key if it has been imported
     * <LI> importedKeyCascade - delete rows that import a deleted key
     * <LI> importedKeySetNull - change imported key to NULL if
     * its primary key has been deleted
     * <LI> importedKeyRestrict - same as importedKeyNoAction
     * (for ODBC 2.x compatibility)
     * <LI> importedKeySetDefault - change imported key to default if
     * its primary key has been deleted
     * </UL>
     * <LI><B>FK_NAME</B> String => foreign key name (may be null)
     * <LI><B>PK_NAME</B> String => primary key name (may be null)
     * <LI><B>DEFERRABILITY</B> short => can the evaluation of foreign key
     * constraints be deferred until commit
     * <UL>
     * <LI> importedKeyInitiallyDeferred - see SQL92 for definition
     * <LI> importedKeyInitiallyImmediate - see SQL92 for definition
     * <LI> importedKeyNotDeferrable - see SQL92 for definition
     * </UL>
     * </OL>
     *
     * @param primaryCatalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param primarySchema a schema name; "" retrieves those
     * without a schema
     * @param primaryTable the table name that exports the key
     * @param foreignCatalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param foreignSchema a schema name; "" retrieves those
     * without a schema
     * @param foreignTable the table name that imports the key
     * @return ResultSet - each row is a foreign key column description
     * @exception SQLException if a database access error occurs
     * @see #getImportedKeys
     */
    public ResultSet getCrossReference(String primaryCatalog,
				       String primarySchema,
				       String primaryTable,
				       String foreignCatalog,
				       String foreignSchema,
				       String foreignTable) throws SQLException {
	return executeSelect("SYSTEM_CROSSREFERENCE", null);
    }

    /**
     * Gets a description of all the standard SQL types supported by
     * this database. They are ordered by DATA_TYPE and then by how
     * closely the data type maps to the corresponding JDBC SQL type.
     *
     * <P>Each type description has the following columns:
     * <OL>
     * <LI><B>TYPE_NAME</B> String => Type name
     * <LI><B>DATA_TYPE</B> short => SQL data type from java.sql.Types
     * <LI><B>PRECISION</B> int => maximum precision
     * <LI><B>LITERAL_PREFIX</B> String => prefix used to quote a literal
     * (may be null)
     * <LI><B>LITERAL_SUFFIX</B> String => suffix used to quote a literal
     * (may be null)
     * <LI><B>CREATE_PARAMS</B> String => parameters used in creating
     * the type (may be null)
     * <LI><B>NULLABLE</B> short => can you use NULL for this type?
     * <UL>
     * <LI> typeNoNulls - does not allow NULL values
     * <LI> typeNullable - allows NULL values
     * <LI> typeNullableUnknown - nullability unknown
     * </UL>
     * <LI><B>CASE_SENSITIVE</B> boolean=> is it case sensitive?
     * <LI><B>SEARCHABLE</B> short => can you use "WHERE" based on this type:
     * <UL>
     * <LI> typePredNone - No support
     * <LI> typePredChar - Only supported with WHERE .. LIKE
     * <LI> typePredBasic - Supported except for WHERE .. LIKE
     * <LI> typeSearchable - Supported for all WHERE ..
     * </UL>
     * <LI><B>UNSIGNED_ATTRIBUTE</B> boolean => is it unsigned?
     * <LI><B>FIXED_PREC_SCALE</B> boolean => can it be a money value?
     * <LI><B>AUTO_INCREMENT</B> boolean => can it be used for an
     * auto-increment value?
     * <LI><B>LOCAL_TYPE_NAME</B> String => localized version of type name
     * (may be null)
     * <LI><B>MINIMUM_SCALE</B> short => minimum scale supported
     * <LI><B>MAXIMUM_SCALE</B> short => maximum scale supported
     * <LI><B>SQL_DATA_TYPE</B> int => unused
     * <LI><B>SQL_DATETIME_SUB</B> int => unused
     * <LI><B>NUM_PREC_RADIX</B> int => usually 2 or 10
     * </OL>
     *
     * @return ResultSet - each row is a SQL type description
     * @exception SQLException if a database access error occurs
     */
    public ResultSet getTypeInfo() throws SQLException {
	return executeSelect("SYSTEM_TYPEINFO", null);
    }

    /**
     * Gets a description of a table's indices and statistics. They are
     * ordered by NON_UNIQUE, TYPE, INDEX_NAME, and ORDINAL_POSITION.
     *
     * <P>Each index column description has the following columns:
     * <OL>
     * <LI><B>TABLE_CAT</B> String => table catalog (may be null)
     * <LI><B>TABLE_SCHEM</B> String => table schema (may be null)
     * <LI><B>TABLE_NAME</B> String => table name
     * <LI><B>NON_UNIQUE</B> boolean => Can index values be non-unique?
     * false when TYPE is tableIndexStatistic
     * <LI><B>INDEX_QUALIFIER</B> String => index catalog (may be null);
     * null when TYPE is tableIndexStatistic
     * <LI><B>INDEX_NAME</B> String => index name; null when TYPE is
     * tableIndexStatistic
     * <LI><B>TYPE</B> short => index type:
     * <UL>
     * <LI> tableIndexStatistic - this identifies table statistics that are
     * returned in conjuction with a table's index descriptions
     * <LI> tableIndexClustered - this is a clustered index
     * <LI> tableIndexHashed - this is a hashed index
     * <LI> tableIndexOther - this is some other style of index
     * </UL>
     * <LI><B>ORDINAL_POSITION</B> short => column sequence number
     * within index; zero when TYPE is tableIndexStatistic
     * <LI><B>COLUMN_NAME</B> String => column name; null when TYPE is
     * tableIndexStatistic
     * <LI><B>ASC_OR_DESC</B> String => column sort sequence, "A" => ascending,
     * "D" => descending, may be null if sort sequence is not supported;
     * null when TYPE is tableIndexStatistic
     * <LI><B>CARDINALITY</B> int => When TYPE is tableIndexStatistic, then
     * this is the number of rows in the table; otherwise, it is the
     * number of unique values in the index.
     * <LI><B>PAGES</B> int => When TYPE is  tableIndexStatisic then
     * this is the number of pages used for the table, otherwise it
     * is the number of pages used for the current index.
     * <LI><B>FILTER_CONDITION</B> String => Filter condition, if any.
     * (may be null)
     * </OL>
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schema a schema name; "" retrieves those without a schema
     * @param table a table name
     * @param unique when true, return only indices for unique values;
     * when false, return indices regardless of whether unique or not
     * @param approximate when true, result is allowed to reflect approximate
     * or out of data values; when false, results are requested to be
     * accurate
     * @return ResultSet - each row is an index column description
     * @exception SQLException if a database access error occurs
     */
    public ResultSet getIndexInfo(String catalog, String schema,
				  String tableP, boolean unique,
				  boolean approximate) throws SQLException {
	String table = "SYSTEM_INDEXINFO";
	String where = null;

	if (tableP != null) {

	    // other databases may not support table as pattern
	    where = "TABLE_NAME LIKE '" + tableP.toUpperCase() + "'";
	}

	return executeSelect(table, where);
    }

    /**
     * JDBC 2.0
     *
     * Does the database support the given result set type?
     *
     * @param type defined in <code>java.sql.ResultSet</code>
     * @return <code>true</code> if so; <code>false</code> otherwise
     * @see Connection
     */

//#ifdef JAVA2
    public boolean supportsResultSetType(int type) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return type == ResultSet.TYPE_FORWARD_ONLY;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Does the database support the concurrency type in combination
     * with the given result set type?
     *
     * @param type defined in <code>java.sql.ResultSet</code>
     * @param concurrency type defined in <code>java.sql.ResultSet</code>
     * @return <code>true</code> if so; <code>false</code> otherwise
     * @see Connection
     */

//#ifdef JAVA2
    public boolean supportsResultSetConcurrency(int type, int conc) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return type == ResultSet.TYPE_FORWARD_ONLY
	       && conc == ResultSet.CONCUR_READ_ONLY;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Indicates whether a result set's own updates are visible.
     *
     * @param result set type, i.e. ResultSet.TYPE_XXX
     * @return <code>true</code> if updates are visible for the result set type;
     * <code>false</code> otherwise
     */

//#ifdef JAVA2
    public boolean ownUpdatesAreVisible(int type) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Indicates whether a result set's own deletes are visible.
     *
     * @param result set type, i.e. ResultSet.TYPE_XXX
     * @return <code>true</code> if deletes are visible for the result set type;
     * <code>false</code> otherwise
     */

//#ifdef JAVA2
    public boolean ownDeletesAreVisible(int type) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Indicates whether a result set's own inserts are visible.
     *
     * @param result set type, i.e. ResultSet.TYPE_XXX
     * @return <code>true</code> if inserts are visible for the result set type;
     * <code>false</code> otherwise
     */

//#ifdef JAVA2
    public boolean ownInsertsAreVisible(int type) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Indicates whether updates made by others are visible.
     *
     * @param result set type, i.e. ResultSet.TYPE_XXX
     * @return <code>true</code> if updates made by others
     * are visible for the result set type;
     * <code>false</code> otherwise
     */

//#ifdef JAVA2
    public boolean othersUpdatesAreVisible(int type) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Indicates whether deletes made by others are visible.
     *
     * @param result set type, i.e. ResultSet.TYPE_XXX
     * @return <code>true</code> if deletes made by others
     * are visible for the result set type;
     * <code>false</code> otherwise
     */

//#ifdef JAVA2
    public boolean othersDeletesAreVisible(int type) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Indicates whether inserts made by others are visible.
     *
     * @param result set type, i.e. ResultSet.TYPE_XXX
     * @return true if updates are visible for the result set type
     * @return <code>true</code> if inserts made by others
     * are visible for the result set type;
     * <code>false</code> otherwise
     */

//#ifdef JAVA2
    public boolean othersInsertsAreVisible(int type) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Indicates whether or not a visible row update can be detected by
     * calling the method <code>ResultSet.rowUpdated</code>.
     *
     * @param result set type, i.e. ResultSet.TYPE_XXX
     * @return <code>true</code> if changes are detected by the result set type;
     * <code>false</code> otherwise
     */

//#ifdef JAVA2
    public boolean updatesAreDetected(int type) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Indicates whether or not a visible row delete can be detected by
     * calling ResultSet.rowDeleted().  If deletesAreDetected()
     * returns false, then deleted rows are removed from the result set.
     *
     * @param result set type, i.e. ResultSet.TYPE_XXX
     * @return true if changes are detected by the resultset type
     */

//#ifdef JAVA2
    public boolean deletesAreDetected(int type) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Indicates whether or not a visible row insert can be detected
     * by calling ResultSet.rowInserted().
     *
     * @param result set type, i.e. ResultSet.TYPE_XXX
     * @return true if changes are detected by the resultset type
     */

//#ifdef JAVA2
    public boolean insertsAreDetected(int type) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Indicates whether the driver supports batch updates.
     * @return true if the driver supports batch updates; false otherwise
     */

//#ifdef JAVA2
    public boolean supportsBatchUpdates() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return false;
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     *
     * Gets a description of the user-defined types defined in a particular
     * schema.  Schema-specific UDTs may have type JAVA_OBJECT, STRUCT,
     * or DISTINCT.
     *
     * <P>Only types matching the catalog, schema, type name and type
     * criteria are returned.  They are ordered by DATA_TYPE, TYPE_SCHEM
     * and TYPE_NAME.  The type name parameter may be a fully-qualified
     * name.  In this case, the catalog and schemaPattern parameters are
     * ignored.
     *
     * <P>Each type description has the following columns:
     * <OL>
     * <LI><B>TYPE_CAT</B> String => the type's catalog (may be null)
     * <LI><B>TYPE_SCHEM</B> String => type's schema (may be null)
     * <LI><B>TYPE_NAME</B> String => type name
     * <LI><B>CLASS_NAME</B> String => Java class name
     * <LI><B>DATA_TYPE</B> String => type value defined in java.sql.Types.
     * One of JAVA_OBJECT, STRUCT, or DISTINCT
     * <LI><B>REMARKS</B> String => explanatory comment on the type
     * </OL>
     *
     * <P><B>Note:</B> If the driver does not support UDTs, an empty
     * result set is returned.
     *
     * @param catalog a catalog name; "" retrieves those without a
     * catalog; null means drop catalog name from the selection criteria
     * @param schemaPattern a schema name pattern; "" retrieves those
     * without a schema
     * @param typeNamePattern a type name pattern; may be a fully-qualified
     * name
     * @param types a list of user-named types to include (JAVA_OBJECT,
     * STRUCT, or DISTINCT); null returns all types
     * @return ResultSet - each row is a type description
     * @exception SQLException if a database access error occurs
     */

//#ifdef JAVA2
    public ResultSet getUDTs(String catalog, String schemaPattern,
			     String typeNamePattern,
			     int[] types) throws SQLException {
	return executeSelect("SYSTEM_UDTS", null);
    }

//#endif JAVA2

    /**
     * JDBC 2.0
     * Retrieves the connection that produced this metadata object.
     *
     * @return the connection that produced this metadata object
     */

//#ifdef JAVA2
    public Connection getConnection() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return cConnection;
    }

//#endif JAVA2

    /**
     * Method declaration
     *
     *
     * @param where
     * @param s
     *
     * @return
     */
    private String addWhere(String where, String s) {
	if (where == null) {
	    return s;
	}

	return where + " AND " + s;
    }

    /**
     * Method declaration
     *
     *
     * @param table
     * @param where
     *
     * @return
     *
     * @throws SQLException
     */
    private ResultSet executeSelect(String table,
				    String where) throws SQLException {
	if (where != null) {
	    table += " WHERE " + where;
	}

	return cConnection.execute("SELECT * FROM " + table);
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    private static String getList(String s[]) {
	StringBuffer b = new StringBuffer();

	for (int i = 0; i < s.length; i++) {
	    b.append(s[i]);
	    b.append(',');
	}

	b.setLength(b.length() - 1);

	return b.toString();
    }

}
