/*
 * DatabaseInformation.java
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
import java.io.*;
import java.net.*;
import java.util.*;

/**
 * DatabaseInformation class declaration
 *
 *
 * @version 1.0.0.1
 */
class DatabaseInformation {
    private Database dDatabase;
    private Access   aAccess;
    private Vector   tTable;

    /**
     * Constructor declaration
     *
     *
     * @param db
     * @param tables
     * @param access
     */
    DatabaseInformation(Database db, Vector tables, Access access) {
	dDatabase = db;
	tTable = tables;
	aAccess = access;
    }

    // some drivers use the following titles:
    // static final String META_SCHEM="OWNER";
    // static final String META_CAT="QUALIFIER";
    // static final String META_COLUMN_SIZE="PRECISION";
    // static final String META_BUFFER_LENGTH="LENGTH";
    // static final String META_DECIMAL_DIGITS="SCALE";
    // static final String META_NUM_PREC_RADIX="RADIX";
    // static final String META_FIXED_PREC_SCALE="MONEY";
    // static final String META_ORDINAL_POSITON="SEQ_IN_INDEX";
    // static final String META_ASC_OR_DESC="COLLATION";
    static final String META_SCHEM = "SCHEM";
    static final String META_CAT = "CAT";
    static final String META_COLUMN_SIZE = "COLUMN_SIZE";
    static final String META_BUFFER_LENGTH = "BUFFER_LENGTH";
    static final String META_DECIMAL_DIGITS = "DECIMAL_DIGITS";
    static final String META_NUM_PREC_RADIX = "NUM_PREC_RADIX";
    static final String META_FIXED_PREC_SCALE = "FIXED_PREC_SCALE";
    static final String META_ORDINAL_POSITON = "ORDINAL_POSITON";
    static final String META_ASC_OR_DESC = "ASC_OR_DESC";

