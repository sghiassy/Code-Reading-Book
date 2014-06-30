/*
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

import java.io.*;
import java.util.*;

import javax.servlet.jsp.tagext.*;

import org.xml.sax.InputSource;
import org.xml.sax.Attributes;

import org.apache.jasper.*;

import org.apache.jasper.logging.Logger;

/**
 * Controller for the parsing of a JSP page.
 * <p>
 * A translation unit (JSP source file and any files included via the
 * include directive) may involve the processing of JSP pages
 * written with different syntaxes (currently the original JSP syntax,
 * as well as the XML syntax (as of JSP 1.2)). This class encapsulates 
 * the behavior related to the selection and invocation of 
 * the proper parser.
 *
 * Note: There's some 'commented out' code that would allow
 * the dynamic generation of a DTD so we could use a validating
 * parser. Should be working, but would like to investigate
 * XML schema before using validating parser.
 *
 * @author Pierre Delisle
 */
public class ParserController {
    /*
     * The compilation context
     */
    private JspCompilationContext ctxt;

    /*
     * A stack to keep track of the 'current base directory'
     * for include directives that refer to relative paths.
     */
    private Stack baseDirStack = new Stack();

    /*
     * The parse 'event handler', shared by both
     * types of parsers.
     */
    private ParseEventListener jspHandler = null;

    /*
     * Document information which tells us what
     * kind of document we are dealing with.
     */
    private boolean isXml;
    
    /*
     * Static information used in the process of figuring out
     * the kind of document we're dealing with.
     */
    private static final String JSP_ROOT_TAG   = "<jsp:root";

    /*
     * Tells if the file being processed is the "top" file
     * in the translation unit.
     */
    private boolean isTopFile = true;

    /*
     * The encoding of the "top" file. This encoding is used
     * for included files by default.
     * Defaults to "ISO-8859-1" per JSP spec.
     */
    private String topFileEncoding = "ISO-8859-1"; 
    
    /*
     * The 'new' encoding required to read a page.
     */
    private String newEncoding;

    /*
     * Information to allow us to dynamically generate the
     * DTD to be used to validate the XML document.
     * @@@[This has been pulled out for the time being.
     * We only support non-validating SAX parser. Might want
     * to put it back eventually, or use XML Schema]
     */
    /* NOT COMPILED
    static final String JSP_TAGLIB_TAG = "jsp:directive.taglib";
    boolean hasTaglib;  // does xml document have tag lib(s)?
    InputSource dtdInputSource;
    static String jspDtd_part1 = null;
    static String jspDtd_part2 = null;
    */

    //*********************************************************************
    // Constructor

    public ParserController(JspCompilationContext ctxt) {
        this.ctxt = ctxt; // @@@ can we assert that ctxt is not null?
        jspHandler = new JspParseEventListener(ctxt, this);
	
	/* @@@ NOT COMPILED
	// Cache the content of the jsp DTD
	if (jspDtd_part1 == null) {
	    jspDtd_part1 = getFileContent("/org/apache/jasper/resources/jsp12_part1.dtd");
	    jspDtd_part2 = getFileContent("/org/apache/jasper/resources/jsp12_part2.dtd");
	}
        */
    }
   
    //*********************************************************************
    // Accessors

    public ParseEventListener getParseEventListener() {
	return jspHandler;
    }

    //*********************************************************************
    // Parse

    /**
     * Parse the jsp page provided as an argument.
     * First invoked by the compiler, then invoked recursively by the
     * parser event handler for each 'include' directive.
     *
     * @param The name of the jsp file to be parsed.
     */
    public void parse(String inFileName)
	throws FileNotFoundException, JasperException
    {
        //p("parse(" + inFileName + ")");

        String absFileName = resolveFileName(inFileName);
	//p("filePath: " + filePath);
	String encoding = topFileEncoding;
        InputStreamReader reader = null;
        try {
            // Figure out what type of JSP document we are dealing with
            reader = getReader(absFileName, encoding);
            figureOutJspDocument(absFileName, encoding, reader);
            //p("isXml = " + isXml + "   hasTaglib = " + hasTaglib);
	    encoding = (newEncoding!=null) ? newEncoding : encoding;
	    if (isTopFile) {
		// Set the "top level" file encoding that will be used
		// for all included files where encoding is not defined.
		topFileEncoding = encoding;
		isTopFile = false;
	    }
	    try {
		reader.close();
	    } catch (IOException ex) {}

            // dispatch to the proper parser
	    
            reader = getReader(absFileName, encoding);
            if (isXml) {
                (new ParserXJspSax(absFileName, reader, jspHandler)).parse();
            } else {
                (new Parser(ctxt, absFileName, encoding, reader, jspHandler)).parse();
            }
	    baseDirStack.pop();
        } finally {
            if (reader != null) {
                try {
                    reader.close();
		} catch (Exception any) {}
	    }
        }
    }

