/*
 * Index.java
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

/**
 * Index class declaration
 *
 *
 * @version 1.0.0.1
 */
class Index {
    private String     sName;
    private int	       iFields;
    private int	       iColumn[];
    private int	       iType[];
    private boolean    bUnique;    // just for scripting; all indexes are made unique
    private Node       root;
    private int	       iColumn_0, iType_0;    // just for tuning
    private static int iNeedCleanUp;

    /**
     * Constructor declaration
     *
     *
     * @param name
     * @param column
     * @param type
     * @param unique
     */
    Index(String name, int column[], int type[], boolean unique) {
	sName = name;
	iFields = column.length;
	iColumn = column;
	iType = type;
	bUnique = unique;
	iColumn_0 = iColumn[0];
	iType_0 = iType[0];
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    Node getRoot() {
	return root;
    }

    /**
     * Method declaration
     *
     *
     * @param r
     */
    void setRoot(Node r) {
	root = r;
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
    boolean isUnique() {
	return bUnique;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    int[] getColumns() {
	return iColumn;    // todo: this gives back also primary key field!
    }

    /**
     * Method declaration
     *
     *
     * @param i
     *
     * @throws SQLException
     */
    void insert(Node i) throws SQLException {
	Object  data[] = i.getData();
	Node    n = root, x = n;
	boolean way = true;
	int     compare = -1;

	while (true) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    if (n == null) {
		if (x == null) {
		    root = i;

		    return;
		}

		set(x, way, i);

		break;
	    }

	    x = n;
	    compare = compareRow(data, x.getData());

	    Trace.check(compare != 0, Trace.VIOLATION_OF_UNIQUE_INDEX);

	    way = compare < 0;
	    n = child(x, way);
	}

	while (true) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    int sign = way ? 1 : -1;

	    switch (x.getBalance() * sign) {

	    case 1:
		x.setBalance(0);

		return;

	    case 0:
		x.setBalance(-sign);

		break;

	    case -1:
		Node l = child(x, way);

		if (l.getBalance() == -sign) {
		    replace(x, l);
		    set(x, way, child(l, !way));
		    set(l, !way, x);
		    x.setBalance(0);
		    l.setBalance(0);
		} else {
		    Node r = child(l, !way);

		    replace(x, r);
		    set(l, !way, child(r, way));
		    set(r, way, l);
		    set(x, way, child(r, !way));
		    set(r, !way, x);

		    int rb = r.getBalance();

		    x.setBalance((rb == -sign) ? sign : 0);
		    l.setBalance((rb == sign) ? -sign : 0);
		    r.setBalance(0);
		}

		return;
	    }

	    if (x.equals(root)) {
		return;
	    }

	    way = from(x);
	    x = x.getParent();
	}
    }

    /**
     * Method declaration
     *
     *
     * @param row
     * @param datatoo
     *
     * @throws SQLException
     */
    void delete(Object row[], boolean datatoo) throws SQLException {
	Node x = search(row);

	if (x == null) {
	    return;
	}

	Node n;

	if (x.getLeft() == null) {
	    n = x.getRight();
	} else if (x.getRight() == null) {
	    n = x.getLeft();
	} else {
	    Node d = x;

	    x = x.getLeft();

	    // todo: this can be improved
	    while (x.getRight() != null) {
		if (Trace.STOP) {
		    Trace.stop();
		}

		x = x.getRight();
	    }

	    // x will be replaced with n later
	    n = x.getLeft();

	    // swap d and x
	    int b = x.getBalance();

	    x.setBalance(d.getBalance());
	    d.setBalance(b);

	    // set x.parent
	    Node xp = x.getParent();
	    Node dp = d.getParent();

	    if (d == root) {
		root = x;
	    }

	    x.setParent(dp);

	    if (dp != null) {
		if (dp.getRight().equals(d)) {
		    dp.setRight(x);
		} else {
		    dp.setLeft(x);
		}
	    }

	    // for in-memory tables we could use: d.rData=x.rData;
	    // but not for cached tables
	    // relink d.parent, x.left, x.right
	    if (xp == d) {
		d.setParent(x);

		if (d.getLeft().equals(x)) {
		    x.setLeft(d);
		    x.setRight(d.getRight());
		} else {
		    x.setRight(d);
		    x.setLeft(d.getLeft());
		}
	    } else {
		d.setParent(xp);
		xp.setRight(d);
		x.setRight(d.getRight());
		x.setLeft(d.getLeft());
	    }

	    x.getRight().setParent(x);
	    x.getLeft().setParent(x);

	    // set d.left, d.right
	    d.setLeft(n);

	    if (n != null) {
		n.setParent(d);
	    }

	    d.setRight(null);

	    x = d;
	}

	boolean way = from(x);

	replace(x, n);

	n = x.getParent();

	x.delete();

	if (datatoo) {
	    x.rData.delete();
	}

	while (n != null) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    x = n;

	    int sign = way ? 1 : -1;

	    switch (x.getBalance() * sign) {

	    case -1:
		x.setBalance(0);

		break;

	    case 0:
		x.setBalance(sign);

		return;

	    case 1:
		Node r = child(x, !way);
		int  b = r.getBalance();

		if (b * sign >= 0) {
		    replace(x, r);
		    set(x, !way, child(r, way));
		    set(r, way, x);

		    if (b == 0) {
			x.setBalance(sign);
			r.setBalance(-sign);

			return;
		    }

		    x.setBalance(0);
		    r.setBalance(0);

		    x = r;
		} else {
		    Node l = child(r, way);

		    replace(x, l);

		    b = l.getBalance();

		    set(r, way, child(l, !way));
		    set(l, !way, r);
		    set(x, !way, child(l, way));
		    set(l, way, x);
		    x.setBalance((b == sign) ? -sign : 0);
		    r.setBalance((b == -sign) ? sign : 0);
		    l.setBalance(0);

		    x = l;
		}
	    }

	    way = from(x);
	    n = x.getParent();
	}
    }

