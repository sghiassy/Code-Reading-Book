/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/CharDataGenerator.java,v 1.3 2001/07/13 19:17:07 remm Exp $
 * $Revision: 1.3 $
 * $Date: 2001/07/13 19:17:07 $
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

/**
 * CharDataGenerator generates the character data present in the JSP
 * file. Typically this is HTML which lands up as strings in
 * out.println(...). 
 *
 * @author Anil K. Vijendran
 */
public class CharDataGenerator 
    extends GeneratorBase
    implements ServiceMethodPhase
{
    char[] chars;

    // process in 32k chunks
    private static final int MAXSIZE = 32 * 1024;
    
    public CharDataGenerator(char[] chars) {
	this.chars = chars;
    }

    public void generate(ServletWriter writer, Class phase) {
	writer.indent();
	int current	= 0;
	int limit       = chars.length;
	while (current < limit) {
	    int from = current;
	    int to = Math.min(current + MAXSIZE, limit);
	    generateChunk(writer, from, to);
	    current = to;
	    writer.println();
	}
    }

    private void generateChunk(ServletWriter writer, int from, int to) {
	writer.print("out.write(\"");
	// Generate the char data:
	StringBuffer sb = new StringBuffer();
	for (int i = from ; i < to ; i++) {
	    int ch = chars[i];
	    switch(ch) {
	    case '"':
		sb.append("\\\"");
		break;
	    case '\\':
		sb.append("\\\\");
		break;
	    case '\r':
		continue;
		/*
		  case '\'':
		  sb.append('\\');
		  sb.append('\'');
		  break;
		*/
	    case '\n':
		sb.append("\\r\\n");
		break;
	    case '\t':
		sb.append("\\t");
		break;
	    default:
		sb.append((char) ch);
	    }
	}
	writer.print(sb.toString());
        writer.print("\");");
        writer.println();
    }



}
