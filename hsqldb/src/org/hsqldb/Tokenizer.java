/*
 * Tokenizer.java
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

/**
 * Tokenizer class declaration
 *
 *
 * @version 1.0.0.1
 */
class Tokenizer {
    private final static int NAME = 1, LONG_NAME = 2, SPECIAL = 3,
			     NUMBER = 4, FLOAT = 5, STRING = 6, LONG = 7;

    // used only internally
    private final static int QUOTED_IDENTIFIER = 9, REMARK_LINE = 10,
			     REMARK = 11;
    private String	     sCommand;
    private char	     cCommand[];
    private int		     iLength;
    private Object	     oValue;
    private int		     iIndex;
    private int		     iType;
    private String	     sToken, sLongNameFirst, sLongNameLast;
    private boolean	     bWait;
    private static Hashtable hKeyword;
    static {
	hKeyword = new Hashtable();

	String keyword[] = {
	    "AND", "ALL", "AVG", "BY", "BETWEEN", "COUNT", "CASEWHEN",
	    "DISTINCT", "DISTINCT", "EXISTS", "EXCEPT", "FALSE", "FROM",
	    "GROUP", "IF", "INTO", "IFNULL", "IS", "IN", "INTERSECT", "INNER",
	    "LEFT", "LIKE", "MAX", "MIN", "NULL", "NOT", "ON", "ORDER", "OR",
	    "OUTER", "PRIMARY", "SELECT", "SET", "SUM", "TO", "TRUE",
	    "UNIQUE", "UNION", "VALUES", "WHERE", "CONVERT", "CAST",
	    "CONCAT", "MINUS", "CALL"
	};

	for (int i = 0; i < keyword.length; i++) {
	    hKeyword.put(keyword[i], hKeyword);
	}
    }