    /**
     * Method declaration
     *
     *
     * @param data
     *
     * @return
     *
     * @throws SQLException
     */
    Node find(Object data[]) throws SQLException {
	Node x = root, n;

	while (x != null) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    int i = compareRowNonUnique(data, x.getData());

	    if (i == 0) {
		return x;
	    } else if (i > 0) {
		n = x.getRight();
	    } else {
		n = x.getLeft();
	    }

	    if (n == null) {
		return null;
	    }

	    x = n;
	}

	return null;
    }

    /**
     * Method declaration
     *
     *
     * @param value
     * @param compare
     *
     * @return
     *
     * @throws SQLException
     */
    Node findFirst(Object value, int compare) throws SQLException {
	Trace.assert(compare == Expression.BIGGER
		     || compare == Expression.EQUAL
		     || compare == Expression.BIGGER_EQUAL,
			"Index.findFirst");

	Node x = root;
	int  iTest = 1;

	if (compare == Expression.BIGGER) {
	    iTest = 0;
	}

	while (x != null) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    boolean t = compareValue(value, x.getData()[iColumn_0]) >= iTest;

	    if (t) {
		Node r = x.getRight();

		if (r == null) {
		    break;
		}

		x = r;
	    } else {
		Node l = x.getLeft();

		if (l == null) {
		    break;
		}

		x = l;
	    }
	}

	while (x != null
	       && compareValue(value, x.getData()[iColumn_0]) >= iTest) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    x = next(x);
	}

	return x;
    }

    /**
     * Method declaration
     *
     *
     * @return
     *
     * @throws SQLException
     */
    Node first() throws SQLException {
	Node x = root, l = x;

	while (l != null) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    x = l;
	    l = x.getLeft();
	}

	return x;
    }

    /**
     * Method declaration
     *
     *
     * @param x
     *
     * @return
     *
     * @throws SQLException
     */
    Node next(Node x) throws SQLException {

	// if(x==null) {
	// return null;
	// }
	if ((++iNeedCleanUp & 127) == 0) {
	    x.rData.cleanUpCache();
	}

	Node r = x.getRight();

	if (r != null) {
	    x = r;

	    Node l = x.getLeft();

	    while (l != null) {
		if (Trace.STOP) {
		    Trace.stop();
		}

		x = l;
		l = x.getLeft();
	    }

	    return x;
	}

	Node ch = x;

	x = x.getParent();

	while (x != null && ch.equals(x.getRight())) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    ch = x;
	    x = x.getParent();
	}

	return x;
    }

    /**
     * Method declaration
     *
     *
     * @param x
     * @param w
     *
     * @return
     *
     * @throws SQLException
     */
    private Node child(Node x, boolean w) throws SQLException {
	return w ? x.getLeft() : x.getRight();
    }

    /**
     * Method declaration
     *
     *
     * @param x
     * @param n
     *
     * @throws SQLException
     */
    private void replace(Node x, Node n) throws SQLException {
	if (x.equals(root)) {
	    root = n;

	    if (n != null) {
		n.setParent(null);
	    }
	} else {
	    set(x.getParent(), from(x), n);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param x
     * @param w
     * @param n
     *
     * @throws SQLException
     */
    private void set(Node x, boolean w, Node n) throws SQLException {
	if (w) {
	    x.setLeft(n);
	} else {
	    x.setRight(n);
	}

	if (n != null) {
	    n.setParent(x);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param x
     *
     * @return
     *
     * @throws SQLException
     */
    private boolean from(Node x) throws SQLException {
	if (x.equals(root)) {
	    return true;
	}

	if (Trace.ASSERT) {
	    Trace.assert(x.getParent() != null);
	}

	return x.equals(x.getParent().getLeft());
    }

    /**
     * Method declaration
     *
     *
     * @param d
     *
     * @return
     *
     * @throws SQLException
     */
    private Node search(Object d[]) throws SQLException {
	Node x = root;

	while (x != null) {
	    if (Trace.STOP) {
		Trace.stop();
	    }

	    int c = compareRow(d, x.getData());

	    if (c == 0) {
		return x;
	    } else if (c < 0) {
		x = x.getLeft();
	    } else {
		x = x.getRight();
	    }
	}

	return null;
    }

    // todo: this is a hack

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
    private int compareRowNonUnique(Object a[],
				    Object b[]) throws SQLException {
	int i = Column.compare(a[iColumn_0], b[iColumn_0], iType_0);

	if (i != 0) {
	    return i;
	}

	for (int j = 1; j < iFields - (bUnique ? 0 : 1); j++) {
	    i = Column.compare(a[iColumn[j]], b[iColumn[j]], iType[j]);

	    if (i != 0) {
		return i;
	    }
	}

	return 0;
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
    private int compareRow(Object a[], Object b[]) throws SQLException {
	int i = Column.compare(a[iColumn_0], b[iColumn_0], iType_0);

	if (i != 0) {
	    return i;
	}

	for (int j = 1; j < iFields; j++) {
	    i = Column.compare(a[iColumn[j]], b[iColumn[j]], iType[j]);

	    if (i != 0) {
		return i;
	    }
	}

	return 0;
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
    private int compareValue(Object a, Object b) throws SQLException {
	return Column.compare(a, b, iType_0);
    }

}
