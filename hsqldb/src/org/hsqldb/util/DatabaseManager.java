/*
 * DatabaseManager.java
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

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.applet.*;
import java.sql.*;
import java.net.*;
import java.io.*;
import java.util.*;

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
public class DatabaseManager extends Applet implements ActionListener,
	WindowListener, KeyListener {
    final static int iMaxRecent = 24;
    Connection       cConn;
    DatabaseMetaData dMeta;
    Statement	     sStatement;
    Menu	     mRecent;
    String	     sRecent[];
    int		     iRecent;
    TextArea	     txtCommand;
    Button	     butExecute;
    Tree	     tTree;
    Panel	     pResult;
    long	     lTime;
    int		     iResult;    // 0: grid; 1: text
    Grid	     gResult;
    TextArea	     txtResult;
    boolean	     bHelp;
    Frame	     fMain;
    Image	     imgEmpty;
    static boolean   bMustExit;

    /**
     * Method declaration
     *
     *
     * @param c
     */
    void connect(Connection c) {
	if (c == null) {
	    return;
	}

	if (cConn != null) {
	    try {
		cConn.close();
	    } catch (SQLException e) {}
	}

	cConn = c;

	try {
	    dMeta = cConn.getMetaData();
	    sStatement = cConn.createStatement();
	} catch (SQLException e) {
	    e.printStackTrace();
	}

	refreshTree();
    }

    /**
     * Method declaration
     *
     */
    public void init() {
	DatabaseManager m = new DatabaseManager();

	m.main();

	try {
	    m.connect(ConnectionDialog.createConnection("org.hsqldb.jdbcDriver",
							"jdbc:hsqldb:.",
							"sa", ""));
	    m.insertTestData();
	    m.refreshTree();
	} catch (Exception e) {
	    e.printStackTrace();
	}
    }

    /**
     * Method declaration
     *
     *
     * @param arg
     */
    public static void main(String arg[]) {
	bMustExit = true;

	DatabaseManager m = new DatabaseManager();

	m.main();

	Connection c = ConnectionDialog.createConnection(m.fMain, "Connect");

	if (c == null) {
	    return;
	}

	m.connect(c);
    }

    /**
     * Method declaration
     *
     */
    void insertTestData() {
	String demo[] = {
	    "DROP TABLE Address", "DROP TABLE Product",
	    "DROP TABLE Document", "DROP TABLE Position",
	    "CREATE TABLE Address(ID INTEGER PRIMARY KEY,FirstName VARCHAR(255),"
	    + "LastName VARCHAR(255),Street VARCHAR(255),City VARCHAR(255))",
		    "CREATE TABLE Product(ID INTEGER PRIMARY KEY,Name VARCHAR(255),"
		    + "Cost DECIMAL)",
				       "CREATE TABLE Document(ID INTEGER PRIMARY KEY,AddressID INTEGER,"
				       + "Total DECIMAL)",
				       "CREATE TABLE Position(DocumentID INTEGER,Position INTEGER,"
				       + "ProductID INTEGER,Quantity INTEGER,Price DECIMAL,"
				       + "PRIMARY KEY(DocumentID,Position))"
	};
	String name[] = {
	    "White", "Karsen", "Smith", "Ringer", "May", "King", "Fuller",
	    "Miller", "Ott", "Sommer", "Schneider", "Steel", "Peterson",
	    "Heiniger", "Clancy"
	};
	String firstname[] = {
	    "Mary", "James", "Anne", "George", "Sylvia", "Robert", "Janet",
	    "Michael", "Andrew", "Bill", "Susanne", "Laura", "Bob", "Julia",
	    "John"
	};
	String street[] = {
	    "Upland Pl.", "College Av.", "- 20th Ave.", "Seventh Av."
	};
	String city[] = {
	    "New York", "Dallas", "Boston", "Chicago", "Seattle",
	    "San Francisco", "Berne", "Oslo", "Paris", "Lyon", "Palo Alto",
	    "Olten"
	};
	String product[] = {
	    "Iron", "Ice Tea", "Clock", "Chair", "Telephone", "Shoe"
	};

	try {
	    for (int i = 0; i < demo.length; i++) {

		// drop table may fail
		try {
		    sStatement.execute(demo[i]);
		} catch (SQLException e) {}
	    }

	    refreshTree();

	    int max = 50;

	    for (int i = 0; i < max; i++) {
		sStatement.execute("INSERT INTO Address VALUES(" + i + ",'"
				   + random(firstname) + "','" + random(name)
				   + "','" + random(554) + " "
				   + random(street) + "','" + random(city)
				   + "')");
		sStatement.execute("INSERT INTO Product VALUES(" + i + ",'"
				   + random(product) + " " + random(product)
				   + "'," + (20 + 2 * random(120)) + ")");
		sStatement.execute("INSERT INTO Document VALUES(" + i + ","
				   + random(max) + ",0.0)");

		for (int j = random(20) + 2; j >= 0; j--) {
		    sStatement.execute("INSERT INTO Position VALUES(" + i
				       + "," + j + "," + random(max) + ","
				       + (1 + random(24)) + ",1.5)");
		}
	    }

	    sStatement.execute("UPDATE Product SET Cost=ROUND(Cost*.1,2)");
	    sStatement.execute("UPDATE Position SET Price=Price*"
			       + "SELECT Cost FROM Product prod WHERE ProductID=prod.ID");
	    sStatement.execute("UPDATE Document SET Total=SELECT SUM(Price*"
			       + "Quantity) FROM Position WHERE DocumentID=Document.ID");

	    String recent[] = {
		"SELECT * FROM Product", "SELECT * FROM Document",
		"SELECT * FROM Position",
		"SELECT * FROM Address a\nINNER JOIN Document d ON a.ID=d.AddressID",
		"SELECT * FROM Document d\nINNER JOIN Position p ON d.ID=p.DocumentID",
		"SELECT * FROM Address WHERE Street LIKE '1%' ORDER BY Lastname"
	    };

	    for (int i = 0; i < recent.length; i++) {
		addToRecent(recent[i]);
	    }

	    refreshTree();
	    txtCommand.setText("SELECT * FROM Address");
	    execute();
	} catch (SQLException e) {
	    e.printStackTrace();
	}
    }

    static Random rRandom = new Random(100);

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    public static String random(String s[]) {
	return s[random(s.length)];
    }

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     */
    public static int random(int i) {
	i = rRandom.nextInt() % i;

	return i < 0 ? -i : i;
    }

    /**
     * Method declaration
     *
     */
    void main() {
	fMain = new Frame("HSQL Database Manager");
	imgEmpty = createImage(new MemoryImageSource(2, 2, new int[4 * 4], 2,
		2));

	fMain.setIconImage(imgEmpty);
	fMain.addWindowListener(this);

	MenuBar bar = new MenuBar();

	// used shortcuts: CERGTSIUDOLM
	String  fitems[] = {
	    "-Connect...", "--", "-Open Script...", "-Save Script...",
	    "-Save Result...", "--", "-Exit"
	};

	addMenu(bar, "File", fitems);

	String vitems[] = {
	    "RRefresh Tree", "--", "GResults in Grid", "TResults in Text",
	    "--", "1Shrink Tree", "2Enlarge Tree", "3Shrink Command",
	    "4Enlarge Command"
	};

	addMenu(bar, "View", vitems);

	String sitems[] = {
	    "SSELECT", "IINSERT", "UUPDATE", "DDELETE", "--",
	    "-CREATE TABLE", "-DROP TABLE", "-CREATE INDEX", "-DROP INDEX",
	    "--", "-SCRIPT", "-SHUTDOWN", "--", "-Test Script"
	};

	addMenu(bar, "Command", sitems);

	Menu recent = new Menu("Recent");

	mRecent = new Menu("Recent");

	bar.add(mRecent);

	String soptions[] = {
	    "-AutoCommit on", "-AutoCommit off", "OCommit", "LRollback",
	    "--", "-Disable MaxRows", "-Set MaxRows to 100", "--",
	    "-Logging on", "-Logging off", "--", "-Insert test data",
	    "-Transfer"
	};

	addMenu(bar, "Options", soptions);
	fMain.setMenuBar(bar);
	fMain.setSize(640, 480);
	fMain.add("Center", this);
	initGUI();

	sRecent = new String[iMaxRecent];

	Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
	Dimension size = fMain.getSize();

	fMain.setLocation((d.width - size.width) / 2,
			  (d.height - size.height) / 2);
	fMain.show();
	txtCommand.requestFocus();
    }

    /**
     * Method declaration
     *
     *
     * @param b
     * @param name
     * @param items
     */
    void addMenu(MenuBar b, String name, String items[]) {
	Menu menu = new Menu(name);

	addMenuItems(menu, items);
	b.add(menu);
    }

    /**
     * Method declaration
     *
     *
     * @param f
     * @param m
     */
    void addMenuItems(Menu f, String m[]) {
	for (int i = 0; i < m.length; i++) {
	    MenuItem item = new MenuItem(m[i].substring(1));
	    char     c = m[i].charAt(0);

	    if (c != '-') {
		item.setShortcut(new MenuShortcut(c));
	    }

	    item.addActionListener(this);
	    f.add(item);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param k
     */
    public void keyPressed(KeyEvent k) {}

    /**
     * Method declaration
     *
     *
     * @param k
     */
    public void keyReleased(KeyEvent k) {}

    /**
     * Method declaration
     *
     *
     * @param k
     */
    public void keyTyped(KeyEvent k) {
	if (k.getKeyChar() == '\n' && k.isControlDown()) {
	    k.consume();
	    execute();
	}
    }

    /**
     * Method declaration
     *
     *
     * @param ev
     */
    public void actionPerformed(ActionEvent ev) {
	String s = ev.getActionCommand();

	if (s == null) {
	    if (ev.getSource() instanceof MenuItem) {
		MenuItem i;

		s = ((MenuItem) ev.getSource()).getLabel();
	    }
	}

	if (s.equals("Execute")) {
	    execute();
	} else if (s.equals("Exit")) {
	    windowClosing(null);
	} else if (s.equals("Transfer")) {
	    Transfer.work();
	} else if (s.equals("Logging on")) {
	    DriverManager.setLogStream(System.out);
	} else if (s.equals("Logging off")) {
	    DriverManager.setLogStream(null);
	} else if (s.equals("Refresh Tree")) {
	    refreshTree();
	} else if (s.startsWith("#")) {
	    int i = Integer.parseInt(s.substring(1));

	    txtCommand.setText(sRecent[i]);
	} else if (s.equals("Connect...")) {
	    connect(ConnectionDialog.createConnection(fMain, "Connect"));
	    refreshTree();
	} else if (s.equals("Results in Grid")) {
	    iResult = 0;

	    pResult.removeAll();
	    pResult.add("Center", gResult);
	    pResult.doLayout();
	} else if (s.equals("Open Script...")) {
	    FileDialog f = new FileDialog(fMain, "Open Script",
					  FileDialog.LOAD);

	    f.show();

	    String file = f.getFile();

	    if (file != null) {
		txtCommand.setText(readFile(f.getDirectory() + file));
	    }
	} else if (s.equals("Save Script...")) {
	    FileDialog f = new FileDialog(fMain, "Save Script",
					  FileDialog.SAVE);

	    f.show();

	    String file = f.getFile();

	    if (file != null) {
		writeFile(f.getDirectory() + file, txtCommand.getText());
	    }
	} else if (s.equals("Save Result...")) {
	    FileDialog f = new FileDialog(fMain, "Save Result",
					  FileDialog.SAVE);

	    f.show();

	    String file = f.getFile();

	    if (file != null) {
		showResultInText();
		writeFile(f.getDirectory() + file, txtResult.getText());
	    }
	} else if (s.equals("Results in Text")) {
	    iResult = 1;

	    pResult.removeAll();
	    pResult.add("Center", txtResult);
	    pResult.doLayout();
	    showResultInText();
	} else if (s.equals("AutoCommit on")) {
	    try {
		cConn.setAutoCommit(true);
	    } catch (SQLException e) {}
	} else if (s.equals("AutoCommit off")) {
	    try {
		cConn.setAutoCommit(false);
	    } catch (SQLException e) {}
	} else if (s.equals("Enlarge Tree")) {
	    Dimension d = tTree.getMinimumSize();

	    d.width += 20;

	    tTree.setMinimumSize(d);
	    fMain.pack();
	} else if (s.equals("Shrink Tree")) {
	    Dimension d = tTree.getMinimumSize();

	    d.width -= 20;

	    if (d.width >= 0) {
		tTree.setMinimumSize(d);
	    }

	    fMain.pack();
	} else if (s.equals("Enlarge Command")) {
	    txtCommand.setRows(txtCommand.getRows() + 1);
	    fMain.pack();
	} else if (s.equals("Shrink Command")) {
	    int i = txtCommand.getRows() - 1;

	    txtCommand.setRows(i < 1 ? 1 : i);
	    fMain.pack();
	} else if (s.equals("Commit")) {
	    try {
		cConn.commit();
	    } catch (SQLException e) {}
	} else if (s.equals("Insert test data")) {
	    insertTestData();
	} else if (s.equals("Rollback")) {
	    try {
		cConn.rollback();
	    } catch (SQLException e) {}
	} else if (s.equals("Disable MaxRows")) {
	    try {
		sStatement.setMaxRows(0);
	    } catch (SQLException e) {}
	} else if (s.equals("Set MaxRows to 100")) {
	    try {
		sStatement.setMaxRows(100);
	    } catch (SQLException e) {}
	} else if (s.equals("SELECT")) {
	    showHelp("SELECT * FROM ",
		     "SELECT [DISTINCT] \n"
		     + "{ selectExpression | table.* | * } [, ... ] \n"
		     + "[INTO newTable] \n" + "FROM tableList \n"
		     + "[WHERE Expression] \n"
		     + "[ORDER BY selectExpression [{ASC | DESC}] [, ...] ] \n"
		     + "[GROUP BY Expression [, ...] ] \n"
		     + "[UNION [ALL] selectStatement]");
	} else if (s.equals("INSERT")) {
	    showHelp("INSERT INTO ",
		     "INSERT INTO table [ (column [,...] ) ] \n"
		     + "{ VALUES(Expression [,...]) | SelectStatement }");
	} else if (s.equals("UPDATE")) {
	    showHelp("UPDATE ",
		     "UPDATE table SET column = Expression [, ...] \n"
		     + "[WHERE Expression]");
	} else if (s.equals("DELETE")) {
	    showHelp("DELETE FROM ", "DELETE FROM table [WHERE Expression]");
	} else if (s.equals("CREATE TABLE")) {
	    showHelp("CREATE TABLE ",
		     "CREATE TABLE name \n"
		     + "( columnDefinition [, ...] ) \n\n"
		     + "columnDefinition: \n"
		     + "column DataType [ [NOT] NULL] [PRIMARY KEY] \n"
		     + "DataType: \n"
		     + "{ INTEGER | DOUBLE | VARCHAR | DATE | TIME |... }");
	} else if (s.equals("DROP TABLE")) {
	    showHelp("DROP TABLE ", "DROP TABLE table");
	} else if (s.equals("CREATE INDEX")) {
	    showHelp("CREATE INDEX ",
		     "CREATE [UNIQUE] INDEX index ON \n"
		     + "table (column [, ...])");
	} else if (s.equals("DROP INDEX")) {
	    showHelp("DROP INDEX ", "DROP INDEX table.index");
	} else if (s.equals("SCRIPT")) {
	    showHelp("SCRIPT",
		     "SCRIPT ['file']\n\n" + "(HSQL Database Engine only)");
	} else if (s.equals("SHUTDOWN")) {
	    showHelp("SHUTDOWN",
		     "SHUTDOWN [IMMEDIATELY]\n\n" + "(HSQL Database Engine only)");
	} else if (s.equals("Test Script")) {
	    showHelp("-->>>TEST<<<-- ;\n" + "--#1000;\n"
		     + "DROP TABLE Test ;\n" + "CREATE TABLE Test(\n"
		     + "  Id INTEGER PRIMARY KEY,\n"
		     + "  FirstName VARCHAR(20),\n" + "  Name VARCHAR(50),\n"
		     + "  ZIP INTEGER) ;\n" + "INSERT INTO Test \n"
		     + "  VALUES(#,'Julia','Peterson-Clancy',#) ;\n"
		     + "UPDATE Test SET Name='Hans' WHERE Id=# ;\n"
		     + "SELECT * FROM Test WHERE Id=# ;\n"
		     + "DELETE FROM Test WHERE Id=# ;\n" + "DROP TABLE Test",
		     "This test script is parsed by the DatabaseManager\n"
		     + "It may be changed manually. Rules:\n"
		     + "- it must start with -->>>TEST<<<--.\n"
		     + "- each line must end with ';' (no spaces after)\n"
		     + "- lines starting with -- are comments\n"
		     + "- lines starting with --#<count> means set new count\n");
	}
    }

    /**
     * Method declaration
     *
     *
     * @param file
     *
     * @return
     */
    String readFile(String file) {
	try {
	    FileReader   read = new FileReader(file);
	    char	 buffer[] = new char[1024];
	    StringBuffer b = new StringBuffer();

	    while (true) {
		int i = read.read(buffer, 0, 1024);

		if (i == -1) {
		    break;
		}

		b.append(buffer, 0, i);
	    }

	    read.close();

	    return b.toString();
	} catch (IOException e) {
	    return e.getMessage();
	}
    }

    /**
     * Method declaration
     *
     *
     * @param file
     * @param text
     */
    void writeFile(String file, String text) {
	try {
	    FileWriter write = new FileWriter(file);

	    write.write(text.toCharArray());
	    write.close();
	} catch (IOException e) {
	    e.printStackTrace();
	}
    }

    /**
     * Method declaration
     *
     *
     * @param s
     * @param help
     */
    void showHelp(String s, String help) {
	txtCommand.setText(s);
	txtResult.setText(help);

	bHelp = true;

	pResult.removeAll();
	pResult.add("Center", txtResult);
	pResult.doLayout();
	txtCommand.requestFocus();
	txtCommand.setCaretPosition(s.length());
    }

    /**
     * Method declaration
     *
     *
     * @param e
     */
    public void windowActivated(WindowEvent e) {}

    /**
     * Method declaration
     *
     *
     * @param e
     */
    public void windowDeactivated(WindowEvent e) {}

    /**
     * Method declaration
     *
     *
     * @param e
     */
    public void windowClosed(WindowEvent e) {}

    /**
     * Method declaration
     *
     *
     * @param ev
     */
    public void windowClosing(WindowEvent ev) {
	try {
	    cConn.close();
	} catch (Exception e) {}

	fMain.dispose();

	if (bMustExit) {
	    System.exit(0);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param e
     */
    public void windowDeiconified(WindowEvent e) {}

    /**
     * Method declaration
     *
     *
     * @param e
     */
    public void windowIconified(WindowEvent e) {}

    /**
     * Method declaration
     *
     *
     * @param e
     */
    public void windowOpened(WindowEvent e) {}

    /**
     * Method declaration
     *
     *
     * @param s
     */
    void trace(String s) {
	System.out.println(s);
    }

    /**
     * Method declaration
     *
     */
    void execute() {
	String sCmd = txtCommand.getText();

	if (sCmd.startsWith("-->>>TEST<<<--")) {
	    testPerformance();

	    return;
	}

	String g[] = new String[1];

	try {
	    lTime = System.currentTimeMillis();

	    sStatement.execute(sCmd);

	    int r = sStatement.getUpdateCount();

	    if (r == -1) {
		formatResultSet(sStatement.getResultSet());
	    } else {
		g[0] = "update count";

		gResult.setHead(g);

		g[0] = "" + r;

		gResult.addRow(g);
	    }

	    lTime = System.currentTimeMillis() - lTime;

	    addToRecent(txtCommand.getText());
	} catch (SQLException e) {
	    lTime = System.currentTimeMillis() - lTime;
	    g[0] = "SQL Error";

	    gResult.setHead(g);

	    String s = e.getMessage();

	    s += " / Error Code: " + e.getErrorCode();
	    s += " / State: " + e.getSQLState();
	    g[0] = s;

	    gResult.addRow(g);
	}

	updateResult();
    }

    /**
     * Method declaration
     *
     */
    void updateResult() {
	if (iResult == 0) {

	    // in case 'help' has removed the grid
	    if (bHelp) {
		pResult.removeAll();
		pResult.add("Center", gResult);
		pResult.doLayout();

		bHelp = false;
	    }

	    gResult.update();
	    gResult.repaint();
	} else {
	    showResultInText();
	}

	txtCommand.selectAll();
	txtCommand.requestFocus();
    }

    /**
     * Method declaration
     *
     *
     * @param r
     */
    void formatResultSet(ResultSet r) {
	if (r == null) {
	    String g[] = new String[1];

	    g[0] = "Result";

	    gResult.setHead(g);

	    g[0] = "(empty)";

	    gResult.addRow(g);

	    return;
	}

	try {
	    ResultSetMetaData m = r.getMetaData();
	    int		      col = m.getColumnCount();
	    String	      h[] = new String[col];

	    for (int i = 1; i <= col; i++) {
		h[i - 1] = m.getColumnLabel(i);
	    }

	    gResult.setHead(h);

	    while (r.next()) {
		for (int i = 1; i <= col; i++) {
		    h[i - 1] = r.getString(i);

		    if (r.wasNull()) {
			h[i - 1] = "(null)";
		    }
		}

		gResult.addRow(h);
	    }

	    r.close();
	} catch (SQLException e) {}
    }

    /**
     * Method declaration
     *
     *
     * @param sql
     * @param max
     *
     * @return
     *
     * @throws SQLException
     */
    long testStatement(String sql, int max) throws SQLException {
	long start = System.currentTimeMillis();

	if (sql.indexOf('#') == -1) {
	    max = 1;
	}

	for (int i = 0; i < max; i++) {
	    String s = sql;

	    while (true) {
		int j = s.indexOf("#r#");

		if (j == -1) {
		    break;
		}

		s = s.substring(0, j) + ((int) (Math.random() * i))
		    + s.substring(j + 3);
	    }

	    while (true) {
		int j = s.indexOf('#');

		if (j == -1) {
		    break;
		}

		s = s.substring(0, j) + i + s.substring(j + 1);
	    }

	    sStatement.execute(s);
	}

	return (System.currentTimeMillis() - start);
    }

    /**
     * Method declaration
     *
     */
    void testPerformance() {
	String       all = txtCommand.getText();
	StringBuffer b = new StringBuffer();
	long	     total = 0;

	for (int i = 0; i < all.length(); i++) {
	    char c = all.charAt(i);

	    if (c != '\n') {
		b.append(c);
	    }
	}

	all = b.toString();

	String g[] = new String[4];

	g[0] = "ms";
	g[1] = "count";
	g[2] = "sql";
	g[3] = "error";

	gResult.setHead(g);

	int max = 1;

	lTime = System.currentTimeMillis() - lTime;

	while (!all.equals("")) {
	    int    i = all.indexOf(';');
	    String sql;

	    if (i != -1) {
		sql = all.substring(0, i);
		all = all.substring(i + 1);
	    } else {
		sql = all;
		all = "";
	    }

	    if (sql.startsWith("--#")) {
		max = Integer.parseInt(sql.substring(3));

		continue;
	    } else if (sql.startsWith("--")) {
		continue;
	    }

	    g[2] = sql;

	    long l = 0;

	    try {
		l = testStatement(sql, max);
		total += l;
		g[0] = "" + l;
		g[1] = "" + max;
		g[3] = "";
	    } catch (SQLException e) {
		g[0] = g[1] = "n/a";
		g[3] = e.toString();
	    }

	    gResult.addRow(g);
	    System.out.println(l + " ms : " + sql);
	}

	g[0] = "" + total;
	g[1] = "total";
	g[2] = "";

	gResult.addRow(g);

	lTime = System.currentTimeMillis() - lTime;

	updateResult();
    }

    /**
     * Method declaration
     *
     */
    void showResultInText() {
	String row[] = gResult.getHead();
	int    width = row.length;
	Vector data = gResult.getData();
	int    size[] = new int[width];

	for (int i = 0; i < width; i++) {
	    size[i] = row[i].length();
	}

	int len = data.size();

	for (int i = 0; i < len; i++) {
	    row = (String[]) data.elementAt(i);

	    for (int j = 0; j < width; j++) {
		int l = row[j].length();

		if (l > size[j]) {
		    size[j] = l;
		}
	    }
	}

	StringBuffer b = new StringBuffer();

	row = gResult.getHead();

	for (int i = 0; i < width; i++) {
	    b.append(row[i]);

	    for (int l = row[i].length(); l <= size[i]; l++) {
		b.append(' ');
	    }
	}

	b.append('\n');

	for (int i = 0; i < width; i++) {
	    for (int l = 0; l < size[i]; l++) {
		b.append('-');
	    }

	    b.append(' ');
	}

	b.append('\n');

	for (int i = 0; i < len; i++) {
	    row = (String[]) data.elementAt(i);

	    for (int j = 0; j < width; j++) {
		b.append(row[j]);

		for (int l = row[j].length(); l <= size[j]; l++) {
		    b.append(' ');
		}
	    }

	    b.append('\n');
	}

	b.append("\n" + len + " row(s) in " + lTime + " ms");
	txtResult.setText(b.toString());
    }

    /**
     * Method declaration
     *
     *
     * @param s
     */
    private void addToRecent(String s) {
	for (int i = 0; i < iMaxRecent; i++) {
	    if (s.equals(sRecent[i])) {
		return;
	    }
	}

	if (sRecent[iRecent] != null) {
	    mRecent.remove(iRecent);
	}

	sRecent[iRecent] = s;

	if (s.length() > 43) {
	    s = s.substring(0, 40) + "...";
	}

	MenuItem item = new MenuItem(s);

	item.setActionCommand("#" + iRecent);
	item.addActionListener(this);
	mRecent.insert(item, iRecent);

	iRecent = (iRecent + 1) % iMaxRecent;
    }

    /**
     * Method declaration
     *
     */
    private void initGUI() {
	Panel pQuery = new Panel();
	Panel pCommand = new Panel();

	pResult = new Panel();

	pQuery.setLayout(new BorderLayout());
	pCommand.setLayout(new BorderLayout());
	pResult.setLayout(new BorderLayout());

	Font fFont = new Font("Dialog", Font.PLAIN, 12);

	txtCommand = new TextArea(5, 40);

	txtCommand.addKeyListener(this);

	txtResult = new TextArea(20, 40);

	txtCommand.setFont(fFont);
	txtResult.setFont(new Font("Courier", Font.PLAIN, 12));

	butExecute = new Button("Execute");

	butExecute.addActionListener(this);
	pCommand.add("East", butExecute);
	pCommand.add("Center", txtCommand);

	gResult = new Grid();

	setLayout(new BorderLayout());
	pResult.add("Center", gResult);
	pQuery.add("North", pCommand);
	pQuery.add("Center", pResult);
	fMain.add("Center", pQuery);

	tTree = new Tree();

	tTree.setMinimumSize(new Dimension(200, 100));
	gResult.setMinimumSize(new Dimension(200, 300));
	fMain.add("West", tTree);
	doLayout();
	fMain.pack();
    }

    /**
     * Method declaration
     *
     */
    private void refreshTree() {
	tTree.removeAll();

	try {
	    int color_table = Color.yellow.getRGB();
	    int color_column = Color.orange.getRGB();
	    int color_index = Color.red.getRGB();

	    tTree.addRow("", dMeta.getURL(), "-", 0);

	    String    usertables[] = {
		"TABLE"
	    };
	    ResultSet result = dMeta.getTables(null, null, null, usertables);
	    Vector    tables = new Vector();

	    while (result.next()) {
		tables.addElement(result.getString(3));
	    }

	    result.close();

	    for (int i = 0; i < tables.size(); i++) {
		String name = (String) tables.elementAt(i);
		String key = "tab-" + name + "-";

		tTree.addRow(key, name, "+", color_table);

		ResultSet col = dMeta.getColumns(null, null, name, null);

		while (col.next()) {
		    String c = col.getString(4);
		    String k1 = key + "col-" + c + "-";

		    tTree.addRow(k1, c, "+", color_column);

		    String type = col.getString(6);

		    tTree.addRow(k1 + "t", "Type: " + type);

		    boolean nullable = col.getInt(11)
				       != DatabaseMetaData.columnNoNulls;

		    tTree.addRow(k1 + "n", "Nullable: " + nullable);
		}

		col.close();
		tTree.addRow(key + "ind", "Indices", "+", 0);

		ResultSet ind = dMeta.getIndexInfo(null, null, name, false,
						   false);
		String    oldiname = null;

		while (ind.next()) {
		    boolean nonunique = ind.getBoolean(4);
		    String  iname = ind.getString(6);
		    String  k2 = key + "ind-" + iname + "-";

		    if ((oldiname == null ||!oldiname.equals(iname))) {
			tTree.addRow(k2, iname, "+", color_index);
			tTree.addRow(k2 + "u", "Unique: " + !nonunique);

			oldiname = iname;
		    }

		    String c = ind.getString(9);

		    tTree.addRow(k2 + "c-" + c + "-", c);
		}

		ind.close();
	    }

	    tTree.addRow("p", "Properties", "+", 0);
	    tTree.addRow("pu", "User: " + dMeta.getUserName());
	    tTree.addRow("pr", "ReadOnly: " + cConn.isReadOnly());
	    tTree.addRow("pa", "AutoCommit: " + cConn.getAutoCommit());
	    tTree.addRow("pd", "Driver: " + dMeta.getDriverName());
	    tTree.addRow("pp", "Product: " + dMeta.getDatabaseProductName());
	    tTree.addRow("pv",
			 "Version: " + dMeta.getDatabaseProductVersion());
	} catch (SQLException e) {
	    tTree.addRow("", "Error getting metadata:", "-", 0);
	    tTree.addRow("-", e.getMessage());
	    tTree.addRow("-", e.getSQLState());
	}

	tTree.update();
    }

}
