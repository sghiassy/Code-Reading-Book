/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/ForwardGenerator.java,v 1.4 2000/11/30 21:47:52 pierred Exp $
 * $Revision: 1.4 $
 * $Date: 2000/11/30 21:47:52 $
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

import org.apache.jasper.JasperException;
import org.apache.jasper.Constants;

import org.xml.sax.Attributes;

/**
 * Generator for <jsp:forward>
 *
 * @author Anil K. Vijendran
 * @author Danno Ferrin
 */
public class ForwardGenerator 
    extends GeneratorBase
    implements ServiceMethodPhase 
{
    String page;
    boolean isExpression = false;
    Hashtable params;
    boolean isXml;
    
    public ForwardGenerator(Mark start, Attributes attrs, Hashtable param,
                            boolean isXml)
	throws JasperException {
	    if (attrs.getLength() != 1)
		throw new JasperException(Constants.getString("jsp.error.invalid.forward"));
	    
	    page = attrs.getValue("page");
	    if (page == null)
		throw new CompileException(start,
					   Constants.getString("jsp.error.invalid.forward"));
	    
	    this.params = param;
            this.isXml = isXml;
	    isExpression = JspUtil.isExpression (page, isXml);
    }
    
    public void generate(ServletWriter writer, Class phase) {
	boolean initial = true;
	String sep = "?";	
        writer.println("if (true) {");
        writer.pushIndent();
        writer.println("out.clear();");
	writer.println("String _jspx_qfStr = \"\";");
	
	if (params != null && params.size() > 0) {
	    Enumeration en = params.keys();
	    while (en.hasMoreElements()) {
		String key = (String) en.nextElement();
		String []value = (String []) params.get(key);
		if (initial == true) {
		    sep = "?";
		    initial = false;
		} else sep = "&";
		
		if (value.length == 1 && JspUtil.isExpression(value[0], isXml))
		    writer.println("_jspx_qfStr = _jspx_qfStr + \"" + sep +
				   key + "=\" + " + JspUtil.getExpr(value[0], isXml) + ";");
		else {
		    if (value.length == 1)
			writer.println("_jspx_qfStr = _jspx_qfStr + \"" + sep +
				       key + "=\" + \"" + value[0] + "\";");			
		    else {
			for (int i = 0; i < value.length; i++) {
			    if (!JspUtil.isExpression(value[i], isXml))
				writer.println("_jspx_qfStr = _jspx_qfStr + \"" + sep +
					       key + "=\" + \"" + value[i] + "\";");
			    else
				writer.println("_jspx_qfStr = _jspx_qfStr + \"" + sep +
					       key + "=\" +" + JspUtil.getExpr(value[i], isXml)+ ";");
			    if (sep.equals("?")) sep = "&";			    
			}
		    }
		}
	    }
	}
	if (!isExpression)
            writer.println("pageContext.forward(" +
			   writer.quoteString(page) + " +  _jspx_qfStr);");
	else
            writer.println("pageContext.forward(" +
			   JspUtil.getExpr (page, isXml) +  " +  _jspx_qfStr);");
	
        writer.println("return;");
        writer.popIndent();
        writer.println("}");
    }
}