    //*********************************************************************
    // Figure out input Document

    private void figureOutJspDocument(String file, 
				      String encoding,
				      InputStreamReader reader)
	 throws JasperException
    {
	JspReader jspReader;
	try {
	    jspReader = new JspReader(ctxt, file, encoding, reader);
	} catch (FileNotFoundException ex) {
	    throw new JasperException(ex);
	}
        jspReader.setSingleFile(true);
        Mark startMark = jspReader.mark();

	// Check for the jsp:root tag
	// No check for xml prolog, since nothing prevents a page
	// to output XML and still use JSP syntax.
	jspReader.reset(startMark);
	Mark mark = jspReader.skipUntil(JSP_ROOT_TAG);
	if (mark != null) {
	    isXml = true;
	} else {
	    isXml = false;
	}

	newEncoding = null;

	// Figure out the encoding of the page
	// FIXME: We assume xml parser will take care of
        // encoding for page in XML syntax. Correct?
	if (!isXml) {
	    jspReader.reset(startMark);
	    while (jspReader.skipUntil("<%@") != null) {
		jspReader.skipSpaces();
		if (jspReader.matches("page")) {
		    jspReader.advance(4);
		    jspReader.skipSpaces();
		    Attributes attrs = jspReader.parseTagAttributes();
		    String attribute = "pageEncoding";
		    newEncoding = attrs.getValue("pageEncoding");
		    if (newEncoding == null) {
			String contentType = attrs.getValue("contentType");
			if (contentType != null) {
			    int loc = contentType.indexOf("charset=");
			    if (loc != -1) {
				newEncoding = contentType.substring(loc+8);
				return;
			    }
			}
		    } else {
			return;
		    }
		}
	    }
	}

	/* NOT COMPILED
        // This is an XML document. Let's see if it uses tag libraries.
        jspReader.reset(startMark);
        Vector taglibMarks = new Vector();
        mark = jspReader.skipUntil(JSP_TAGLIB_TAG);
        while (mark != null) {
            taglibMarks.add(mark);
            mark = jspReader.skipUntil(JSP_TAGLIB_TAG);
        }
        hasTaglib = (taglibMarks.size() > 0);
        if (!hasTaglib) return;

        // The JSP document uses tag libraries. We parse the tag libraries and then
        // we will generate the JSP DTD on the fly to include the custom tags.
        // This way, we can use a validating xml parser.
	dtdInputSource = buildDtd(jspReader, taglibMarks);
	*/
    }
    
