/*
 * Like.java
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

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
class Like {
    private char    cLike[];
    private int[]   iType;
    private int     iLen;
    private boolean bIgnoreCase;

    /**
     * Constructor declaration
     *
     *
     * @param s
     * @param escape
     * @param ignorecase
     */
    Like(String s, char escape, boolean ignorecase) {
	if (ignorecase) {
	    s = s.toUpperCase();
	}

	normalize(s, true, escape);

	bIgnoreCase = ignorecase;
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    String getStartsWith() {
	String s = "";
	int    i = 0;

	for (; i < iLen && iType[i] == 0; i++) {
	    s = s + cLike[i];
	}

	if (i == 0) {
	    return null;
	}

	return s;
    }

    /**
     * Method declaration
     *
     *
     * @param o
     *
     * @return
     */
    boolean compare(Object o) {
	if (o == null) {
	    return iLen == 0;
	}

	String s = o.toString();

	if (bIgnoreCase) {
	    s = s.toUpperCase();
	}

	return compareAt(s, 0, 0, s.length());
    }

    /**
     * Method declaration
     *
     *
     * @param s
     * @param i
     * @param j
     * @param jLen
     *
     * @return
     */
    private boolean compareAt(String s, int i, int j, int jLen) {
	for (; i < iLen; i++) {
	    switch (iType[i]) {

	    case 0:    // general character
		if (j >= jLen || cLike[i] != s.charAt(j++)) {
		    return false;
		}

		break;

	    case 1:    // underscore: do not test this character
		if (j++ >= jLen) {
		    return false;
		}

		break;

	    case 2:    // percent: none or any character(s)
		if (++i >= iLen) {
		    return true;
		}

		while (j < jLen) {
		    if (cLike[i] == s.charAt(j) && compareAt(s, i, j, jLen)) {
			return true;
		    }

		    j++;
		}

		return false;
	    }
	}

	if (j != jLen) {
	    return false;
	}

	return true;
    }

    /**
     * Method declaration
     *
     *
     * @param s
     * @param b
     * @param e
     */
    private void normalize(String s, boolean b, char e) {
	iLen = 0;

	if (s == null) {
	    return;
	}

	int l = s.length();

	cLike = new char[l];
	iType = new int[l];

	boolean bEscaping = false, bPercent = false;

	for (int i = 0; i < l; i++) {
	    char c = s.charAt(i);

	    if (bEscaping == false) {
		if (b && c == e) {
		    bEscaping = true;

		    continue;
		} else if (c == '_') {
		    iType[iLen] = 1;
		} else if (c == '%') {
		    if (bPercent) {
			continue;
		    }

		    bPercent = true;
		    iType[iLen] = 2;
		} else {
		    bPercent = false;
		}
	    } else {
		bPercent = false;
		bEscaping = false;
	    }

	    cLike[iLen++] = c;
	}

	for (int i = 0; i < iLen - 1; i++) {
	    if (iType[i] == 2 && iType[i + 1] == 1) {
		iType[i] = 1;
		iType[i + 1] = 2;
	    }
	}
    }

}
