/*
 * Servlet.java
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

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;

/**
 * <font color="#009900">
 * Servlet acts as a interface between the applet and the database for the
 * the client / server mode of HSQL Database Engine. It is not required if
 * the included HSQL Database Engine WebServer is used, but if another
 * HTTP server is used. The HTTP Server must support the Servlet API.
 * <P>
 * This class should not be used directly by the application. It will be
 * called by the HTTP Server. The applet / application should use the
 * jdbc* classes.
 * </font>
 */
public class Servlet extends HttpServlet {
    private String   sError;
    private Database dDatabase;
    private String   sDatabase;

    /**
     * Constructor declaration
     *
     */
    public Servlet() {
	init("test");
    }

    /**
     * Constructor declaration
     *
     *
     * @param database
     */
    public Servlet(String database) {
	init(database);
    }

    /**
     * Method declaration
     *
     *
     * @param database
     */
    void init(String database) {
	try {
	    sDatabase = database;
	    dDatabase = new Database(database);
	} catch (Exception e) {
	    sError = e.getMessage();
	}
    }

    private static long lModified = 0;

    /**
     * Method declaration
     *
     *
     * @param req
     *
     * @return
     */
    protected long getLastModified(HttpServletRequest req) {

	// this is made so that the cache of the http server is not used
	// maybe there is some other way
	return lModified++;
    }

    /**
     * Method declaration
     *
     *
     * @param request
     * @param response
     *
     * @throws IOException
     * @throws ServletException
     */
    public void doGet(HttpServletRequest request,
		      HttpServletResponse response) throws IOException,
		      ServletException {
	String query = request.getQueryString();

	if (query == "" || query == null) {
	    response.setContentType("text/html");

	    PrintWriter out = response.getWriter();

	    out.println("<html><head><title>HSQL Database Engine Servlet</title>");
	    out.println("</head><body><h1>HSQL Database Engine Servlet</h1>");
	    out.println("The servlet is running.<P>");

	    if (dDatabase != null) {
		out.println("The database is also running.<P>");
		out.println("Database name: " + sDatabase + "<P>");
		out.println("Queries processed: " + iQueries + "<P>");
	    } else {
		out.println("<h2>The database is not running!</h2>");
		out.println("The error message is:<P>");
		out.println(sError);
	    }

	    out.println("</body></html>");
	}
    }

    /**
     * Method declaration
     *
     *
     * @param request
     * @param response
     *
     * @throws IOException
     * @throws ServletException
     */
    public void doPost(HttpServletRequest request,
		       HttpServletResponse response) throws IOException,
		       ServletException {
	ServletInputStream input = request.getInputStream();
	int		   len = request.getContentLength();
	byte		   b[] = new byte[len];

	input.read(b, 0, len);

	String s = new String(b);
	int    p = s.indexOf('+');
	int    q = s.indexOf('+', p + 1);

	if (p == -1 || q == -1) {
	    doGet(request, response);
	}

	String user = s.substring(0, p);

	user = StringConverter.hexStringToUnicode(user);

	String password = s.substring(p + 1, q);

	password = StringConverter.hexStringToUnicode(password);
	s = s.substring(q + 1);
	s = StringConverter.hexStringToUnicode(s);

	response.setContentType("application/octet-stream");

	ServletOutputStream out = response.getOutputStream();
	byte		    result[] = dDatabase.execute(user, password, s);

	response.setContentLength(result.length);
	out.write(result);
	out.flush();
	out.close();

	iQueries++;

	// System.out.print("Queries processed: "+iQueries+"  \r");
    }

    static private int iQueries;
}
