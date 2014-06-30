/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/UninterpretedTagBeginGenerator.java,v 1.4 2001/07/13 19:41:31 horwat Exp $
 * $Revision: 1.4 $
 * $Date: 2001/07/13 19:41:31 $
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

import java.util.Hashtable;
import java.util.Enumeration;

import org.xml.sax.Attributes;

/**
 * Generates the start element of an uninterpreted tag.
 *
 * @author Pierre Delisle
 * @author Danno Ferrin
 */
public class UninterpretedTagBeginGenerator
    extends GeneratorBase
    implements ServiceMethodPhase
{
    private static final String singleQuote = "'";
    private static final String doubleQuote = "\\\"";

    private String tag;
    private Attributes attrs;

    public UninterpretedTagBeginGenerator(String tag, Attributes attrs) {
	this.tag = tag;
	this.attrs = attrs;
    }

    public void generate(ServletWriter writer, Class phase) {
	writer.indent();
    	writer.print("out.write(\"");

	StringBuffer sb = new StringBuffer();
        sb.append("<").append(tag);
        if (attrs == null) {
            sb.append(">");
        } else {
            sb.append(" ");
            int attrsLength = attrs.getLength();
            for (int i = 0; i < attrsLength; i++) {
		String quote = doubleQuote;
                String name = attrs.getQName(i);
                String value = attrs.getValue(i);
		if (value.indexOf('"') != -1) quote = singleQuote;
                sb.append("  ").append(name).append("=").append(quote);
		sb.append(value).append(quote);
            }
            sb.append(">");
        }
	writer.print(sb.toString());
        writer.print("\");");
        writer.println();
    }
}