    /* NOT COMPILED 
    private InputSource buildDtd(JspReader jspReader, Vector taglibMarks) 
	throws JasperException
    {
        TagLibraries libraries = new TagLibraries(null);

        Enumeration enum1 = taglibMarks.elements();
        while (enum1.hasMoreElements()) {
            Mark mark = (Mark)enum1.nextElement();
            jspReader.reset(mark);
            jspReader.advance(JSP_TAGLIB_TAG.length());
            Hashtable attrs = jspReader.parseTagAttributes();
            p("taglib attributes are: " + attrs);

            JspUtil.checkAttributes("Taglib directive", attrs,
            ParserJsp.Directive.tagDvalidAttrs, mark);
            String uri = (String) attrs.get("uri");
            String prefix = (String) attrs.get("prefix");
            p("uri = " + uri + "   prefix = " + prefix);
            try {
                TagLibraryInfo tli = new TagLibraryInfoImpl(ctxt, prefix, uri);
                libraries.addTagLibrary(prefix, tli);
            } catch (Exception ex) {
	        // @@@
                ex.printStackTrace();
                Object[] args = new Object[] { uri, ex.getMessage() };
                throw new CompileException(mark,
                Constants.getString("jsp.error.badtaglib",
                args));
            }
        }
        
        // Build a JSP DTD on the fly that will include the custom tag
        // ibraries.
        
        ByteArrayOutputStream baos = new ByteArrayOutputStream();        
	StringBuffer part1Buf = new StringBuffer();
	StringBuffer bufXmlns = new StringBuffer();
	StringBuffer bufTags = new StringBuffer();

        part1Buf.append(jspDtd_part1);

        Enumeration enum2 = libraries.getTagLibInfos();
        while (enum2.hasMoreElements()) {
            TagLibraryInfo tli = (TagLibraryInfo)enum2.nextElement();
	    bufXmlns.append("\nxmlns:").append(tli.getPrefixString()).append(" CDATA #FIXED \"");
            bufXmlns.append(tli.getURI()).append("\"");
            TagInfo[] tags = tli.getTags();
            for (int i=0; i<tags.length; i++) {
		TagInfo tag = tags[i];
                part1Buf.append("\n|").append(tli.getPrefixString()).append(":").append(tag.getTagName());
                part1Buf.append("\n");
                bufTags.append("\n<!ELEMENT ");
                bufTags.append(tli.getPrefixString());
                bufTags.append(":");
		bufTags.append(tag.getTagName());
		bufTags.append(' ');
		if (TagInfo.BODY_CONTENT_EMPTY.equalsIgnoreCase(tag.getBodyContent())) {
		    bufTags.append("EMPTY>");
		} else {
		    bufTags.append("#PCDATA>");
		}
		TagAttributeInfo[] tais = tag.getAttributes();
		if (tais.length > 0) {
		    bufTags.append("\n<!ATTLIST ");
		    bufTags.append(tli.getPrefixString());
		    bufTags.append(":");
		    bufTags.append("tag.getTagName()");
		    bufTags.append(' ');		    
		    for (int j=0; j<tais.length; j++) {
			TagAttributeInfo tai = tais[j];
			bufTags.append('\n'); //@@@ newline
			bufTags.append(tai.getName());
			if (tai.canBeRequestTime()) {
			    bufTags.append(" PCDATA ");
			} else {
			    bufTags.append(" CDATA ");
			}
			if (tai.isRequired()) {
			    bufTags.append("#REQUIRED");
			} else {
			    bufTags.append("#IMPLIED");
			}
		    }
		}
	    }
	}

        part1Buf.append(jspDtd_part2);
        part1Buf.append(bufXmlns).append(">\n");
        part1Buf.append(bufTags);

        p("---------ZE DTD:-----------");
        p(part1Buf.toString());
	p("ZE DTD:----END-------");
        
        CharArrayReader car = new CharArrayReader(part1Buf.toString().toCharArray());
        return new InputSource(car);
    }
    */

    //*********************************************************************
    // Utility methods

    /*
     * Resolve the name of the file and update
     * baseDirStack() to keep track ot the current
     * base directory for each included file.
     * The 'root' file is always an 'absolute' path,
     * so no need to put an initial value in the
     * baseDirStack.
     */
    private String resolveFileName(String inFileName) {
        String fileName = inFileName.replace('\\', '/');
        boolean isAbsolute = fileName.startsWith("/");
	fileName = isAbsolute ? fileName 
            : (String) baseDirStack.peek() + fileName;
	String baseDir = 
	    fileName.substring(0, fileName.lastIndexOf("/") + 1);
	baseDirStack.push(baseDir);
	return fileName;
    }

    private InputStreamReader getReader(String file, String encoding)
	throws FileNotFoundException, JasperException
    {
        InputStream in;
        InputStreamReader reader;

	try {
            in = ctxt.getResourceAsStream(file);
            if (in == null) {
                throw new FileNotFoundException(file);
            }
            return new InputStreamReader(in, encoding);
	} catch (UnsupportedEncodingException ex) {
	    throw new JasperException(
                Constants.getString("jsp.error.unsupported.encoding",
				    new Object[]{encoding}));
	}
    }

    /* NOT COMPILED
    private String getFileContent(String fileName) {
        InputStream inStream;
        StringBuffer sb = new StringBuffer();
        inStream = getClass().getResourceAsStream(fileName);
        InputStreamReader in = new InputStreamReader(inStream);
        char[] buf = new char[1024];
        try {
            for (int i=0 ; (i=in.read(buf)) != -1 ;) {
                  sb.append(buf, 0 , i);
            }
            return sb.toString().trim();
        } catch (IOException ex) {
            p("IOException on jsp12.dtd");
            ex.printStackTrace();
            return null;
        }
    }
    */

    private void p(String s) {
        System.out.println("[ParserController] " + s);
    }

    private void p(String s, Throwable ex) {
        p(s);
        p(ex.getMessage());
        ex.printStackTrace();
    }
}
