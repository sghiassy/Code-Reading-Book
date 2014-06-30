/*
 * SelfTest.java
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
package org.hsqldb.util;

import org.hsqldb.*;
import java.sql.*;
import java.io.*;

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
class SelfTest {

    /**
     * Method declaration
     *
     *
     * @param argv
     */
    public static void main(String argv[]) {
	print("Usage: SelfTest [records [-m]] (-m means in-memory only)");

	int max = 500;

	if (argv.length >= 1) {
	    max = Integer.parseInt(argv[0]);
	}

	boolean persistent = true;
	boolean update = false;

	if (argv.length >= 2) {
	    String a1 = argv[1];

	    if (a1.equals("-m")) {
		persistent = false;
	    }
	}

	test(max, persistent);
    }

    /**
     * Method declaration
     *
     *
     * @param max
     * @param persistent
     */
    static void test(int max, boolean persistent) {

	// DriverManager.setLogStream(System.out);
	try {
	    DriverManager.registerDriver(new org.hsqldb.jdbcDriver());

	    if (persistent) {
		delete("test2.backup");
		delete("test2.properties");
		delete("test2.script");
		delete("test2.data");
		test("Persistent", "jdbc:hsqldb:test2", "sa", "", max,
		     true);
	    }

	    test("In-Memory", "jdbc:hsqldb:.", "sa", "", max, false);
	    Profile.listUnvisited();
	} catch (Exception e) {
	    print("SelfTest error: " + e.getMessage());
	    e.printStackTrace();
	}
    }

    /**
     * Method declaration
     *
     *
     * @param file
     */
    static void delete(String file) {
	try {
	    new File(file).delete();
	} catch (Exception e) {}
    }

    /**
     * Method declaration
     *
     *
     * @param name
     * @param url
     * @param user
     * @param password
     * @param max
     * @param persistent
     *
     * @throws Exception
     */
    static void test(String name, String url, String user, String password,
		     int max, boolean persistent) throws Exception {
	print(name);

	Statement  sStatement = null;
	Connection cConnection = null;

	try {
	    cConnection = DriverManager.getConnection(url, user, password);
	    sStatement = cConnection.createStatement();
	} catch (Exception e) {
	    e.printStackTrace();
	    print("SelfTest init error: " + e.getMessage());
	}

	LineNumberReader read =
	    new LineNumberReader(new FileReader("SelfTest.txt"));
	String		 s = "";

	while (true) {
	    String line = read.readLine();

	    if (line == null) {
		break;
	    }

	    if (line.startsWith(" ")) {
		s += line;
	    } else {
		test(sStatement, s);

		s = line;
	    }
	}

	long      start;
	boolean   bDropError = false;
	ResultSet r;

	try {

	    // test duplicate keys & small transaction rollback
	    s = "CREATE TABLE marotest (id int PRIMARY KEY, dat int);"
		+ "INSERT INTO marotest VALUES (1,0);"
		+ "INSERT INTO marotest VALUES (2,0);"
		+ "INSERT INTO marotest VALUES (2,0);";

	    try {
		sStatement.execute(s);

		s = "";
	    } catch (Exception e) {}

	    if (s.equals("")) {
		throw new Exception("Duplicate key gave no error on insert");
	    }

	    try {
		s = "UPDATE marotest SET id=1, dat=-1 WHERE dat=0";

		sStatement.execute(s);

		s = "";
	    } catch (Exception e) {}

	    if (s.equals("")) {
		throw new Exception("Duplicate key gave no error on update");
	    }

	    int count = 0;

	    s = "SELECT *, id as marotest_id FROM marotest";
	    r = sStatement.executeQuery(s);

	    while (r.next()) {
		r.getFloat(1);
		r.getString("id");
		r.getInt("DaT");
		r.getInt("marotest_id");

		if (r.getShort("dat") != 0) {
		    throw new Exception("Bad update worked");
		}

		r.getLong("DAT");
		r.getString(2);
		r.getObject("ID");
		r.clearWarnings();

		try {

		    // this must throw an error
		    r.getTimestamp("Timestamp?");

		    count = 99;
		} catch (Exception e) {}

		count++;
	    }

	    r.close();

	    if (count != 2) {
		throw new Exception("Should have 2 but has " + count
				    + " rows");
	    }

	    // test database meta data
	    DatabaseMetaData dbMeta = cConnection.getMetaData();

	    r = dbMeta.getColumns(null, "dbo", "MAROTEST", "%");

	    while (r.next()) {
		s = r.getString(4).trim();    // COLUMN_NAME

		int i = r.getInt(5);	      // DATA_TYPE

		s += i + r.getString("TYPE_NAME");
		i = r.getInt(7);	      // COLUMN_SIZE
		i = r.getInt(9);	      // "Decimal_Digits"
		i = r.getInt(11);	      // NULLABLE
		s = s.toUpperCase();

		if (!s.equals("ID4INTEGER") &&!s.equals("DAT4INTEGER")) {
		    throw new Exception("Wrong database meta data");
		}
	    }

	    s = "DROP TABLE marotest";

	    sStatement.execute(s);

	    // prepared statements
	    {
		s = "create table TabProfile(id int primary key,"
		    + "car char,won bit,licence varbinary,"
		    + "name char,sex char,chance double,birthday date)";

		sStatement.execute(s);

		s = "insert into TabProfile values ( ?, ?, ?, ?,"
		    + "'\"John\" the bird''s best friend', 'M',?,?);";

		PreparedStatement p = cConnection.prepareStatement(s);

		p.clearParameters();
		p.setInt(1, 10);
		p.setString(2, "Matchcartoon");
		p.setBoolean(3, true);

		byte[] b1 = {
		    0, 1, -128, 44, 12
		};

		p.setBytes(4, b1);
		p.setDouble(5, 50.5);
		p.setNull(6, Types.DATE);
		p.executeUpdate();
		p.clearParameters();
		p.setInt(1, -2);
		p.setString(2, "\"Birdie\"'s car ?");
		p.setBoolean(3, false);

		byte b2[] = {
		    10, 127
		};

		p.setBytes(4, b2);
		p.setDouble(5, -3.1415e-20);
		p.setDate(6, new Date(100, 1, 29));    // strange, but 2000-02-29
		p.executeUpdate();

		s = "select * from TabProfile where id=-2";
		r = sStatement.executeQuery(s);

		r.next();

		if (!r.getString(2).equals("\"Birdie\"'s car ?")) {
		    throw new Exception("Unicode error.");
		}

		r.close();

		s = "drop table TabProfile";

		sStatement.execute(s);

		s = "create table obj(id int,o object)";

		sStatement.execute(s);

		s = "insert into obj values(?,?)";
		p = cConnection.prepareStatement(s);

		p.setInt(1, 1);

		int ia1[] = {
		    1, 2, 3
		};

		p.setObject(2, ia1);
		p.executeUpdate();
		p.clearParameters();
		p.setInt(1, 2);

		java.awt.Rectangle r1 = new java.awt.Rectangle(10, 11, 12,
			13);

		p.setObject(2, r1);
		p.executeUpdate();

		r = sStatement.executeQuery("SELECT o FROM obj ORDER BY id DESC");

		r.next();

		java.awt.Rectangle r2 = (java.awt.Rectangle) r.getObject(1);

		if (r2.x != 10 || r2.y != 11 || r2.width != 12
			|| r2.height != 13) {
		    throw new Exception("Object data error: Rectangle");
		}

		r.next();

		int ia2[] = (int[]) (r.getObject(1));

		if (ia2[0] != 1 || ia2[1] != 2 || ia2[2] != 3
			|| ia2.length != 3) {
		    throw new Exception("Object data error: int[]");
		}

		s = "drop table obj";

		sStatement.execute(s);
	    }

	    // cache, index and performance tests
	    s = "CREATE CACHED TABLE Addr(ID INT PRIMARY KEY,First CHAR,"
		+ "Name CHAR,ZIP INT)";

	    sStatement.execute(s);

	    s = "CREATE INDEX iName ON Addr(Name)";

	    sStatement.execute(s);

	    s = "SET WRITE_DELAY TRUE";

	    sStatement.execute(s);

	    start = System.currentTimeMillis();

	    for (int i = 0; i < max; i++) {
		s = "INSERT INTO Addr VALUES(" + i + ",'Marcel" + i + "',"
		    + "'Renggli" + (max - i - (i % 31)) + "',"
		    + (3000 + i % 100) + ")";

		if (sStatement.executeUpdate(s) != 1) {
		    throw new Exception("Insert failed");
		}

		if (i % 100 == 0) {
		    printStatus("insert   ", i, max, start);
		}
	    }

	    printStatus("insert   ", max, max, start);
	    print("");

	    s = "SELECT COUNT(*) FROM Addr";
	    r = sStatement.executeQuery(s);

	    r.next();

	    int c = r.getInt(1);

	    if (c != max) {
		throw new Exception("Count should be " + (max) + " but is "
				    + c);
	    }

	    if (persistent) {

		// close & reopen to test backup
		cConnection.close();

		cConnection = DriverManager.getConnection(url, user,
							  password);
		sStatement = cConnection.createStatement();
	    }

	    start = System.currentTimeMillis();

	    for (int i = 0; i < max; i++) {
		s = "UPDATE Addr SET Name='Robert" + (i + (i % 31))
		    + "' WHERE ID=" + i;

		if (sStatement.executeUpdate(s) != 1) {
		    throw new Exception("Update failed");
		}

		if (i % 100 == 0) {
		    printStatus("updated  ", i, max, start);

		    // s="SELECT COUNT(*) FROM Addr";
		    // r=sStatement.executeQuery(s);
		    // r.next();
		    // int c=r.getInt(1);
		    // if(c!=max) {
		    // throw new Exception("Count should be "+max+" but is "+c);
		    // }
		}
	    }

	    printStatus("update   ", max, max, start);
	    print("");

	    if (persistent) {
		s = "SHUTDOWN IMMEDIATELY";

		sStatement.execute(s);

		// open the database; it must be restored after shutdown
		cConnection.close();

		cConnection = DriverManager.getConnection(url, user,
							  password);
		sStatement = cConnection.createStatement();
	    }

	    start = System.currentTimeMillis();

	    for (int i = 0; i < max; i++) {
		s = "DELETE FROM Addr WHERE ID=" + (max - 1 - i);

		if (sStatement.executeUpdate(s) != 1) {
		    throw new Exception("Delete failed");
		}

		if (i % 100 == 0) {
		    printStatus("deleting ", i, max, start);

		    // s="SELECT COUNT(*) FROM Addr";
		    // r=sStatement.executeQuery(s);
		    // r.next();
		    // int c=r.getInt(1);
		    // if(c!=max-i-1) {
		    // throw new Exception("Count should be "+(max-i-1)+" but is "+c);
		    // }
		}
	    }

	    printStatus("delete   ", max, max, start);
	    print("");
	    sStatement.execute("DROP TABLE Addr");
	} catch (Exception e) {
	    print("");
	    print("SelfTest error: " + e);
	    print("with SQL command: " + s);
	    e.printStackTrace();
	}

	cConnection.close();
	print("Test finished");
    }

    /**
     * Method declaration
     *
     *
     * @param stat
     * @param s
     *
     * @throws Exception
     */
    static void test(Statement stat, String s) throws Exception {
	String result = "";
	char   type = ' ';

	if (s.startsWith("/*")) {
	    type = s.charAt(2);

	    int end = s.indexOf("*/");

	    result = s.substring(3, end);
	}

	try {
	    stat.execute(s);

	    int       u = stat.getUpdateCount();
	    int       i = 0;
	    ResultSet r;

	    switch (type) {

	    case ' ':
		break;

	    case 'u':
		if (u != Integer.parseInt(result)) {
		    throw new Exception("Expected update count=" + result
					+ " but update count was " + u
					+ " / " + s);
		}

		break;

	    case 'r':
		if (u != -1) {
		    throw new Exception("Expected ResultSet"
					+ " but update count was " + u
					+ " / " + s);
		}

		r = stat.getResultSet();

		r.next();

		String col = r.getString(1);

		if (r.wasNull() || col == null) {
		    if (!result.equals("")) {
			throw new Exception("Expected " + result
					    + " but got null / " + s);
		    }
		} else if (!col.equals(result)) {
		    throw new Exception("Expected >" + result + "<"
					+ " but got >" + col + "< / " + s);
		}

		break;

	    case 'c':
		if (u != -1) {
		    throw new Exception("Expected ResultSet"
					+ " but update count was " + u
					+ " / " + s);
		}

		r = stat.getResultSet();

		while (r.next()) {
		    i++;
		}

		if (i != Integer.parseInt(result)) {
		    throw new Exception("Expected " + result + " rows "
					+ " but got " + i + " rows / " + s);
		}

		break;

	    case 'e':
		throw new Exception("Expected error " + "but got no error / "
				    + s);
	    }
	} catch (SQLException e) {
	    if (type != 'e') {
		throw new Exception("Expected " + type + "/" + result
				    + " but got error " + e.getMessage()
				    + " / " + s);
	    }
	}
    }

    /**
     * Method declaration
     *
     *
     * @param s
     * @param i
     * @param max
     * @param start
     */
    static void printStatus(String s, int i, int max, long start) {
	System.out.print(s + ": " + i + "/" + max + " " + (100 * i / max)
			 + "% ");

	long now = System.currentTimeMillis();

	if (now > start) {
	    System.out.print((i * 1000 / (now - start)));
	}

	System.out.print(" rows/s                \r");
    }

    /**
     * Method declaration
     *
     *
     * @param s
     */
    private static void print(String s) {
	System.out.println(s);
    }

}
