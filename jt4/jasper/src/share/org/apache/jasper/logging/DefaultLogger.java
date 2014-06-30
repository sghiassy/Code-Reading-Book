/*
 * ====================================================================
 * 
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowlegement:  
 *       "This product includes software developed by the 
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowlegement may appear in the software itself,
 *    if and wherever such third-party acknowlegements normally appear.
 *
 * 4. The names "The Jakarta Project", "Tomcat", and "Apache Software
 *    Foundation" must not be used to endorse or promote products derived
 *    from this software without prior written permission. For written 
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 */ 
package org.apache.jasper.logging;

import java.io.Writer;
import java.io.IOException;
import javax.servlet.ServletContext;

/**
 * Trivial logger that sends all messages to the default sink.  To
 * change default sink, call Logger.setDefaultSink(Writer)
 *
 * @author Alex Chaffee (alex@jguru.com)
 * @since  Tomcat 3.1
 **/
public class DefaultLogger extends Logger {

    static private char[] newline;
    static {
	String separator = System.getProperty("line.separator", "\n");
	newline = separator.toCharArray();
    }
    

    /**
     * Default constructor leaves the debug output going to the
     * default sink.
     */
    public DefaultLogger() {
        super();
    }


    /**
     * The servlet context we are associated with.
     */
    protected ServletContext servletContext;


    /**
     * Construct a logger that writes output to the servlet context log
     * for the current web application.
     *
     * @param servletContext The servlet context for our web application
     */
    public DefaultLogger(ServletContext servletContext) {
        super();
        this.servletContext = servletContext;
    }



    /**
     * Prints log message to default sink
     * 
     * @param	message		the message to log.
     */
    protected void realLog(String message) {
        if (servletContext != null) {
            servletContext.log(message);
            return;
        }
	try {
	    defaultSink.write(message);
	    defaultSink.write(newline);
	    flush();
	}
	catch (IOException e) {
	    bad(e, message, null);
	}
    }
    
    /**
     * Prints log message to default sink
     * 
     * @param	message		the message to log.
     * @param	t		the exception that was thrown.
     */
    protected void realLog(String message, Throwable t) {
        if (servletContext != null) {
            servletContext.log(message, t);
            return;
        }
	try {
	    defaultSink.write(message);
	    defaultSink.write(newline);
	    defaultSink.write(throwableToString(t));
	    defaultSink.write(newline);
	    flush();
	}
	catch (IOException e) {
	    bad(e, message, t);
	}
    }

    private void bad(Throwable t1, String message, Throwable t2)
    {
	System.err.println("Default sink is unwritable! Reason:");
	if (t1 != null) t1.printStackTrace();
	if (message != null) System.err.println(message);
	if (t2 != null) t2.printStackTrace();
    }	
    
    /**
     * Flush the log. 
     */
    public void flush() {
	try {
	    defaultSink.flush();
	}
	catch (IOException e) {
	}
    }    
}
