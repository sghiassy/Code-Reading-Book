/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/Compiler.java,v 1.12 2001/09/15 23:47:17 craigmcc Exp $
 * $Revision: 1.12 $
 * $Date: 2001/09/15 23:47:17 $
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
import java.io.FileNotFoundException;
import java.io.File;
import java.io.PrintWriter;
import java.io.ByteArrayOutputStream;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.BufferedReader;
import java.io.StringReader;
import java.io.IOException;

import org.apache.jasper.JspCompilationContext;
import org.apache.jasper.Constants;
import org.apache.jasper.JasperException;
import org.apache.jasper.compiler.ParseException;

import org.apache.jasper.logging.Logger;

import org.xml.sax.Attributes;

/**
 * If you want to customize JSP compilation aspects, this class is
 * something you should take a look at. 
 * 
 * Hope is that people can just extend Compiler and override things
 * like isOutDated() but inherit things like compile(). This might
 * change. 
 *
 * @author Anil K. Vijendran
 * @author Mandar Raje
 * @author Pierre Delisle
 */
public class Compiler {
    protected JavaCompiler javac;
    protected Mangler mangler;
    protected JspCompilationContext ctxt;

    public Compiler(JspCompilationContext ctxt) {
        this.ctxt = ctxt;
    }
    
    /** 
     * Compile the jsp file from the current engine context
     *
     * @return true if the class file was outdated the jsp file
     *         was recompiled. 
     */
    public boolean compile()
        throws FileNotFoundException, JasperException, Exception 
    {
        String classFileName = mangler.getClassFileName();

        String className = mangler.getClassName();
        ctxt.setServletClassName(className);
        Constants.message("jsp.message.class_name_is",
                          new Object[] { className },
                          Logger.DEBUG);

	if (!isOutDated())
            return false;

        String javaFileName = mangler.getJavaFileName();
        ctxt.setServletJavaFileName(javaFileName);

        Constants.message("jsp.message.java_file_name_is",
                          new Object[] { javaFileName },
                          Logger.DEBUG);

        // Setup the ServletWriter
	// We try UTF8 by default. If it fails, we use the java encoding 
	// specified for JspServlet init parameter "javaEncoding".

	String javaEncoding = "UTF8"; 
	OutputStreamWriter osw; 
	try {
	    osw = new OutputStreamWriter(
		      new FileOutputStream(javaFileName),javaEncoding);
	} catch (java.io.UnsupportedEncodingException ex) {
	    // Try to get the java encoding from the "javaEncoding"
	    // init parameter for JspServlet.
	    javaEncoding = ctxt.getOptions().getJavaEncoding();
	    if (javaEncoding != null) {
		try {
		    osw = new OutputStreamWriter(
			      new FileOutputStream(javaFileName),javaEncoding);
		} catch (java.io.UnsupportedEncodingException ex2) {
		    // no luck :-(
		    throw new JasperException(
			Constants.getString("jsp.error.invalid.javaEncoding",
					    new Object[] { 
						"UTF8", 
						javaEncoding,
					    }));
		}
	    } else {
		throw new JasperException(
		    Constants.getString("jsp.error.needAlternateJavaEncoding",
					new Object[] { "UTF8" }));		
	    }
	}
	ServletWriter writer = new ServletWriter(new PrintWriter(osw));
        ctxt.setWriter(writer);

	/* NOT COMPILED -- remove eventually -- kept for temp reference
	   @@@ ParserController still needs to handle jspEncoding
	       for JSP syntax files
        // Need the encoding specified in the JSP 'page' directive for
        //  - reading the JSP page
        //  - writing the JSP servlet source
        //  - compiling the generated servlets (pass -encoding to javac).
        // XXX - There are really three encodings of interest.

        String jspEncoding = "8859_1";          // default per JSP spec

	// This seems to be a reasonable point to scan the JSP file
	// for a 'contentType' directive. If it found then the set
	// the value of 'jspEncoding to reflect the value specified.
	// Note: if (true) is convenience programming. It can be
	// taken out once we have a more efficient method.

	if (true) {
	    JspReader tmpReader = JspReader.createJspReader(
							    ctxt.getJspFile(),
							    ctxt,
							    jspEncoding);
	    String newEncode = changeEncodingIfNecessary(tmpReader);
	    if (newEncode != null) jspEncoding = newEncode;
	}

        JspReader reader = JspReader.createJspReader(
            ctxt.getJspFile(),
            ctxt,
            jspEncoding
        );
        ctxt.setReader(reader);
        ParseEventListener listener = new JspParseEventListener(ctxt);
	*/

	/* NOT COMPILED
        Parser p = new Parser(reader, listener);
        listener.beginPageProcessing();
        p.parse();
        listener.endPageProcessing();
	*/

	// Parse the file
	ParserController parserCtl = new ParserController(ctxt);
	parserCtl.parse(ctxt.getJspFile());

        // Generate the servlet
	ParseEventListener listener = parserCtl.getParseEventListener();
        listener.beginPageProcessing();
        listener.endPageProcessing();
        writer.close();
	// An XML input stream has been produced and can be validated
	// by TagLibraryValidator classes 
	((JspParseEventListener)listener).validate();

        String classpath = ctxt.getClassPath(); 

        // I'm nuking
        //          System.getProperty("jsp.class.path", ".") 
        // business. If anyone badly needs this we can talk. -akv

        String sep = System.getProperty("path.separator");
        String[] argv = new String[] 
        {
            "-encoding",
            javaEncoding,
            "-classpath",
            System.getProperty("java.class.path")+ sep + classpath,
            "-d", ctxt.getOutputDir(),
            javaFileName
        };

        StringBuffer b = new StringBuffer();
        for(int i = 0; i < argv.length; i++) {
            b.append(argv[i]);
            b.append(" ");
        }

        Constants.message("jsp.message.compiling_with",
                          new Object[] { b.toString() },
                          Logger.DEBUG);

        /**
         * 256 chosen randomly. The default is 32 if you don't pass
         * anything to the constructor which will be less. 
         */
        ByteArrayOutputStream out = new ByteArrayOutputStream (256);

        // if no compiler was set we can kick out now

        if (javac == null) {
            return true;
        }

        /**
         * Configure the compiler object
         */
        javac.setEncoding(javaEncoding);
        javac.setClasspath( System.getProperty("java.class.path")+ sep + classpath);
        javac.setOutputDir(ctxt.getJavacOutputDir());
        javac.setMsgOutput(out);

        /**
         * Execute the compiler
         */
        boolean status = javac.compile(javaFileName);

        if (!ctxt.keepGenerated()) {
            File javaFile = new File(javaFileName);
            javaFile.delete();
        }
    
        if (status == false) {
            String msg = out.toString ();
            msg = getJspLineErrors(msg, writer.getLineMap());

            //FIXME!!
            //Using an exception as flow control is not a good idea. Refactor me!
            throw new JasperException(Constants.getString("jsp.error.unable.compile")
                                      + msg);
        }

        return true;
    }

