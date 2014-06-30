/*
 * User.java
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
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
class User {
    private boolean   bAdministrator;
    private Hashtable hRight;
    private String    sName, sPassword;
    private User      uPublic;

    /**
     * Constructor declaration
     *
     *
     * @param name
     * @param password
     * @param admin
     * @param pub
     */
    User(String name, String password, boolean admin, User pub) {
	hRight = new Hashtable();
	sName = name;

	setPassword(password);

	bAdministrator = admin;
	uPublic = pub;
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
    String getPassword() {

	// necessary to create the script
	return sPassword;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    Hashtable getRights() {

	// necessary to create the script
	return hRight;
    }

    /**
     * Method declaration
     *
     *
     * @param password
     */
    void setPassword(String password) {
	sPassword = password;
    }

    /**
     * Method declaration
     *
     *
     * @param test
     *
     * @throws SQLException
     */
    void checkPassword(String test) throws SQLException {
	Trace.check(test.equals(sPassword), Trace.ACCESS_IS_DENIED);

	// this is a safer (but slower) version:
	// if the password is not over 64 characters then this
	// algorithm needs always about the same amount of time

	/*
	 * int maxtest=test.length();
	 * int maxpass=sPassword.length();
	 * int max=64;
	 * if(maxtest>max) {
	 * max=maxtest;
	 * }
	 * if(maxpass>max) {
	 * max=maxpass;
	 * }
	 * boolean correct=true,dummy=true;
	 * for(int i=0;i<max;i++) {
	 * char a= (i>=maxtest) ? 0 : test.charAt(i);
	 * char b= (i>=maxpass) ? 0 : sPassword.charAt(i);
	 * if(a!=b) {
	 * correct=false;
	 * } else {
	 * dummy=true;
	 * }
	 * }
	 * Trace.check(correct,Trace.ACCESS_IS_DENIED);
	 */
    }

    /**
     * Method declaration
     *
     *
     * @param object
     * @param right
     */
    void grant(String object, int right) {
	Integer n = (Integer) hRight.get(object);

	if (n == null) {
	    n = new Integer(right);
	} else {
	    n = new Integer(n.intValue() | right);
	}

	hRight.put(object, n);
    }

    /**
     * Method declaration
     *
     *
     * @param object
     * @param right
     */
    void revoke(String object, int right) {
	Integer n = (Integer) hRight.get(object);

	if (n == null) {
	    n = new Integer(right);
	} else {
	    n = new Integer(n.intValue() & (Access.ALL - right));
	}

	hRight.put(object, n);
    }

    /**
     * Method declaration
     *
     */
    void revokeAll() {
	hRight = null;
	bAdministrator = false;
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
	if (bAdministrator) {
	    return;
	}

	Integer n;

	n = (Integer) hRight.get(object);

	if (n != null && (n.intValue() & right) != 0) {
	    return;
	}

	if (uPublic != null) {
	    n = (Integer) (uPublic.hRight).get(object);

	    if (n != null && (n.intValue() & right) != 0) {
		return;
	    }
	}

	throw Trace.error(Trace.ACCESS_IS_DENIED);
    }

    /**
     * Method declaration
     *
     *
     * @throws SQLException
     */
    void checkAdmin() throws SQLException {
	Trace.check(isAdmin(), Trace.ACCESS_IS_DENIED);
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    boolean isAdmin() {
	return bAdministrator;
    }

}
