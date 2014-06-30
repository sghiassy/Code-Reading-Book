/*
 * jdbcDriver.java
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
import java.util.Properties;
import java.io.*;

/**
 * <P>Each JDBC driver must supply a class that implements
 * the Driver interface.
 *
 * <P>The Java SQL framework allows for multiple database drivers.
 *
 * <P>The DriverManager will try to load as many drivers as it can
 * find and then for any given connection request, it will ask each
 * driver in turn to try to connect to the target URL.
 * <P><font color="#009900">
 * The application developer will normally not need to call any function of
 * the Driver directly. All required calls are made by the DriverManager.
 * <P>
 * When the HSQL Database Engine Driver class is loaded, it creates an
 * instance of itself and register it with the DriverManager. This means
 * that a user can load and register the HSQL Database Engine driver by calling
 * <pre>
 * <code>Class.forName("org.hsqldb.jdbcDriver")</code>
 * </pre>
 * For more information about how to connect to a HSQL Database Engine database,
 * please see jdbcConnection.
 * </font><P>
 * @see jdbcConnection
 */
public class jdbcDriver implements Driver {
    private final static String sStartURL = "jdbc:hsqldb:";
    final static int		MAJOR = 1, MINOR = 6;
    final static String		VERSION = MAJOR + "." + MINOR;
    final static String		PRODUCT = "HSQL Database Engine";

    /**
     * Attempts to make a database connection to the given URL.
     * The driver returns "null" if it realizes it is the wrong kind
     * of driver to connect to the given URL.  This will be common, as when
     * the JDBC driver manager is asked to connect to a given URL it passes
     * the URL to each loaded driver in turn.
     *
     * <P>The driver raises a SQLException if it is the right
     * driver to connect to the given URL, but has trouble connecting to
     * the database.
     *
     * <P>The java.util.Properties argument can be used to passed arbitrary
     * string tag/value pairs as connection arguments.
     * <P><font color="#009900">
     * For HSQL Database Engine, at least "user" and "password" properties must be
     * included in the Properties.
     * </font><P>
     * @param url the URL of the database to which to connect
     * @param info a list of arbitrary string tag/value pairs as
     * connection arguments. Normally at least a "user" and
     * "password" property should be included.
     * @return a <code>Connection</code> object that represents a
     * connection to the URL
     * @exception SQLException if a database access error occurs
     */
    public Connection connect(String url,
			      Properties info) throws SQLException {
	if (!acceptsURL(url)) {
	    return null;
	}

	String s = url.substring(sStartURL.length());
	String user = info.getProperty("user");

	if (user == null) {
	    user = "";
	}

	String password = info.getProperty("password");

	if (password == null) {
	    password = "";
	}

	return new jdbcConnection(s, user, password);
    }

    /**
     * Returns true if the driver thinks that it can open a connection
     * to the given URL.  Typically drivers will return true if they
     * understand the subprotocol specified in the URL and false if
     * they don't.
     *
     * @param url the URL of the database
     * @return true if this driver can connect to the given URL
     */
    public boolean acceptsURL(String url) {
	if (Trace.TRACE) {
	    Trace.trace(url);
	}

	return url.startsWith(sStartURL);
    }

    /**
     * Gets information about the possible properties for this driver.
     * <p>The getPropertyInfo method is intended to allow a generic GUI tool to
     * discover what properties it should prompt a human for in order to get
     * enough information to connect to a database.  Note that depending on
     * the values the human has supplied so far, additional values may become
     * necessary, so it may be necessary to iterate though several calls
     * to getPropertyInfo.
     *
     * @param url the URL of the database to which to connect
     * @param info a proposed list of tag/value pairs that will be sent on
     * connect open
     * @return an array of DriverPropertyInfo objects describing possible
     * properties.  This array may be an empty array if no properties
     * are required.
     */
    public DriverPropertyInfo[] getPropertyInfo(String url, Properties info) {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	DriverPropertyInfo pinfo[] = new DriverPropertyInfo[2];
	DriverPropertyInfo p;

	p = new DriverPropertyInfo("user", null);
	p.value = info.getProperty("user");
	p.required = true;
	pinfo[0] = p;
	p = new DriverPropertyInfo("password", null);
	p.value = info.getProperty("password");
	p.required = true;
	pinfo[1] = p;

	return pinfo;
    }

    /**
     * Gets the driver's major version number.
     * @return this driver's major version number
     */
    public int getMajorVersion() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return MAJOR;
    }

    /**
     * Gets the driver's minor version number.
     * @return this driver's minor version number
     */
    public int getMinorVersion() {
	if (Trace.TRACE) {
	    Trace.trace();
	}

	return MINOR;
    }

    /**
     * Reports whether this driver is a genuine JDBC
     * COMPLIANT<sup><font size=-2>TM</font></sup> driver.
     * A driver may only report true here if it passes the JDBC compliance
     * tests; otherwise it is required to return false.
     *
     * JDBC compliance requires full support for the JDBC API and full support
     * for SQL 92 Entry Level.  It is expected that JDBC compliant drivers will
     * be available for all the major commercial databases.
     * <P><font color="#009900">
     * HSQL Database Engine currently does not yet support all required SQL 92
     * Entry Level functionality and thus returns false. The features that
     * are missing are currently 'HAVING', views and referential integrity.
     * It looks like other drivers returns true but do not support all features.
     * </font><P>
     * This method is not intended to encourage the development of non-JDBC
     * compliant drivers, but is a recognition of the fact that some vendors
     * are interested in using the JDBC API and framework for lightweight
     * databases that do not support full database functionality, or for
     * special databases such as document information retrieval where a SQL
     * implementation may not be feasible.
     */
    public boolean jdbcCompliant() {
	if (Trace.TRACE) {
	    Trace.trace();

	    // todo: not all required features are implemented yet
	}

	return false;
    }

    static {
	try {
	    DriverManager.registerDriver(new jdbcDriver());

	    if (Trace.TRACE) {
		Trace.trace(PRODUCT + " " + MAJOR + "." + MINOR);
	    }
	} catch (Exception e) {
	    if (Trace.TRACE) {
		Trace.trace(e.getMessage());
	    }
	}
    }
}