    /**
     * Parse compiler error message. Get line number. Use JspLineMap object to
     * find the corresponding line in the jsp file. Create new error message with
     * jsp line information.
     */
    private String getJspLineErrors(String msg, JspLineMap map) throws IOException {

        if (map == null) return msg;

        //System.out.println(map.toString());

        StringBuffer errorMsg = new StringBuffer();
        BufferedReader br = new BufferedReader(new StringReader(msg));
        String line;

        while (true) {
            line=br.readLine();
            if (line==null) break;

            // line number is between a set of colons
            int beginColon=line.indexOf(':', 2); // Drive letter on Windows !!
            int endColon=line.indexOf(':', beginColon+1);

            if (beginColon<0 || endColon<0) {
                errorMsg.append(line);
                errorMsg.append('\n');
                continue;
            }

            String nr = line.substring(beginColon+1, endColon);
            int lineNr = Integer.parseInt( nr );

            //System.out.println("lineNr: " + lineNr);

            // Now do the mapping
            String mapping = findMapping(map, lineNr);
            if (mapping != null) {
                errorMsg.append(findMapping(map, lineNr));
                errorMsg.append(line);
                errorMsg.append('\n');
            }
        }
        br.close();
        map.clear();

        return errorMsg.toString();
    }

    /**
     * Find map item that corresponds to the specified line number
     */
    private String findMapping(JspLineMap map, int lineNr) {

        for (int i=map.size() - 1; i >= 0; i--) {
            JspLineMapItem mapItem = (JspLineMapItem) map.get(i);

            if (mapItem == null) continue;

            if (mapItem.getBeginServletLnr() <= lineNr &&
                mapItem.getEndServletLnr() >= lineNr) {
                return createErrorMsg(map, mapItem);
            }
        }
        return null;
    }

    /**
     * Create error message including the jsp line numbers and file name
     */
    private String createErrorMsg(JspLineMap map, JspLineMapItem mapping) {
        StringBuffer msg = new StringBuffer();

        if (mapping.getBeginJspLnr() == mapping.getEndJspLnr()) {
            msg.append(Constants.getString("jsp.error.single.line.number",
                       new Object[] {
                           new Integer(mapping.getBeginJspLnr()), 
                           map.getFileName(mapping.getStartJspFileNr())
                       }));
        }
        else {
            msg.append(Constants.getString("jsp.error.multiple.line.number",
                       new Object[] { 
                           new Integer(mapping.getBeginJspLnr()), 
                           new Integer(mapping.getEndJspLnr()), 
                           map.getFileName(mapping.getStartJspFileNr())
                       }));
        }

        msg.append(Constants.getString("jsp.error.corresponding.servlet"));

        return msg.toString();
    }

    /**
     * This is a protected method intended to be overridden by 
     * subclasses of Compiler. This is used by the compile method
     * to do all the compilation. 
     */
    public boolean isOutDated() {
	return true;
    }
    
    /**
     * Set java compiler info
     */
    public void setJavaCompiler(JavaCompiler javac) {
        this.javac = javac;
    }

    /**
     * Set Mangler which will be used as part of compile().
     */
    public void setMangler(Mangler mangler) {
        this.mangler = mangler;
    }

    /**
     * Change the encoding for the reader if specified.
     */
    public String changeEncodingIfNecessary(JspReader tmpReader)
    throws ParseException {

	// A lot of code replicated from Parser.java
	// Main aim is to "get-it-to-work".
	while (tmpReader.skipUntil("<%@") != null) {

	    tmpReader.skipSpaces();

	    // check if it is a page directive.
	    if (tmpReader.matches("page")) {

		tmpReader.advance(4);
		tmpReader.skipSpaces();
		
		try {
		    Attributes attrs = tmpReader.parseTagAttributes();
		    String ct = attrs.getValue("contentType");
		    if (ct != null) {
			int loc = ct.indexOf("charset=");
			if (loc > 0) {
			    String encoding = ct.substring(loc + 8);
			    return encoding;
			}
		    }
		} catch (ParseException ex) {
		    // Ignore the exception here, it will be caught later.
		    return null;
		}
	    }
	}
	return null;
    }

    /**
     * Remove generated files
     */
    public void removeGeneratedFiles() {
        try {
            // XXX Should we delete the generated .java file too?
            String classFileName = mangler.getClassFileName();
            if (classFileName != null) {
                File classFile = new File(classFileName);
                classFile.delete();
            }
        } catch (Exception e) {
            //Remove as much as possible, ignore possible exceptions
        }
    }
}
