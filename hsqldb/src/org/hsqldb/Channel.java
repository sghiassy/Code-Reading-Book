/*
 * Channel.java
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
import java.util.Vector;

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
class Channel {
    private Database dDatabase;
    private User     uUser;
    private Vector   tTransaction;
    private boolean  bAutoCommit;
    private boolean  bNestedTransaction;
    private boolean  bNestedOldAutoCommit;
    private int      iNestedOldTransIndex;
    private boolean  bReadOnly;
    private int      iMaxRows;
    private int      iLastIdentity;
    private boolean  bClosed;
    private int      iId;

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    public void finalize() throws SQLException {
	disconnect();
    }

    /**
     * Constructor declaration
     *
     *
     * @param c
     * @param id
     */
    Channel(Channel c, int id) {
	this(c.dDatabase, c.uUser, true, c.bReadOnly, id);
    }

    /**
     * Constructor declaration
     *
     *
     * @param db
     * @param user
     * @param autocommit
     * @param readonly
     * @param id
     */
    Channel(Database db, User user, boolean autocommit, boolean readonly,
	    int id) {
	iId = id;
	dDatabase = db;
	uUser = user;
	tTransaction = new Vector();
	bAutoCommit = autocommit;
	bReadOnly = readonly;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    int getId() {
	return iId;
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void disconnect() throws SQLException {
	if (bClosed) {
	    return;
	}

	rollback();

	dDatabase = null;
	uUser = null;
	tTransaction = null;
	bClosed = true;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean isClosed() {
	return bClosed;
    }

    /**
     * Method declaration
     *
     *
     * @param i
     */
    void setLastIdentity(int i) {
	iLastIdentity = i;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    int getLastIdentity() {
	return iLastIdentity;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    Database getDatabase() {
	return dDatabase;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    String getUsername() {
	return uUser.getName();
    }

    /**
     * Method declaration
     *
     *
     * @param user
     */
    void setUser(User user) {
	uUser = user;
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void checkAdmin() throws SQLException {
	uUser.checkAdmin();
    }

    /**
     * Method declaration
     *
     *
     * @param object
     * @param right
     *
     * @throws SQLException
     */
    void check(String object, int right) throws SQLException {
	uUser.check(object, right);
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void checkReadWrite() throws SQLException {
	Trace.check(!bReadOnly, Trace.DATABASE_IS_READONLY);
    }

    /**
     * Method declaration
     *
     *
     * @param s
     */
    void setPassword(String s) {
	uUser.setPassword(s);
    }

    /**
     * Method declaration
     *
     *
     * @param table
     * @param row
     *
     * @throws SQLException
     */
    void addTransactionDelete(Table table, Object row[]) throws SQLException {
	if (!bAutoCommit) {
	    Transaction t = new Transaction(true, table, row);

	    tTransaction.addElement(t);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param table
     * @param row
     *
     * @throws SQLException
     */
    void addTransactionInsert(Table table, Object row[]) throws SQLException {
	if (!bAutoCommit) {
	    Transaction t = new Transaction(false, table, row);

	    tTransaction.addElement(t);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param autocommit
     *
     * @throws SQLException
     */
    void setAutoCommit(boolean autocommit) throws SQLException {
	commit();

	bAutoCommit = autocommit;
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void commit() throws SQLException {
	tTransaction.removeAllElements();
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void rollback() throws SQLException {
	int i = tTransaction.size() - 1;

	while (i >= 0) {
	    Transaction t = (Transaction) tTransaction.elementAt(i);

	    t.rollback();

	    i--;
	}

	tTransaction.removeAllElements();
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void beginNestedTransaction() throws SQLException {
	Trace.assert(!bNestedTransaction, "beginNestedTransaction");

	bNestedOldAutoCommit = bAutoCommit;

	// now all transactions are logged
	bAutoCommit = false;
	iNestedOldTransIndex = tTransaction.size();
	bNestedTransaction = true;
    }

    /**
     * Method declaration
     *
     *
     * @param rollback
     *
     * @throws SQLException
     */
    void endNestedTransaction(boolean rollback) throws SQLException {
	Trace.assert(bNestedTransaction, "endNestedTransaction");

	int i = tTransaction.size() - 1;

	if (rollback) {
	    while (i >= iNestedOldTransIndex) {
		Transaction t = (Transaction) tTransaction.elementAt(i);

		t.rollback();

		i--;
	    }
	}

	bNestedTransaction = false;
	bAutoCommit = bNestedOldAutoCommit;

	if (bAutoCommit == true) {
	    tTransaction.setSize(iNestedOldTransIndex);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param readonly
     */
    void setReadOnly(boolean readonly) {
	bReadOnly = readonly;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean isReadOnly() {
	return bReadOnly;
    }

    /**
     * Method declaration
     *
     *
     * @param max
     */
    void setMaxRows(int max) {
	iMaxRows = max;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    int getMaxRows() {
	return iMaxRows;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean isNestedTransaction() {
	return bNestedTransaction;
    }

}
