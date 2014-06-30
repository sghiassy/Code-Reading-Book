/*
 * Profile.java
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

import java.io.*;
import java.util.*;

/**
 * <font color="#009900">
 * This class is used to test and profile HSQL Database Engine.
 * Before it can be used, the source code of the database must
 * be changed using CodeSwitcher, then compiled, then the test
 * program must be run, and at the end the static function
 * listUnvisited must be called. This will list all not
 * visited lines and it will print out the top 20 lines with
 * the most visits, the longest run and the longest run per call.
 * <P>
 * This class is generic can be used to profile also other
 * programs.
 * </font>
 */
public class Profile {
    static Profile   main = new Profile();
    final static int TOP = 20;
    final static int VISIT = 0, TIME = 1, PERCALL = 2;
    Hashtable	     hash = new Hashtable();
    boolean	     bVisited;
    boolean	     bStop;
    File	     fLastFile;
    int		     iLastLine;
    long	     lLastTime = System.currentTimeMillis();

    /**
     * Method declaration
     *
     *
     * @param file
     * @param line
     * @param maxline
     */
    public static void visit(String file, int line, int maxline) {    // +
	main.m_visit(file, line, maxline);    // +
    }					      // +

    /**
     * Method declaration
     *
     */
    public static void listUnvisited() {      // +
	main.m_listUnvisited();    // +
    }				   // +

    /**
     * Class declaration
     *
     *
     * @author
     * @version %I%, %G%
     */
    class File {		   // +
	String sName;			// +
	int    iMaxLine;		// +
	long   val[][];			// +

	/**
	 * Constructor declaration
	 *
	 *
	 * @param name
	 * @param max
	 */
	File(String name, int max) {    // +
	    sName = name;		    // +
	    iMaxLine = max;		    // +
	    val = new long[iMaxLine][3];    // +
	}				    // +

	/**
	 * Method declaration
	 *
	 *
	 * @param line
	 */
	void visit(int line) {		    // +
	    val[line][VISIT]++;    // +

	    long time = System.currentTimeMillis();    // +

	    if (fLastFile != null) {				       // +
		fLastFile.val[iLastLine][TIME] += time - lLastTime;    // +
	    }							       // +

	    fLastFile = this;				  // +
	    iLastLine = line;				  // +
	    lLastTime = time;				  // +
	}						  // +

    }							  // +

    /**
     * Method declaration
     *
     *
     * @param file
     * @param line
     * @param maxline
     */
    void m_visit(String file, int line, int maxline) {    // +
	if (bStop) {    // +
	    return;     // +
	}		// +

	bVisited = true;    // +

	File f = (File) hash.get(file);    // +

	if (f == null) {		    // +
	    f = new File(file, maxline);    // +

	    hash.put(file, f);		    // +
	}				    // +

	f.visit(line);		// +
    }				// +

    /**
     * Method declaration
     *
     */
    void m_listUnvisited() {    // +
	bStop = true;    // +

	if (!bVisited) {    // +
	    return;	    // +
	}		    // +

	Enumeration e = hash.keys();

	printline('=');
	print("UNVISITED");
	printline('-');

	int total = 0, unvisited = 0;

	while (e.hasMoreElements()) {
	    String file = (String) e.nextElement();
	    File   f = (File) hash.get(file);
	    int    maxline = f.iMaxLine;

	    total += maxline;

	    for (int l = 0; l < maxline; l++) {
		if (f.val[l][VISIT] == 0) {
		    int lto = l + 1;

		    for (; lto < maxline; lto++) {
			unvisited++;

			if (f.val[lto][VISIT] != 0) {
			    break;
			}
		    }

		    if (l == lto - 1) {
			print(file + " " + l);
		    } else {
			print(file + " " + l + " - " + (lto - 1));
		    }

		    l = lto;
		} else {
		    f.val[l][PERCALL] = f.val[l][TIME] / f.val[l][VISIT];
		}
	    }
	}

	printline('-');
	print("Total    : " + total);
	print("Unvisited: " + (100 * unvisited / total) + " %");
	printTimePerFile();
	printHigh("MOST VISITS", VISIT);
	printHigh("LONGEST RUN", TIME);
	printHigh("LONGEST PER CALL", PERCALL);
	printline('=');
    }

    /**
     * Method declaration
     *
     */
    void printTimePerFile() {
	Enumeration e = hash.keys();

	printline('-');
	print("TIME PER FILE");
	printline('-');

	int total = 0;

	while (e.hasMoreElements()) {
	    String file = (String) e.nextElement();
	    File   f = (File) hash.get(file);
	    int    time = 0;
	    int    maxline = f.iMaxLine;

	    for (int l = 0; l < maxline; l++) {
		time += f.val[l][TIME];
	    }

	    print(time + "\t" + file);

	    total += time;
	}

	printline('-');
	print("Total time: " + total);
    }

    /**
     * Method declaration
     *
     *
     * @param name
     * @param type
     */
    void printHigh(String name, int type) {
	printline('-');
	print(name);
	printline('-');

	for (int i = 0; i < TOP; i++) {
	    File	topfile = null;
	    int		topline = -1;
	    long	top = -1;
	    Enumeration e = hash.keys();

	    while (e.hasMoreElements()) {
		String file = (String) e.nextElement();
		File   f = (File) hash.get(file);
		int    maxline = f.iMaxLine;

		for (int l = 0; l < maxline; l++) {
		    long v = f.val[l][type];

		    if (v > top) {
			topfile = f;
			topline = l;
			top = v;
		    }
		}
	    }

	    print(top + "\t" + topline + "\t" + topfile.sName);

	    topfile.val[topline][type] = -1;
	}
    }

    /**
     * Method declaration
     *
     *
     * @param s
     */
    void print(String s) {
	System.out.println(s);
    }

    /**
     * Method declaration
     *
     *
     * @param c
     */
    void printline(char c) {
	for (int i = 0; i < 60; i++) {
	    System.out.print(c);
	}

	print("");
    }

}
