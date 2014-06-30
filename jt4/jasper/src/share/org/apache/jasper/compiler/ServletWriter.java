/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/ServletWriter.java,v 1.2 2001/07/12 21:40:51 horwat Exp $
 * $Revision: 1.2 $
 * $Date: 2001/07/12 21:40:51 $
 *
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
package org.apache.jasper.compiler;

import java.io.PrintWriter;
import java.io.BufferedReader;
import java.io.StringReader;
import java.io.IOException;

/**
 * This is what is used to generate servlets. 
 *
 * @author Anil K. Vijendran
 */
public class ServletWriter {
    public static int TAB_WIDTH = 4;
    public static String SPACES = "                              ";

    // Current indent level:
    int indent = 0;

    // The sink writer:
    PrintWriter writer;
    
    // servlet line numbers start from 1, but we pre-increment
    private int javaLine = 0;
    private JspLineMap lineMap = new JspLineMap();


    public ServletWriter(PrintWriter writer) {
	this.writer = writer;
    }

    public void close() throws IOException {
	writer.close();
    }
    
    // -------------------- Access informations --------------------

    public int getJavaLine() {
        return javaLine;
    }

    public void setLineMap(JspLineMap map) {
        this.lineMap = map;
    }

    public JspLineMap getLineMap() {
        return lineMap;
    }

    // -------------------- Formatting --------------------

    public void pushIndent() {
	if ((indent += TAB_WIDTH) > SPACES.length())
	    indent = SPACES.length();
    }

    public void popIndent() {
	if ((indent -= TAB_WIDTH) <= 0 )
	    indent = 0;
    }

    /**
     * Print a standard comment for echo outputed chunk.
     * @param start The starting position of the JSP chunk being processed. 
     * @param stop  The ending position of the JSP chunk being processed. 
     */
    public void printComment(Mark start, Mark stop, char[] chars) {
        if (start != null && stop != null) {
            println("// from="+start);
            println("//   to="+stop);
        }
        
        if (chars != null)
            for(int i = 0; i < chars.length;) {
                indent();
                print("// ");
                while (chars[i] != '\n' && i < chars.length)
                    writer.print(chars[i++]);
            }
    }

    /**
     * Quote the given string to make it appear in a chunk of java code.
     * @param s The string to quote.
     * @return The quoted string.
     */

    public String quoteString(String s) {
	// Turn null string into quoted empty strings:
	if ( s == null )
	    return "null";
	// Hard work:
	if ( s.indexOf('"') < 0 && s.indexOf('\\') < 0 && s.indexOf ('\n') < 0
	     && s.indexOf ('\r') < 0)
	    return "\""+s+"\"";
	StringBuffer sb  = new StringBuffer();
	int          len = s.length();
	sb.append('"');
	for (int i = 0 ; i < len ; i++) {
	    char ch = s.charAt(i);
	    if ( ch == '\\' && i+1 < len) {
		sb.append('\\');
		sb.append('\\');
		sb.append(s.charAt(++i));
	    } else if ( ch == '"' ) {
		sb.append('\\');
		sb.append('"');
	    } else if (ch == '\n') {
	        sb.append ("\\n");
	    }else if (ch == '\r') {
	   	sb.append ("\\r");
	    }else {
		sb.append(ch);
	    }
	}
	sb.append('"');
	return sb.toString();
    }

    public void println(String line) {
        javaLine++;
	writer.println(SPACES.substring(0, indent)+line);
    }

    public void println() {
        javaLine++;
	writer.println("");
    }

    public void indent() {
	writer.print(SPACES.substring(0, indent));
    }
    

    public void print(String s) {
        int index = 0;

        // look for hidden newlines inside strings
        while ((index=s.indexOf('\n',index)) > -1 ) {
            javaLine++;
            index++;
        }

	writer.print(s);
    }

    public void printMultiLn(String multiline) {
	// Try to be smart (i.e. indent properly) at generating the code:
	BufferedReader reader = 
            new BufferedReader(new StringReader(multiline));
	try {
    	    for (String line = null ; (line = reader.readLine()) != null ; ) {
		//		println(SPACES.substring(0, indent)+line);
		println(line);
            }
	} catch (IOException ex) {
	    // Unlikely to happen, since we're acting on strings
	}
    }


}
