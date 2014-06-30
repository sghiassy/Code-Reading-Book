/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/BeanGenerator.java,v 1.4 2000/12/10 05:56:43 pierred Exp $
 * $Revision: 1.4 $
 * $Date: 2000/12/10 05:56:43 $
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
import java.util.Vector;
import java.util.Enumeration;
import java.lang.reflect.Method;

import org.apache.jasper.JasperException;
import org.apache.jasper.Constants;

import org.xml.sax.Attributes;

/**
 * Generate code for useBean.
 *
 * @author Mandar Raje
 * @author Danno Ferrin
 */
public class BeanGenerator extends GeneratorBase implements ServiceMethodPhase, 
    ClassDeclarationPhase {
  
	Attributes attrs;
	BeanRepository beanInfo;
	boolean genSession;
	boolean beanRT = false;
	Mark start;
        boolean isXml;
  
    public BeanGenerator (Mark start, Attributes attrs, BeanRepository beanInfo,
			  boolean genSession, boolean isXml) {
	this.attrs = attrs;
	this.beanInfo = beanInfo;
	this.genSession = genSession;
	this.start = start;
        this.isXml = isXml;
    }
	
    public void generate (ServletWriter writer, Class phase)
	throws JasperException {

	    if (ClassDeclarationPhase.class.equals (phase)) 
		checkSyntax (writer, phase);
	    else if (ServiceMethodPhase.class.equals (phase))
		generateMethod (writer, phase);
    }
  
    public void checkSyntax (ServletWriter writer, Class phase) 
	throws JasperException		{
	    String  name       = getAttribute ("id");
	    String  varname    = name;
	    String  serfile    = name;
	    String  scope      = getAttribute ("scope");
	    String  clsname    = getAttribute ("class");
	    String  type       = getAttribute ("type");
	    String beanName    = getAttribute ("beanName");
	    
	    // Check for mandatory attributes:
	    if ( name == null ) {
                String m = Constants.getString("jsp.error.usebean.missing.attribute");
		throw new CompileException(start, m);
	    }
	    
	    if (clsname == null && type == null) {
                String m = Constants.getString("jsp.error.usebean.missing.type",
					       new Object[] {name});
		throw new CompileException (start, m);
	    }

	    if (beanInfo.checkVariable(name) == true) {
                String m = Constants.getString("jsp.error.usebean.duplicate",
					       new Object[] {name});
                throw new CompileException (start, m);
	    }
            
	    if (scope != null && scope.equalsIgnoreCase ("session")) {
		if (genSession != true) {
                    String m = Constants.getString("jsp.error.usebean.prohibited.as.session",
						   new Object[] {name});
                    throw new CompileException (start, m);
                }
	    }

	    if (clsname != null && beanName != null) {
		String m = Constants.getString("jsp.error.usebean.not.both",
					       new Object[] {name});
		throw new CompileException (start, m);
	    }	     
	    
	    if (clsname == null) clsname = type;
	    if (scope == null || scope.equals("page")) {
		beanInfo.addPageBean(name, clsname);
	    } else if (scope.equals("request")) {
		beanInfo.addRequestBean(name, clsname);
	    } else if (scope.equals("session")) {
		beanInfo.addSessionBean(name,clsname);
	    } else if (scope.equals("application")) {
		beanInfo.addApplicationBean(name,clsname);
	    } else {
                String m = Constants.getString("jsp.error.usebean.invalid.scope",
					       new Object[] {name, scope});
	        throw new CompileException (start, m);
            }
    }
  
    public void generateMethod (ServletWriter writer, Class phase) 
	throws JasperException {
	    String  name       = getAttribute ("id");
	    String  varname    = name;
	    String  serfile    = name;
	    String  scope      = getAttribute ("scope");
	    String  clsname    = getAttribute ("class");
	    String  type       = getAttribute ("type");
	    String  beanName   = getAttribute ("beanName");
	    
	    if (type == null) type = clsname;

	    // See if beanName is a request-time expression.
	    if (beanName != null && JspUtil.isExpression (beanName, isXml)) {
		beanName = JspUtil.getExpr (beanName, isXml);
		beanRT = true;
	    }
	    
	    if (scope == null || scope.equals ("page")) {

		// declare the variable.
		declareBean (writer, type, varname);
		
		// synchronized inspection.
		lock (writer, "pageContext");

		// Generate code to locate the bean.
		locateBean (writer, type, varname, name,
			    "PageContext.PAGE_SCOPE");

		// create the bean if it doesn't exists.
		createBean (writer, varname, clsname, beanName, name, type,
			    "PageContext.PAGE_SCOPE");

		// unlock
		unlock (writer);
		
		// Initialize the bean if the body is present.
		generateInit (writer, varname);
		
	    } else  if (scope.equals ("request")) {
		
		// declare the variable.
		declareBean (writer, type, varname);
		
		// synchronized inspection.
		lock (writer, "request");
		    
		// Generate code to locate the bean.
		locateBean (writer, type, varname, name,
			    "PageContext.REQUEST_SCOPE");
		
		// create the bean if it doesn't exists.
		createBean (writer, varname, clsname, beanName, name, type,
			    "PageContext.REQUEST_SCOPE");

		// unlock.
		unlock (writer);
	       
		// Initialize the bean if the body is present.
		generateInit (writer, varname);
		
	    } else if (scope.equals ("session")) {
		
		// declare the variable.
		declareBean (writer, type, varname);
		
		// synchronized inspection.
		lock (writer, "session");
		  
		// Generate code to locate the bean.
		locateBean (writer, type, varname, name,
			    "PageContext.SESSION_SCOPE");
		
		// create the bean if it doesn't exists.
		createBean (writer, varname, clsname, beanName, name, type,
			    "PageContext.SESSION_SCOPE");

		// unlock.
		unlock (writer);
		
		// Initialize the bean.
		generateInit (writer, varname);
		
	    } else if (scope.equals ("application")) {

		// declare the variable.
		declareBean (writer, type, varname);
		
		// synchronized inspection
		lock (writer, "application");
		
		// Generate code to locate the bean.
		locateBean (writer, type, varname, name,
			    "PageContext.APPLICATION_SCOPE");
		
		// create the bean if it doesn't exist.
		createBean (writer, varname, clsname, beanName, name, type,
			    "PageContext.APPLICATION_SCOPE");

		// unlock.
		unlock (writer);
		
		// Initialize the bean.
		generateInit (writer, varname);
	    }     
    }

    private void lock (ServletWriter writer, String scope) {
	
	writer.println(" synchronized (" + scope + ") {");
	writer.pushIndent();
    }

    private void unlock (ServletWriter writer) {

	writer.popIndent();
	writer.println(" } ");
    }
	
    private void generateInit (ServletWriter writer, String name) {
	    
	    writer.println ("if(_jspx_special" + name + " == true) {");
    }

    private void declareBean (ServletWriter writer, String type, String varname) {

	writer.println (type + " " + varname + " = null;");
	
	// Variable _jspx_special used for initialization.
	writer.println ("boolean _jspx_special" + varname + "  = false;");
	
    }
	
    private void locateBean (ServletWriter writer,
			     String type,
			     String varname,
			     String name,
			     String scope) {
	
	writer.println (varname + "= (" + type + ")");
	writer.println ("pageContext.getAttribute(" +
			writer.quoteString(name) + "," + scope + ");");
    }
		
    private void createBean(ServletWriter writer, String varname, String clsname,
			    String beanName, String name, String type, String scope) {
	

	// Create the bean only if classname is specified.
	// Otherwise bean must be located in the scope.
	if (clsname != null || beanName != null) {
	    writer.println ("if ( "+varname+" == null ) {");
	    
	    // Code to create the bean:
	    writer.pushIndent ();
	    
	    // Set the boolean var. so that bean can be initialized.
	    writer.println ("_jspx_special" + name + " = true;");
	    
	    generateBeanCreate (writer, varname, clsname, beanName, type);

	    writer.println("pageContext.setAttribute(" +
			   writer.quoteString(name) + ", " + varname + ", " +
			   scope + ");");
	    
	    writer.popIndent ();
	    writer.println ("}");
	}
	else {
	    
	    // clsname not specified -- object must be found inside the scope.
	    writer.println ("if (" + varname + "  == null) ");
	    writer.println (" throw new java.lang.InstantiationException (\"bean " +
			    varname + "  not found within scope \"); ");
	}
    }
	
    protected void generateBeanCreate (ServletWriter writer,
				       String varname,
				       String clsname,
				       String beanName,
				       String type) {

	String convert = (clsname == null) ? type : clsname;
	if (beanName != null) clsname = beanName;
	writer.println ("try {");
	writer.pushIndent ();
	if (beanRT == false)
	    writer.println(varname+" = ("+ convert + 
			   ") java.beans.Beans.instantiate(this.getClass().getClassLoader(), "+
			   writer.quoteString(clsname) +");");
	else
	    writer.println(varname+" = ("+ convert + 
			   ") java.beans.Beans.instantiate(this.getClass().getClassLoader(), "+
			   clsname +");");
	writer.popIndent ();
	writer.println ("} catch (Exception exc) {");
	writer.pushIndent ();
	writer.println (" throw new ServletException (\" Cannot create bean of class \""  +
			"+\"" + clsname + "\", exc);"); 
	writer.popIndent ();
	writer.println ("}");
	
    }
	
    public String getAttribute(String name) {
	return (attrs != null) ? attrs.getValue(name) : null;
    }
	
}
