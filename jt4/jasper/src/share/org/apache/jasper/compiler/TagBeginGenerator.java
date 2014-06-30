/*
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
import java.util.Stack;
import java.util.Enumeration;
import java.lang.reflect.Method;

import javax.servlet.jsp.tagext.TagLibraryInfo;
import javax.servlet.jsp.tagext.TagInfo;
import javax.servlet.jsp.tagext.TagAttributeInfo;
import javax.servlet.jsp.tagext.VariableInfo;
import javax.servlet.jsp.tagext.TagVariableInfo;
import javax.servlet.jsp.tagext.TagData;
import javax.servlet.jsp.tagext.Tag;
import javax.servlet.jsp.tagext.BodyTag;

import org.apache.jasper.JasperException;
import org.apache.jasper.JspCompilationContext;
import org.apache.jasper.Constants;

import org.apache.jasper.compiler.ServletWriter;

import org.xml.sax.Attributes;
import org.xml.sax.helpers.AttributesImpl;

/**
 * Custom tag support.
 *
 * @author Anil K. Vijendran
 * @author Danno Ferrin
 */
public class TagBeginGenerator
    extends TagGeneratorBase
    implements ServiceMethodPhase
{
    String prefix;
    String shortTagName;
    Attributes attrs;
    TagLibraryInfo tli;
    TagInfo ti;
    TagAttributeInfo[] attributes;
    String baseVarName, thVarName;
    TagCache tc;
    TagData tagData;
    Mark start;
    TagLibraries libraries;
    boolean hasBody;
    boolean isXml;


    public TagBeginGenerator(Mark start, String prefix, String shortTagName, Attributes attrs,
			     TagLibraryInfo tli, TagInfo ti, TagLibraries libraries,
                             Stack tagHandlerStack, Hashtable tagVarNumbers,
                             boolean hasBody, boolean isXml)
        throws JasperException
    {
        setTagHandlerStack(tagHandlerStack);
        setTagVarNumbers(tagVarNumbers);
        this.prefix = prefix;
        this.shortTagName = shortTagName;
        this.attrs = attrs;
	this.tli = tli;
	this.ti = ti;
	this.attributes = ti.getAttributes();
	this.baseVarName = getTagVarName(prefix, shortTagName);
	this.thVarName = "_jspx_th_"+baseVarName;
	this.start = start;
	this.libraries = libraries;
        this.hasBody = hasBody;
	this.isXml = isXml;
    }

    public void init(JspCompilationContext ctxt) throws JasperException {
        validate();
        tc = libraries.getTagCache(prefix, shortTagName);
        if (tc == null) {
            tc = new TagCache(shortTagName);

            ClassLoader cl = ctxt.getClassLoader();
            Class clz = null;
            try {
                clz = cl.loadClass(ti.getTagClassName());
            } catch (Exception ex) {
                throw new CompileException(start,
					   Constants.getString("jsp.error.unable.loadclass",
                                                              new Object[] { ti.getTagClassName(),
                                                                             ex.getMessage()
                                                              }
                                                              ));
            }
            tc.setTagHandlerClass(clz);
            libraries.putTagCache(prefix, shortTagName, tc);
        }
    }

    void validate() throws JasperException {

        // Sigh! I wish I didn't have to clone here.
        Hashtable attribs = JspUtil.attrsToHashtable(attrs);

        // First make sure all required attributes are indeed present.
        for(int i = 0; i < attributes.length; i++)
            if (attributes[i].isRequired() && attribs.get(attributes[i].getName()) == null)
                throw new CompileException(start,
					   Constants.getString("jsp.error.missing_attribute",
                                                              new Object[] {
                                                                  attributes[i].getName(),
                                                                  shortTagName
                                                              }
                                                              ));
        // Now make sure there are no invalid attributes...
        Enumeration e = attribs.keys();
        while (e.hasMoreElements()) {
            String attr = (String) e.nextElement();
            boolean found = false;
            for(int i = 0; i < attributes.length; i++)
                if (attr.equals(attributes[i].getName())) {
                    found = true;
                    if (attributes[i].canBeRequestTime() &&
			JspUtil.isExpression((String)attribs.get(attr), isXml))
                        attribs.put(attr, TagData.REQUEST_TIME_VALUE);
		}

            if (!found)
                throw new CompileException(start,
					   Constants.getString("jsp.error.bad_attribute",
                                                              new Object[] {
                                                                  attr
                                                              }
                                                              ));
        }
        
        tagData = new TagData(attribs);
        if (!ti.isValid(tagData))
            throw new CompileException(start,
				       Constants.getString("jsp.error.invalid_attributes"));
    }

    /**
     * Generates the code that sets the attributes of the tag bean.
     * <BR><B>Precondition</B>: All attributes must be validated, specifically
     * any attributes in the attribs field correspond to an exposed proeprty of
     * the tag bean, and only those attributes that are to be run-time 
     * expressions are encoded as such in the code<BR>
     */
    private final void generateSetters(ServletWriter writer, String parent)
        throws JasperException
    {
        writer.println(thVarName+".setPageContext(pageContext);");
        writer.println(thVarName+".setParent("+parent+");");

        int attrsLength = attrs.getLength();
        for(int i = 0; i < attrsLength; i++) {
            String attrValue = attrs.getValue(i);
            if (attrValue != null) {
                String attrName = attrs.getQName(i);
		    Method m = tc.getSetterMethod(attrName);
                if (m == null)
                    throw new CompileException
                        (start, Constants.getString
                         ("jsp.error.unable.to_find_method",
                          new Object[] { attrName }));

                Class c[] = m.getParameterTypes();
                // assert(c.length > 0)

                // match tld defined attribute to its element value
                for(int j = 0; j < attributes.length; j++) {
                    if (attrs.getQName(i).equals(attributes[j].getName())) {
                        if (attributes[j].canBeRequestTime() && 
                            JspUtil.isExpression(attrValue, isXml)) {
                            attrValue = JspUtil.getExpr(attrValue, isXml);
                        } else {
                            attrValue = convertString(c[0], attrValue, writer, attrName,
                                                      tc.getPropertyEditorClass(attrName));
                        }
                    }
                }


                writer.println(thVarName+"."+m.getName()+"("+attrValue+");");
            }
        }
    }

    public String convertString(Class c, String s, ServletWriter writer, String attrName,
				Class propertyEditorClass)
        throws JasperException 
    {
	if (propertyEditorClass != null) {
	    return 
		"(" + c.getName() + 
		")JspRuntimeLibrary.getValueFromBeanInfoPropertyEditor(" +
		c.getName() + ".class, \"" + attrName + "\", " + writer.quoteString(s) + ", " +
		propertyEditorClass.getName() + ".class)";
	} else if (c == String.class) {
            return writer.quoteString(s);
        } else if (c == boolean.class) {
            return Boolean.valueOf(s).toString();
        } else if (c == Boolean.class) {
            return "new Boolean(" + Boolean.valueOf(s).toString() + ")";
        } else if (c == byte.class) {
            return "((byte)" + Byte.valueOf(s).toString() + ")";
        } else if (c == Byte.class) {
            return "new Byte((byte)" + Byte.valueOf(s).toString() + ")";
        } else if (c == char.class) {
            // non-normative, because a normative method would fail to compile
            if (s.length() > 0) {
                char ch = s.charAt(0);
                // this trick avoids escaping issues
                return "((char) " + (int) ch + ")";
            } else {
                throw new NumberFormatException(Constants.getString(
                            "jsp.error.bad_string_char",
                            new Object[0]));
            }
        } else if (c == Character.class) {
            // non-normative, because a normative method would fail to compile
            if (s.length() > 0) {
                char ch = s.charAt(0);
                // this trick avoids escaping issues
                return "new Character((char) " + (int) ch + ")";
            } else {
                throw new NumberFormatException(Constants.getString(
                            "jsp.error.bad_string_Character",
                            new Object[0]));
            }
        } else if (c == double.class) {
            return Double.valueOf(s).toString();
        } else if (c == Double.class) {
            return "new Double(" + Double.valueOf(s).toString() + ")";
        } else if (c == float.class) {
            return Float.valueOf(s).toString() + "f";
        } else if (c == Float.class) {
            return "new Float(" + Float.valueOf(s).toString() + "f)";
        } else if (c == int.class) {
            return Integer.valueOf(s).toString();
        } else if (c == Integer.class) {
            return "new Integer(" + Integer.valueOf(s).toString() + ")";
        } else if (c == short.class) {
            return "((short) " + Short.valueOf(s).toString() + ")";
        } else if (c == Short.class) {
            return "new Short(" + Short.valueOf(s).toString() + ")";
        } else if (c == long.class) {
            return Long.valueOf(s).toString() + "l";
        } else if (c == Long.class) {
            return "new Long(" + Long.valueOf(s).toString() + "l)";
        } else if (c == Object.class) {
            return "new String(" + s + ")";
	} else {
	    return "(" + c.getName() + 
		")JspRuntimeLibrary.getValueFromPropertyEditorManager(" +
		c.getName() + ".class, \"" + attrName + "\", " + writer.quoteString(s) + ")";
        }
	/* FIXME TOBEREMOVED
        } else {
             throw new CompileException
                    (start, Constants.getString
                     ("jsp.error.unable.to_convert_string",
                      new Object[] { c.getName(), attrName }));
        }
	*/
    }   
    
    public void generateServiceMethodStatements(ServletWriter writer)
        throws JasperException
    {
        TagVariableData top = topTag();
        String parent = top == null ? null : top.tagHandlerInstanceName;

        String evalVar = "_jspx_eval_"+baseVarName;
	tagBegin(new TagVariableData(thVarName, evalVar));

        writer.println("/* ----  "+prefix+":"+shortTagName+" ---- */");

        writer.println(ti.getTagClassName()+" "+thVarName+" = new "+ti.getTagClassName()+"();");
        generateSetters(writer, parent);

        VariableInfo[] vi = ti.getVariableInfo(tagData);
	TagVariableInfo[] tvi = ti.getTagVariableInfos();
        // Just declare AT_BEGIN here...
        declareVariables(writer, vi, tvi, tagData, true, false, VariableInfo.AT_BEGIN);

	writer.println("try {");
	writer.pushIndent();

        writer.println("int "+evalVar+" = "
                       +thVarName+".doStartTag();");

        boolean implementsBodyTag = 
	    BodyTag.class.isAssignableFrom(tc.getTagHandlerClass());
        // Need to update AT_BEGIN variables here
        declareVariables(writer, vi, tvi, tagData, false, true, VariableInfo.AT_BEGIN);

        // FIXME: I'm not too sure if this is the right approach. I don't like
        //        throwing English language strings into the generated servlet.
        //        Perhaps, I'll just define an inner classes as necessary for these
        //        types of exceptions? -akv

        if (!implementsBodyTag) {
            writer.println("if ("+evalVar+" == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)");
            writer.pushIndent();
            writer.println("throw new JspTagException(\"Since tag handler "+tc.getTagHandlerClass()+
                           " does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG\");");
            writer.popIndent();
        }

        if (hasBody) {
            writer.println("if ("+evalVar+" != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {");
	    writer.pushIndent();

	    if (implementsBodyTag) {
                writer.println("try {");
                writer.pushIndent();

	        writer.println("if ("+evalVar+" != javax.servlet.jsp.tagext.Tag.EVAL_BODY_INCLUDE) {");
	        writer.pushIndent();

	        writer.println("out = pageContext.pushBody();");
	        writer.println(thVarName+".setBodyContent((javax.servlet.jsp.tagext.BodyContent) out);");

	        writer.popIndent();
	        writer.println("}");

	        writer.println(thVarName+".doInitBody();");
	    }

	    writer.println("do {");
	    writer.pushIndent();
        }
        // Need to declare and update NESTED variables here
        declareVariables(writer, vi, tvi, tagData, true, true, VariableInfo.NESTED);
        // Need to update AT_BEGIN variables here
        declareVariables(writer, vi, tvi, tagData, false, true, VariableInfo.AT_BEGIN);
    }

    public void generate(ServletWriter writer, Class phase)
        throws JasperException
    {
        generateServiceMethodStatements(writer);
    }
}

