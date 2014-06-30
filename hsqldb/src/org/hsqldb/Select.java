/*
 * Select.java
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
class Select {
    boolean	     bDistinct;
    TableFilter      tFilter[];
    Expression       eCondition;    // null means no condition
    Expression       eColumn[];     // 'result', 'group' and 'order' columns
    int		     iResultLen;    // number of columns that are 'result'
    int		     iGroupLen;     // number of columns that are 'group'
    int		     iOrderLen;     // number of columns that are 'order'
    Select	     sUnion;	    // null means no union select
    String	     sIntoTable;    // null means not select..into
    int		     iUnionType;
    final static int UNION = 1, UNIONALL = 2, INTERSECT = 3, EXCEPT = 4;
// fredt@users.sourceforge.net begin changes from 1.50
    int limitStart = 0;
    int limitCount = -1;
// fredt@users.sourceforge.net end changes from 1.50

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void resolve() throws SQLException {
	int len = tFilter.length;

	for (int i = 0; i < len; i++) {
	    resolve(tFilter[i], true);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param f
     * @param ownfilter
     *
     * @throws SQLException
     */
    void resolve(TableFilter f, boolean ownfilter) throws SQLException {
	if (eCondition != null) {

	    // first set the table filter in the condition
	    eCondition.resolve(f);

	    if (f != null && ownfilter) {

		// the table filter tries to get as many conditions as possible
		// but only if the table filter belongs to this query
		f.setCondition(eCondition);
	    }
	}

	int len = eColumn.length;

	for (int i = 0; i < len; i++) {
	    eColumn[i].resolve(f);
	}
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void checkResolved() throws SQLException {
	if (eCondition != null) {
	    eCondition.checkResolved();
	}

	int len = eColumn.length;

	for (int i = 0; i < len; i++) {
	    eColumn[i].checkResolved();
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
    Object getValue(int type) throws SQLException {
	resolve();

	Result r = getResult(2);    // 2 records are (already) too much
	int    size = r.getSize();
	int    len = r.getColumnCount();

	Trace.check(size == 1 && len == 1, Trace.SINGLE_VALUE_EXPECTED);

	Object o = r.rRoot.data[0];

	if (r.iType[0] == type) {
	    return o;
	}

	String s = Column.convertObject(o);

	return Column.convertString(s, type);
    }

    /**
     * Method declaration
     *
     *
     * @param maxrows
     *
     * @return
     *
     * @throws SQLException
     */
    Result getResult(int maxrows) throws SQLException {
// fredt@users.sourceforge.net begin changes from 1.50
  	return getResult( 0, maxrows );
  }
// fredt@users.sourceforge.net end changes from 1.50
// fredt@users.sourceforge.net begin changes from 1.50
    Result getResult(int start, int cnt) throws SQLException {
	int maxrows=start+cnt;  //<-new, cut definitly
// fredt@users.sourceforge.net begin changes from 1.50
	resolve();
	checkResolved();

	if (sUnion != null && sUnion.iResultLen != iResultLen) {
	    throw Trace.error(Trace.COLUMN_COUNT_DOES_NOT_MATCH);
	}

	int     len = eColumn.length;
	Result  r = new Result(len);
	boolean aggregated = false;
	boolean grouped = false;

	for (int i = 0; i < len; i++) {
	    Expression e = eColumn[i];

	    r.iType[i] = e.getDataType();

	    if (e.isAggregate()) {
		aggregated = true;
	    }
	}

	Object agg[] = null;

	if (aggregated) {
	    agg = new Object[len];
	}

	if (iGroupLen > 0) {    // has been set in Parser
	    grouped = true;
	}

	boolean simple_maxrows = false;

	if (maxrows != 0 && grouped == false && sUnion == null
		&& iOrderLen == 0) {
	    simple_maxrows = true;
	} else {
	    simple_maxrows = false;
	}

	int     count = 0;
	int     filter = tFilter.length;
	boolean first[] = new boolean[filter];
	int     level = 0;

	while (level >= 0) {
	    TableFilter t = tFilter[level];
	    boolean     found;

	    if (!first[level]) {
		found = t.findFirst();
		first[level] = found;
	    } else {
		found = t.next();
		first[level] = found;
	    }

	    if (!found) {
		level--;

		continue;
	    }

	    if (level < filter - 1) {
		level++;

		continue;
	    }

	    if (eCondition == null || eCondition.test()) {
		Object row[] = new Object[len];

		for (int i = 0; i < len; i++) {
		    row[i] = eColumn[i].getValue();
		}

		count++;

		if (aggregated) {
		    updateAggregateRow(agg, row, len);
// fredt@users.sourceforge.net 20010701
// bug fix for 416144 416146 430615 by fredt
                    if ( grouped )
   	                r.add(row);
		} else {
		    r.add(row);

		    if (simple_maxrows && count >= maxrows) {
			break;
		    }
      }
	    }
	}

	if (aggregated &&!grouped) {
	    addAggregateRow(r, agg, len, count);
	} else if (grouped) {
	    int order[] = new int[iGroupLen];
	    int way[] = new int[iGroupLen];

	    for (int i = iResultLen, j = 0; j < iGroupLen; i++, j++) {
		order[j] = i;
		way[j] = 1;
	    }

	    r = sortResult(r, order, way);

	    Record n = r.rRoot;
	    Result x = new Result(len);

	    for (int i = 0; i < len; i++) {
		x.iType[i] = r.iType[i];
	    }

	    do {
		Object row[] = new Object[len];

		count = 0;

		boolean newgroup = false;

		while (n != null && newgroup == false) {
		    count++;

		    for (int i = 0; i < iGroupLen; i++) {
			if (n.next == null) {
			    newgroup = true;
			} else if (Column.compare(n.data[i], n.next.data[i], r.iType[i])
				   != 0) {

			    // can't use .equals because 'null' is also one group
			    newgroup = true;
			}
		    }

		    updateAggregateRow(row, n.data, len);

		    n = n.next;
		}

		addAggregateRow(x, row, len, count);
	    } while (n != null);

	    r = x;
	}

	if (iOrderLen != 0) {
	    int order[] = new int[iOrderLen];
	    int way[] = new int[iOrderLen];

	    for (int i = iResultLen, j = 0; j < iOrderLen; i++, j++) {
		order[j] = i;
		way[j] = eColumn[i].isDescending() ? -1 : 1;
	    }

	    r = sortResult(r, order, way);
	}

	// the result is maybe bigger (due to group and order by)
	// but don't tell this anybody else
	r.setColumnCount(iResultLen);

	if (bDistinct) {
	    r = removeDuplicates(r);
	}

	for (int i = 0; i < iResultLen; i++) {
	    Expression e = eColumn[i];

	    r.sLabel[i] = e.getAlias();
	    r.sTable[i] = e.getTableName();
	    r.sName[i] = e.getColumnName();
	}

	if (sUnion != null) {
	    Result x = sUnion.getResult(0);

	    if (iUnionType == UNION) {
		r.append(x);

		r = removeDuplicates(r);
	    } else if (iUnionType == UNIONALL) {
		r.append(x);
	    } else if (iUnionType == INTERSECT) {
		r = removeDuplicates(r);
		x = removeDuplicates(x);
		r = removeDifferent(r, x);
	    } else if (iUnionType == EXCEPT) {
		r = removeDuplicates(r);
		x = removeDuplicates(x);
		r = removeSecond(r, x);
	    }
	}

	if (maxrows > 0 &&!simple_maxrows) {
	    trimResult(r, maxrows);
	}

// fredt@users.sourceforge.net begin changes from 1.50
	if (start > 0) {	//then cut the first 'start' elements
 	    trimResultFront( r, start );
  	}
// fredt@users.sourceforge.net end changes from 1.50

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @param row
     * @param n
     * @param len
     *
     * @throws SQLException
     */
    private void updateAggregateRow(Object row[], Object n[],
				    int len) throws SQLException {
	for (int i = 0; i < len; i++) {
	    int type = eColumn[i].getDataType();

	    switch (eColumn[i].getType()) {

	    case Expression.AVG:

	    case Expression.SUM:

	    case Expression.COUNT:
		row[i] = Column.sum(row[i], n[i], type);

		break;

	    case Expression.MIN:
		row[i] = Column.min(row[i], n[i], type);

		break;

	    case Expression.MAX:
		row[i] = Column.max(row[i], n[i], type);

		break;

	    default:
		row[i] = n[i];

		break;
	    }
	}
    }

    /**
     * Method declaration
     *
     *
     * @param x
     * @param row
     * @param len
     * @param count
     *
     * @throws SQLException
     */
    private void addAggregateRow(Result x, Object row[], int len,
				 int count) throws SQLException {
	for (int i = 0; i < len; i++) {
	    int t = eColumn[i].getType();

	    if (t == Expression.AVG) {
		row[i] = Column.avg(row[i], eColumn[i].getDataType(), count);
	    } else if (t == Expression.COUNT) {

		// this fixes the problem with count(*) on a empty table
		if (row[i] == null) {
		    row[i] = new Integer(0);
		}
	    }
	}

	x.add(row);
    }

    /**
     * Method declaration
     *
     *
     * @param r
     *
     * @return
     *
     * @throws SQLException
     */
    private static Result removeDuplicates(Result r) throws SQLException {
	int len = r.getColumnCount();
	int order[] = new int[len];
	int way[] = new int[len];

	for (int i = 0; i < len; i++) {
	    order[i] = i;
	    way[i] = 1;
	}

	r = sortResult(r, order, way);

	Record n = r.rRoot;

	while (n != null) {
	    Record next = n.next;

	    if (next == null) {
		break;
	    }

	    if (compareRecord(n.data, next.data, r, len) == 0) {
		n.next = next.next;
	    } else {
		n = n.next;
	    }
	}

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @param r
     * @param minus
     *
     * @return
     *
     * @throws SQLException
     */
    private static Result removeSecond(Result r,
				       Result minus) throws SQLException {
	int     len = r.getColumnCount();
	Record  n = r.rRoot;
	Record  last = r.rRoot;
	boolean rootr = true;    // checking rootrecord
	Record  n2 = minus.rRoot;
	int     i = 0;

	while (n != null && n2 != null) {
	    i = compareRecord(n.data, n2.data, r, len);

	    if (i == 0) {
		if (rootr) {
		    r.rRoot = n.next;
		} else {
		    last.next = n.next;
		}

		n = n.next;
	    } else if (i > 0) {    // r > minus
		n2 = n2.next;
	    } else {		   // r < minus
		last = n;
		rootr = false;
		n = n.next;
	    }
	}

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @param r
     * @param r2
     *
     * @return
     *
     * @throws SQLException
     */
    private static Result removeDifferent(Result r,
					  Result r2) throws SQLException {
	int     len = r.getColumnCount();
	Record  n = r.rRoot;
	Record  last = r.rRoot;
	boolean rootr = true;    // checking rootrecord
	Record  n2 = r2.rRoot;
	int     i = 0;

	while (n != null && n2 != null) {
	    i = compareRecord(n.data, n2.data, r, len);

	    if (i == 0) {	      // same rows
		if (rootr) {
		    r.rRoot = n;      // make this the first record
		} else {
		    last.next = n;    // this is next record in resultset
		}

		rootr = false;
		last = n;	      // this is last record in resultset
		n = n.next;
		n2 = n2.next;
	    } else if (i > 0) {       // r > r2
		n2 = n2.next;
	    } else {		      // r < r2
		n = n.next;
	    }
	}

	if (rootr) {		 // if no lines in resultset
	    r.rRoot = null;      // then return null
	} else {
	    last.next = null;    // else end resultset
	}

	return r;
    }

    /**
     * Method declaration
     *
     *
     * @param r
     * @param order
     * @param way
     *
     * @return
     *
     * @throws SQLException
     */
    private static Result sortResult(Result r, int order[],
				     int way[]) throws SQLException {
	if (r.rRoot == null || r.rRoot.next == null) {
	    return r;
	}

	Record source0, source1;
	Record target[] = new Record[2];
	Record targetlast[] = new Record[2];
	int    dest = 0;
	Record n = r.rRoot;

	while (n != null) {
	    Record next = n.next;

	    n.next = target[dest];
	    target[dest] = n;
	    n = next;
	    dest ^= 1;
	}

	for (int blocksize = 1; target[1] != null; blocksize <<= 1) {
	    source0 = target[0];
	    source1 = target[1];
	    target[0] = target[1] = targetlast[0] = targetlast[1] = null;

	    for (dest = 0; source0 != null; dest ^= 1) {
		int n0 = blocksize, n1 = blocksize;

		while (true) {
		    if (n0 == 0 || source0 == null) {
			if (n1 == 0 || source1 == null) {
			    break;
			}

			n = source1;
			source1 = source1.next;
			n1--;
		    } else if (n1 == 0 || source1 == null) {
			n = source0;
			source0 = source0.next;
			n0--;
		    } else if (compareRecord(source0.data, source1.data, r, order, way)
			       > 0) {
			n = source1;
			source1 = source1.next;
			n1--;
		    } else {
			n = source0;
			source0 = source0.next;
			n0--;
		    }

		    if (target[dest] == null) {
			target[dest] = n;
		    } else {
			targetlast[dest].next = n;
		    }

		    targetlast[dest] = n;
		    n.next = null;
		}
	    }
	}

	r.rRoot = target[0];

	return r;
    }
// fredt@users.sourceforge.net begin changes from 1.50
    /**
     * Method declaration
     *
     *
     * @param r
     * @param start
     */
  private static void trimResultFront( Result r, int start ) {
    Record n=r.rRoot;
    if(n==null) {
      return;
    }
    while(--start >= 0) {
    	n = n.next;
      if(n == null) {
        return;
      }
	 }
    r.rRoot=n;
  	}
// fredt@users.sourceforge.net end changes from 1.50

    /**
     * Method declaration
     *
     *
     * @param r
     * @param maxrows
     */
    private static void trimResult(Result r, int maxrows) {
	Record n = r.rRoot;

	if (n == null) {
	    return;
	}

	while (--maxrows > 0) {
	    n = n.next;

	    if (n == null) {
		return;
	    }
	}

	n.next = null;
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     * @param r
     * @param order
     * @param way
     *
     * @return
     *
     * @throws SQLException
     */
    private static int compareRecord(Object a[], Object b[], Result r,
				     int order[],
				     int way[]) throws SQLException {
	int i = Column.compare(a[order[0]], b[order[0]], r.iType[order[0]]);

	if (i == 0) {
	    for (int j = 1; j < order.length; j++) {
		i = Column.compare(a[order[j]], b[order[j]],
				   r.iType[order[j]]);

		if (i != 0) {
		    return i * way[j];
		}
	    }
	}

	return i * way[0];
    }

    /**
     * Method declaration
     *
     *
     * @param a
     * @param b
     * @param r
     * @param len
     *
     * @return
     *
     * @throws SQLException
     */
    private static int compareRecord(Object a[], Object b[], Result r,
				     int len) throws SQLException {
	for (int j = 0; j < len; j++) {
	    int i = Column.compare(a[j], b[j], r.iType[j]);

	    if (i != 0) {
		return i;
	    }
	}

	return 0;
    }

}
