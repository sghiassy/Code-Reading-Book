/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/TagLibraryInfoImpl.java,v 1.23 2001/07/10 23:50:24 horwat Exp $
 * $Revision: 1.23 $
 * $Date: 2001/07/10 23:50:24 $
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

import java.net.URL;
import java.net.MalformedURLException;
import java.net.JarURLConnection;
import java.util.Iterator;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipEntry;
import java.util.jar.*;
import java.util.Enumeration;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.IOException;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Vector;
import java.util.Hashtable;
import java.util.Map;

import javax.servlet.jsp.tagext.TagLibraryInfo;
import javax.servlet.jsp.tagext.TagInfo;
import javax.servlet.jsp.tagext.TagAttributeInfo;
import javax.servlet.jsp.tagext.TagExtraInfo;
import javax.servlet.jsp.tagext.TagLibraryValidator;
import javax.servlet.jsp.tagext.PageData;
import javax.servlet.jsp.tagext.VariableInfo;
import javax.servlet.jsp.tagext.TagVariableInfo;
import javax.servlet.jsp.tagext.ValidationMessage;

import org.apache.jasper.JspCompilationContext;
import org.apache.jasper.JasperException;
import org.apache.jasper.Constants;

import org.apache.jasper.logging.Logger;

import org.apache.jasper.parser.ParserUtils;
import org.apache.jasper.parser.TreeNode;


/**
 * Implementation of the TagLibraryInfo class from the JSP spec. 
 *
 * @author Anil K. Vijendran
 * @author Mandar Raje
 * @author Pierre Delisle
 */
public class TagLibraryInfoImpl extends TagLibraryInfo {
    static private final String TAGLIB_TLD = "META-INF/taglib.tld";
    static private final String WEB_XML = "/WEB-INF/web.xml";

    Hashtable jarEntries;

    JspCompilationContext ctxt;

    

    private final void print(String name, String value, PrintWriter w) {
        if (value != null) {
            w.print(name+" = {\n\t");
            w.print(value);
            w.print("\n}\n");
        }
    }

    public String toString() {
        StringWriter sw = new StringWriter();
        PrintWriter out = new PrintWriter(sw);
        print("tlibversion", tlibversion, out);
        print("jspversion", jspversion, out);
        print("shortname", shortname, out);
        print("urn", urn, out);
        print("info", info, out);
        print("uri", uri, out);
        print("tagLibraryValidator", tagLibraryValidator.toString(), out);

        for(int i = 0; i < tags.length; i++)
            out.println(tags[i].toString());
        
        return sw.toString();
    }
    
    // XXX FIXME
    // resolveRelativeUri and/or getResourceAsStream don't seem to properly
    // handle relative paths when dealing when home and getDocBase are set
    // the following is a workaround until these problems are resolved.
    private InputStream getResourceAsStream(String uri) 
        throws FileNotFoundException 
    {
        try {
            // see if file exists on the filesystem first
            String real = ctxt.getRealPath(uri);
            if (real == null) {
                return ctxt.getResourceAsStream(uri);
            } else {
                return new FileInputStream(real);
            }
        }
        catch (FileNotFoundException ex) {
            // if file not found on filesystem, get the resource through
            // the context
            return ctxt.getResourceAsStream(uri);
        }
       
    }

    public TagLibraryInfoImpl(JspCompilationContext ctxt, String prefix, 
			      String uriIn) 
        throws JasperException
    {
	this(ctxt, prefix, uriIn, null);
    }