    /**
     * Method declaration
     *
     *
     * @param name
     * @param channel
     *
     * @return
     *
     * @throws SQLException
     */
    Table getSystemTable(String name, Channel channel) throws SQLException {
	if (name.equals("SYSTEM_PROCEDURES")) {
	    Table t = createTable(name);

	    t.addColumn("PROCEDURE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("PROCEDURE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("PROCEDURE_NAME", Column.VARCHAR);
	    t.addColumn("NUM_INPUT_PARAMS", Column.INTEGER);
	    t.addColumn("NUM_OUTPUT_PARAMS", Column.INTEGER);
	    t.addColumn("NUM_RESULT_SETS", Column.INTEGER);
	    t.addColumn("REMARKS", Column.VARCHAR);
	    t.addColumn("PROCEDURE_TYPE", Column.SMALLINT);
	    t.createPrimaryKey();

	    return t;
	} else if (name.equals("SYSTEM_PROCEDURECOLUMNS")) {
	    Table t = createTable(name);

	    t.addColumn("PROCEDURE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("PROCEDURE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("PROCEDURE_NAME", Column.VARCHAR);
	    t.addColumn("COLUMN_NAME", Column.VARCHAR);
	    t.addColumn("COLUMN_TYPE", Column.SMALLINT);
	    t.addColumn("DATA_TYPE", Column.SMALLINT);
	    t.addColumn("TYPE_NAME", Column.VARCHAR);
	    t.addColumn("PRECISION", Column.INTEGER);
	    t.addColumn("LENGTH", Column.INTEGER);
	    t.addColumn("SCALE", Column.SMALLINT);
	    t.addColumn("RADIX", Column.SMALLINT);
	    t.addColumn("NULLABLE", Column.SMALLINT);
	    t.addColumn("REMARKS", Column.VARCHAR);
	    t.createPrimaryKey();

	    return t;
	} else if (name.equals("SYSTEM_TABLES")) {
	    Table t = createTable(name);

	    t.addColumn("TABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("TABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("TABLE_NAME", Column.VARCHAR);
	    t.addColumn("TABLE_TYPE", Column.VARCHAR);
	    t.addColumn("REMARKS", Column.VARCHAR);
	    t.createPrimaryKey();

	    for (int i = 0; i < tTable.size(); i++) {
		Table  table = (Table) tTable.elementAt(i);
		Object o[] = t.getNewRow();

		o[2] = table.getName();
		o[3] = "TABLE";

		t.insert(o, null);
	    }

	    return t;
	} else if (name.equals("SYSTEM_SCHEMAS")) {
	    Table t = createTable(name);

	    t.addColumn("TABLE_" + META_SCHEM, Column.VARCHAR);
	    t.createPrimaryKey();

	    return t;
	} else if (name.equals("SYSTEM_CATALOGS")) {
	    Table t = createTable(name);

	    t.addColumn("TABLE_" + META_CAT, Column.VARCHAR);
	    t.createPrimaryKey();

	    return t;
	} else if (name.equals("SYSTEM_TABLETYPES")) {
	    Table t = createTable(name);

	    t.addColumn("TABLE_TYPE", Column.VARCHAR);
	    t.createPrimaryKey();

	    Object o[] = t.getNewRow();

	    o[0] = "TABLE";

	    t.insert(o, null);

	    return t;
	} else if (name.equals("SYSTEM_COLUMNS")) {
	    Table t = createTable(name);

	    t.addColumn("TABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("TABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("TABLE_NAME", Column.VARCHAR);
	    t.addColumn("COLUMN_NAME", Column.VARCHAR);
	    t.addColumn("DATA_TYPE", Column.SMALLINT);
	    t.addColumn("TYPE_NAME", Column.VARCHAR);
	    t.addColumn(META_COLUMN_SIZE, Column.INTEGER);
	    t.addColumn(META_BUFFER_LENGTH, Column.INTEGER);
	    t.addColumn(META_DECIMAL_DIGITS, Column.INTEGER);
	    t.addColumn(META_NUM_PREC_RADIX, Column.INTEGER);
	    t.addColumn("NULLABLE", Column.INTEGER);
	    t.addColumn("REMARKS", Column.VARCHAR);

	    // Access and Intersolv do not return this fields
	    t.addColumn("COLUMN_DEF", Column.VARCHAR);
	    t.addColumn("SQL_DATA_TYPE", Column.VARCHAR);
	    t.addColumn("SQL_DATETIME_SUB", Column.INTEGER);
	    t.addColumn("CHAR_OCTET_LENGTH", Column.INTEGER);
	    t.addColumn("ORDINAL_POSITION", Column.VARCHAR);
	    t.addColumn("IS_NULLABLE", Column.VARCHAR);
	    t.createPrimaryKey();

	    for (int i = 0; i < tTable.size(); i++) {
		Table table = (Table) tTable.elementAt(i);
		int   columns = table.getColumnCount();

		for (int j = 0; j < columns; j++) {
		    Object o[] = t.getNewRow();

		    o[2] = table.getName();
		    o[3] = table.getColumnName(j);
		    o[4] = new Short((short) table.getColumnType(j));
		    o[5] = Column.getType(table.getColumnType(j));

		    int nullable;

		    if (table.getColumnIsNullable(j)) {
			nullable = DatabaseMetaData.columnNullable;
		    } else {
			nullable = DatabaseMetaData.columnNoNulls;
		    }

		    o[10] = new Integer(nullable);

		    if (table.getIdentityColumn() == j) {
			o[11] = "IDENTITY";
		    }

		    t.insert(o, null);
		}
	    }

	    return t;
	} else if (name.equals("SYSTEM_COLUMNPRIVILEGES")) {
	    Table t = createTable(name);

	    t.addColumn("TABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("TABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("TABLE_NAME", Column.VARCHAR);
	    t.addColumn("COLUMN_NAME", Column.VARCHAR);
	    t.addColumn("GRANTOR", Column.VARCHAR);
	    t.addColumn("GRANTEE", Column.VARCHAR);
	    t.addColumn("PRIVILEGE", Column.VARCHAR);
	    t.addColumn("IS_GRANTABLE", Column.VARCHAR);
	    t.createPrimaryKey();

	    /*
	     * // todo: get correct info
	     * for(int i=0;i<tTable.size();i++) {
	     * Table table=(Table)tTable.elementAt(i);
	     * int columns=table.getColumnCount();
	     * for(int j=0;j<columns;j++) {
	     * Object o[]=t.getNewRow();
	     * o[2]=table.getName();
	     * o[3]=table.getColumnName(j);
	     * o[4]="sa";
	     * o[6]="FULL";
	     * o[7]="NO";
	     * t.insert(o,null);
	     * }
	     * }
	     */
	    return t;
	} else if (name.equals("SYSTEM_TABLEPRIVILEGES")) {
	    Table t = createTable(name);

	    t.addColumn("TABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("TABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("TABLE_NAME", Column.VARCHAR);
	    t.addColumn("GRANTOR", Column.VARCHAR);
	    t.addColumn("GRANTEE", Column.VARCHAR);
	    t.addColumn("PRIVILEGE", Column.VARCHAR);
	    t.addColumn("IS_GRANTABLE", Column.VARCHAR);
	    t.createPrimaryKey();

	    for (int i = 0; i < tTable.size(); i++) {
		Table  table = (Table) tTable.elementAt(i);
		Object o[] = t.getNewRow();

		o[2] = table.getName();
		o[3] = "sa";
		o[5] = "FULL";

		t.insert(o, null);
	    }

	    return t;
	} else if (name.equals("SYSTEM_BESTROWIDENTIFIER")) {
	    Table t = createTable(name);

	    t.addColumn("SCOPE", Column.SMALLINT);
	    t.addColumn("COLUMN_NAME", Column.VARCHAR);
	    t.addColumn("DATA_TYPE", Column.SMALLINT);
	    t.addColumn("TYPE_NAME", Column.VARCHAR);
	    t.addColumn(META_COLUMN_SIZE, Column.INTEGER);
	    t.addColumn(META_BUFFER_LENGTH, Column.INTEGER);
	    t.addColumn(META_DECIMAL_DIGITS, Column.SMALLINT);
	    t.addColumn("PSEUDO_COLUMN", Column.SMALLINT);
	    t.createPrimaryKey();

	    return t;
	} else if (name.equals("SYSTEM_VERSIONCOLUMNS")) {
	    Table t = createTable(name);

	    t.addColumn("SCOPE", Column.INTEGER);
	    t.addColumn("COLUMN_NAME", Column.VARCHAR);
	    t.addColumn("DATA_TYPE", Column.SMALLINT);
	    t.addColumn("TYPE_NAME", Column.VARCHAR);
	    t.addColumn(META_COLUMN_SIZE, Column.SMALLINT);
	    t.addColumn(META_BUFFER_LENGTH, Column.INTEGER);
	    t.addColumn(META_DECIMAL_DIGITS, Column.SMALLINT);
	    t.addColumn("PSEUDO_COLUMN", Column.SMALLINT);
	    t.createPrimaryKey();

	    return t;
	} else if (name.equals("SYSTEM_PRIMARYKEYS")) {
	    Table t = createTable(name);

	    t.addColumn("TABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("TABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("TABLE_NAME", Column.VARCHAR);
	    t.addColumn("COLUMN_NAME", Column.VARCHAR);
	    t.addColumn("KEY_SEQ", Column.SMALLINT);
	    t.addColumn("PK_NAME", Column.VARCHAR);
	    t.createPrimaryKey();

	    for (int i = 0; i < tTable.size(); i++) {
		Table table = (Table) tTable.elementAt(i);
		Index index = table.getIndex("SYSTEM_PK");
		int   cols[] = index.getColumns();
		int   len = cols.length;

		for (int j = 0; j < len; j++) {
		    Object o[] = t.getNewRow();

		    o[2] = table.getName();
		    o[3] = table.getColumnName(cols[j]);
		    o[4] = new Short((short) (j + 1));
		    o[5] = "SYSTEM_PK";

		    t.insert(o, null);
		}
	    }

	    return t;
	} else if (name.equals("SYSTEM_IMPORTEDKEYS")) {
	    Table t = createTable(name);

	    t.addColumn("PKTABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("PKTABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("PKTABLE_NAME", Column.VARCHAR);
	    t.addColumn("PKCOLUMN_NAME", Column.VARCHAR);
	    t.addColumn("FKTABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("FKTABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("FKTABLE_NAME", Column.VARCHAR);
	    t.addColumn("FKCOLUMN_NAME", Column.VARCHAR);
	    t.addColumn("KEY_SEQ", Column.SMALLINT);
	    t.addColumn("UPDATE_RULE", Column.SMALLINT);
	    t.addColumn("DELETE_RULE", Column.SMALLINT);
	    t.addColumn("FK_NAME", Column.VARCHAR);
	    t.addColumn("PK_NAME", Column.VARCHAR);
	    t.addColumn("DEFERRABILITY", Column.SMALLINT);
	    t.createPrimaryKey();

	    return t;
	} else if (name.equals("SYSTEM_EXPORTEDKEYS")) {
	    Table t = createTable(name);

	    t.addColumn("PKTABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("PKTABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("PKTABLE_NAME", Column.VARCHAR);
	    t.addColumn("PKCOLUMN_NAME", Column.VARCHAR);
	    t.addColumn("FKTABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("FKTABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("FKTABLE_NAME", Column.VARCHAR);
	    t.addColumn("FKCOLUMN_NAME", Column.VARCHAR);
	    t.addColumn("KEY_SEQ", Column.SMALLINT);
	    t.addColumn("UPDATE_RULE", Column.SMALLINT);
	    t.addColumn("DELETE_RULE", Column.SMALLINT);
	    t.addColumn("FK_NAME", Column.VARCHAR);
	    t.addColumn("PK_NAME", Column.VARCHAR);
	    t.addColumn("DEFERRABILITY", Column.SMALLINT);
	    t.createPrimaryKey();

	    return t;
	} else if (name.equals("SYSTEM_CROSSREFERENCE")) {
	    Table t = createTable(name);

	    t.addColumn("PKTABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("PKTABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("PKTABLE_NAME", Column.VARCHAR);
	    t.addColumn("PKCOLUMN_NAME", Column.VARCHAR);
	    t.addColumn("FKTABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("FKTABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("FKTABLE_NAME", Column.VARCHAR);
	    t.addColumn("FKCOLUMN_NAME", Column.VARCHAR);
	    t.addColumn("KEY_SEQ", Column.INTEGER);
	    t.addColumn("UPDATE_RULE", Column.SMALLINT);
	    t.addColumn("DELETE_RULE", Column.SMALLINT);
	    t.addColumn("FK_NAME", Column.VARCHAR);
	    t.addColumn("PK_NAME", Column.VARCHAR);
	    t.addColumn("DEFERRABILITY", Column.SMALLINT);
	    t.createPrimaryKey();

	    return t;
	} else if (name.equals("SYSTEM_TYPEINFO")) {
	    Table t = createTable(name);

	    t.addColumn("TYPE_NAME", Column.VARCHAR);
	    t.addColumn("DATA_TYPE", Column.SMALLINT);
	    t.addColumn("PRECISION", Column.INTEGER);
	    t.addColumn("LITERAL_PREFIX", Column.VARCHAR);
	    t.addColumn("LITERAL_SUFFIX", Column.VARCHAR);
	    t.addColumn("CREATE_PARAMS", Column.VARCHAR);
	    t.addColumn("NULLABLE", Column.SMALLINT);
	    t.addColumn("CASE_SENSITIVE", Column.VARCHAR);
	    t.addColumn("SEARCHABLE", Column.SMALLINT);
	    t.addColumn("UNSIGNED_ATTRIBUTE", Column.BIT);
	    t.addColumn(META_FIXED_PREC_SCALE, Column.BIT);
	    t.addColumn("AUTO_INCREMENT", Column.BIT);
	    t.addColumn("LOCAL_TYPE_NAME", Column.VARCHAR);
	    t.addColumn("MINIMUM_SCALE", Column.SMALLINT);
	    t.addColumn("MAXIMUM_SCALE", Column.SMALLINT);

	    // this columns are not supported by Access and Intersolv
	    t.addColumn("SQL_DATE_TYPE", Column.INTEGER);
	    t.addColumn("SQL_DATETIME_SUB", Column.INTEGER);
	    t.addColumn("NUM_PREC_RADIX", Column.INTEGER);
	    t.createPrimaryKey();

	    for (int i = 0; i < Column.TYPES.length; i++) {
		Object o[] = t.getNewRow();
		int    type = Column.TYPES[i];

		o[0] = Column.getType(type);
		o[1] = new Short((short) type);
		o[2] = new Integer(0);		 // precision
		o[6] = new Short((short) DatabaseMetaData.typeNullable);
		o[7] = new Boolean(true);	 // case sensitive
		o[8] = new Short((short) DatabaseMetaData.typeSearchable);
		o[9] = new Boolean(false);       // unsigned
		o[10] = new Boolean(type == Column.NUMERIC
				    || type == Column.DECIMAL);
		o[11] = new Boolean(type == Column.INTEGER);
		o[12] = o[0];
		o[13] = new Short((short) 0);
		o[14] = new Short((short) 0);    // maximum scale
		o[15] = new Integer((short) 0);
		o[16] = o[15];
		o[17] = new Integer(10);

		t.insert(o, null);
	    }

	    return t;
	} else if (name.equals("SYSTEM_INDEXINFO")) {
	    Table t = createTable(name);

	    t.addColumn("TABLE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("TABLE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("TABLE_NAME", Column.VARCHAR);
	    t.addColumn("NON_UNIQUE", Column.BIT);
	    t.addColumn("INDEX_QUALIFIER", Column.VARCHAR);
	    t.addColumn("INDEX_NAME", Column.VARCHAR);
	    t.addColumn("TYPE", Column.SMALLINT);
	    t.addColumn(META_ORDINAL_POSITON, Column.SMALLINT);
	    t.addColumn("COLUMN_NAME", Column.VARCHAR);
	    t.addColumn(META_ASC_OR_DESC, Column.VARCHAR);
	    t.addColumn("CARDINALITY", Column.INTEGER);
	    t.addColumn("PAGES", Column.INTEGER);
	    t.addColumn("FILTER_CONDITION", Column.VARCHAR);
	    t.createPrimaryKey();

	    for (int i = 0; i < tTable.size(); i++) {
		Table table = (Table) tTable.elementAt(i);
		Index index = null;

		while (true) {
		    index = table.getNextIndex(index);

		    if (index == null) {
			break;
		    }

		    int cols[] = index.getColumns();
		    int len = cols.length;

		    // this removes the column that makes every index unique
		    if (!index.isUnique()) {
			len--;
		    }

		    for (int j = 0; j < len; j++) {
			Object o[] = t.getNewRow();

			o[2] = table.getName();
			o[3] = new Boolean(!index.isUnique());
			o[5] = index.getName();
			o[6] = new Short(DatabaseMetaData.tableIndexOther);
			o[7] = new Short((short) (j + 1));
			o[8] = table.getColumnName(cols[j]);
			o[9] = "A";

			t.insert(o, null);
		    }
		}
	    }

	    return t;
	} else if (name.equals("SYSTEM_UDTS")) {
	    Table t = createTable(name);

	    t.addColumn("TYPE_" + META_CAT, Column.VARCHAR);
	    t.addColumn("TYPE_" + META_SCHEM, Column.VARCHAR);
	    t.addColumn("TYPE_NAME", Column.VARCHAR);
	    t.addColumn("CLASS_NAME", Column.BIT);
	    t.addColumn("DATA_TYPE", Column.VARCHAR);
	    t.addColumn("REMARKS", Column.VARCHAR);
	    t.createPrimaryKey();

	    return t;
	} else if (name.equals("SYSTEM_CONNECTIONINFO")) {
	    Table t = createTable(name);

	    t.addColumn("KEY", Column.VARCHAR);
	    t.addColumn("VALUE", Column.VARCHAR);
	    t.createPrimaryKey();

	    Object o[] = t.getNewRow();

	    o[0] = "USER";
	    o[1] = channel.getUsername();

	    t.insert(o, null);

	    o = t.getNewRow();
	    o[0] = "READONLY";
	    o[1] = channel.isReadOnly() ? "TRUE" : "FALSE";

	    t.insert(o, null);

	    o = t.getNewRow();
	    o[0] = "MAXROWS";
	    o[1] = "" + channel.getMaxRows();

	    t.insert(o, null);

	    o = t.getNewRow();
	    o[0] = "DATABASE";
	    o[1] = "" + channel.getDatabase().getName();

	    t.insert(o, null);

	    o = t.getNewRow();
	    o[0] = "IDENTITY";
	    o[1] = "" + channel.getLastIdentity();

	    t.insert(o, null);

	    return t;
	} else if (name.equals("SYSTEM_USERS")) {
	    Table t = createTable(name);

	    t.addColumn("USER", Column.VARCHAR);
	    t.addColumn("ADMIN", Column.BIT);
	    t.createPrimaryKey();

	    Vector v = aAccess.getUsers();

	    for (int i = 0; i < v.size(); i++) {
		User u = (User) v.elementAt(i);

		// todo: this is not a nice implementation
		if (u == null) {
		    continue;
		}

		String user = u.getName();

		if (!user.equals("PUBLIC")) {
		    Object o[] = t.getNewRow();

		    o[0] = user;
		    o[1] = new Boolean(u.isAdmin());

		    t.insert(o, null);
		}
	    }

	    return t;
	}

	return null;
    }

    /**
     * Method declaration
     *
     *
     * @param bDrop
     * @param bInsert
     * @param bCached
     * @param channel
     *
     * @return
     *
     * @throws SQLException
     */
    Result getScript(boolean bDrop, boolean bInsert, boolean bCached,
		     Channel channel) throws SQLException {
	channel.checkAdmin();

	Result r = new Result(1);

	r.iType[0] = Column.VARCHAR;
	r.sTable[0] = "SYSTEM_SCRIPT";
	r.sLabel[0] = "COMMAND";
	r.sName[0] = "COMMAND";

	StringBuffer a;

	for (int i = 0; i < tTable.size(); i++) {
	    Table t = (Table) tTable.elementAt(i);

	    if (bDrop) {
		addRow(r, "DROP TABLE " + t.getName());
	    }

	    a = new StringBuffer("CREATE ");

	    if (t.isCached()) {
		a.append("CACHED ");
	    }

	    a.append("TABLE ");
	    a.append(t.getName());
	    a.append('(');

	    int   columns = t.getColumnCount();
	    Index pki = t.getIndex("SYSTEM_PK");
	    int   pk = (pki == null) ? -1 : pki.getColumns()[0];

	    for (int j = 0; j < columns; j++) {
		a.append(t.getColumnName(j));
		a.append(' ');
		a.append(Column.getType(t.getType(j)));

		if (!t.getColumnIsNullable(j)) {
		    a.append(" NOT NULL");
		}

		if (j == t.getIdentityColumn()) {
		    a.append(" IDENTITY");
		}

		if (j == pk) {
		    a.append(" PRIMARY KEY");
		}

		if (j < columns - 1) {
		    a.append(',');
		}
	    }

	    Vector v = t.getConstraints();

	    for (int j = 0; j < v.size(); j++) {
		Constraint c = (Constraint) v.elementAt(j);

		if (c.getType() == Constraint.FOREIGN_KEY) {
		    a.append(",FOREIGN KEY");

		    int col[] = c.getRefColumns();

		    a.append(getColumnList(c.getRef(), col, col.length));
		    a.append("REFERENCES ");
		    a.append(c.getMain().getName());

		    col = c.getMainColumns();

		    a.append(getColumnList(c.getMain(), col, col.length));
		} else if (c.getType() == Constraint.UNIQUE) {
		    a.append(",UNIQUE");

		    int col[] = c.getMainColumns();

		    a.append(getColumnList(c.getMain(), col, col.length));
		}
	    }

	    a.append(')');
	    addRow(r, a.toString());

	    Index index = null;

	    while (true) {
		index = t.getNextIndex(index);

		if (index == null) {
		    break;
		}

		String indexname = index.getName();

		if (indexname.equals("SYSTEM_PK")) {
		    continue;
		} else if (indexname.startsWith("SYSTEM_FOREIGN_KEY")) {

		    // foreign keys where created in the 'create table'
		    continue;
		} else if (indexname.startsWith("SYSTEM_CONSTRAINT")) {

		    // constraints where created in the 'create table'
		    continue;
		}

		a = new StringBuffer("CREATE ");

		if (index.isUnique()) {
		    a.append("UNIQUE ");
		}

		a.append("INDEX ");
		a.append(indexname);
		a.append(" ON ");
		a.append(t.getName());

		int col[] = index.getColumns();
		int len = col.length;

		if (!index.isUnique()) {
		    len--;
		}

		a.append(getColumnList(t, col, len));
		addRow(r, a.toString());
	    }

	    if (bInsert) {
		Index   primary = t.getPrimaryIndex();
		Node    x = primary.first();
		boolean integrity = true;

		if (x != null) {
		    integrity = false;

		    addRow(r, "SET REFERENTIAL_INTEGRITY FALSE");
		}

		while (x != null) {
		    addRow(r, t.getInsertStatement(x.getData()));

		    x = primary.next(x);
		}

		if (!integrity) {
		    addRow(r, "SET REFERENTIAL_INTEGRITY TRUE");
		}
	    }

	    if (bCached && t.isCached()) {
		a = new StringBuffer("SET TABLE ");

		a.append(t.getName());
		a.append(" INDEX '");
		a.append(t.getIndexRoots());
		a.append("'");
		addRow(r, a.toString());
	    }

	    // trigger script
	    int numTrigs = TriggerDef.numTrigs();
	    for (int tv = 0; tv < numTrigs; tv++) {
		    Vector trigVec = t.vTrigs[tv];
			int trCount = trigVec.size();

    		for (int k = 0; k < trCount; k++) {
	    	a = ((TriggerDef) trigVec.elementAt(k)).toBuf();

		    addRow(r, a.toString());
		}
	    }
	}

	Vector v = aAccess.getUsers();

	for (int i = 0; i < v.size(); i++) {
	    User u = (User) v.elementAt(i);

	    // todo: this is not a nice implementation
	    if (u == null) {
		continue;
	    }

	    String name = u.getName();

	    if (!name.equals("PUBLIC")) {
		a = new StringBuffer("CREATE USER ");

		a.append(name);
		a.append(" PASSWORD ");
		a.append("\"" + u.getPassword() + "\"");

		if (u.isAdmin()) {
		    a.append(" ADMIN");
		}

		addRow(r, a.toString());
	    }

	    Hashtable rights = u.getRights();

	    if (rights == null) {
		continue;
	    }

	    Enumeration e = rights.keys();

	    while (e.hasMoreElements()) {
		String object = (String) e.nextElement();
		int    right = ((Integer) (rights.get(object))).intValue();

		if (right == 0) {
		    continue;
		}

		a = new StringBuffer("GRANT ");

		a.append(Access.getRight(right));
		a.append(" ON ");
		a.append(object);
		a.append(" TO ");
		a.append(u.getName());
		addRow(r, a.toString());
	    }
	}

	if (dDatabase.isIgnoreCase()) {
	    addRow(r, "SET IGNORECASE TRUE");
	}

	Hashtable   h = dDatabase.getAlias();
	Enumeration e = h.keys();

	while (e.hasMoreElements()) {
	    String alias = (String) e.nextElement();
	    String java = (String) h.get(alias);

	    addRow(r, "CREATE ALIAS " + alias + " FOR \"" + java + "\"");
	}

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @param t
     * @param col
     * @param len
     *
     * @return
     */
    private String getColumnList(Table t, int col[], int len) {
	StringBuffer a = new StringBuffer("(");

	for (int i = 0; i < len; i++) {
	    a.append(t.getColumnName(col[i]));

	    if (i < len - 1) {
		a.append(',');
	    }
	}

	return a.append(')').toString();
    }

    /**
     * Method declaration
     *
     *
     * @param r
     * @param sql
     */
    private void addRow(Result r, String sql) {
	String s[] = new String[1];

	s[0] = sql;

	r.add(s);
    }

    /**
     * Method declaration
     *
     *
     * @param name
     *
     * @return
     */
    private Table createTable(String name) {
	return new Table(dDatabase, false, name, false);
    }

}
