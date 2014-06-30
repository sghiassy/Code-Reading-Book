/*
 * ConnectionDialog.java
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
public class ConnectionDialog extends Dialog implements ActionListener,
	ItemListener {
    final static String sJDBCTypes[][] = {
	 {
	    "HSQL Database Engine In-Memory", "org.hsqldb.jdbcDriver",
	    "jdbc:hsqldb:."
	}, {
	    "HSQL Database Engine Standalone", "org.hsqldb.jdbcDriver",
	    "jdbc:hsqldb:test"
	}, {
	    "HSQL Database Engine Server", "org.hsqldb.jdbcDriver",
	    "jdbc:hsqldb:hsql://localhost"
	}, {
	    "HSQL Database Engine WebServer", "org.hsqldb.jdbcDriver",
	    "jdbc:hsqldb:http://localhost"
	}, {
	    "JDBC-ODBC Brigde from Sun", "sun.jdbc.odbc.JdbcOdbcDriver",
	    "jdbc:odbc:test"
	}, {
	    "Oracle", "oracle.jdbc.driver.OracleDriver", "jdbc:oracle:oci8:@"
	}, {
	    "IBM DB2", "COM.ibm.db2.jdbc.app.DB2Driver", "jdbc:db2:test"
	}, {
	    "Cloudscape RMI", "RmiJdbc.RJDriver",
	    "jdbc:rmi://localhost:1099/jdbc:cloudscape:test;create=true"
	}, {
	    "InstantDb", "jdbc.idbDriver", "jdbc:idb:sample.prp"
	}, {
	    "PointBase", "com.pointbase.jdbc.jdbcUniversalDriver",
	    "jdbc:pointbase://localhost/sample"
	},    // PUBLIC / public
    };
    Connection		mConnection;
    TextField		mDriver, mURL, mUser, mPassword;
    Label		mError;

    /**
     * Method declaration
     *
     *
     * @param driver
     * @param url
     * @param user
     * @param password
     *
     * @return
     *
     * @throws Exception
     */
    public static Connection createConnection(String driver, String url,
					      String user,
					      String password) throws Exception {
	Class.forName(driver).newInstance();

	return DriverManager.getConnection(url, user, password);
    }

    /**
     * Constructor declaration
     *
     *
     * @param owner
     * @param title
     */
    ConnectionDialog(Frame owner, String title) {
	super(owner, title, true);
    }

    /**
     * Method declaration
     *
     */
    void create() {
	setLayout(new BorderLayout());

	Panel p = new Panel(new GridLayout(6, 2, 10, 10));

	p.setBackground(SystemColor.control);
	p.add(createLabel("Type:"));

	Choice types = new Choice();

	types.addItemListener(this);

	for (int i = 0; i < sJDBCTypes.length; i++) {
	    types.add(sJDBCTypes[i][0]);
	}

	p.add(types);
	p.add(createLabel("Driver:"));

	mDriver = new TextField("org.hsqldb.jdbcDriver");

	p.add(mDriver);
	p.add(createLabel("URL:"));

	mURL = new TextField("jdbc:hsqldb:.");

	p.add(mURL);
	p.add(createLabel("User:"));

	mUser = new TextField("sa");

	p.add(mUser);
	p.add(createLabel("Password:"));

	mPassword = new TextField("");

	mPassword.setEchoChar('*');
	p.add(mPassword);

	Button b;

	b = new Button("Ok");

	b.setActionCommand("ConnectOk");
	b.addActionListener(this);
	p.add(b);

	b = new Button("Cancel");

	b.setActionCommand("ConnectCancel");
	b.addActionListener(this);
	p.add(b);
	setLayout(new BorderLayout());
	add("East", createLabel(""));
	add("West", createLabel(""));

	mError = new Label("");

	Panel pMessage = createBorderPanel(mError);

	add("South", pMessage);
	add("North", createLabel(""));
	add("Center", p);
	doLayout();
	pack();

	Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
	Dimension size = getSize();

	setLocation((d.width - size.width) / 2, (d.height - size.height) / 2);
	show();
    }

    /**
     * Method declaration
     *
     *
     * @param s
     */
    void trace(String s) {
	mError.setText(s);
	System.out.println(s);
    }

    /**
     * Method declaration
     *
     *
     * @param owner
     * @param title
     *
     * @return
     */
    public static Connection createConnection(Frame owner, String title) {
	ConnectionDialog dialog = new ConnectionDialog(owner, title);

	dialog.create();

	return dialog.mConnection;
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    public static Label createLabel(String s) {
	Label l = new Label(s);

	l.setBackground(SystemColor.control);

	return l;
    }

    /**
     * Method declaration
     *
     *
     * @param center
     *
     * @return
     */
    public static Panel createBorderPanel(Component center) {
	Panel p = new Panel();

	p.setBackground(SystemColor.control);
	p.setLayout(new BorderLayout());
	p.add("Center", center);
	p.add("North", createLabel(""));
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
     * @param ev
     */
    public void actionPerformed(ActionEvent ev) {
	String s = ev.getActionCommand();

	if (s.equals("ConnectOk")) {
	    try {
		mConnection = createConnection(mDriver.getText(),
					       mURL.getText(),
					       mUser.getText(),
					       mPassword.getText());

		dispose();
	    } catch (Exception e) {
		e.printStackTrace();
		mError.setText(e.toString());
	    }
	} else if (s.equals("ConnectCancel")) {
	    dispose();
	}
    }

    /**
     * Method declaration
     *
     *
     * @param e
     */
    public void itemStateChanged(ItemEvent e) {
	String s = (String) e.getItem();

	for (int i = 0; i < sJDBCTypes.length; i++) {
	    if (s.equals(sJDBCTypes[i][0])) {
		mDriver.setText(sJDBCTypes[i][1]);
		mURL.setText(sJDBCTypes[i][2]);
	    }
	}
    }

}
