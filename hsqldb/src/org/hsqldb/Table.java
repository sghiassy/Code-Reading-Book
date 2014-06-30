/*
 * Table.java
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

import java.sql.SQLException;
import java.util.Vector;

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
class Table {
    private String   sName;
    private Vector   vColumn;
    private Vector   vIndex;		 // vIndex(0) is always the primary key index
    private int      iVisibleColumns;    // table may contain a hidden primary key
    private int      iColumnCount;    // inclusive the (maybe hidden) primary key
    private int      iPrimaryKey;
    private boolean  bCached;
    private Database dDatabase;
    private Log      lLog;
    private int      iIndexCount;
    private int      iIdentityColumn;    // -1 means no such row
    private int      iIdentityId;
    private Vector   vConstraint;
    private int      iConstraintCount;
    Cache	     cCache;
    Vector	     vTrigs[];

    /**
     * Constructor declaration
     *
     *
     * @param db
     * @param log
     * @param name
     * @param cached
     */
    Table(Database db, boolean log, String name, boolean cached) {
	dDatabase = db;
	lLog = log ? db.getLog() : null;

	if (cached) {
	    cCache = lLog.cCache;
	    bCached = true;
	}

	sName = name;
	iPrimaryKey = -1;
	iIdentityColumn = -1;
	vColumn = new Vector();
	vIndex = new Vector();
	vConstraint = new Vector();
	vTrigs = new Vector[TriggerDef.numTrigs()];

	for (int vi = 0; vi < TriggerDef.numTrigs(); vi++) {
	    vTrigs[vi] = new Vector();
	}
    }

    /**
     * Method declaration
     *
     *
     * @param c
     */
    void addConstraint(Constraint c) {
	vConstraint.addElement(c);

	iConstraintCount++;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    Vector getConstraints() {
	return vConstraint;
    }

    /**
     * Method declaration
     *
     *
     * @param name
     * @param type
     *
     * @throws SQLException
     */
    void addColumn(String name, int type) throws SQLException {
	addColumn(name, type, true, false);
    }

    /**
     * Method declaration
     *
     *
     * @param c
     *
     * @throws SQLException
     */
    void addColumn(Column c) throws SQLException {
	addColumn(c.sName, c.iType, c.isNullable(), c.isIdentity());
    }

    /**
     * Method declaration
     *
     *
     * @param name
     * @param type
     * @param nullable
     * @param identity
     *
     * @throws SQLException
     */
    void addColumn(String name, int type, boolean nullable,
		   boolean identity) throws SQLException {
	if (identity) {
	    Trace.check(type == Column.INTEGER, Trace.WRONG_DATA_TYPE, name);
	    Trace.check(iIdentityColumn == -1, Trace.SECOND_PRIMARY_KEY,
			name);

	    iIdentityColumn = iColumnCount;
	}

	Trace.assert(iPrimaryKey == -1, "Table.addColumn");
	vColumn.addElement(new Column(name, nullable, type, identity));

	iColumnCount++;
    }

    /**
     * Method declaration
     *
     *
     * @param result
     *
     * @throws SQLException
     */
    void addColumns(Result result) throws SQLException {
	for (int i = 0; i < result.getColumnCount(); i++) {
	    addColumn(result.sLabel[i], result.iType[i], true, false);
	}
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    String getName() {
	return sName;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    int getInternalColumnCount() {

	// todo: this is a temporary solution;
	// the the hidden column is not really required
	return iColumnCount;
    }

    /**
     * Method declaration
     *
     *
     * @param withoutindex
     *
     * @return
     *
     * @throws SQLException
     */
    Table moveDefinition(String withoutindex) throws SQLException {
	Table tn = new Table(dDatabase, true, getName(), isCached());

	for (int i = 0; i < getColumnCount(); i++) {
	    tn.addColumn(getColumn(i));
	}

	// todo: there should be nothing special with the primary key!
	if (iVisibleColumns < iColumnCount) {
	    tn.createPrimaryKey();
	} else {
	    tn.createPrimaryKey(getPrimaryIndex().getColumns()[0]);
	}

	Index idx = null;

	while (true) {
	    idx = getNextIndex(idx);

	    if (idx == null) {
		break;
	    }

	    if (withoutindex != null && idx.getName().equals(withoutindex)) {
		continue;
	    }

	    if (idx == getPrimaryIndex()) {
		continue;
	    }

	    tn.createIndex(idx);
	}

	for (int i = 0; i < iConstraintCount; i++) {
	    Constraint c = (Constraint) vConstraint.elementAt(i);

	    c.replaceTable(this, tn);
	}

	tn.vConstraint = vConstraint;

	return tn;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    int getColumnCount() {
	return iVisibleColumns;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    int getIndexCount() {
	return iIndexCount;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    int getIdentityColumn() {
	return iIdentityColumn;
    }

    /**
     * Method declaration
     *
     *
     * @param c
     *
     * @return
     *
     * @throws SQLException
     */
    int getColumnNr(String c) throws SQLException {
	int i = searchColumn(c);

	if (i == -1) {
	    throw Trace.error(Trace.COLUMN_NOT_FOUND, c);
	}

	return i;
    }

    /**
     * Method declaration
     *
     *
     * @param c
     *
     * @return
     */
    int searchColumn(String c) {
	for (int i = 0; i < iColumnCount; i++) {
	    if (c.equals(((Column) vColumn.elementAt(i)).sName)) {
		return i;
	    }
	}

	return -1;
    }

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     */
    String getColumnName(int i) {
	return getColumn(i).sName;
    }

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     */
    int getColumnType(int i) {
	return getColumn(i).iType;
    }

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     */
    boolean getColumnIsNullable(int i) {
	return getColumn(i).isNullable();
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    Index getPrimaryIndex() throws SQLException {
	if (iPrimaryKey == -1) {
	    return null;
	}

	return getIndex(0);
    }

    /**
     * Method declaration
     *
     *
     * @param column
     *
     * @return
     *
     * @throws SQLException
     */
    Index getIndexForColumn(int column) throws SQLException {
	for (int i = 0; i < iIndexCount; i++) {
	    Index h = getIndex(i);

	    if (h.getColumns()[0] == column) {
		return h;
	    }
	}

	return null;
    }

    /**
     * Method declaration
     *
     *
     * @param col
     *
     * @return
     *
     * @throws SQLException
     */
    Index getIndexForColumns(int col[]) throws SQLException {
	for (int i = 0; i < iIndexCount; i++) {
	    Index h = getIndex(i);
	    int   icol[] = h.getColumns();
	    int   j = 0;

	    for (; j < col.length; j++) {
		if (j >= icol.length) {
		    break;
		}

		if (icol[j] != col[j]) {
		    break;
		}
	    }

	    if (j == col.length) {
		return h;
	    }
	}

	return null;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    String getIndexRoots() throws SQLException {
	Trace.assert(bCached, "Table.getIndexRootData");

	String s = "";

	for (int i = 0; i < iIndexCount; i++) {
	    Node f = getIndex(i).getRoot();

	    if (f != null) {
		s = s + f.getKey() + " ";
	    } else {
		s = s + "-1 ";
	    }
	}

	s += iIdentityId;

	return s;
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @throws SQLException
     */
    void setIndexRoots(String s) throws SQLException {

	// the user may try to set this; this is not only internal problem
	Trace.check(bCached, Trace.TABLE_NOT_FOUND);

	int j = 0;

	for (int i = 0; i < iIndexCount; i++) {
	    int n = s.indexOf(' ', j);
	    int p = Integer.parseInt(s.substring(j, n));

	    if (p != -1) {
		Row  r = cCache.getRow(p, this);
		Node f = r.getNode(i);

		getIndex(i).setRoot(f);
	    }

	    j = n + 1;
	}

	iIdentityId = Integer.parseInt(s.substring(j));
    }

    /**
     * Method declaration
     *
     *
     * @param index
     *
     * @return
     */
    Index getNextIndex(Index index) {
	int i = 0;

	if (index != null) {
	    for (; i < iIndexCount && getIndex(i) != index; i++);

	    i++;
	}

	if (i < iIndexCount) {
	    return getIndex(i);
	}

	return null;    // no more indexes
    }

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     */
    int getType(int i) {
	return getColumn(i).iType;
    }

    /**
     * Method declaration
     *
     *
     * @param column
     *
     * @throws SQLException
     */
    void createPrimaryKey(int column) throws SQLException {
	Trace.assert(iPrimaryKey == -1, "Table.createPrimaryKey(column)");

	iVisibleColumns = iColumnCount;
	iPrimaryKey = column;

	int col[] = {
	    column
	};

	createIndex(col, "SYSTEM_PK", true);
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void createPrimaryKey() throws SQLException {
	Trace.assert(iPrimaryKey == -1, "Table.createPrimaryKey");
	addColumn("SYSTEM_ID", Column.INTEGER, true, true);
	createPrimaryKey(iColumnCount - 1);

	iVisibleColumns = iColumnCount - 1;
    }

    /**
     * Method declaration
     *
     *
     * @param index
     *
     * @throws SQLException
     */
    void createIndex(Index index) throws SQLException {
	createIndex(index.getColumns(), index.getName(), index.isUnique());
    }

    /**
     * Method declaration
     *
     *
     * @param column
     * @param name
     * @param unique
     *
     * @throws SQLException
     */
    void createIndex(int column[], String name,
		     boolean unique) throws SQLException {
	Trace.assert(iPrimaryKey != -1, "createIndex");

	for (int i = 0; i < iIndexCount; i++) {
	    Index index = getIndex(i);

	    if (name.equals(index.getName())) {
		throw Trace.error(Trace.INDEX_ALREADY_EXISTS);
	    }
	}

	int s = column.length;

	// The primary key field is added for non-unique indexes
	// making all indexes unique
	int col[] = new int[unique ? s : s + 1];
	int type[] = new int[unique ? s : s + 1];

	for (int j = 0; j < s; j++) {
	    col[j] = column[j];
	    type[j] = getColumn(col[j]).iType;
	}

	if (!unique) {
	    col[s] = iPrimaryKey;
	    type[s] = getColumn(iPrimaryKey).iType;
	}

	Index index = new Index(name, col, type, unique);

	if (iIndexCount != 0) {
	    Trace.assert(isEmpty(), "createIndex");
	}

	vIndex.addElement(index);

	iIndexCount++;
    }

    /**
     * Method declaration
     *
     *
     * @param index
     *
     * @throws SQLException
     */
    void checkDropIndex(String index) throws SQLException {
	for (int i = 0; i < iIndexCount; i++) {
	    if (index.equals(getIndex(i).getName())) {
		Trace.check(i != 0, Trace.DROP_PRIMARY_KEY);

		return;
	    }
	}

	throw Trace.error(Trace.INDEX_NOT_FOUND, index);
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean isEmpty() {
	return getIndex(0).getRoot() == null;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    Object[] getNewRow() {
	return new Object[iColumnCount];
    }

    /**
     * Method declaration
     *
     *
     * @param from
     *
     * @throws SQLException
     */
    void moveData(Table from) throws SQLException {
	Index index = from.getPrimaryIndex();
	Node  n = index.first();

	while (n != null) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    Object o[] = n.getData();

	    insertNoCheck(o, null);

	    n = index.next(n);
	}

	index = getPrimaryIndex();
	n = index.first();

	while (n != null) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    Object o[] = n.getData();

	    from.deleteNoCheck(o, null);

	    n = index.next(n);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param col
     * @param deleted
     * @param inserted
     *
     * @throws SQLException
     */
    void checkUpdate(int col[], Result deleted,
		     Result inserted) throws SQLException {
	if (dDatabase.isReferentialIntegrity()) {
	    for (int i = 0; i < iConstraintCount; i++) {
		Constraint v = (Constraint) vConstraint.elementAt(i);

		v.checkUpdate(col, deleted, inserted);
	    }
	}
    }

    /**
     * Method declaration
     *
     *
     * @param result
     * @param c
     *
     * @throws SQLException
     */
    void insert(Result result, Channel c) throws SQLException {

	// if violation of constraints can occur, insert must be rolled back
	// outside of this function!
	Record r = result.rRoot;
	int    len = result.getColumnCount();

	while (r != null) {
	    Object row[] = getNewRow();

	    for (int i = 0; i < len; i++) {
		row[i] = r.data[i];
	    }

	    insert(row, c);

	    r = r.next;
	}
    }

    /**
     * Method declaration
     *
     *
     * @param row
     * @param c
     *
     * @throws SQLException
     */
    void insert(Object row[], Channel c) throws SQLException {
	fireAll(TriggerDef.INSERT_BEFORE, row);

	if (dDatabase.isReferentialIntegrity()) {
	    for (int i = 0; i < iConstraintCount; i++) {
		((Constraint) vConstraint.elementAt(i)).checkInsert(row);
	    }
	}

	insertNoCheck(row, c);
	fireAll(TriggerDef.INSERT_AFTER, row);
    }

    /**
     * Method declaration
     *
     *
     * @param row
     * @param c
     *
     * @throws SQLException
     */
    void insertNoCheck(Object row[], Channel c) throws SQLException {
	insertNoCheck(row, c, true);
    }

    /**
     * Method declaration
     *
     *
     * @param row
     * @param c
     * @param log
     *
     * @throws SQLException
     */
    void insertNoCheck(Object row[], Channel c,
		       boolean log) throws SQLException {
	if (iIdentityColumn != -1) {
	    Integer id = (Integer) row[iIdentityColumn];

	    if (id == null) {
		if (c != null) {
		    c.setLastIdentity(iIdentityId);
		}

		row[iIdentityColumn] = new Integer(iIdentityId++);
	    } else {
		int i = id.intValue();

		if (iIdentityId <= i) {
		    if (c != null) {
			c.setLastIdentity(i);
		    }

		    iIdentityId = i + 1;
		}
	    }
	}

	for (int i = 0; i < iColumnCount; i++) {
	    if (row[i] == null &&!getColumn(i).isNullable()) {
		throw Trace.error(Trace.TRY_TO_INSERT_NULL);
	    }
	}

	int i = 0;

	try {
	    Row r = new Row(this, row);

	    for (; i < iIndexCount; i++) {
		Node n = r.getNode(i);

		getIndex(i).insert(n);
	    }
	} catch (SQLException e) {    // rollback insert
	    for (--i; i >= 0; i--) {
		getIndex(i).delete(row, i == 0);
	    }

	    throw e;		      // and throw error again
	}

	if (c != null) {
	    c.addTransactionInsert(this, row);
	}

	if (lLog != null) {
	    lLog.write(c, getInsertStatement(row));
	}
    }

    /**
     * Method declaration
     *
     *
     * @param trigVecIndx
     * @param row
     */
    void fireAll(int trigVecIndx, Object row[]) {
	if (!dDatabase.isReferentialIntegrity()) {    // reloading db
	    return;
	}

    Vector trigVec = vTrigs[trigVecIndx];
	int trCount = trigVec.size();

    for (int i = 0; i < trCount; i++) {
	    TriggerDef td = (TriggerDef) trigVec.elementAt(i);
	    td.push(row);    // tell the trigger thread to fire with this row
	}
    }

    // statement-level triggers

    /**
     * Method declaration
     *
     *
     * @param trigVecIndx
     */
    void fireAll(int trigVecIndx) {
	Object row[] = new Object[1];

	row[0] = new String("Statement-level");

	fireAll(trigVecIndx, row);
    }

    /**
     * Method declaration
     *
     *
     * @param trigDef
     */
    void addTrigger(TriggerDef trigDef) {
	if (Trace.TRACE) {
	    Trace.trace("Trigger added "
			+ String.valueOf(trigDef.vectorIndx));
	}

	vTrigs[trigDef.vectorIndx].addElement(trigDef);
    }

    /**
     * Method declaration
     *
     *
     * @param row
     * @param c
     *
     * @throws SQLException
     */
    void delete(Object row[], Channel c) throws SQLException {
	fireAll(TriggerDef.DELETE_BEFORE_ROW, row);

	if (dDatabase.isReferentialIntegrity()) {
	    for (int i = 0; i < iConstraintCount; i++) {
		((Constraint) vConstraint.elementAt(i)).checkDelete(row);
	    }
	}

	deleteNoCheck(row, c);

	// fire the delete after statement trigger
	fireAll(TriggerDef.DELETE_AFTER_ROW, row);
    }

    /**
     * Method declaration
     *
     *
     * @param row
     * @param c
     *
     * @throws SQLException
     */
    void deleteNoCheck(Object row[], Channel c) throws SQLException {
	deleteNoCheck(row, c, true);
    }

    /**
     * Method declaration
     *
     *
     * @param row
     * @param c
     * @param log
     *
     * @throws SQLException
     */
    void deleteNoCheck(Object row[], Channel c,
		       boolean log) throws SQLException {
	for (int i = 1; i < iIndexCount; i++) {
	    getIndex(i).delete(row, false);
	}

	// must delete data last
	getIndex(0).delete(row, true);

	if (c != null) {
	    c.addTransactionDelete(this, row);
	}

	if (lLog != null) {
	    lLog.write(c, getDeleteStatement(row));
	}
    }

    /**
     * Method declaration
     *
     *
     * @param row
     *
     * @return
     *
     * @throws SQLException
     */
    String getInsertStatement(Object row[]) throws SQLException {
	StringBuffer a = new StringBuffer("INSERT INTO ");

	a.append(getName());
	a.append(" VALUES(");

	for (int i = 0; i < iVisibleColumns; i++) {
	    a.append(Column.createString(row[i], getColumn(i).iType));
	    a.append(',');
	}

	a.setCharAt(a.length() - 1, ')');

	return a.toString();
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean isCached() {
	return bCached;
    }

    /**
     * Method declaration
     *
     *
     * @param s
     *
     * @return
     */
    Index getIndex(String s) {
	for (int i = 0; i < iIndexCount; i++) {
	    Index h = getIndex(i);

	    if (s.equals(h.getName())) {
		return h;
	    }
	}

	// no such index
	return null;
    }

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     */
    Column getColumn(int i) {
	return (Column) vColumn.elementAt(i);
    }

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @return
     */
    private Index getIndex(int i) {
	return (Index) vIndex.elementAt(i);
    }

    /**
     * Method declaration
     *
     *
     * @param row
     *
     * @return
     *
     * @throws SQLException
     */
    private String getDeleteStatement(Object row[]) throws SQLException {
	StringBuffer a = new StringBuffer("DELETE FROM ");

	a.append(sName);
	a.append(" WHERE ");

	if (iVisibleColumns < iColumnCount) {
	    for (int i = 0; i < iVisibleColumns; i++) {
		a.append(getColumn(i).sName);
		a.append('=');
		a.append(Column.createString(row[i], getColumn(i).iType));

		if (i < iVisibleColumns - 1) {
		    a.append(" AND ");
		}
	    }
	} else {
	    a.append(getColumn(iPrimaryKey).sName);
	    a.append("=");
	    a.append(Column.createString(row[iPrimaryKey],
					 getColumn(iPrimaryKey).iType));
	}

	return a.toString();
    }

}