    public TagLibraryInfoImpl(JspCompilationContext ctxt, String prefix, 
			      String uriIn, String[] location) 
        throws JasperException
    {
        super(prefix, uriIn);

	this.ctxt = ctxt;
        ZipInputStream zin;
        InputStream in = null;
        URL url = null;
        boolean relativeURL = false;

	if (location == null) {
	    // The URI points to the TLD itself or to a jar
	    // file where the TLD is located
	    int uriType = TldLocationsCache.uriType(uri);
	    if (uriType == TldLocationsCache.ABS_URI) {
		throw new JasperException(
                    Constants.getString("jsp.error.taglibDirective.absUriCannotBeResolved",
					new Object[] {uri}));
	    } else if (uriType == 
		       TldLocationsCache.NOROOT_REL_URI) {
		uri = ctxt.resolveRelativeUri(uri);
	    }
	    location = new String[2];
	    location[0] = uri;
	    if (uri.endsWith("jar")) {
		location[1] = "META-INF/taglib.tld";
	    }
	}

        if (!location[0].endsWith("jar")) {
	    // Location points to TLD file
	    try {
		in = getResourceAsStream(location[0]);
		if (in == null) throw new FileNotFoundException(location[0]);
	    } catch (FileNotFoundException ex) {
		throw new JasperException(
                    Constants.getString("jsp.error.file.not.found",
					new Object[] {location[0]}));
	    }
	    // Now parse the tld.
	    parseTLD(ctxt, location[0], in);
	} else {
	    // Location points to a jar file
	    // tag library in jar file
	    JarFile jarFile = null;
	    ZipEntry jarEntry = null;
	    InputStream stream = null;
	    try {
		url = ctxt.getResource(location[0]);
		if (url == null) return;
		url = new URL("jar:" + url.toString() + "!/");
		JarURLConnection conn =
		    (JarURLConnection) url.openConnection();
		conn.connect(); //@@@ necessary???
		jarFile = conn.getJarFile();
		jarEntry = jarFile.getEntry(location[1]);
		stream = jarFile.getInputStream(jarEntry);
		parseTLD(ctxt, location[0], stream);
		// FIXME @@@
		// -- it seems that the JarURLConnection class caches JarFile 
		// objects for particular URLs, and there is no way to get 
		// it to release the cached entry, so
		// there's no way to redeploy from the same JAR file.  Wierd.
	    } catch (Exception ex) {
		Constants.message(
                    "jsp.error.taglib.jarFileException",
		    new Object[] {url.toString(), ex.getMessage()},
		    Logger.ERROR);
		if (stream != null) {
		    try {
			stream.close();
		    } catch (Throwable t) {}
		}
		if (jarFile != null) {
		    try {
			jarFile.close();
		    } catch (Throwable t) {}
		}
	    }
	}
    }
    
    /** Returns true if the given URI is relative in this web application, false if it is an internet URI.
     */
    private boolean isRelativeURI(String uri) {
        return (uri.indexOf(':') == -1);
    }
    
        
    private void parseTLD(JspCompilationContext ctxt,
                          String uri, InputStream in) 
        throws JasperException
    {

        Vector tagVector = new Vector();

        // Create an iterator over the child elements of our <taglib> element
        ClassLoader cl = ctxt.getClassLoader();
        ParserUtils pu = ParserUtils.createParserUtils(cl);
        TreeNode tld = pu.parseXMLDocument(uri, in);
        Iterator list = tld.findChildren();

        // Process each child element of our <taglib> element
        while (list.hasNext()) {

            TreeNode element = (TreeNode) list.next();
            String tname = element.getName();

            if ("tlibversion".equals(tname) ||          // JSP 1.1
                "tlib-version".equals(tname))           // JSP 1.2
                this.tlibversion = element.getBody();
            else if ("jspversion".equals(tname) ||
                     "jsp-version".equals(tname))
                this.jspversion = element.getBody();
            else if ("shortname".equals(tname) ||
                     "short-name".equals(tname))
                this.shortname = element.getBody();
            else if ("uri".equals(tname))
                this.urn = element.getBody();
            else if ("info".equals(tname) ||
                     "description".equals(tname))
                this.info = element.getBody();
            else if ("validator".equals(tname))
                this.tagLibraryValidator = createValidator(element);
            else if ("tag".equals(tname))
                tagVector.addElement(createTagInfo(element));
            else if ("display-name".equals(tname) ||    // Ignored elements
                     "small-icon".equals(tname) ||
                     "large-icon".equals(tname) ||
                     "listener".equals(tname))
                ;
            else {
                Constants.message("jsp.warning.unknown.element.in.TLD", 
                                  new Object[] {tname},
                                  Logger.WARNING
                                  );
            }

        }


        this.tags = new TagInfo[tagVector.size()];
        tagVector.copyInto (this.tags);
    }

