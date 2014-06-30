/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/StoredCharDataGenerator.java,v 1.2 2001/07/25 01:08:13 craigmcc Exp $
 * $Revision: 1.2 $
 * $Date: 2001/07/25 01:08:13 $
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

import java.util.Vector;

/**
 * StoredCharDataGenerator generates HTML and other data present in
 * JSP files to be stored/serialized into a .dat file. 
 *
 * @author Anil K. Vijendran
 */
public class StoredCharDataGenerator 
    extends GeneratorBase
    implements ServiceMethodPhase, InitMethodPhase, ClassDeclarationPhase
{
    int stringId;
    char[] chars;
    Vector vector;
    String fileName;
    
    public StoredCharDataGenerator(Vector vector, String fileName, 
                                   int stringId, char[] chars) {
        this.stringId = stringId;
	this.chars = chars;
        this.vector = vector;
        this.fileName = fileName;
    }

    private final String getStringVar() {
        return "_jspx_html_data["+stringId+"]";
    }

    private final void generateRef(ServletWriter writer) {
        if (stringId == 0)
            writer.println("static char[][] _jspx_html_data = null;");
    }

    private final void generateInit(ServletWriter writer) {
        if (stringId == 0) {
            String name = writer.quoteString(fileName);
            writer.println("java.io.ObjectInputStream oin = null;");
            writer.println("int numStrings = 0;");
            writer.println("try {");
            writer.pushIndent();
            writer.println("java.io.FileInputStream fin = new java.io.FileInputStream("+name+");");
            writer.println("oin = new java.io.ObjectInputStream(fin);");
            writer.println("_jspx_html_data = (char[][]) oin.readObject();");
            writer.popIndent();
            writer.println("} catch (Exception ex) {");
            writer.pushIndent();
            writer.println("throw new org.apache.jasper.runtime.JspException(\"Unable to open data file\");");
            writer.popIndent();
            writer.println("} finally {");
            writer.pushIndent();
            writer.println("if (oin != null)");
            writer.pushIndent();
            writer.println("try { oin.close(); } catch (java.io.IOException ignore) { }");
            writer.popIndent();
            writer.popIndent();
            writer.println("}");
        }
    }

    private final void generatePrint(ServletWriter writer) {
        writer.println("out.print("+getStringVar()+");");
        vector.addElement(chars);
    }

    public void generate(ServletWriter writer, Class phase) {
        if (phase.equals(ClassDeclarationPhase.class))
            generateRef(writer);
        else if (phase.equals(InitMethodPhase.class))
            generateInit(writer);
        else if (phase.equals(ServiceMethodPhase.class))
            generatePrint(writer);
    }

    public boolean generateCoordinates(Class phase) {
        return false;
    }
}
