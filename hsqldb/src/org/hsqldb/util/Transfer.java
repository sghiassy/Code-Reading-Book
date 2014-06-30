/*
 * Transfer.java
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
import java.util.*;

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
public class Transfer extends Applet implements WindowListener,
	ActionListener, ItemListener {
    Connection       cSource, cTarget;
    DatabaseMetaData dSourceMeta, dTargetMeta;
    Statement	     sSourceStatement, sTargetStatement;
    Frame	     fMain;
    Image	     imgEmpty;
    Table	     tCurrent;
    int		     iMaxRows;
    Vector	     tTable;
    java.awt.List    lTable;
    TextField	     tSourceTable, tDestTable;
    TextField	     tDestDrop, tDestCreate, tDestDelete;
    TextField	     tSourceSelect, tDestInsert;
    Checkbox	     cTransfer, cDrop, cCreate, cDelete, cInsert;
    Button	     bStart;
    Hashtable	     hTypes;
    TextField	     tMessage;
    static boolean   bMustExit;

    /**
     * Method declaration
     *
     */
    public void init() {
	Transfer m = new Transfer();

	m.main();
    }

    /**
     * Method declaration
     *
     */
    public static void work() {
	Transfer m = new Transfer();

	m.main();
    }

    /**
     * Method declaration
     *
     *
     * @param arg
     */
    public static void main(String arg[]) {
	bMustExit = true;

	work();
    }

    /**
     * Method declaration
     *
     */
    void main() {
	fMain = new Frame("Hypersonic Transfer Tool");
	imgEmpty = createImage(new MemoryImageSource(2, 2, new int[4 * 4], 2,
		2));

	fMain.setIconImage(imgEmpty);
	fMain.addWindowListener(this);
	fMain.setSize(640, 480);
	fMain.add("Center", this);

	MenuBar bar = new MenuBar();
	String  extras[] = {
	    "Insert 10 rows only", "Insert 1000 rows only", "Insert all rows"
	};
	Menu    menu = new Menu("Options");

	addMenuItems(menu, extras);
	bar.add(menu);
	fMain.setMenuBar(bar);
	initGUI();

	Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
	Dimension size = fMain.getSize();

	fMain.setLocation((d.width - size.width) / 2,
			  (d.height - size.height) / 2);
	fMain.setVisible(true);

	cSource = ConnectionDialog.createConnection(fMain, "Source Database");

	if (cSource == null) {
	    return;
	}

	try {
	    dSourceMeta = cSource.getMetaData();
	    sSourceStatement = cSource.createStatement();
	} catch (Exception e) {
	    trace(e.toString());
	    e.printStackTrace();
	}

	cTarget = ConnectionDialog.createConnection(fMain, "Target Database");

	if (cTarget == null) {
	    return;
	}

	try {
	    dTargetMeta = cTarget.getMetaData();
	    sTargetStatement = cTarget.createStatement();
	} catch (Exception e) {
	    trace(e.toString());
	    e.printStackTrace();
	}

	String usertables[] = {
	    "TABLE"
	};

	lTable.removeAll();

	tTable = new Vector();

	try {
	    hTypes = new Hashtable();

	    trace("Building target type info");

	    ResultSet result = dTargetMeta.getTypeInfo();

	    while (result.next()) {
		hTypes.put(new Integer(result.getShort(2)),
			   result.getString(1));

		// todo: most database need parameters, for example VARCHAR(size)
	    }

	    result.close();
	    trace("Reading source tables");

	    result = dSourceMeta.getTables(null, null, null, usertables);

	    int index = 0;

	    while (result.next()) {
		Table t = new Table();

		t.iTableIndex = index++;

		String name = result.getString(3);

		t.bTransfer = true;
		t.sSourceTable = name;
		t.sDestTable = name;
		t.sDestDrop = "DROP TABLE " + name;
		t.sSourceSelect = "SELECT * FROM " + name;
		t.sDestDelete = "DELETE FROM " + name;
		t.bCreate = true;
		t.bDelete = true;
		t.bDrop = true;
		t.bInsert = true;

		tTable.addElement(t);
		lTable.add(name);
	    }

	    result.close();

	    for (int i = 0; i < tTable.size(); i++) {
		Table t = (Table) tTable.elementAt(i);

		trace("Reading source columns for table " + t.sSourceTable);

		ResultSet col = dSourceMeta.getColumns(null, null,
						       t.sSourceTable, null);
		String    create = "CREATE TABLE " + t.sSourceTable + "(";
		String    insert = "INSERT INTO " + t.sSourceTable
				   + " VALUES(";
		Vector    v = new Vector();

		while (col.next()) {
		    String name = col.getString(4);
		    int    type = col.getShort(5);
		    String source = col.getString(6);

		    // MS SQL 7 specific problems (Northwind database)
		    if (type == 11
			    && source.toUpperCase().equals("DATETIME")) {
			trace("Converted DATETIME (type 11) to TIMESTAMP");

			type = Types.TIMESTAMP;
		    } else if (type == -9
			       && source.toUpperCase().equals("NVARCHAR")) {
			trace("Converted NVARCHAR (type -9) to VARCHAR");

			type = Types.VARCHAR;
		    } else if (type == -8
			       && source.toUpperCase().equals("NCHAR")) {
			trace("Converted NCHAR (type -8) to VARCHAR");

			type = Types.VARCHAR;
		    } else if (type == -10
			       && source.toUpperCase().equals("NTEXT")) {
			trace("Converted NTEXT (type -10) to VARCHAR");

			type = Types.VARCHAR;
		    } else if (type == -1
			       && source.toUpperCase().equals("LONGTEXT")) {
			trace("Converted LONGTEXT (type -1) to LONGVARCHAR");

			type = Types.LONGVARCHAR;
		    }

		    Integer inttype = new Integer(type);
		    String  datatype = (String) hTypes.get(inttype);

		    if (datatype == null) {
			datatype = source;

			trace("No mapping for type: " + inttype
			      + " source type is " + datatype);
		    }

		    v.addElement(inttype);

		    create += name + " " + datatype + ",";
		    insert += "?,";
		}

		col.close();

		create = create.substring(0, create.length() - 1) + ")";
		insert = insert.substring(0, insert.length() - 1) + ")";
		t.sDestCreate = create;
		t.sDestInsert = insert;
		t.iColumnType = new int[v.size()];

		for (int j = 0; j < v.size(); j++) {
		    t.iColumnType[j] = ((Integer) v.elementAt(j)).intValue();
		}
	    }

	    trace("Edit definitions and press [Start Transfer]");
	} catch (SQLException e) {
	    trace("SQL Exception reading Metadata: " + e.getMessage());
	    e.printStackTrace();
	}

	fMain.show();
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
	    MenuItem item = new MenuItem(m[i]);

	    item.addActionListener(this);
	    f.add(item);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param e
     */
    public void itemStateChanged(ItemEvent e) {
	ItemSelectable item = e.getItemSelectable();

	if (item == lTable) {
	    String table = lTable.getSelectedItem();

	    for (int i = 0; i < tTable.size(); i++) {
		Table t = (Table) tTable.elementAt(i);

		if (t != null && t.sSourceTable.equals(table)) {
		    saveTable();
		    displayTable(t);
		    updateEnabled(true);
		}
	    }
	} else {

	    // it must be a checkbox
	    updateEnabled(true);
	}
    }

    /**
     * Method declaration
     *
     */
    void saveTable() {
	if (tCurrent == null) {
	    return;
	}

	Table t = tCurrent;

	t.sSourceTable = tSourceTable.getText();
	t.sDestTable = tDestTable.getText();
	t.sDestDrop = tDestDrop.getText();
	t.sDestCreate = tDestCreate.getText();
	t.sDestDelete = tDestDelete.getText();
	t.sSourceSelect = tSourceSelect.getText();
	t.sDestInsert = tDestInsert.getText();
	t.bTransfer = cTransfer.getState();
	t.bDrop = cDrop.getState();
	t.bCreate = cCreate.getState();
	t.bDelete = cDelete.getState();
	t.bInsert = cInsert.getState();
    }

    /**
     * Method declaration
     *
     *
     * @param t
     */
    void displayTable(Table t) {
	tCurrent = t;

	if (t == null) {
	    return;
	}

	tSourceTable.setText(t.sSourceTable);
	tDestTable.setText(t.sDestTable);
	tDestDrop.setText(t.sDestDrop);
	tDestCreate.setText(t.sDestCreate);
	tDestDelete.setText(t.sDestDelete);
	tSourceSelect.setText(t.sSourceSelect);
	tDestInsert.setText(t.sDestInsert);
	cTransfer.setState(t.bTransfer);
	cDrop.setState(t.bDrop);
	cCreate.setState(t.bCreate);
	cDelete.setState(t.bDelete);
	cInsert.setState(t.bInsert);
    }

    /**
     * Method declaration
     *
     *
     * @param and
     */
    void updateEnabled(boolean and) {
	boolean b = cTransfer.getState();

	tDestTable.setEnabled(and && b);
	tDestDrop.setEnabled(and && b && cDrop.getState());
	tDestCreate.setEnabled(and && b && cCreate.getState());
	tDestDelete.setEnabled(and && b && cDelete.getState());
	tSourceSelect.setEnabled(and && b);
	tDestInsert.setEnabled(and && b && cInsert.getState());
	cDrop.setEnabled(and && b);
	cCreate.setEnabled(and && b);
	cDelete.setEnabled(and && b);
	cInsert.setEnabled(and && b);
	bStart.setEnabled(and);
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

	if (s.equals("Start Transfer")) {
	    transfer();
	} else if (s.equals("Insert 10 rows only")) {
	    iMaxRows = 10;
	} else if (s.equals("Insert 1000 rows only")) {
	    iMaxRows = 1000;
	} else if (s.equals("Insert all rows")) {
	    iMaxRows = 0;
	}
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
	    if (cSource != null) {
		cSource.close();
	    }

	    if (cTarget != null) {
		cTarget.close();
	    }
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
     * Class declaration
     *
     *
     * @author
     * @version %I%, %G%
     */
    class Table {
	int     iTableIndex;
	String  sSourceTable, sDestTable;
	String  sDestDrop, sDestCreate, sDestDelete;
	String  sSourceSelect, sDestInsert;
	boolean bTransfer, bDrop, bCreate, bDelete, bInsert;
	int     iColumnType[];
    }

    /**
     * Method declaration
     *
     */
    void initGUI() {
	Font fFont = new Font("Dialog", Font.PLAIN, 12);

	setLayout(new BorderLayout());

	Panel p = new Panel();

	p.setBackground(SystemColor.control);
	p.setLayout(new GridLayout(16, 1));

	tSourceTable = new TextField();

	tSourceTable.setEnabled(false);

	tDestTable = new TextField();
	tDestDrop = new TextField();
	tDestCreate = new TextField();
	tDestDelete = new TextField();
	tSourceSelect = new TextField();
	tDestInsert = new TextField();
	cTransfer = new Checkbox("Transfer to destination table", true);

	cTransfer.addItemListener(this);

	cDrop = new Checkbox("Drop destination table (ignore error)", true);

	cDrop.addItemListener(this);

	cCreate = new Checkbox("Create destination table", true);

	cCreate.addItemListener(this);

	cDelete = new Checkbox("Delete rows in destination table", true);

	cDelete.addItemListener(this);

	cInsert = new Checkbox("Insert into destination", true);

	cInsert.addItemListener(this);
	p.add(createLabel("Source table"));
	p.add(tSourceTable);
	p.add(cTransfer);
	p.add(tDestTable);
	p.add(cDrop);
	p.add(tDestDrop);
	p.add(cCreate);
	p.add(tDestCreate);
	p.add(cDelete);
	p.add(tDestDelete);
	p.add(createLabel("Select source records"));
	p.add(tSourceSelect);
	p.add(cInsert);
	p.add(tDestInsert);
	p.add(createLabel(""));

	bStart = new Button("Start Transfer");

	bStart.addActionListener(this);
	p.add(bStart);
	fMain.add("Center", createBorderPanel(p));

	lTable = new java.awt.List(10);

	lTable.addItemListener(this);
	fMain.add("West", createBorderPanel(lTable));

	tMessage = new TextField();

	Panel pMessage = createBorderPanel(tMessage);

	fMain.add("South", pMessage);
    }

    /**
     * Method declaration
     *
     *
     * @param center
     *
     * @return
     */
    Panel createBorderPanel(Component center) {
	Panel p = new Panel();

	p.setBackground(SystemColor.control);
	p.setLayout(new BorderLayout());
	p.add("Center", center);
	p.add("South", createLabel(""));
	p.add("East", createLabel(""));
	p.add("West", createLabel(""));
	p.setBackground(SystemColor.control);

	return p;
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    Label createLabel(String s) {
	Label l = new Label(s);

	l.setBackground(SystemColor.control);

	return l;
    }

    /**
     * Method declaration
     *
     *
     * @param s
     */
    void trace(String s) {
	tMessage.setText(s);
	System.out.println(s);
    }

    /**
     * Method declaration
     *
     */
    void transfer() {
	saveTable();
	updateEnabled(false);
	trace("Start Transfer");

	try {
	    for (int i = 0; i < tTable.size(); i++) {
		transfer((Table) tTable.elementAt(i));
	    }

	    trace("Transfer finished successfully");
	} catch (SQLException e) {
	    String last = tMessage.getText();

	    trace("Transfer stopped - " + last + " / " + sLast + " / Error: "
		  + e.getMessage());
	    e.printStackTrace();
	} catch (Exception e) {
	    String last = tMessage.getText();

	    trace("Transfer stopped - " + last + " / " + sLast + " / Error: "
		  + e.getMessage());
	    e.printStackTrace();
	}

	updateEnabled(true);
    }

    /**
     * Method declaration
     *
     *
     * @param t
     *
     * @throws SQLException
     */
    void transfer(Table t) throws SQLException {
	lTable.select(t.iTableIndex);
	displayTable(t);

	if (t.bTransfer == false) {
	    trace("Table " + t.sSourceSelect + " not transfered");

	    return;
	}

	trace("Table " + t.sSourceSelect + ": start transfer");

	if (t.bDelete) {
	    try {
		trace("Executing " + t.sDestDrop);
		sTargetStatement.execute(t.sDestDrop);
	    } catch (SQLException e) {
		trace("Ignoring error " + e.getMessage());
	    }
	}

	if (t.bCreate) {
	    trace("Executing " + t.sDestCreate);
	    sTargetStatement.execute(t.sDestCreate);
	}

	if (t.bDelete) {
	    trace("Executing " + t.sDestDelete);
	    sTargetStatement.execute(t.sDestDelete);
	}

	if (t.bInsert) {
	    trace("Executing " + t.sDestInsert);

	    PreparedStatement p = cTarget.prepareStatement(t.sDestInsert);

	    trace("Executing " + t.sSourceSelect);

	    ResultSet	      r =
		sSourceStatement.executeQuery(t.sSourceSelect);
	    int		      i = 0;
	    ResultSetMetaData m = r.getMetaData();
	    int		      type[] = new int[m.getColumnCount()];

	    for (int j = 0; j < m.getColumnCount(); j++) {
		type[j] = m.getColumnType(j + 1);
	    }

	    trace("Start transfering data...");

	    while (r.next()) {
		transferRow(type, r, p);

		if (iMaxRows != 0 && i == iMaxRows) {
		    break;
		}

		i++;

		if (iMaxRows != 0 || i % 100 == 0) {
		    trace("Transfered " + i + " rows");
		}
	    }

	    trace("Finished");
	}
    }

    String sLast;

    /**
     * Method declaration
     *
     *
     * @param type
     * @param r
     * @param p
     *
     * @throws SQLException
     */
    void transferRow(int type[], ResultSet r,
		     PreparedStatement p) throws SQLException {
	sLast = null;

	p.clearParameters();

	int len = type.length;

	for (int i = 0; i < len; i++) {
	    int t = type[i];

	    sLast = "column=" + (i + 1) + " datatype=" + t;

	    Object o = r.getObject(i + 1);

	    if (o == null) {
		sLast += " value=" + o;

		p.setNull(i + 1, type[i]);
	    } else {

		// solves a problem for MS SQL 7
		if (t == Types.SMALLINT && o instanceof Integer) {
		    sLast += " SMALLINT: Converted Integer to Short";
		    o = new Short((short) ((Integer) o).intValue());
		} else if (t == Types.TINYINT && o instanceof Integer) {
		    sLast += " TINYINT: Converted Integer to Byte";
		    o = new Byte((byte) ((Integer) o).intValue());
		}

		sLast += " value=" + o;

		p.setObject(i + 1, o, type[i]);
	    }
	}

	p.execute();

	sLast = null;
    }

}