    private TagInfo createTagInfo(TreeNode elem) throws JasperException {
        String name = null;
	String tagclass = null;
	String teiclass = null;
        String bodycontent = "JSP"; // Default body content is JSP
	String info = null;
	String displayName = null;
	String smallIcon = null;
	String largeIcon = null;
        
        Vector attributeVector = new Vector();
        Vector variableVector = new Vector();
        Iterator list = elem.findChildren();
        while (list.hasNext()) {
            TreeNode element = (TreeNode) list.next();
            String tname = element.getName();

            if ("name".equals(tname))
                name = element.getBody();
            else if ("tagclass".equals(tname) ||
                     "tag-class".equals(tname))
                tagclass = element.getBody();
            else if ("teiclass".equals(tname) ||
                     "tei-class".equals(tname))
                teiclass = element.getBody();
            else if ("bodycontent".equals(tname) ||
                     "body-content".equals(tname))
                bodycontent = element.getBody();
            else if ("display-name".equals(tname))
                displayName = element.getBody();
            else if ("small-icon".equals(tname))
                smallIcon = element.getBody();
            else if ("large-icon".equals(tname))
                largeIcon = element.getBody();
            else if ("info".equals(tname) ||
                     "description".equals(tname))
                info = element.getBody();
            else if ("variable".equals(tname)) {
                if (teiclass != null)
                    throw new JasperException
                        (Constants.getString("tld.error.variableNotAllowed"));
                variableVector.addElement(createVariable(element));
            } else if ("attribute".equals(tname))
                attributeVector.addElement(createAttribute(element));
            else if ("example".equals(tname) ||   // Ignored elements
		     false)
                ;
            else {
                Constants.message("jsp.warning.unknown.element.in.tag", 
                                  new Object[] {tname},
                                  Logger.WARNING
                                  );
	    }
	}
	TagAttributeInfo[] tagAttributeInfo 
            = new TagAttributeInfo[attributeVector.size()];
	attributeVector.copyInto (tagAttributeInfo);

	TagVariableInfo[] tagVariableInfos
            = new TagVariableInfo[variableVector.size()];
	variableVector.copyInto(tagVariableInfos);


        TagExtraInfo tei = null;

        if (teiclass != null && !teiclass.equals(""))
            try {
                Class teiClass = ctxt.getClassLoader().loadClass(teiclass);
                tei = (TagExtraInfo) teiClass.newInstance();
	    } catch (ClassNotFoundException cex) {
                Constants.message("jsp.warning.teiclass.is.null",
                                  new Object[] {
                                      teiclass, cex.getMessage()
                                  },
                                  Logger.WARNING
                                  );
            } catch (IllegalAccessException iae) {
                Constants.message("jsp.warning.teiclass.is.null",
                                  new Object[] {
                                      teiclass, iae.getMessage()
                                  },
                                  Logger.WARNING
                                  );
            } catch (InstantiationException ie) {
                Constants.message("jsp.warning.teiclass.is.null",
                                  new Object[] {
                                      teiclass, ie.getMessage()
                                  },
                                  Logger.WARNING
                                  );
            }

        TagInfo taginfo = new TagInfo(name, tagclass, bodycontent,
                                      info, this, 
                                      tei,
                                      tagAttributeInfo,
				      displayName,
				      smallIcon,
				      largeIcon,
				      tagVariableInfos);
        return taginfo;
    }

    TagAttributeInfo createAttribute(TreeNode elem) {
        //        p("createAttribute\n" + elem);
        String name = null;
        boolean required = false, rtexprvalue = false, reqTime = false;
        String type = null;
        
        Iterator list = elem.findChildren();
        while (list.hasNext()) {
            TreeNode element = (TreeNode) list.next();
            String tname = element.getName();

            if ("name".equals(tname))
                name = element.getBody();
            else if ("required".equals(tname)) {
                String s = element.getBody();
                if (s != null)
                    required = JspUtil.booleanValue(s);
            } else if ("rtexprvalue".equals(tname)) {
                String s = element.getBody();
                if (s != null)
                    rtexprvalue = JspUtil.booleanValue(s);
            } else if ("type".equals(tname))
                type = element.getBody();
            else if ("description".equals(tname) ||    // Ignored elements
		     false ) 
	      ;
            else {
                Constants.message("jsp.warning.unknown.element.in.attribute", 
                                  new Object[] {tname},
                                  Logger.WARNING
                                  );
            }
        }
        
        // p("name=" + name + ", required=" + required + ", type=" + type +
        //   ", rtexprvalue=" + rtexprvalue);
        return new TagAttributeInfo(name, required, type, rtexprvalue);
    }