    /**
     * Constructor declaration
     *
     *
     * @param s
     */
    Tokenizer(String s) {
	sCommand = s;
	cCommand = s.toCharArray();
	iLength = cCommand.length;
	iIndex = 0;
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void back() throws SQLException {
	Trace.assert(!bWait, "back");

	bWait = true;
    }

    /**
     * Method declaration
     *
     *
     * @param match
     *
     * @throws SQLException
     */
    void getThis(String match) throws SQLException {
	getToken();

	if (!sToken.equals(match)) {
	    throw Trace.error(Trace.UNEXPECTED_TOKEN, sToken);
	}
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    String getStringToken() throws SQLException {
	getToken();

	// todo: this is just compatibility for old style USER 'sa'
	if (iType == STRING) {
	    return sToken.substring(1).toUpperCase();
	} else if (iType == NAME) {
	    return sToken;
	} else if (iType == QUOTED_IDENTIFIER) {
	    return sToken.toUpperCase();
	}

	throw Trace.error(Trace.UNEXPECTED_TOKEN, sToken);
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean wasValue() {
	if (iType == STRING || iType == NUMBER || iType == FLOAT) {
	    return true;
	}

	if (sToken.equals("NULL")) {
	    return true;
	}

	if (sToken.equals("TRUE") || sToken.equals("FALSE")) {
	    return true;
	}

	return false;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean wasLongName() {
	return iType == LONG_NAME;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean wasName() {
	if (iType == QUOTED_IDENTIFIER) {
	    return true;
	}

	if (iType != NAME) {
	    return false;
	}

	return !hKeyword.containsKey(sToken);
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    String getLongNameFirst() {
	return sLongNameFirst;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    String getLongNameLast() {
	return sLongNameLast;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    String getName() throws SQLException {
	getToken();

	if (!wasName()) {
	    throw Trace.error(Trace.UNEXPECTED_TOKEN, sToken);
	}

	return sToken;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    String getString() throws SQLException {
	getToken();

	return sToken;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    int getType() {

	// todo: make sure it's used only for Values!
	// todo: synchronize iType with hColumn
	switch (iType) {

	case STRING:
	    return Column.VARCHAR;

	case NUMBER:
	    return Column.INTEGER;

	case FLOAT:
	    return Column.DOUBLE;

	case LONG:
	    return Column.BIGINT;
	}

	return Column.NULL;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    Object getAsValue() throws SQLException {
	if (!wasValue()) {
	    throw Trace.error(Trace.UNEXPECTED_TOKEN, sToken);
	}

	if (iType == STRING) {
	    return sToken.substring(1);    // todo: this is a bad solution: remove '
	}

	// convert NULL to null String if not a String
	// todo: make this more straightforward
	if (sToken.equals("NULL")) {
	    return null;
	}

	if (iType == NUMBER) {
	    if (sToken.length() > 9) {

		// 2147483647 is the biggest Integer value, so more than
		// 9 digits are better returend as a long
		iType = LONG;

		return new Long(sToken);
	    }

	    return new Integer(sToken);
	} else if (iType == FLOAT) {
	    return new Double(sToken);
	}

	return sToken;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    int getPosition() {
	return iIndex;
    }

    /**
     * Method declaration
     *
     *
     * @param begin
     * @param end
     *
     * @return
     */
    String getPart(int begin, int end) {
	return sCommand.substring(begin, end);
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    private void getToken() throws SQLException {
	if (bWait) {
	    bWait = false;

	    return;
	}

	while (iIndex < iLength && Character.isWhitespace(cCommand[iIndex])) {
	    iIndex++;
	}

	sToken = "";

	if (iIndex >= iLength) {
	    iType = 0;

	    return;
	}

	boolean      point = false, digit = false, exp = false,
		     afterexp = false;
	boolean      end = false;
	char	     c = cCommand[iIndex];
	char	     cfirst = 0;
	StringBuffer name = new StringBuffer();

	if (Character.isJavaIdentifierStart(c)) {
	    iType = NAME;
	} else if ("(),*=;+%".indexOf(c) >= 0) {
	    iType = SPECIAL;
	    iIndex++;
	    sToken = "" + c;

	    return;
	} else if (Character.isDigit(c)) {
	    iType = NUMBER;
	    digit = true;
	} else if ("!<>|/-".indexOf(c) >= 0) {
	    cfirst = c;
	    iType = SPECIAL;
	} else if (c == '\"') {
	    iType = QUOTED_IDENTIFIER;
	} else if (c == '\'') {
	    iType = STRING;

	    name.append('\'');
	} else if (c == '.') {
	    iType = FLOAT;
	    point = true;
	} else {
	    throw Trace.error(Trace.UNEXPECTED_TOKEN, "" + c);
	}

	int start = iIndex++;

	while (true) {
	    if (iIndex >= iLength) {
		c = ' ';
		end = true;

		Trace.check(iType != STRING && iType != QUOTED_IDENTIFIER,
			    Trace.UNEXPECTED_END_OF_COMMAND);
	    } else {
		c = cCommand[iIndex];
	    }

	    switch (iType) {

	    case NAME:
		if (Character.isJavaIdentifierPart(c)) {
		    break;
		}

		sToken = sCommand.substring(start, iIndex).toUpperCase();

		if (c == '.') {
		    sLongNameFirst = sToken;
		    iIndex++;

		    getToken();	       // todo: eliminate recursion

		    sLongNameLast = sToken;
		    iType = LONG_NAME;
		    sToken = sLongNameFirst + "." + sLongNameLast;
		}

		return;

	    case QUOTED_IDENTIFIER:
		if (c == '\"') {
		    iIndex++;

		    if (iIndex >= iLength) {
			sToken = name.toString();

			return;
		    }

		    c = cCommand[iIndex];

		    if (c == '.') {
			sLongNameFirst = name.toString();
			iIndex++;

			getToken();    // todo: eliminate recursion

			sLongNameLast = sToken;
			iType = LONG_NAME;
			sToken = sLongNameFirst + "." + sLongNameLast;

			return;
		    }

		    if (c != '\"') {
			sToken = name.toString();

			return;
		    }
		}

		name.append(c);

		break;

	    case STRING:
		if (c == '\'') {
		    iIndex++;

		    if (iIndex >= iLength || cCommand[iIndex] != '\'') {
			sToken = name.toString();

			return;
		    }
		}

		name.append(c);

		break;

	    case REMARK:
		if (end) {

		    // unfinished remark
		    // maybe print error here
		    iType = 0;

		    return;
		} else if (c == '*') {
		    iIndex++;

		    if (iIndex < iLength && cCommand[iIndex] == '/') {

			// using recursion here
			iIndex++;

			getToken();

			return;
		    }
		}

		break;

	    case REMARK_LINE:
		if (end) {
		    iType = 0;

		    return;
		} else if (c == '\r' || c == '\n') {

		    // using recursion here
		    getToken();

		    return;
		}

		break;

	    case SPECIAL:
		if (c == '/' && cfirst == '/') {
		    iType = REMARK_LINE;

		    break;
		} else if (c == '-' && cfirst == '-') {
		    iType = REMARK_LINE;

		    break;
		} else if (c == '*' && cfirst == '/') {
		    iType = REMARK;

		    break;
		} else if (">=|".indexOf(c) >= 0) {
		    break;
		}

		sToken = sCommand.substring(start, iIndex);

		return;

	    case FLOAT:

	    case NUMBER:
		if (Character.isDigit(c)) {
		    digit = true;
		} else if (c == '.') {
		    iType = FLOAT;

		    if (point) {
			throw Trace.error(Trace.UNEXPECTED_TOKEN, ".");
		    }

		    point = true;
		} else if (c == 'E' || c == 'e') {
		    if (exp) {
			throw Trace.error(Trace.UNEXPECTED_TOKEN, "E");
		    }

		    afterexp = true;    // first character after exp may be + or -
		    point = true;
		    exp = true;
		} else if (c == '-' && afterexp) {
		    afterexp = false;
		} else if (c == '+' && afterexp) {
		    afterexp = false;
		} else {
		    afterexp = false;

		    if (!digit) {
			if (point && start == iIndex - 1) {
			    sToken = ".";
			    iType = SPECIAL;

			    return;
			}

			throw Trace.error(Trace.UNEXPECTED_TOKEN, "" + c);
		    }

		    sToken = sCommand.substring(start, iIndex);

		    return;
		}
	    }

	    iIndex++;
	}
    }

}
