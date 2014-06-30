/*
 * Trace.java
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
import java.sql.SQLException;
import java.io.*;

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
class Trace extends PrintWriter {

//#ifdef TRACE
/*
    public static final boolean TRACE=true;
*/
//#else
    public static final boolean TRACE = false;
//#endif
    public static final boolean STOP = false;
    public static final boolean ASSERT = false;
    private static Trace	tTracer = new Trace();
    private static int		iLine;
    private static String       sTrace;
    private static int		iStop = 0;
    final static int		DATABASE_ALREADY_IN_USE = 0,
				CONNECTION_IS_CLOSED = 1,
				CONNECTION_IS_BROKEN = 2,
				DATABASE_IS_SHUTDOWN = 3,
				COLUMN_COUNT_DOES_NOT_MATCH = 4,
				DIVISION_BY_ZERO = 5, INVALID_ESCAPE = 6,
				INTEGRITY_CONSTRAINT_VIOLATION = 7,
				VIOLATION_OF_UNIQUE_INDEX = 8,
				TRY_TO_INSERT_NULL = 9,
				UNEXPECTED_TOKEN = 10,
				UNEXPECTED_END_OF_COMMAND = 11,
				UNKNOWN_FUNCTION = 12, NEED_AGGREGATE = 13,
				SUM_OF_NON_NUMERIC = 14,
				WRONG_DATA_TYPE = 15,
				SINGLE_VALUE_EXPECTED = 16,
				SERIALIZATION_FAILURE = 17,
				TRANSFER_CORRUPTED = 18,
				FUNCTION_NOT_SUPPORTED = 19,
				TABLE_ALREADY_EXISTS = 20,
				TABLE_NOT_FOUND = 21,
				INDEX_ALREADY_EXISTS = 22,
				SECOND_PRIMARY_KEY = 23,
				DROP_PRIMARY_KEY = 24, INDEX_NOT_FOUND = 25,
				COLUMN_ALREADY_EXISTS = 26,
				COLUMN_NOT_FOUND = 27, FILE_IO_ERROR = 28,
				WRONG_DATABASE_FILE_VERSION = 29,
				DATABASE_IS_READONLY = 30,
				ACCESS_IS_DENIED = 31,
				INPUTSTREAM_ERROR = 32,
				NO_DATA_IS_AVAILABLE = 33,
				USER_ALREADY_EXISTS = 34,
				USER_NOT_FOUND = 35, ASSERT_FAILED = 36,
				EXTERNAL_STOP = 37, GENERAL_ERROR = 38,
				WRONG_OUT_PARAMETER = 39,
				ERROR_IN_FUNCTION = 40,
				TRIGGER_NOT_FOUND = 21;
    private static String       sDescription[] = {
	"08001 The database is already in use by another process",
	"08003 Connection is closed", "08003 Connection is broken",
	"08003 The database is shutdown",
	"21S01 Column count does not match", "22012 Division by zero",
	"22019 Invalid escape character",
	"23000 Integrity constraint violation",
	"23000 Violation of unique index",
	"23000 Try to insert null into a non-nullable column",
	"37000 Unexpected token", "37000 Unexpected end of command",
	"37000 Unknown function",
	"37000 Need aggregate function or group by",
	"37000 Sum on non-numeric data not allowed", "37000 Wrong data type",
	"37000 Single value expected", "40001 Serialization failure",
	"40001 Transfer corrupted", "IM001 This function is not supported",
	"S0001 Table already exists", "S0002 Table not found",
	"S0011 Index already exists",
	"S0011 Attempt to define a second primary key",
	"S0011 Attempt to drop the primary key", "S0012 Index not found",
	"S0021 Column already exists", "S0022 Column not found",
	"S1000 File input/output error", "S1000 Wrong database file version",
	"S1000 The database is in read only mode", "S1000 Access is denied",
	"S1000 InputStream error", "S1000 No data is available",
	"S1000 User already exists", "S1000 User not found",
	"S1000 Assert failed", "S1000 External stop request",
	"S1000 General error", "S1009 Wrong OUT parameter",
	"S1010 Error in function", "S0002 Trigger not found"
    };

    /**
     * Method declaration
     *
     *
     * @param code
     * @param add
     *
     * @return
     */
    static SQLException getError(int code, String add) {
	String s = getMessage(code);

	if (add != null) {
	    s += ": " + add;
	}

	return getError(s);
    }

    /**
     * Method declaration
     *
     *
     * @param code
     *
     * @return
     */
    static String getMessage(int code) {
	return sDescription[code];
    }

    /**
     * Method declaration
     *
     *
     * @param e
     *
     * @return
     */
    static String getMessage(SQLException e) {
	return e.getSQLState() + " " + e.getMessage();
    }

    /**
     * Method declaration
     *
     *
     * @param msg
     *
     * @return
     */
    static SQLException getError(String msg) {
	return new SQLException(msg.substring(6), msg.substring(0, 5));
    }

    /**
     * Method declaration
     *
     *
     * @param code
     *
     * @return
     */
    static SQLException error(int code) {
	return getError(code, null);
    }

    /**
     * Method declaration
     *
     *
     * @param code
     * @param s
     *
     * @return
     */
    static SQLException error(int code, String s) {
	return getError(code, s);
    }

    /**
     * Method declaration
     *
     *
     * @param code
     * @param i
     *
     * @return
     */
    static SQLException error(int code, int i) {
	return getError(code, "" + i);
    }

    /**
     * Method declaration
     *
     *
     * @param condition
     *
     * @throws SQLException
     */
    static void assert(boolean condition) throws SQLException {
	assert(condition, null);
    }

    /**
     * Method declaration
     *
     *
     * @param condition
     * @param error
     *
     * @throws SQLException
     */
    static void assert(boolean condition, String error) throws SQLException {
	if (!condition) {
	    printStack();

	    throw getError(ASSERT_FAILED, error);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param condition
     * @param code
     *
     * @throws SQLException
     */
    static void check(boolean condition, int code) throws SQLException {
	check(condition, code, null);
    }

    /**
     * Method declaration
     *
     *
     * @param condition
     * @param code
     * @param s
     *
     * @throws SQLException
     */
    static void check(boolean condition, int code,
		      String s) throws SQLException {
	if (!condition) {
	    throw getError(code, s);
	}
    }

    // for the PrinterWriter interface

    /**
     * Method declaration
     *
     *
     * @param c
     */
    public void println(char c[]) {
	if (iLine++ == 2) {
	    String s = new String(c);
	    int    i = s.indexOf('.');

	    if (i != -1) {
		s = s.substring(i + 1);
	    }

	    i = s.indexOf('(');

	    if (i != -1) {
		s = s.substring(0, i);
	    }

	    sTrace = s;
	}
    }

    /**
     * Constructor declaration
     *
     */
    Trace() {
	super(System.out);
    }

    /**
     * Method declaration
     *
     *
     * @param l
     */
    static void trace(long l) {
	traceCaller("" + l);
    }

    /**
     * Method declaration
     *
     *
     * @param i
     */
    static void trace(int i) {
	traceCaller("" + i);
    }

    /**
     * Method declaration
     *
     */
    static void trace() {
	traceCaller("");
    }

    /**
     * Method declaration
     *
     *
     * @param s
     */
    static void trace(String s) {
	traceCaller(s);
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    static void stop() throws SQLException {
	stop(null);
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @throws SQLException
     */
    static void stop(String s) throws SQLException {
	if (iStop++ % 10000 != 0) {
	    return;
	}

	if (new File("trace.stop").exists()) {
	    printStack();

	    throw getError(EXTERNAL_STOP, s);
	}
    }

    /**
     * Method declaration
     *
     */

// fredt@users.sourceforge.net 20010701
// patch 418014 by deforest@users.sourceforge.net
/* With trace enabled, it is sometimes hard to figure out
what a true exception is versus an exception generated
by the tracing engine. These two methods define
specialized versions Exceptions that are thrown during
tracing so you can more easily differentiate between a
Exception and a TraceException.  */

static private void printStack() {
class TraceException extends Exception {
TraceException() {
super("Trace");
}
};
Exception e=new TraceException();
e.printStackTrace();
}

static private void traceCaller(String s) {
class TraceCallerException extends Exception {
TraceCallerException() {
super("TraceCaller");
}
};
Exception e=new TraceCallerException();
iLine=0;
e.printStackTrace(tTracer);
s=sTrace+"\t"+s;
// trace to System.out is handy if only trace messages of hsql are required
//#ifdef TRACESYSTEMOUT
System.out.println(s);
//#else
/*
DriverManager.println(s);
*/
//#endif
}


}
