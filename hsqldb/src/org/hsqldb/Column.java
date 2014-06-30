/*
 * Column.java
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
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
class Column {
    private static Hashtable hTypes;
    final static int	     BIT = Types.BIT;			     // -7
    final static int	     TINYINT = Types.TINYINT;		     // -6
    final static int	     BIGINT = Types.BIGINT;		     // -5
    final static int	     LONGVARBINARY = Types.LONGVARBINARY;    // -4
    final static int	     VARBINARY = Types.VARBINARY;	     // -3
    final static int	     BINARY = Types.BINARY;		     // -2
    final static int	     LONGVARCHAR = Types.LONGVARCHAR;	     // -1
    final static int	     CHAR = Types.CHAR;			     // 1
    final static int	     NUMERIC = Types.NUMERIC;		     // 2
    final static int	     DECIMAL = Types.DECIMAL;		     // 3
    final static int	     INTEGER = Types.INTEGER;		     // 4
    final static int	     SMALLINT = Types.SMALLINT;		     // 5
    final static int	     FLOAT = Types.FLOAT;		     // 6
    final static int	     REAL = Types.REAL;			     // 7
    final static int	     DOUBLE = Types.DOUBLE;		     // 8
    final static int	     VARCHAR = Types.VARCHAR;		     // 12
    final static int	     DATE = Types.DATE;			     // 91
    final static int	     TIME = Types.TIME;			     // 92
    final static int	     TIMESTAMP = Types.TIMESTAMP;	     // 93
    final static int	     OTHER = Types.OTHER;		     // 1111
    final static int	     NULL = Types.NULL;			     // 0
    final static int	     VARCHAR_IGNORECASE =
	100;							     // this is the only non-standard type

    // NULL and VARCHAR_IGNORECASE is not part of TYPES
    final static int	     TYPES[] = {
	BIT, TINYINT, BIGINT, LONGVARBINARY, VARBINARY, BINARY, LONGVARCHAR,
	CHAR, NUMERIC, DECIMAL, INTEGER, SMALLINT, FLOAT, REAL, DOUBLE,
	VARCHAR, DATE, TIME, TIMESTAMP, OTHER
    };
    String		     sName;
    int			     iType;
    private boolean	     bNullable;
    private boolean	     bIdentity;
    static {
	hTypes = new Hashtable();

	addTypes(INTEGER, "INTEGER", "int", "java.lang.Integer");
	addType(INTEGER, "INT");
	addTypes(DOUBLE, "DOUBLE", "double", "java.lang.Double");
	addType(FLOAT, "FLOAT");		       // this is a Double
	addTypes(VARCHAR, "VARCHAR", "java.lang.String", null);
	addTypes(CHAR, "CHAR", "CHARACTER", null);
	addType(LONGVARCHAR, "LONGVARCHAR");

	// for ignorecase data types, the 'original' type name is lost
	addType(VARCHAR_IGNORECASE, "VARCHAR_IGNORECASE");
	addTypes(DATE, "DATE", "java.sql.Date", null);
	addTypes(TIME, "TIME", "java.sql.Time", null);

	// DATETIME is for compatibility with MS SQL 7
	addTypes(TIMESTAMP, "TIMESTAMP", "java.sql.Timestamp", "DATETIME");
	addTypes(DECIMAL, "DECIMAL", "java.math.BigDecimal", null);
	addType(NUMERIC, "NUMERIC");
	addTypes(BIT, "BIT", "java.lang.Boolean", "boolean");
	addTypes(TINYINT, "TINYINT", "java.lang.Short", "short");
	addType(SMALLINT, "SMALLINT");
	addTypes(BIGINT, "BIGINT", "java.lang.Long", "long");
	addTypes(REAL, "REAL", "java.lang.Float", "float");
	addTypes(BINARY, "BINARY", "byte[]", null);    // maybe better "[B"
	addType(VARBINARY, "VARBINARY");
	addType(LONGVARBINARY, "LONGVARBINARY");
	addTypes(OTHER, "OTHER", "java.lang.Object", "OBJECT");
    }

    /**
     * Method declaration
     *
     *
     * @param type
     * @param name
     * @param n2
     * @param n3
     */
    private static void addTypes(int type, String name, String n2,
				 String n3) {
	addType(type, name);
	addType(type, n2);
	addType(type, n3);
    }

    /**
     * Method declaration
     *
     *
     * @param type
     * @param name
     */
    private static void addType(int type, String name) {
	if (name != null) {
	    hTypes.put(name, new Integer(type));
	}
    }

    /**
     * Constructor declaration
     *
     *
     * @param name
     * @param nullable
     * @param type
     * @param identity
     */
    Column(String name, boolean nullable, int type, boolean identity) {
	sName = name;
	bNullable = nullable;
	iType = type;
	bIdentity = identity;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean isIdentity() {
	return bIdentity;
    }

    /**
     * Method declaration
     *
     *
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static int getTypeNr(String type) throws SQLException {
	Integer i = (Integer) hTypes.get(type);

	Trace.check(i != null, Trace.WRONG_DATA_TYPE, type);

	return i.intValue();
    }

    /**
     * Method declaration
     *
     *
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static String getType(int type) throws SQLException {
	switch (type) {

	case NULL:
	    return "NULL";

	case INTEGER:
	    return "INTEGER";

	case DOUBLE:
	    return "DOUBLE";

	case VARCHAR_IGNORECASE:
	    return "VARCHAR_IGNORECASE";

	case VARCHAR:
	    return "VARCHAR";

	case CHAR:
	    return "CHAR";

	case LONGVARCHAR:
	    return "LONGVARCHAR";

	case DATE:
	    return "DATE";

	case TIME:
	    return "TIME";

	case DECIMAL:
	    return "DECIMAL";

	case BIT:
	    return "BIT";

	case TINYINT:
	    return "TINYINT";

	case SMALLINT:
	    return "SMALLINT";

	case BIGINT:
	    return "BIGINT";

	case REAL:
	    return "REAL";

	case FLOAT:
	    return "FLOAT";

	case NUMERIC:
	    return "NUMERIC";

	case TIMESTAMP:
	    return "TIMESTAMP";

	case BINARY:
	    return "BINARY";

	case VARBINARY:
	    return "VARBINARY";

	case LONGVARBINARY:
	    return "LONGVARBINARY";

	case OTHER:
	    return "OBJECT";

	default:
	    throw Trace.error(Trace.WRONG_DATA_TYPE, type);
	}
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean isNullable() {
	return bNullable;
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static Object add(Object a, Object b, int type) throws SQLException {
	if (a == null || b == null) {
	    return null;
	}

	switch (type) {

	case NULL:
	    return null;

	case INTEGER:
	    int ai = ((Integer) a).intValue();
	    int bi = ((Integer) b).intValue();

	    return new Integer(ai + bi);

	case FLOAT:

	case DOUBLE:
	    double ad = ((Double) a).doubleValue();
	    double bd = ((Double) b).doubleValue();

	    return new Double(ad + bd);

	case VARCHAR:

	case CHAR:

	case LONGVARCHAR:

	case VARCHAR_IGNORECASE:
	    return (String) a + (String) b;

	case NUMERIC:

	case DECIMAL:
	    BigDecimal abd = (BigDecimal) a;
	    BigDecimal bbd = (BigDecimal) b;

	    return abd.add(bbd);

	case TINYINT:

	case SMALLINT:
	    short shorta = ((Short) a).shortValue();
	    short shortb = ((Short) b).shortValue();

	    return new Short((short) (shorta + shortb));

	case BIGINT:
	    long longa = ((Long) a).longValue();
	    long longb = ((Long) b).longValue();

	    return new Long(longa + longb);

	case REAL:
	    float floata = ((Float) a).floatValue();
	    float floatb = ((Float) b).floatValue();

	    return new Float(floata + floatb);

	default:
	    throw Trace.error(Trace.FUNCTION_NOT_SUPPORTED, type);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     *
     * @return
     *
     * @throws SQLException
     */
    static Object concat(Object a, Object b) throws SQLException {
	if (a == null) {
	    return b;
	} else if (b == null) {
	    return a;
	}

	return convertObject(a) + convertObject(b);
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static Object negate(Object a, int type) throws SQLException {
	if (a == null) {
	    return null;
	}

	switch (type) {

	case NULL:
	    return null;

	case INTEGER:
	    return new Integer(-((Integer) a).intValue());

	case FLOAT:

	case DOUBLE:
	    return new Double(-((Double) a).doubleValue());

	case NUMERIC:

	case DECIMAL:
	    return ((BigDecimal) a).negate();

	case TINYINT:

	case SMALLINT:
	    return new Short((short) -((Short) a).shortValue());

	case BIGINT:
	    return new Long(-((Long) a).longValue());

	case REAL:
	    return new Float(-((Float) a).floatValue());

	default:
	    throw Trace.error(Trace.FUNCTION_NOT_SUPPORTED, type);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static Object multiply(Object a, Object b, int type) throws SQLException {
	if (a == null || b == null) {
	    return null;
	}

	switch (type) {

	case NULL:
	    return null;

	case INTEGER:
	    int ai = ((Integer) a).intValue();
	    int bi = ((Integer) b).intValue();

	    return new Integer(ai * bi);

	case FLOAT:

	case DOUBLE:
	    double ad = ((Double) a).doubleValue();
	    double bd = ((Double) b).doubleValue();

	    return new Double(ad * bd);

	case NUMERIC:

	case DECIMAL:
	    BigDecimal abd = (BigDecimal) a;
	    BigDecimal bbd = (BigDecimal) b;

	    return abd.multiply(bbd);

	case TINYINT:

	case SMALLINT:
	    short shorta = ((Short) a).shortValue();
	    short shortb = ((Short) b).shortValue();

	    return new Short((short) (shorta * shortb));

	case BIGINT:
	    long longa = ((Long) a).longValue();
	    long longb = ((Long) b).longValue();

	    return new Long(longa * longb);

	case REAL:
	    float floata = ((Float) a).floatValue();
	    float floatb = ((Float) b).floatValue();

	    return new Float(floata * floatb);

	default:
	    throw Trace.error(Trace.FUNCTION_NOT_SUPPORTED, type);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static Object divide(Object a, Object b, int type) throws SQLException {
	if (a == null || b == null) {
	    return null;
	}

	switch (type) {

	case NULL:
	    return null;

	case INTEGER:
	    int ai = ((Integer) a).intValue();
	    int bi = ((Integer) b).intValue();

	    Trace.check(bi != 0, Trace.DIVISION_BY_ZERO);

	    return new Integer(ai / bi);

	case FLOAT:

	case DOUBLE:
	    double ad = ((Double) a).doubleValue();
	    double bd = ((Double) b).doubleValue();

	    return bd == 0 ? null : new Double(ad / bd);

	case NUMERIC:

	case DECIMAL:
	    BigDecimal abd = (BigDecimal) a;
	    BigDecimal bbd = (BigDecimal) b;

	    return bbd.signum() == 0 ? null
		   : abd.divide(bbd, BigDecimal.ROUND_HALF_DOWN);

	case TINYINT:

	case SMALLINT:
	    short shorta = ((Short) a).shortValue();
	    short shortb = ((Short) b).shortValue();

	    return shortb == 0 ? null : new Short((short) (shorta / shortb));

	case BIGINT:
	    long longa = ((Long) a).longValue();
	    long longb = ((Long) b).longValue();

	    return longb == 0 ? null : new Long(longa / longb);

	case REAL:
	    float floata = ((Float) a).floatValue();
	    float floatb = ((Float) b).floatValue();

	    return floatb == 0 ? null : new Float(floata / floatb);

	default:
	    throw Trace.error(Trace.FUNCTION_NOT_SUPPORTED, type);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static Object subtract(Object a, Object b, int type) throws SQLException {
	if (a == null || b == null) {
	    return null;
	}

	switch (type) {

	case NULL:
	    return null;

	case INTEGER:
	    int ai = ((Integer) a).intValue();
	    int bi = ((Integer) b).intValue();

	    return new Integer(ai - bi);

	case FLOAT:

	case DOUBLE:
	    double ad = ((Double) a).doubleValue();
	    double bd = ((Double) b).doubleValue();

	    return new Double(ad - bd);

	case NUMERIC:

	case DECIMAL:
	    BigDecimal abd = (BigDecimal) a;
	    BigDecimal bbd = (BigDecimal) b;

	    return abd.subtract(bbd);

	case TINYINT:

	case SMALLINT:
	    short shorta = ((Short) a).shortValue();
	    short shortb = ((Short) b).shortValue();

	    return new Short((short) (shorta - shortb));

	case BIGINT:
	    long longa = ((Long) a).longValue();
	    long longb = ((Long) b).longValue();

	    return new Long(longa - longb);

	case REAL:
	    float floata = ((Float) a).floatValue();
	    float floatb = ((Float) b).floatValue();

	    return new Float(floata - floatb);

	default:
	    throw Trace.error(Trace.FUNCTION_NOT_SUPPORTED, type);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static Object sum(Object a, Object b, int type) throws SQLException {
	if (a == null) {
	    return b;
	}

	if (b == null) {
	    return a;
	}

	switch (type) {

	case NULL:
	    return null;

	case INTEGER:
	    return new Integer(((Integer) a).intValue()
			       + ((Integer) b).intValue());

	case FLOAT:

	case DOUBLE:
	    return new Double(((Double) a).doubleValue()
			      + ((Double) b).doubleValue());

	case NUMERIC:

	case DECIMAL:
	    return ((BigDecimal) a).add((BigDecimal) b);

	case TINYINT:

	case SMALLINT:
	    return new Short((short) (((Short) a).shortValue()
				      + ((Short) b).shortValue()));

	case BIGINT:
	    return new Long(((Long) a).longValue() + ((Long) b).longValue());

	case REAL:
	    return new Float(((Float) a).floatValue()
			     + ((Float) b).floatValue());

	default:
	    Trace.error(Trace.SUM_OF_NON_NUMERIC);
	}

	return null;
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param type
     * @param count
     *
     * @return
     *
     * @throws SQLException
     */
    static Object avg(Object a, int type, int count) throws SQLException {
	if (a == null || count == 0) {
	    return null;
	}

	switch (type) {

	case NULL:
	    return null;

	case INTEGER:
	    return new Integer(((Integer) a).intValue() / count);

	case FLOAT:

	case DOUBLE:
	    return new Double(((Double) a).doubleValue() / count);

	case NUMERIC:

	case DECIMAL:
	    return ((BigDecimal) a).divide(new BigDecimal(count),
					   BigDecimal.ROUND_HALF_DOWN);

	case TINYINT:

	case SMALLINT:
	    return new Short((short) (((Short) a).shortValue() / count));

	case BIGINT:
	    return new Long(((Long) a).longValue() / count);

	case REAL:
	    return new Float(((Float) a).floatValue() / count);

	default:
	    Trace.error(Trace.SUM_OF_NON_NUMERIC);
	}

	return null;
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static Object min(Object a, Object b, int type) throws SQLException {
	if (a == null) {
	    return b;
	}

	if (b == null) {
	    return a;
	}

	if (compare(a, b, type) < 0) {
	    return a;
	}

	return b;
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static Object max(Object a, Object b, int type) throws SQLException {
	if (a == null) {
	    return b;
	}

	if (b == null) {
	    return a;
	}

	if (compare(a, b, type) > 0) {
	    return a;
	}

	return b;
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static int compare(Object a, Object b, int type) throws SQLException {
	int i = 0;

	// null handling: null==null and smaller any value
	// todo: implement standard SQL null handling
	// it is also used for grouping ('null' is one group)
	if (a == null) {
	    if (b == null) {
		return 0;
	    }

	    return -1;
	}

	if (b == null) {
	    return 1;
	}

	switch (type) {

	case NULL:
	    return 0;

	case INTEGER:
	    int ai = ((Integer) a).intValue();
	    int bi = ((Integer) b).intValue();

	    return (ai > bi) ? 1 : (bi > ai ? -1 : 0);

	case FLOAT:

	case DOUBLE:
	    double ad = ((Double) a).doubleValue();
	    double bd = ((Double) b).doubleValue();

	    return (ad > bd) ? 1 : (bd > ad ? -1 : 0);

	case VARCHAR:

	case LONGVARCHAR:
	    i = ((String) a).compareTo((String) b);

	    break;

// fredt@users.sourceforge.net 20010707
// patch 418022 by deforest@users.sourceforge.net
// to mimic SQL92 behaviour
	case CHAR:
            i = Library.rtrim((String) a).compareTo(Library.rtrim((String) b));

            break;

	case VARCHAR_IGNORECASE:

	    // for jdk 1.1 compatibility; jdk 1.2 could use compareToIgnoreCase
	    i = ((String) a).toUpperCase().compareTo(((String) b).toUpperCase());

	    break;

	case DATE:
	    if (((java.sql.Date) a).after((java.sql.Date) b)) {
		return 1;
	    } else if (((java.sql.Date) a).before((java.sql.Date) b)) {
		return -1;
	    } else {
		return 0;
	    }

	case TIME:
	    if (((Time) a).after((Time) b)) {
		return 1;
	    } else if (((Time) a).before((Time) b)) {
		return -1;
	    } else {
		return 0;
	    }

	case TIMESTAMP:
	    if (((Timestamp) a).after((Timestamp) b)) {
		return 1;
	    } else if (((Timestamp) a).before((Timestamp) b)) {
		return -1;
	    } else {
		return 0;
	    }

	case NUMERIC:

	case DECIMAL:
	    i = ((BigDecimal) a).compareTo((BigDecimal) b);

	    break;

	case BIT:
	    boolean boola = ((Boolean) a).booleanValue();
	    boolean boolb = ((Boolean) b).booleanValue();

	    return (boola == boolb) ? 0 : (boolb ? -1 : 1);

	case TINYINT:

	case SMALLINT:
	    short shorta = ((Short) a).shortValue();
	    short shortb = ((Short) b).shortValue();

	    return (shorta > shortb) ? 1 : (shortb > shorta ? -1 : 0);

	case BIGINT:
	    long longa = ((Long) a).longValue();
	    long longb = ((Long) b).longValue();

	    return (longa > longb) ? 1 : (longb > longa ? -1 : 0);

	case REAL:
	    float floata = ((Float) a).floatValue();
	    float floatb = ((Float) b).floatValue();

	    return (floata > floatb) ? 1 : (floatb > floata ? -1 : 0);

	case BINARY:

	case VARBINARY:

	case LONGVARBINARY:

	case OTHER:
	    i = ((ByteArray) a).compareTo((ByteArray) b);

	    break;

	default:
	    throw Trace.error(Trace.FUNCTION_NOT_SUPPORTED, type);
	}

	return (i > 0) ? 1 : (i < 0 ? -1 : 0);
    }

    /**
     * Method declaration
     *
     *
     * @param s
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static Object convertString(String s, int type) throws SQLException {
	if (s == null) {
	    return null;
	}

	switch (type) {

	case NULL:
	    return null;

	case INTEGER:
	    return new Integer(s);

	case FLOAT:

	case DOUBLE:
	    return new Double(s);

	case VARCHAR_IGNORECASE:

	case VARCHAR:

	case CHAR:

	case LONGVARCHAR:
	    return s;

	case DATE:
	    return java.sql.Date.valueOf(s);

	case TIME:
	    return Time.valueOf(s);

// fredt@users.sourceforge.net 20010701
// patch 418019 by deforest@users.sourceforge.net
	case TIMESTAMP:
            return HsqlTimestamp.valueOf(s);

	case NUMERIC:

	case DECIMAL:
	    return new BigDecimal(s.trim());

	case BIT:
	    return new Boolean(s);

	case TINYINT:

	case SMALLINT:
	    return new Short(s);

	case BIGINT:
	    return new Long(s);

	case REAL:
	    return new Float(s);

	case BINARY:

	case VARBINARY:

	case LONGVARBINARY:

	case OTHER:
	    return new ByteArray(s);

	default:
	    throw Trace.error(Trace.FUNCTION_NOT_SUPPORTED, type);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param o
     *
     * @return
     */
    static String convertObject(Object o) {
	if (o == null) {
	    return null;
	}

	return o.toString();
    }

    /**
     * Method declaration
     *
     *
     * @param o
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static Object convertObject(Object o, int type) throws SQLException {
	if (o == null) {
	    return null;
	}
	switch(type) {
		case BINARY:
		case VARBINARY:
		case LONGVARBINARY:
		if (o instanceof byte[])
			return new ByteArray( (byte[])o);
		else
			return convertString(o.toString(), type);
		default:
		return convertString(o.toString(), type);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param o
     * @param type
     *
     * @return
     *
     * @throws SQLException
     */
    static String createString(Object o, int type) throws SQLException {
	if (o == null) {
	    return "NULL";
	}

	switch (type) {

	case NULL:
	    return "NULL";

	case BINARY:

	case VARBINARY:

	case LONGVARBINARY:

	case DATE:

	case TIME:

	case TIMESTAMP:

	case OTHER:
	    return "'" + o.toString() + "'";

	case VARCHAR_IGNORECASE:

	case VARCHAR:

	case CHAR:

	case LONGVARCHAR:
	    return createString((String) o);

	default:
	    return o.toString();
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
    static String createString(String s) {
	StringBuffer b = new StringBuffer().append('\'');

	if (s != null) {
	    for (int i = 0, len = s.length(); i < len; i++) {
		char c = s.charAt(i);

		if (c == '\'') {
		    b.append(c);
		}

		b.append(c);
	    }
	}

	return b.append('\'').toString();
    }

    /**
     * Method declaration
     *
     *
     * @param in
     * @param l
     *
     * @return
     *
     * @throws IOException
     * @throws SQLException
     */
    static Object[] readData(DataInput in,
			     int l) throws IOException, SQLException {
	Object data[] = new Object[l];

	for (int i = 0; i < l; i++) {
	    int    type = in.readInt();
	    Object o = null;

	    switch (type) {

	    case NULL:
		o = null;

		break;

	    case INTEGER:
		o = new Integer(in.readInt());

		break;

	    case FLOAT:

	    case DOUBLE:
		o = new Double(Double.longBitsToDouble(in.readLong()));

		// some JDKs have a problem with this:
		// o=new Double(in.readDouble());
		break;

	    case VARCHAR_IGNORECASE:

	    case VARCHAR:

	    case CHAR:

	    case LONGVARCHAR:
		o = in.readUTF();

		break;

	    case DATE:
		o = java.sql.Date.valueOf(in.readUTF());

		break;

	    case TIME:
		o = Time.valueOf(in.readUTF());

		break;

	    case TIMESTAMP:
		o = Timestamp.valueOf(in.readUTF());

		break;

	    case NUMERIC:

	    case DECIMAL:
		o = new BigDecimal(in.readUTF());

		break;

	    case BIT:
		o = new Boolean(in.readUTF());

		break;

	    case TINYINT:

	    case SMALLINT:
		o = new Short(in.readUTF());

		break;

	    case BIGINT:
		o = new Long(in.readUTF());

		break;

	    case REAL:
		o = new Float(in.readUTF());

		break;

	    case BINARY:

	    case VARBINARY:

	    case LONGVARBINARY:

	    case OTHER:

	    String str = in.readUTF();
	    if (str.equals("**")) {
			//new format
			int len = in.readInt();
			byte[] b = new byte[len];
			in.readFully(b);
			o = new ByteArray(b);
		}
		else {
			// old format
			o = new ByteArray(str);
		}

		break;

	    default:
		throw Trace.error(Trace.FUNCTION_NOT_SUPPORTED, type);
	    }

	    data[i] = o;
	}

	return data;
    }

    /**
     * Method declaration
     *
     *
     * @param out
     * @param data
     * @param t
     *
     * @throws IOException
     */
    static void writeData(DataOutput out, Object data[],
			  Table t) throws IOException {
	int len = t.getInternalColumnCount();
	int type[] = new int[len];

	for (int i = 0; i < len; i++) {
	    type[i] = t.getType(i);
	}

	writeData(out, len, type, data);
    }

    /**
     * Method declaration
     *
     *
     * @param out
     * @param l
     * @param type
     * @param data
     *
     * @throws IOException
     */
    static void writeData(DataOutput out, int l, int type[],
			  Object data[]) throws IOException {
	for (int i = 0; i < l; i++) {
	    Object o = data[i];

	    if (o == null) {
		out.writeInt(NULL);
	    } else {
		int t = type[i];

		out.writeInt(t);

		switch (t) {

		case INTEGER:
		    out.writeInt(((Integer) o).intValue());

		    break;

		case FLOAT:

		case DOUBLE:
		    out.writeLong(Double.doubleToLongBits(((Double) o).doubleValue()));

		    // some JDKs have a problem with this:
		    // out.writeDouble(((Double)o).doubleValue());
		    break;

		case BINARY:
		case VARBINARY:
		case LONGVARBINARY:
		case OTHER:
			out.writeUTF("**"); //new format flag
			byte [] b = ((ByteArray)o).byteValue();
			out.writeInt(b.length);
			out.write(b,0,b.length);

			break;

		default:
		    out.writeUTF(o.toString());

		    break;
		}
	    }
	}
    }

    /**
     * Method declaration
     *
     *
     * @param data
     * @param t
     *
     * @return
     */
    static int getSize(Object data[], Table t) {
	int l = data.length;
	int type[] = new int[l];

	for (int i = 0; i < l; i++) {
	    type[i] = t.getType(i);
	}

	return getSize(data, l, type);
    }

    /**
     * Method declaration
     *
     *
     * @param data
     * @param l
     * @param type
     *
     * @return
     */
    private static int getSize(Object data[], int l, int type[]) {
	int s = 0;

	for (int i = 0; i < l; i++) {
	    Object o = data[i];

	    s += 4;    // type

	    if (o != null) {
		switch (type[i]) {

		case INTEGER:
		    s += 4;

		    break;

		case FLOAT:

		case DOUBLE:
		    s += 8;

		    break;

		case BINARY:
		case VARBINARY:
		case LONGVARBINARY:
		case OTHER:
			s += getUTFsize("**"); //new format flag
			s += 4;
			s += ((ByteArray)o).byteValue().length;

			break;

		default:
		    s += getUTFsize(o.toString());
		}
	    }
	}

	return s;
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    private static int getUTFsize(String s) {

	// a bit bigger is not really a problem, but never smaller!
	if (s == null) {
	    s = "";
	}

	int len = s.length();
	int l = 2;

	for (int i = 0; i < len; i++) {
	    int c = s.charAt(i);

	    if ((c >= 0x0001) && (c <= 0x007F)) {
		l++;
	    } else if (c > 0x07FF) {
		l += 3;
	    } else {
		l += 2;
	    }
	}

	return l;
    }

}
