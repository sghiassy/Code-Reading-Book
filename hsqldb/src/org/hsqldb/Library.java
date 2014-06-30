/*
 * Library.java
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
import java.text.*;

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
class Library {
    final static String sNumeric[] = {
	"ABS", "java.lang.Math.abs", "ACOS", "java.lang.Math.acos", "ASIN",
	"java.lang.Math.asin", "ATAN", "java.lang.Math.atan", "ATAN2",
	"java.lang.Math.atan2", "CEILING", "java.lang.Math.ceil", "COS",
	"java.lang.Math.cos", "COT", "org.hsqldb.Library.cot", "DEGREES",
	"java.lang.Math.toDegrees", "EXP", "java.lang.Math.exp", "FLOOR",
	"java.lang.Math.floor", "LOG", "java.lang.Math.log", "LOG10",
	"org.hsqldb.Library.log10", "MOD", "org.hsqldb.Library.mod", "PI",
	"org.hsqldb.Library.pi", "POWER", "java.lang.Math.pow", "RADIANS",
	"java.lang.Math.toRadians", "RAND", "java.lang.Math.random", "ROUND",
	"org.hsqldb.Library.round", "SIGN", "org.hsqldb.Library.sign", "SIN",
	"java.lang.Math.sin", "SQRT", "java.lang.Math.sqrt", "TAN",
	"java.lang.Math.tan", "TRUNCATE", "org.hsqldb.Library.truncate",
	"BITAND", "org.hsqldb.Library.bitand", "BITOR",
	"org.hsqldb.Library.bitor", "ROUNDMAGIC",
	"org.hsqldb.Library.roundMagic",
    };
// fredt@users.sourceforge.net 20010701
// patch 418023 by deforest@users.sourceforge.net
// the definition for SUBSTR was added
    final static String sString[] = {
	"ASCII", "org.hsqldb.Library.ascii", "CHAR",
	"org.hsqldb.Library.character", "CONCAT", "org.hsqldb.Library.concat",
	"DIFFERENCE", "org.hsqldb.Library.difference", "INSERT",
	"org.hsqldb.Library.insert", "LCASE", "org.hsqldb.Library.lcase", "LEFT",
	"org.hsqldb.Library.left", "LENGTH", "org.hsqldb.Library.length",
	"LOCATE", "org.hsqldb.Library.locate", "LTRIM",
	"org.hsqldb.Library.ltrim", "REPEAT", "org.hsqldb.Library.repeat",
	"REPLACE", "org.hsqldb.Library.replace", "RIGHT",
	"org.hsqldb.Library.right", "RTRIM", "org.hsqldb.Library.rtrim",
	"SOUNDEX", "org.hsqldb.Library.soundex", "SPACE",
	"org.hsqldb.Library.space", "SUBSTR","org.hsql.Library.substring",
        "SUBSTRING", "org.hsqldb.Library.substring",
	"UCASE", "org.hsqldb.Library.ucase", "LOWER", "org.hsqldb.Library.lcase",
	"UPPER", "org.hsqldb.Library.ucase"
    };
    final static String sTimeDate[] = {
	"CURDATE", "org.hsqldb.Library.curdate", "CURTIME",
	"org.hsqldb.Library.curtime", "DAYNAME", "org.hsqldb.Library.dayname",
	"DAYOFMONTH", "org.hsqldb.Library.dayofmonth", "DAYOFWEEK",
	"org.hsqldb.Library.dayofweek", "DAYOFYEAR",
	"org.hsqldb.Library.dayofyear", "HOUR", "org.hsqldb.Library.hour",
	"MINUTE", "org.hsqldb.Library.minute", "MONTH",
	"org.hsqldb.Library.month", "MONTHNAME", "org.hsqldb.Library.monthname",
	"NOW", "org.hsqldb.Library.now", "QUARTER", "org.hsqldb.Library.quarter",
	"SECOND", "org.hsqldb.Library.second", "WEEK", "org.hsqldb.Library.week",
	"YEAR", "org.hsqldb.Library.year",
    };
    final static String sSystem[] = {
	"DATABASE", "org.hsqldb.Library.database", "USER",
	"org.hsqldb.Library.user", "IDENTITY", "org.hsqldb.Library.identity"
    };

    /**
     * Method declaration
     *
     *
     * @param h
     */
    static void register(Hashtable h) {
	register(h, sNumeric);
	register(h, sString);
	register(h, sTimeDate);
	register(h, sSystem);
    }

    /**
     * Method declaration
     *
     *
     * @param h
     * @param s
     */
    private static void register(Hashtable h, String s[]) {
	for (int i = 0; i < s.length; i += 2) {
	    h.put(s[i], s[i + 1]);
	}
    }

    static Random rRandom = new Random();

    // NUMERIC

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     */
    public static double rand(Integer i) {
	if (i != null) {
	    rRandom.setSeed(i.intValue());
	}

	return rRandom.nextDouble();
    }

    // this magic number works for 100000000000000; but not for 0.1 and 0.01
    static double LOG10_FACTOR = 0.43429448190325183;

    /**
     * Method declaration
     *
     *
     * @param x
     *
     * @return
     */
    public static double log10(double x) {
	return roundMagic(Math.log(x) * LOG10_FACTOR);
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static double roundMagic(double d) {

	// this function rounds numbers in a good way but slow:
	// - special handling for numbers around 0
	// - only numbers <= +/-1000000000000
	// - convert to a string
	// - check the last 4 characters:
	// '000x' becomes '0000'
	// '999x' becomes '999999' (this is rounded automatically)
	if (d < 0.0000000000001 && d > -0.0000000000001) {
	    return 0.0;
	}

	if (d > 1000000000000. || d < -1000000000000.) {
	    return d;
	}

	StringBuffer s = new StringBuffer();

	s.append(d);

	int len = s.length();

	if (len < 16) {
	    return d;
	}

	char cx = s.charAt(len - 1);
	char c1 = s.charAt(len - 2);
	char c2 = s.charAt(len - 3);
	char c3 = s.charAt(len - 4);

	if (c1 == '0' && c2 == '0' && c3 == '0' && cx != '.') {
	    s.setCharAt(len - 1, '0');
	} else if (c1 == '9' && c2 == '9' && c3 == '9' && cx != '.') {
	    s.setCharAt(len - 1, '9');
	    s.append('9');
	    s.append('9');
	}

	return new Double(s.toString()).doubleValue();
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static double cot(double d) {
	return 1. / Math.tan(d);
    }

    /**
     * Method declaration
     *
     *
     * @param i1
     * @param i2
     *
     * @return
     */
    public static int mod(int i1, int i2) {
	return i1 % i2;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    public static double pi() {
	return Math.PI;
    }

    /**
     * Method declaration
     *
     *
     * @param d
     * @param p
     *
     * @return
     */
    public static double round(double d, int p) {
	double f = Math.pow(10., p);

	return Math.round(d * f) / f;
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static int sign(double d) {
	return d < 0 ? -1 : (d > 0 ? 1 : 0);
    }

    /**
     * Method declaration
     *
     *
     * @param d
     * @param p
     *
     * @return
     */
    public static double truncate(double d, int p) {
	double f = Math.pow(10., p);
	double g = d * f;

	return ((d < 0) ? Math.ceil(g) : Math.floor(g)) / f;
    }

    /**
     * Method declaration
     *
     *
     * @param i
     * @param j
     *
     * @return
     */
    public static int bitand(int i, int j) {
	return i & j;
    }

    /**
     * Method declaration
     *
     *
     * @param i
     * @param j
     *
     * @return
     */
    public static int bitor(int i, int j) {
	return i | j;
    }

    // STRING

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    public static Integer ascii(String s) {
	if (s == null || s.length() == 0) {
	    return null;
	}

	return new Integer(s.charAt(0));
    }

    /**
     * Method declaration
     *
     *
     * @param code
     *
     * @return
     */
    public static String character(int code) {
	return "" + (char) code;
    }

    /**
     * Method declaration
     *
     *
     * @param s1
     * @param s2
     *
     * @return
     */
    public static String concat(String s1, String s2) {
	if (s1 == null) {
	    if (s2 == null) {
		return null;
	    }

	    return s2;
	}

	if (s2 == null) {
	    return s1;
	}

	return s1 + s2;
    }

    /**
     * Method declaration
     *
     *
     * @param s1
     * @param s2
     *
     * @return
     */
    public static int difference(String s1, String s2) {

	// todo: check if this is the standard algorithm
	if (s1 == null || s2 == null) {
	    return 0;
	}

	s1 = soundex(s1);
	s2 = soundex(s2);

	int len1 = s1.length(), len2 = s2.length();
	int e = 0;

	for (int i = 0; i < 4; i++) {
	    if (i >= len1 || i >= len2 || s1.charAt(i) != s2.charAt(i)) {
		e++;
	    }
	}

	return e;
    }

    /**
     * Method declaration
     *
     *
     * @param s1
     * @param start
     * @param length
     * @param s2
     *
     * @return
     */
    public static String insert(String s1, int start, int length, String s2) {
	if (s1 == null) {
	    return s2;
	}

	if (s2 == null) {
	    return s1;
	}

	int len1 = s1.length();
	int len2 = s2.length();

	start--;

	if (start < 0 || length <= 0 || len2 == 0 || start > len1) {
	    return s1;
	}

	if (start + length > len1) {
	    length = len1 - start;
	}

	return s1.substring(0, start) + s2 + s1.substring(start + length);
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    public static String lcase(String s) {
	return s == null ? null : s.toLowerCase();
    }

    /**
     * Method declaration
     *
     *
     * @param s
     * @param i
     *
     * @return
     */
    public static String left(String s, int i) {
	return s == null ? null
	       : s.substring(0,
			     (i < 0 ? 0 : i < s.length() ? i : s.length()));
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    public static int length(String s) {
	return (s == null || s.length() < 1) ? 0 : s.length();
    }

    /**
     * Method declaration
     *
     *
     * @param search
     * @param s
     * @param start
     *
     * @return
     */
    public static int locate(String search, String s, Integer start) {
	if (s == null || search == null) {
	    return 0;
	}

	int i = start == null ? 0 : start.intValue() - 1;

	return s.indexOf(search, i < 0 ? 0 : i) + 1;
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    public static String ltrim(String s) {
	if (s == null) {
	    return s;
	}

	int len = s.length(), i = 0;

	while (i < len && s.charAt(i) <= ' ') {
	    i++;
	}

	return i == 0 ? s : s.substring(i);
    }

    /**
     * Method declaration
     *
     *
     * @param s
     * @param i
     *
     * @return
     */
    public static String repeat(String s, int i) {
	if (s == null) {
	    return null;
	}

	StringBuffer b = new StringBuffer();

	while (i-- > 0) {
	    b.append(s);
	}

	return b.toString();
    }

    /**
     * Method declaration
     *
     *
     * @param s
     * @param replace
     * @param with
     *
     * @return
     */
    public static String replace(String s, String replace, String with) {
	if (s == null || replace == null) {
	    return s;
	}

	if (with == null) {
	    with = "";
	}

	StringBuffer b = new StringBuffer();
	int	     start = 0;
	int	     lenreplace = replace.length();

	while (true) {
	    int i = s.indexOf(replace, start);

	    if (i == -1) {
		b.append(s.substring(start));

		break;
	    }

	    b.append(s.substring(start, i - start));
	    b.append(with);

	    start = i + lenreplace;
	}

	return b.toString();
    }

    /**
     * Method declaration
     *
     *
     * @param s
     * @param i
     *
     * @return
     */
    public static String right(String s, int i) {
	if (s == null) {
	    return null;
	}

	i = s.length() - i;

	return s.substring(i < 0 ? 0 : i < s.length() ? i : s.length());
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    public static String rtrim(String s) {
	if (s == null) {
	    return s;
	}

	int i = s.length() - 1;

	while (i >= 0 && s.charAt(i) <= ' ') {
	    i--;
	}

	return i == s.length() ? s : s.substring(0, i + 1);
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    public static String soundex(String s) {
	if (s == null) {
	    return s;
	}

	s = s.toUpperCase();

	int  len = s.length();
	char b[] = new char[4];

	b[0] = s.charAt(0);

	int j = 1;

	for (int i = 1; i < len && j < 4; i++) {
	    char c = s.charAt(i);

	    if ("BFPV".indexOf(c) != -1) {
		b[j++] = '1';
	    } else if ("CGJKQSXZ".indexOf(c) != -1) {
		b[j++] = '2';
	    } else if (c == 'D' || c == 'T') {
		b[j++] = '3';
	    } else if (c == 'L') {
		b[j++] = '4';
	    } else if (c == 'M' || c == 'N') {
		b[j++] = '5';
	    } else if (c == 'R') {
		b[j++] = '6';
	    }
	}

	return new String(b, 0, j);
    }

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     */
    public static String space(int i) {
	if (i < 0) {
	    return null;
	}

	char c[] = new char[i];

	while (i > 0) {
	    c[--i] = ' ';
	}

	return new String(c);
    }

    /**
     * Method declaration
     *
     *
     * @param s
     * @param start
     * @param length
     *
     * @return
     */
    public static String substring(String s, int start, Integer length) {
	if (s == null) {
	    return null;
	}

	int len = s.length();

	start--;
	start = start > len ? len : start;

	if (length == null) {
	    return s.substring(start);
	} else {
	    int l = length.intValue();

	    return s.substring(start, start + l > len ? len : l);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    public static String ucase(String s) {
	return s == null ? null : s.toUpperCase();
    }

    // TIME AND DATE

    /**
     * Method declaration
     *
     *
     * @return
     */
    public static java.sql.Date curdate() {
	return new java.sql.Date(System.currentTimeMillis());
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    public static java.sql.Time curtime() {
	return new java.sql.Time(System.currentTimeMillis());
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static String dayname(java.sql.Date d) {
	SimpleDateFormat f = new SimpleDateFormat("EEEE");

	return f.format(d).toString();
    }

    /**
     * Method declaration
     *
     *
     * @param d
     * @param part
     *
     * @return
     */
    private static int getDateTimePart(java.util.Date d, int part) {
	Calendar c = new GregorianCalendar();

	c.setTime(d);

	return c.get(part);
    }

    /**
     * Method declaration
     *
     *
     * @param t
     * @param part
     *
     * @return
     */
    private static int getTimePart(java.sql.Time t, int part) {
	Calendar c = new GregorianCalendar();

	c.setTime(t);

	return c.get(part);
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static int dayofmonth(java.sql.Date d) {
	return getDateTimePart(d, Calendar.DAY_OF_MONTH);
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static int dayofweek(java.sql.Date d) {
	return getDateTimePart(d, Calendar.DAY_OF_WEEK);
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static int dayofyear(java.sql.Date d) {
	return getDateTimePart(d, Calendar.DAY_OF_YEAR);
    }

    /**
     * Method declaration
     *
     *
     * @param t
     *
     * @return
     */
    public static int hour(java.sql.Time t) {
	return getDateTimePart(t, Calendar.HOUR);
    }

    /**
     * Method declaration
     *
     *
     * @param t
     *
     * @return
     */
    public static int minute(java.sql.Time t) {
	return getDateTimePart(t, Calendar.MINUTE);
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static int month(java.sql.Date d) {
	return getDateTimePart(d, Calendar.MONTH);
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static String monthname(java.sql.Date d) {
	SimpleDateFormat f = new SimpleDateFormat("MMMM");

	return f.format(d).toString();
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    public static Timestamp now() {
	return new Timestamp(System.currentTimeMillis());
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static int quarter(java.sql.Date d) {
	return (getDateTimePart(d, Calendar.MONTH) / 3) + 1;
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static int second(java.sql.Date d) {
	return getDateTimePart(d, Calendar.SECOND);
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static int week(java.sql.Date d) {
	return getDateTimePart(d, Calendar.WEEK_OF_YEAR);
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     */
    public static int year(java.sql.Date d) {
	return getDateTimePart(d, Calendar.YEAR);
    }

    // SYSTEM

    /**
     * Method declaration
     *
     *
     * @param conn
     *
     * @return
     *
     * @throws SQLException
     */
    public static String database(Connection conn) throws SQLException {
	Statement stat = conn.createStatement();
	String    s =
	    "SELECT Value FROM SYSTEM_CONNECTIONINFO WHERE KEY='DATABASE'";
	ResultSet r = stat.executeQuery(s);

	r.next();

	return r.getString(1);
    }

    /**
     * Method declaration
     *
     *
     * @param conn
     *
     * @return
     *
     * @throws SQLException
     */
    public static String user(Connection conn) throws SQLException {
	Statement stat = conn.createStatement();
	String    s =
	    "SELECT Value FROM SYSTEM_CONNECTIONINFO WHERE KEY='USER'";
	ResultSet r = stat.executeQuery(s);

	r.next();

	return r.getString(1);
    }

    /**
     * Method declaration
     *
     *
     * @param conn
     *
     * @return
     *
     * @throws SQLException
     */
    public static int identity(Connection conn) throws SQLException {
	Statement stat = conn.createStatement();
	String    s =
	    "SELECT VALUE FROM SYSTEM_CONNECTIONINFO WHERE KEY='IDENTITY'";
	ResultSet r = stat.executeQuery(s);

	r.next();

	return r.getInt(1);
    }

}
