/*
 * Parser.java
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

import java.util.*;
import java.sql.*;

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
class Parser {
    private Database  dDatabase;
    private Tokenizer tTokenizer;
    private Channel   cChannel;
    private String    sTable;
    private String    sToken;
    private Object    oData;
    private int       iType;
    private int       iToken;

    /**
     * Constructor declaration
     *
     *
     * @param db
     * @param t
     * @param channel
     */
    Parser(Database db, Tokenizer t, Channel channel) {
	dDatabase = db;
	tTokenizer = t;
	cChannel = channel;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    Result processSelect() throws SQLException {
	Select select = parseSelect();

	if (select.sIntoTable == null) {
// fredt@users.sourceforge.net begin changes from 1.50
//	   return select.getResult(cChannel.getMaxRows());
      return select.getResult( select.limitStart, select.limitCount );
// fredt@users.sourceforge.net end changes from 1.50
	} else {
	    Result r = select.getResult(0);
	    Table  t = new Table(dDatabase, true, select.sIntoTable, false);

	    t.addColumns(r);
	    t.createPrimaryKey();

	    // SELECT .. INTO can't fail because of violation of primary key
	    t.insert(r, cChannel);
	    dDatabase.linkTable(t);

	    int i = r.getSize();

	    r = new Result();
	    r.iUpdateCount = i;

	    return r;
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
    Result processCall() throws SQLException {
	Expression e = parseExpression();

	e.resolve(null);

	int    type = e.getDataType();
	Object o = e.getValue();
	Result r = new Result(1);

	r.sTable[0] = "";
	r.iType[0] = type;
	r.sLabel[0] = "";
	r.sName[0] = "";

	Object row[] = new Object[1];

	row[0] = o;

	r.add(row);

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    Result processUpdate() throws SQLException {
	String token = tTokenizer.getString();

	cChannel.checkReadWrite();
	cChannel.check(token, Access.UPDATE);

	Table       table = dDatabase.getTable(token, cChannel);
	TableFilter filter = new TableFilter(table, null, false);

	tTokenizer.getThis("SET");

	Vector vColumn = new Vector();
	Vector eColumn = new Vector();
	int    len = 0;

	token = null;

	do {
	    len++;

	    int i = table.getColumnNr(tTokenizer.getString());

	    vColumn.addElement(new Integer(i));
	    tTokenizer.getThis("=");

	    Expression e = parseExpression();

	    e.resolve(filter);
	    eColumn.addElement(e);

	    token = tTokenizer.getString();
	} while (token.equals(","));

	Expression eCondition = null;

	if (token.equals("WHERE")) {
	    eCondition = parseExpression();

	    eCondition.resolve(filter);
	    filter.setCondition(eCondition);
	} else {
	    tTokenizer.back();
	}

	// do the update
	table.fireAll(TriggerDef.UPDATE_BEFORE);

	Expression exp[] = new Expression[len];

	eColumn.copyInto(exp);

	int col[] = new int[len];
	int type[] = new int[len];

	for (int i = 0; i < len; i++) {
	    col[i] = ((Integer) vColumn.elementAt(i)).intValue();
	    type[i] = table.getType(col[i]);
	}

	int count = 0;

	if (filter.findFirst()) {
	    Result del = new Result();    // don't need column count and so on
	    Result ins = new Result();
	    int    size = table.getColumnCount();

	    do {
		if (eCondition == null || eCondition.test()) {
		    Object nd[] = filter.oCurrentData;

		    del.add(nd);

		    Object ni[] = table.getNewRow();

		    for (int i = 0; i < size; i++) {
			ni[i] = nd[i];
		    }

		    for (int i = 0; i < len; i++) {
			ni[col[i]] = exp[i].getValue(type[i]);
		    }

		    ins.add(ni);
		}
	    } while (filter.next());

	    cChannel.beginNestedTransaction();

	    try {
		Record nd = del.rRoot;

		while (nd != null) {
		    table.fireAll(TriggerDef.UPDATE_BEFORE_ROW, nd.data);
		    table.deleteNoCheck(nd.data, cChannel);

		    nd = nd.next;
		}

		Record ni = ins.rRoot;

		while (ni != null) {
		    table.insertNoCheck(ni.data, cChannel);

		    ni = ni.next;
		    count++;
		}

		table.checkUpdate(col, del, ins);

		ni = ins.rRoot;

		while (ni
		       != null) {    // fire triggers now that update has been checked
		    table.fireAll(TriggerDef.UPDATE_AFTER_ROW, ni.data);

		    ni = ni.next;
		}

		cChannel.endNestedTransaction(false);
	    } catch (SQLException e) {

		// update failed (violation of primary key / referential integrity)
		cChannel.endNestedTransaction(true);

		throw e;
	    }
	}

	table.fireAll(TriggerDef.UPDATE_AFTER);

	Result r = new Result();

	r.iUpdateCount = count;

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    Result processDelete() throws SQLException {
	tTokenizer.getThis("FROM");

	String token = tTokenizer.getString();

	cChannel.checkReadWrite();
	cChannel.check(token, Access.DELETE);

	Table       table = dDatabase.getTable(token, cChannel);
	TableFilter filter = new TableFilter(table, null, false);

	token = tTokenizer.getString();

	Expression eCondition = null;

	if (token.equals("WHERE")) {
	    eCondition = parseExpression();

	    eCondition.resolve(filter);
	    filter.setCondition(eCondition);
	} else {
	    tTokenizer.back();
	}

	table.fireAll(TriggerDef.DELETE_BEFORE);

	int count = 0;

	if (filter.findFirst()) {
	    Result del = new Result();    // don't need column count and so on

	    do {
		if (eCondition == null || eCondition.test()) {
		    del.add(filter.oCurrentData);
		}
	    } while (filter.next());

	    Record n = del.rRoot;

	    while (n != null) {
		table.delete(n.data, cChannel);

		count++;
		n = n.next;
	    }
	}

	table.fireAll(TriggerDef.DELETE_AFTER);

	Result r = new Result();

	r.iUpdateCount = count;

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    Result processInsert() throws SQLException {
	tTokenizer.getThis("INTO");

	String token = tTokenizer.getString();

	cChannel.checkReadWrite();
	cChannel.check(token, Access.INSERT);

	Table t = dDatabase.getTable(token, cChannel);

	token = tTokenizer.getString();

	Vector vcolumns = null;

	if (token.equals("(")) {
	    vcolumns = new Vector();

	    int i = 0;

	    while (true) {
		vcolumns.addElement(tTokenizer.getString());

		i++;
		token = tTokenizer.getString();

		if (token.equals(")")) {
		    break;
		}

		if (!token.equals(",")) {
		    throw Trace.error(Trace.UNEXPECTED_TOKEN, token);
		}
	    }

	    token = tTokenizer.getString();
	}

	int count = 0;
	int len;

	if (vcolumns == null) {
	    len = t.getColumnCount();
	} else {
	    len = vcolumns.size();
	}

	if (token.equals("VALUES")) {
	    tTokenizer.getThis("(");

	    Object row[] = t.getNewRow();
	    int    i = 0;

	    while (true) {
		int column;

		if (vcolumns == null) {
		    column = i;

		    if (i > len) {
			throw Trace.error(Trace.COLUMN_COUNT_DOES_NOT_MATCH);
		    }
		} else {
		    column = t.getColumnNr((String) vcolumns.elementAt(i));
		}

		row[column] = getValue(t.getType(column));
		i++;
		token = tTokenizer.getString();

		if (token.equals(")")) {
		    break;
		}

		if (!token.equals(",")) {
		    throw Trace.error(Trace.UNEXPECTED_TOKEN, token);
		}
	    }

	    t.insert(row, cChannel);

	    count = 1;
	} else if (token.equals("SELECT")) {
	    Result result = processSelect();
	    Record r = result.rRoot;

	    Trace.check(len == result.getColumnCount(),
			Trace.COLUMN_COUNT_DOES_NOT_MATCH);

	    int col[] = new int[len];
	    int type[] = new int[len];

	    for (int i = 0; i < len; i++) {
		int j;

		if (vcolumns == null) {
		    j = i;
		} else {
		    j = t.getColumnNr((String) vcolumns.elementAt(i));
		}

		col[i] = j;
		type[i] = t.getType(j);
	    }

	    cChannel.beginNestedTransaction();

	    try {
		while (r != null) {
		    Object row[] = t.getNewRow();

		    for (int i = 0; i < len; i++) {
			row[col[i]] = Column.convertObject(r.data[i],
							   type[i]);
		    }

		    t.insert(row, cChannel);

		    count++;
		    r = r.next;
		}

		cChannel.endNestedTransaction(false);
	    } catch (SQLException e) {

		// insert failed (violation of primary key)
		cChannel.endNestedTransaction(true);

		throw e;
	    }
	} else {
	    throw Trace.error(Trace.UNEXPECTED_TOKEN, token);
	}

	Result r = new Result();

	r.iUpdateCount = count;

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    private Select parseSelect() throws SQLException {
	Select select = new Select();
// fredt@users.sourceforge.net begin changes from 1.50
   select.limitStart = 0;
   select.limitCount = cChannel.getMaxRows();
// fredt@users.sourceforge.net end changes from 1.50
	String token = tTokenizer.getString();

	if (token.equals("DISTINCT")) {
	    select.bDistinct = true;
// fredt@users.sourceforge.net begin changes from 1.50
 	} else if( token.equals("LIMIT")) {
 		String limStart = tTokenizer.getString();
 		String limEnd = tTokenizer.getString();
 		//System.out.println( "LIMIT used from "+limStart+","+limEnd);
 		select.limitStart = new Integer(limStart).intValue();
 		select.limitCount = new Integer(limEnd).intValue();
// fredt@users.sourceforge.net end changes from 1.50
	} else {
	    tTokenizer.back();
	}

	// parse column list
	Vector vcolumn = new Vector();

	do {
	    Expression e = parseExpression();

	    token = tTokenizer.getString();

	    if (token.equals("AS")) {
		e.setAlias(tTokenizer.getName());

		token = tTokenizer.getString();
	    } else if (tTokenizer.wasName()) {
		e.setAlias(token);

		token = tTokenizer.getString();
	    }

	    vcolumn.addElement(e);
	} while (token.equals(","));

	if (token.equals("INTO")) {
	    select.sIntoTable = tTokenizer.getString();
	    token = tTokenizer.getString();
	}

	if (!token.equals("FROM")) {
	    throw Trace.error(Trace.UNEXPECTED_TOKEN, token);
	}

	Expression condition = null;

	// parse table list
	Vector     vfilter = new Vector();

	vfilter.addElement(parseTableFilter(false));

	while (true) {
	    token = tTokenizer.getString();

	    if (token.equals("LEFT")) {
		token = tTokenizer.getString();

		if (token.equals("OUTER")) {
		    token = tTokenizer.getString();
		}

		Trace.check(token.equals("JOIN"), Trace.UNEXPECTED_TOKEN,
			    token);
		vfilter.addElement(parseTableFilter(true));
		tTokenizer.getThis("ON");

		condition = addCondition(condition, parseExpression());
	    } else if (token.equals("INNER")) {
		tTokenizer.getThis("JOIN");
		vfilter.addElement(parseTableFilter(false));
		tTokenizer.getThis("ON");

		condition = addCondition(condition, parseExpression());
	    } else if (token.equals(",")) {
		vfilter.addElement(parseTableFilter(false));
	    } else {
		break;
	    }
	}

	tTokenizer.back();

	int	    len = vfilter.size();
	TableFilter filter[] = new TableFilter[len];

	vfilter.copyInto(filter);

	select.tFilter = filter;

	// expand [table.]* columns
	len = vcolumn.size();

	for (int i = 0; i < len; i++) {
	    Expression e = (Expression) (vcolumn.elementAt(i));

	    if (e.getType() == Expression.ASTERIX) {
		int    current = i;
		Table  table = null;
		String n = e.getTableName();

		for (int t = 0; t < filter.length; t++) {
		    TableFilter f = filter[t];

		    e.resolve(f);

		    if (n != null &&!n.equals(f.getName())) {
			continue;
		    }

		    table = f.getTable();

		    int col = table.getColumnCount();

		    for (int c = 0; c < col; c++) {
			Expression ins =
			    new Expression(f.getName(),
					   table.getColumnName(c));

			vcolumn.insertElementAt(ins, current++);

			// now there is one element more to parse
			len++;
		    }
		}

		Trace.check(table != null, Trace.TABLE_NOT_FOUND, n);

		// minus the asterix element
		len--;

		vcolumn.removeElementAt(current);
	    }
	  	else if (e.getType()==Expression.COLUMN){
			if (e.getTableName() == null) {
				for (int filterIndex=0; filterIndex < filter.length; filterIndex++) {
					e.resolve(filter[filterIndex]);
				}
			}
		}
	}

	select.iResultLen = len;

	// where
	token = tTokenizer.getString();

	if (token.equals("WHERE")) {
	    condition = addCondition(condition, parseExpression());
	    token = tTokenizer.getString();
	}

	select.eCondition = condition;

	if (token.equals("GROUP")) {
	    tTokenizer.getThis("BY");

	    len = 0;

	    do {
		vcolumn.addElement(parseExpression());

		token = tTokenizer.getString();
		len++;
	    } while (token.equals(","));

	    select.iGroupLen = len;
	}

	if (token.equals("ORDER")) {
	    tTokenizer.getThis("BY");

	    len = 0;

	    do {
		Expression e = parseExpression();

		if (e.getType() == Expression.VALUE) {

		    // order by 1,2,3
		    if (e.getDataType() == Column.INTEGER) {
			int i = ((Integer) e.getValue()).intValue();

			e = (Expression) vcolumn.elementAt(i - 1);
		    }
		} else if (e.getType() == Expression.COLUMN
			   && e.getTableName() == null) {

		    // this could be an alias column
		    String s = e.getColumnName();

		    for (int i = 0; i < vcolumn.size(); i++) {
			Expression ec = (Expression) vcolumn.elementAt(i);

			if (s.equals(ec.getAlias())) {
			    e = ec;

			    break;
			}
		    }
		}

		token = tTokenizer.getString();

		if (token.equals("DESC")) {
		    e.setDescending();

		    token = tTokenizer.getString();
		} else if (token.equals("ASC")) {
		    token = tTokenizer.getString();
		}

		vcolumn.addElement(e);

		len++;
	    } while (token.equals(","));

	    select.iOrderLen = len;
	}

	len = vcolumn.size();
	select.eColumn = new Expression[len];

	vcolumn.copyInto(select.eColumn);

	if (token.equals("UNION")) {
	    token = tTokenizer.getString();

	    if (token.equals("ALL")) {
		select.iUnionType = Select.UNIONALL;
	    } else {
		select.iUnionType = Select.UNION;

		tTokenizer.back();
	    }

	    tTokenizer.getThis("SELECT");

	    select.sUnion = parseSelect();
	} else if (token.equals("INTERSECT")) {
	    tTokenizer.getThis("SELECT");

	    select.iUnionType = Select.INTERSECT;
	    select.sUnion = parseSelect();
	} else if (token.equals("EXCEPT") || token.equals("MINUS")) {
	    tTokenizer.getThis("SELECT");

	    select.iUnionType = Select.EXCEPT;
	    select.sUnion = parseSelect();
	} else {
	    tTokenizer.back();
	}

	return select;
    }

    /**
     * Method declaration
     *
     *
     * @param outerjoin
     *
     * @return
     *
     * @throws SQLException
     */
    private TableFilter parseTableFilter(boolean outerjoin)
	    throws SQLException {
	String token = tTokenizer.getString();
	Table  t = null;

	if (token.equals("(")) {
	    tTokenizer.getThis("SELECT");

	    Select s = parseSelect();
	    Result r = s.getResult(0);

	    // it's not a problem that this table has not a unique name
	    t = new Table(dDatabase, false, "SYSTEM_SUBQUERY", false);

	    tTokenizer.getThis(")");
	    t.addColumns(r);
	    t.createPrimaryKey();

	    // subquery creation can't fail because of violation of primary key
	    t.insert(r, cChannel);
	} else {
	    cChannel.check(token, Access.SELECT);

	    t = dDatabase.getTable(token, cChannel);
	}

	String sAlias = null;

	token = tTokenizer.getString();

	if (token.equals("AS")) {
	    sAlias = tTokenizer.getName();
	} else if (tTokenizer.wasName()) {
	    sAlias = token;
	} else {
	    tTokenizer.back();
	}

	return new TableFilter(t, sAlias, outerjoin);
    }

    /**
     * Method declaration
     *
     *
     * @param e1
     * @param e2
     *
     * @return
     */
    private Expression addCondition(Expression e1, Expression e2) {
	if (e1 == null) {
	    return e2;
	} else if (e2 == null) {
	    return e1;
	} else {
	    return new Expression(Expression.AND, e1, e2);
	}
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
    private Object getValue(int type) throws SQLException {
	Expression r = parseExpression();

	r.resolve(null);

	return r.getValue(type);
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    private Expression parseExpression() throws SQLException {
	read();

	// todo: really this should be in readTerm
	// but then grouping is much more complex
	if (iToken == Expression.MIN || iToken == Expression.MAX
		|| iToken == Expression.COUNT || iToken == Expression.SUM
		|| iToken == Expression.AVG) {
	    int type = iToken;

	    read();

	    Expression r = new Expression(type, readOr(), null);

	    tTokenizer.back();

	    return r;
	}

	Expression r = readOr();

	tTokenizer.back();

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    private Expression readOr() throws SQLException {
	Expression r = readAnd();

	while (iToken == Expression.OR) {
	    int	       type = iToken;
	    Expression a = r;

	    read();

	    r = new Expression(type, a, readAnd());
	}

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    private Expression readAnd() throws SQLException {
	Expression r = readCondition();

	while (iToken == Expression.AND) {
	    int	       type = iToken;
	    Expression a = r;

	    read();

	    r = new Expression(type, a, readCondition());
	}

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    private Expression readCondition() throws SQLException {
	if (iToken == Expression.NOT) {
	    int type = iToken;

	    read();

	    return new Expression(type, readCondition(), null);
	} else if (iToken == Expression.EXISTS) {
	    int type = iToken;

	    read();
	    readThis(Expression.OPEN);
	    Trace.check(iToken == Expression.SELECT, Trace.UNEXPECTED_TOKEN);

	    Expression s = new Expression(parseSelect());

	    read();
	    readThis(Expression.CLOSE);

	    return new Expression(type, s, null);
	} else {
	    Expression a = readConcat();
	    boolean    not = false;

	    if (iToken == Expression.NOT) {
		not = true;

		read();
	    }

	    if (iToken == Expression.LIKE) {
		read();

		Expression b = readConcat();
		char       escape = 0;

		if (sToken.equals("ESCAPE")) {
		    read();

		    Expression c = readTerm();

		    Trace.check(c.getType() == Expression.VALUE,
				Trace.INVALID_ESCAPE);

		    String s = (String) c.getValue(Column.VARCHAR);

		    if (s == null || s.length() < 1) {
			throw Trace.error(Trace.INVALID_ESCAPE, s);
		    }

		    escape = s.charAt(0);
		}

		a = new Expression(Expression.LIKE, a, b);

		a.setLikeEscape(escape);
	    } else if (iToken == Expression.BETWEEN) {
		read();

		Expression l = new Expression(Expression.BIGGER_EQUAL, a,
					      readConcat());

		readThis(Expression.AND);

		Expression h = new Expression(Expression.SMALLER_EQUAL, a,
					      readConcat());

		a = new Expression(Expression.AND, l, h);
	    } else if (iToken == Expression.IN) {
		int type = iToken;

		read();
		readThis(Expression.OPEN);

		Expression b = null;

		if (iToken == Expression.SELECT) {
		    b = new Expression(parseSelect());

		    read();
		} else {
		    tTokenizer.back();

		    Vector v = new Vector();

		    while (true) {
			v.addElement(getValue(Column.VARCHAR));
			read();

			if (iToken != Expression.COMMA) {
			    break;
			}
		    }

		    b = new Expression(v);
		}

		readThis(Expression.CLOSE);

		a = new Expression(type, a, b);
	    } else {
		Trace.check(!not, Trace.UNEXPECTED_TOKEN);

		if (Expression.isCompare(iToken)) {
		    int type = iToken;

		    read();

		    return new Expression(type, a, readConcat());
		}

		return a;
	    }

	    if (not) {
		a = new Expression(Expression.NOT, a, null);
	    }

	    return a;
	}
    }

    /**
     * Method declaration
     *
     *
     * @param type
     *
     * @throws SQLException
     */
    private void readThis(int type) throws SQLException {
	Trace.check(iToken == type, Trace.UNEXPECTED_TOKEN);
	read();
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    private Expression readConcat() throws SQLException {
	Expression r = readSum();

	while (iToken == Expression.STRINGCONCAT) {
	    int	       type = Expression.CONCAT;
	    Expression a = r;

	    read();

	    r = new Expression(type, a, readSum());
	}

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    private Expression readSum() throws SQLException {
	Expression r = readFactor();

	while (true) {
	    int type;

	    if (iToken == Expression.PLUS) {
		type = Expression.ADD;
	    } else if (iToken == Expression.NEGATE) {
		type = Expression.SUBTRACT;
	    } else {
		break;
	    }

	    Expression a = r;

	    read();

	    r = new Expression(type, a, readFactor());
	}

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    private Expression readFactor() throws SQLException {
	Expression r = readTerm();

	while (iToken == Expression.MULTIPLY || iToken == Expression.DIVIDE) {
	    int	       type = iToken;
	    Expression a = r;

	    read();

	    r = new Expression(type, a, readTerm());
	}

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    private Expression readTerm() throws SQLException {
	Expression r = null;

	if (iToken == Expression.COLUMN) {
	    String name = sToken;

	    r = new Expression(sTable, sToken);

	    read();

	    if (iToken == Expression.OPEN) {
		Function f = new Function(dDatabase.getAlias(name), cChannel);
		int      len = f.getArgCount();
		int      i = 0;

		read();

		if (iToken != Expression.CLOSE) {
		    while (true) {
			f.setArgument(i++, readOr());

			if (iToken != Expression.COMMA) {
			    break;
			}

			read();
		    }
		}

		readThis(Expression.CLOSE);

		r = new Expression(f);
	    }
	} else if (iToken == Expression.NEGATE) {
	    int type = iToken;

	    read();

	    r = new Expression(type, readTerm(), null);
	} else if (iToken == Expression.PLUS) {
	    read();

	    r = readTerm();
	} else if (iToken == Expression.OPEN) {
	    read();

	    r = readOr();

	    if (iToken != Expression.CLOSE) {
		throw Trace.error(Trace.UNEXPECTED_TOKEN, sToken);
	    }

	    read();
	} else if (iToken == Expression.VALUE) {
	    r = new Expression(iType, oData);

	    read();
	} else if (iToken == Expression.SELECT) {
	    r = new Expression(parseSelect());

	    read();
	} else if (iToken == Expression.MULTIPLY) {
	    r = new Expression(sTable, null);

	    read();
	} else if (iToken == Expression.IFNULL
		   || iToken == Expression.CONCAT) {
	    int type = iToken;

	    read();
	    readThis(Expression.OPEN);

	    r = readOr();

	    readThis(Expression.COMMA);

	    r = new Expression(type, r, readOr());

	    readThis(Expression.CLOSE);
	} else if (iToken == Expression.CASEWHEN) {
	    int type = iToken;

	    read();
	    readThis(Expression.OPEN);

	    r = readOr();

	    readThis(Expression.COMMA);

	    Expression thenelse = readOr();

	    readThis(Expression.COMMA);

	    // thenelse part is never evaluated; only init
	    thenelse = new Expression(type, thenelse, readOr());
	    r = new Expression(type, r, thenelse);

	    readThis(Expression.CLOSE);
	} else if (iToken == Expression.CONVERT) {
	    int type = iToken;

	    read();
	    readThis(Expression.OPEN);

	    r = readOr();

	    readThis(Expression.COMMA);

	    int t = Column.getTypeNr(sToken);

	    r = new Expression(type, r, null);

	    r.setDataType(t);
	    read();
	    readThis(Expression.CLOSE);
	} else if (iToken == Expression.CAST) {
	    read();
	    readThis(Expression.OPEN);

	    r = readOr();

	    Trace.check(sToken.equals("AS"), Trace.UNEXPECTED_TOKEN, sToken);
	    read();

	    int t = Column.getTypeNr(sToken);

	    r = new Expression(Expression.CONVERT, r, null);

	    r.setDataType(t);
	    read();
	    readThis(Expression.CLOSE);
	} else {
	    throw Trace.error(Trace.UNEXPECTED_TOKEN, sToken);
	}

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    private void read() throws SQLException {
	sToken = tTokenizer.getString();

	if (tTokenizer.wasValue()) {
	    iToken = Expression.VALUE;
	    oData = tTokenizer.getAsValue();
	    iType = tTokenizer.getType();
	} else if (tTokenizer.wasName()) {
	    iToken = Expression.COLUMN;
	    sTable = null;
	} else if (tTokenizer.wasLongName()) {
	    sTable = tTokenizer.getLongNameFirst();
	    sToken = tTokenizer.getLongNameLast();

	    if (sToken.equals("*")) {
		iToken = Expression.MULTIPLY;
	    } else {
		iToken = Expression.COLUMN;
	    }
	} else if (sToken.equals("")) {
	    iToken = Expression.END;
	} else if (sToken.equals("AND")) {
	    iToken = Expression.AND;
	} else if (sToken.equals("OR")) {
	    iToken = Expression.OR;
	} else if (sToken.equals("NOT")) {
	    iToken = Expression.NOT;
	} else if (sToken.equals("IN")) {
	    iToken = Expression.IN;
	} else if (sToken.equals("EXISTS")) {
	    iToken = Expression.EXISTS;
	} else if (sToken.equals("BETWEEN")) {
	    iToken = Expression.BETWEEN;
	} else if (sToken.equals("+")) {
	    iToken = Expression.PLUS;
	} else if (sToken.equals("-")) {
	    iToken = Expression.NEGATE;
	} else if (sToken.equals("*")) {
	    iToken = Expression.MULTIPLY;
	    sTable = null;    // in case of ASTERIX
	} else if (sToken.equals("/")) {
	    iToken = Expression.DIVIDE;
	} else if (sToken.equals("||")) {
	    iToken = Expression.STRINGCONCAT;
	} else if (sToken.equals("(")) {
	    iToken = Expression.OPEN;
	} else if (sToken.equals(")")) {
	    iToken = Expression.CLOSE;
	} else if (sToken.equals("SELECT")) {
	    iToken = Expression.SELECT;
	} else if (sToken.equals("<")) {
	    iToken = Expression.SMALLER;
	} else if (sToken.equals("<=")) {
	    iToken = Expression.SMALLER_EQUAL;
	} else if (sToken.equals(">=")) {
	    iToken = Expression.BIGGER_EQUAL;
	} else if (sToken.equals(">")) {
	    iToken = Expression.BIGGER;
	} else if (sToken.equals("=")) {
	    iToken = Expression.EQUAL;
	} else if (sToken.equals("IS")) {
	    sToken = tTokenizer.getString();

	    if (sToken.equals("NOT")) {
		iToken = Expression.NOT_EQUAL;
	    } else {
		iToken = Expression.EQUAL;

		tTokenizer.back();
	    }
	} else if (sToken.equals("<>") || sToken.equals("!=")) {
	    iToken = Expression.NOT_EQUAL;
	} else if (sToken.equals("LIKE")) {
	    iToken = Expression.LIKE;
	} else if (sToken.equals("COUNT")) {
	    iToken = Expression.COUNT;
	} else if (sToken.equals("SUM")) {
	    iToken = Expression.SUM;
	} else if (sToken.equals("MIN")) {
	    iToken = Expression.MIN;
	} else if (sToken.equals("MAX")) {
	    iToken = Expression.MAX;
	} else if (sToken.equals("AVG")) {
	    iToken = Expression.AVG;
	} else if (sToken.equals("IFNULL")) {
	    iToken = Expression.IFNULL;
	} else if (sToken.equals("CONVERT")) {
	    iToken = Expression.CONVERT;
	} else if (sToken.equals("CAST")) {
	    iToken = Expression.CAST;
	} else if (sToken.equals("CASEWHEN")) {
	    iToken = Expression.CASEWHEN;
	} else if (sToken.equals(",")) {
	    iToken = Expression.COMMA;
	} else {
	    iToken = Expression.END;
	}
    }

}