    TagVariableInfo createVariable(TreeNode elem) {
        String nameGiven = null;
        String nameFromAttribute = null;
	String className = null;
	boolean declare = true;
	int scope = VariableInfo.NESTED;
        
        Iterator list = elem.findChildren();
        while (list.hasNext()) {
            TreeNode element = (TreeNode) list.next();
            String tname = element.getName();
            if ("name-given".equals(tname))
                nameGiven = element.getBody();
            else if ("name-from-attribute".equals(tname))
                nameFromAttribute = element.getBody();
            else if ("variable-class".equals(tname))
                className = element.getBody();
            else if ("declare".equals(tname)) {
                String s = element.getBody();
                if (s != null)
                    declare = JspUtil.booleanValue(s);
            } else if ("scope".equals(tname)) {
                String s = element.getBody();
                if (s != null) {
		    if ("NESTED".equals(s)) {
			scope = VariableInfo.NESTED;
		    } else if ("AT_BEGIN".equals(s)) {
			scope = VariableInfo.AT_BEGIN;
		    } else if ("AT_END".equals(s)) {
			scope = VariableInfo.AT_END;
		    }
		}
	    }
            else if ("description".equals(tname) ||    // Ignored elements
		     false ) {
            } else {
                Constants.message("jsp.warning.unknown.element.in.variable",
                                  new Object[] {tname},
                                  Logger.WARNING);
	    }
        }
        return new TagVariableInfo(nameGiven, nameFromAttribute,
				   className, declare, scope);
    }

    private TagLibraryValidator createValidator(TreeNode elem) {
        String validatorClass = null;
	Map initParams = new Hashtable();

        Iterator list = elem.findChildren();
        while (list.hasNext()) {
            TreeNode element = (TreeNode) list.next();
            String tname = element.getName();
            if ("validator-class".equals(tname))
                validatorClass = element.getBody();
            else if ("init-param".equals(tname)) {
		String[] initParam = createInitParam(element);
		initParams.put(initParam[0], initParam[1]);
            } else if ("description".equals(tname) ||    // Ignored elements
		     false ) {
            } else {
                Constants.message("jsp.warning.unknown.element.in.validator", //@@@ add in properties
                                  new Object[] {tname},
                                  Logger.WARNING);
	    }
        }

        TagLibraryValidator tlv = null;
        if (validatorClass != null && !validatorClass.equals("")) {
            try {
                Class tlvClass = 
		    ctxt.getClassLoader().loadClass(validatorClass);
                tlv = (TagLibraryValidator)tlvClass.newInstance();
		//p("got validator class: " + tlv);
            } catch (Exception ex) {
                Constants.message("jsp.warning.tlvclass.is.null",
				  new Object[] {
				      validatorClass, 
				      "EXCEPTION: " + ex.getMessage()
				  },
				  Logger.ERROR);
            }
        }
	if (tlv != null) {
	    tlv.setInitParameters(initParams);
	}
	return tlv;
    }

    String[] createInitParam(TreeNode elem) {
        String[] initParam = new String[2];
        
        Iterator list = elem.findChildren();
        while (list.hasNext()) {
            TreeNode element = (TreeNode) list.next();
            String tname = element.getName();
            if ("param-name".equals(tname))
                initParam[0] = element.getBody();
            else if ("param-value".equals(tname))
                initParam[1] = element.getBody();
            else if ("description".equals(tname))
                ; // Do nothing
            else {
                Constants.message("jsp.warning.unknown.element.in.initParam", //@@@ properties
                                  new Object[] {tname},
                                  Logger.WARNING);
	    }
        }
	return initParam;
    }

    static void copy(InputStream in, String fileName) 
        throws IOException, FileNotFoundException 
    {
        byte[] buf = new byte[1024];

        FileOutputStream out = new FileOutputStream(fileName);
        int nRead;
        while ((nRead = in.read(buf, 0, buf.length)) != -1)
            out.write(buf, 0, nRead);
    }

    //*********************************************************************
    // Until javax.servlet.jsp.tagext.TagLibraryInfo is fixed

    /**
     * The instance (if any) for the TagLibraryValidator class.
     * 
     * @return The TagLibraryValidator instance, if any.
     */
    public TagLibraryValidator getTagLibraryValidator() {
	return tagLibraryValidator;
    }

    /**
     * Translation-time validation of the XML docu-ment
     * associated with the JSP page.
     * This is a convenience method on the associated 
     * TagLibraryValidator class.
     *
     * @param thePage The JSP page object
     * @return A string indicating whether the page is valid or not.
     */
    public ValidationMessage[] validate(PageData thePage) {
	TagLibraryValidator tlv = getTagLibraryValidator();
	if (tlv == null) return null;
	return tlv.validate(getPrefixString(), getURI(), thePage);
    }

    protected TagLibraryValidator tagLibraryValidator; 

    private void p(String s) {
	System.out.println("[TagLibraryInfoImpl] " + s);
    }
}
