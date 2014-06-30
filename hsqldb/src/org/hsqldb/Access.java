/*
 * Access.java
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
import java.util.*;

/**
 * Access Class
 * <P>The collection (Vector) of User object instances within a specific
 * database.  Methods are provided for creating, modifying and deleting users,
 * as well as manipulating their access rights to the database objects.
 *
 *
 * @version 1.0.0.1
 * @see User
 */
class Access {
    final static int SELECT = 1, DELETE = 2, INSERT = 4, UPDATE = 8, ALL = 15;
    private Vector   uUser;
    private User     uPublic;

    /**
     * Access Class constructor
     * <P>Creates a new Vector to contain the User object instances, as well
     * as creating an initial PUBLIC user, with no password.
     *
     * @throws SQLException
     */
    Access() throws SQLException {
	uUser = new Vector();
	uPublic = createUser("PUBLIC", null, false);
    }

    /**
     * getRight method declaration
     * <P>This getRight method takes a String argument of the name of the access right.
     *
     * @param A String representation of the right.
     *
     * @return A static int representing the String right passed in.
     *
     * @throws SQLException
     */
    static int getRight(String right) throws SQLException {
	if (right.equals("ALL")) {
	    return ALL;
	} else if (right.equals("SELECT")) {
	    return SELECT;
	} else if (right.equals("UPDATE")) {
	    return UPDATE;
	} else if (right.equals("DELETE")) {
	    return DELETE;
	} else if (right.equals("INSERT")) {
	    return INSERT;
	}

	throw Trace.error(Trace.UNEXPECTED_TOKEN, right);
    }

    /**
     * getRight method declaration
     * <P>This getRight method takes a int argument of the access right.
     *
     * @param A static int representing the right passed in.
     *
     * @return A String representation of the right or rights associated with the argument.
     *
     * @throws SQLException
     */
    static String getRight(int right) {
	if (right == ALL) {
	    return "ALL";
	} else if (right == 0) {
	    return null;
	}

	StringBuffer b = new StringBuffer();

	if ((right & SELECT) != 0) {
	    b.append("SELECT,");
	}

	if ((right & UPDATE) != 0) {
	    b.append("UPDATE,");
	}

	if ((right & DELETE) != 0) {
	    b.append("DELETE,");
	}

	if ((right & INSERT) != 0) {
	    b.append("INSERT,");
	}

	String s = b.toString();

	return s.substring(0, s.length() - 1);
    }

    /**
     * createUser method declaration
     * <P>This method is used to create a new user.  The collection of users
     * is first checked for a duplicate name, and an exception will be thrown
     * if a user of the same name already exists.
     *
     * @param name (User login)
     * @param password (Plaintext password)
     * @param admin (Is this a database admin user?)
     *
     * @return An instance of the newly created User object
     *
     * @throws SQLException
     */
    User createUser(String name, String password,
		    boolean admin) throws SQLException {
	for (int i = 0; i < uUser.size(); i++) {
	    User u = (User) uUser.elementAt(i);

	    if (u != null && u.getName().equals(name)) {
		throw Trace.error(Trace.USER_ALREADY_EXISTS, name);
	    }
	}

	User u = new User(name, password, admin, uPublic);

	uUser.addElement(u);

	return u;
    }

    /**
     * dropUser method declaration
     * <P>This method is used to drop a user.  Since we are using a vector
     * to hold the User objects, we must iterate through the Vector looking
     * for the name.  The user object is currently set to null, and all access
     * rights revoked.
     * <P><B>Note:</B>An ACCESS_IS_DENIED exception will be thrown if an attempt
     * is made to drop the PUBLIC user.
     *
     * @param name of the user to be dropped
     *
     * @throws SQLException
     */
    void dropUser(String name) throws SQLException {
	Trace.check(!name.equals("PUBLIC"), Trace.ACCESS_IS_DENIED);

	for (int i = 0; i < uUser.size(); i++) {
	    User u = (User) uUser.elementAt(i);

	    if (u != null && u.getName().equals(name)) {

		// todo: find a better way. Problem: removeElementAt would not
		// work correctly while others are connected
		uUser.setElementAt(null, i);
		u.revokeAll();    // in case the user is referenced in another way

		return;
	    }
	}

	throw Trace.error(Trace.USER_NOT_FOUND, name);
    }

    /**
     * getUser method declaration
     * <P>This method is used to return an instance of a particular User object,
     * given the user name and password.
     *
     * <P><B>Note:</B>An ACCESS_IS_DENIED exception will be thrown if an attempt
     * is made to get the PUBLIC user.
     *
     * @param user name
     * @param user password
     *
     * @return The requested User object
     *
     * @throws SQLException
     */
    User getUser(String name, String password) throws SQLException {
	Trace.check(!name.equals("PUBLIC"), Trace.ACCESS_IS_DENIED);

	if (name == null) {
	    name = "";
	}

	if (password == null) {
	    password = "";
	}

	User u = get(name);

	u.checkPassword(password);

	return u;
    }

    /**
     * getUsers method declaration
     * <P>This method is used to access the entire Vector of User objects for this database.
     *
     * @return The Vector of our User objects
     */
    Vector getUsers() {
	return uUser;
    }

    /**
     * grant method declaration
     * <P>This method is used to grant a user rights to database objects.
     *
     * @param name of the user
     * @param object in the database
     * @param right to grant to the user
     *
     * @throws SQLException
     */
    void grant(String name, String object, int right) throws SQLException {
	get(name).grant(object, right);
    }

    /**
     * revoke method declaration
     * <P>This method is used to revoke a user's rights to database objects.
     *
     * @param name of the user
     * @param object in the database
     * @param right to grant to the user
     *
     * @throws SQLException
     */
    void revoke(String name, String object, int right) throws SQLException {
	get(name).revoke(object, right);
    }

	/*
	 * This private method is used to access the User objects in the collection
	 * and perform operations on them.
	 */
    private User get(String name) throws SQLException {
	for (int i = 0; i < uUser.size(); i++) {
	    User u = (User) uUser.elementAt(i);

	    if (u != null && u.getName().equals(name)) {
		return u;
	    }
	}

	throw Trace.error(Trace.USER_NOT_FOUND, name);
    }

}

