/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/PluginGenerator.java,v 1.4 2001/09/18 00:48:19 craigmcc Exp $
 * $Revision: 1.4 $
 * $Date: 2001/09/18 00:48:19 $
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
import org.apache.jasper.JspCompilationContext;

import org.xml.sax.Attributes;

/**
 * Generator for <jsp:plugin>
 *
 * @author Rajiv Mordani [mode@eng.sun.com]
 * @author Danno Ferrin
 */
public class PluginGenerator 
    extends GeneratorBase
    implements ServiceMethodPhase 
{
    Attributes attrs;
    Hashtable param;
    String fallback;

    String ieClassId;
    Mark start;
    boolean isXml;
    
    
    public PluginGenerator(Mark start, Attributes attrs, Hashtable param,
			   String fallback, boolean isXml) {
    	this.attrs = attrs;
	this.param = param;
	this.fallback = fallback;
	this.start = start;
        this.isXml = isXml;
    }

    public void init(JspCompilationContext ctxt) throws JasperException {
        ieClassId = ctxt.getOptions().getIeClassId();
    }
    
    public void generate(ServletWriter writer, Class phase) 
    				throws JasperException
    {

	String type = getAttribute ("type");
	String code = getAttribute ("code");
	String codebase = getAttribute ("codebase");
	String archive = getAttribute ("archive");
	String jreversion = getAttribute ("jreversion");
	String name = getAttribute ("name");
	String nspluginurl = getAttribute ("nspluginurl");
	String iepluginurl = getAttribute ("iepluginurl");

	if (type == null)
	    throw new CompileException (start, Constants.getString (
	    				"jsp.error.plugin.notype"));
	if (code == null)
	    throw new CompileException (start, Constants.getString (
	    				"jsp.error.plugin.nocode"));

	writer.popIndent ();
	writer.println ("/*Code generated for plugin*/");
	writer.println("{");	// Work around multi-plugin-tag bug
	writer.indent ();
	writer.print ("out.println (\"<OBJECT classid=\\\"");
	writer.print (ieClassId);
	writer.print ("\\\"");
        if (name != null) {
            writer.print(" name=\\\"");
            writer.print(name);
            writer.print("\\\"");
        }
	generateCommon (writer);
	writer.print (" codebase=\\\"");
	if (iepluginurl == null)
	    writer.print (Constants.IE_PLUGIN_URL);
	else 
	    writer.print (iepluginurl);
	
	writer.print ("\\\"");
	writer.print (">\");");
	// Done to avoid spaces being generated between the closing angular
	// bracket of the tag.
	writer.println ();
	writer.indent ();
	writer.print ("out.println (\"<PARAM name=\\\"java_code\\\"");
	writer.print (" value=\\\"");
	writer.print (code);
	writer.print ("\\\"");
	writer.print (">\");");
	writer.println ();

	if (codebase != null) {
	    writer.indent ();
	    writer.print ("out.println (\"<PARAM name=\\\"java_codebase\\\"");
	    writer.print (" value=\\\"");
	    writer.print (codebase);
	    writer.print ("\\\"");
	    writer.print (">\");");
	    writer.println ();
	}

	if (archive != null) {
	    writer.indent ();
	    writer.print ("out.println (\"<PARAM name=\\\"java_archive\\\"");
	    writer.print (" value=\\\"");
	    writer.print (archive);
	    writer.print ("\\\"");
	    writer.print (">\");");
	    writer.println ();
	}

	writer.indent ();
	writer.print ("out.println (\"<PARAM name=\\\"type\\\"");
	writer.print (" value=\\\"");
	if (type.equals ("applet")) 
	    writer.print ("application/x-java-applet;");
	else if (type.equals ("bean"))
	    writer.print ("application/x-java-bean;");
	if (jreversion != null) {
	    writer.print ("version=");
	    writer.print (jreversion);
	}
	writer.print ("\\\"");
	writer.print (">\");");
	writer.println ();

	// Evaluate params and if expression get the appropriate code for
	// the expression

	Enumeration e = null;
	String value [] = null;
	String key = null;

	writer.indent ();
	writer.print ("String _jspxString = null;");
	writer.println ();

	if (param != null) {
	    e = param.keys ();
	    while (e.hasMoreElements ()) {
	        key = (String) e.nextElement ();
	        value = (String[]) param.get (key);
		writer.indent ();
	 	//Check to see if the param is an expression, if so
		//evaluate that and put the value.
		String temp = null;
		if (JspUtil.isExpression (value[0], isXml)) {
		    temp = JspUtil.getExpr (value[0], isXml);
		    //value[0] = JspUtil.getExpr(value[0], isXml);
		    writer.print ("_jspxString =" + temp + ";");
		} else {
		    writer.print ("_jspxString = \"" + value[0] + "\";");
		}
		writer.println ();
	        writer.indent ();
	        writer.print ("out.println (\"<PARAM name=\\\"");
		if (key.equalsIgnoreCase ("object"))
		    writer.print ("java_object");
		else if (key.equalsIgnoreCase ("type"))
		    writer.print ("java_type");
		else 
	            writer.print (key);
	        writer.print ("\\\"");
	        writer.print (" value=\\\"\"");
	        writer.print ("+ _jspxString + ");
	        writer.print ("\"\\\"");
	        writer.print (">\");");
	        writer.println ();
	    }
	}

	writer.indent ();
	writer.print ("String [][] _jspxNSString = null;"); 
	writer.println ();
	writer.indent ();
	writer.print ("int i = 0;");
	writer.println ();
	if (param != null) {
	    e = param.keys ();
	    writer.indent ();
	    writer.indent ();
	    writer.print ("_jspxNSString = new String [");
	    Integer temp = new Integer (param.size ());
	    writer.print (temp.toString ());
	    writer.print ("][2];");
	    writer.println ();
	    writer.indent ();
	    key = null;
	    value = null;
	    while (e.hasMoreElements ()) {
	        key = (String) e.nextElement ();
	        value = (String[]) param.get (key);
	        writer.print ("_jspxNSString [i][0] = \"");
		if (key.equalsIgnoreCase ("object"))
		    key = "java_object";
		else if (key.equalsIgnoreCase ("type"))
		    key = "java_type";
		writer.print (key);
		writer.print ("\";");
		writer.println ();
		writer.indent ();

	        //value = (String[]) param.get (key);
		if (JspUtil.isExpression(value[0], isXml)) {
		    value[0] = JspUtil.getExpr(value[0], isXml);
		    writer.print ("_jspxNSString[i][1] =" + value[0] + ";");
		} else {
		    writer.print ("_jspxNSString[i][1] = \"" + value[0] + 
		    			"\";");
		}
		writer.println ();
		writer.indent ();
		writer.print ("i++;");
		writer.println ();
	    }
	}
	
	writer.println ("out.println (\"<COMMENT>\");");
	writer.indent ();
	writer.print ("out.print (\"<EMBED type=\\\"");
	if (type.equals ("applet")) 
	    writer.print ("application/x-java-applet;");
	else if (type.equals ("bean"))
	    writer.print ("application/x-java-bean;");
	if (jreversion != null) {
	    writer.print ("version=");
	    writer.print (jreversion);
	}
	writer.print ("\\\" ");
        if (name != null) {
            writer.print(" name=\\\"");
            writer.print(name);
            writer.print("\\\"");
        }
	generateCommon (writer);
	writer.print ("pluginspage=\\\"");
	if (nspluginurl == null)
	    writer.print (Constants.NS_PLUGIN_URL);
	else 
	    writer.print (nspluginurl);
	
	writer.print ("\\\" ");
	
	writer.print ("java_code=\\\"");
	writer.print (code);
	writer.print ("\\\" ");
	if (codebase != null) {
	    writer.print ("java_codebase=\\\"");
	    writer.print (codebase);
	    writer.print ("\\\" ");
	}

	if (archive != null) {
	    writer.print ("java_archive=\\\"");
	    writer.print (archive);
	    writer.print ("\\\" ");
	}

	writer.print ("\");");
	writer.println ();


	writer.indent ();
	writer.print ("if (_jspxNSString != null) {");
	writer.println ();
	writer.pushIndent ();
	writer.indent ();
	writer.print ("for (i = 0; i < _jspxNSString.length; i++) {");
	writer.println ();
	writer.pushIndent ();
	writer.indent ();
	writer.print ("out.println (");
	writer.print (" _jspxNSString [i][0] + ");
	writer.print ("\"=\"");
	writer.print (" + _jspxNSString[i][1]");
	writer.print (");");
	writer.println ();
	writer.popIndent ();
	writer.println ("}");
	writer.popIndent ();
	writer.println ("}");
	writer.println ("out.println (\">\");");
	writer.println ("out.println (\"<NOEMBED>\");");
	writer.println ("out.println (\"</COMMENT>\");");

	if (fallback != null) {
	    fallback = writer.quoteString (fallback);
	    writer.indent ();
	    writer.print ("out.println (");
	    writer.print (fallback);
	    writer.print (");");
	    writer.println ();
	}
	writer.println ("out.println (\"</NOEMBED></EMBED>\");");
	writer.println ("out.println (\"</OBJECT>\");");
	writer.pushIndent ();
	writer.println("}");	// Work around multi-plugin-tag bug
    }

    public void generateCommon (ServletWriter writer) {
	String align = getAttribute ("align");
	String width = getAttribute ("width");
	String height = getAttribute ("height");
	String hspace = getAttribute ("hspace");
	String vspace = getAttribute ("vspace");

	if (width != null) {
	    writer.print (" width=\\\"");
	    writer.print (width);
	    writer.print ("\\\" ");
	}

	if (height != null) {
	    writer.print (" height=\\\"");
	    writer.print (height);
	    writer.print ("\\\" ");
	}

	if (hspace != null) {
	    writer.print (" hspace=\\\"");
	    writer.print (hspace);
	    writer.print ("\\\" ");
	}

	if (vspace != null) {
	    writer.print (" vspace=\\\"");
	    writer.print (vspace);
	    writer.print ("\\\" ");
	}

	if (align != null) {
	    writer.print (" align=\\\"");
	    writer.print (align);
	    writer.print ("\\\" ");
	}

    }

    public String getAttribute(String name) {
	return (attrs != null) ? attrs.getValue(name) : null;
    }
}
