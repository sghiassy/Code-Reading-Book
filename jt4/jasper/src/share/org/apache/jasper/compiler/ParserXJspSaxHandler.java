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

//@@@ have specific imports
import java.io.*;
import java.net.*;
import java.util.*;

import javax.servlet.jsp.tagext.TagLibraryInfo;
import javax.servlet.jsp.tagext.TagInfo;

import org.xml.sax.*;
import org.xml.sax.ext.*;
import org.xml.sax.helpers.*;

import org.apache.jasper.*;

/**
 * The SAX (2.0) parser event handler.
 *
 * @author Pierre Delisle
 * @author Danno Ferrin
 *
 * @@@ TODO
 * - make sure validation is in sync with Parser (jsp syntax)
 */
class ParserXJspSaxHandler 
    extends DefaultHandler implements LexicalHandler 
{
    /*
     * JSP handler on which we piggyback to generate parse data
     */
    private ParseEventListener jspHandler;

    /*
     * Tells whether we are in a DTD
     */
    private boolean inDtd;

    /*
     * Tells whether we are in CDATA
     */
    private boolean inCdata;

    /*
     * Tells whether we are in Entity
     */
    private boolean inEntity;

    /*
     * parsing stack
     */
    private Stack stack;

    /*
     * Locator for sax parse events
     */
    private Locator locator;

    /*
     * File path that's used in parsing errors
     */
    private String filePath;

    //*********************************************************************
    // Constructor

    public ParserXJspSaxHandler(String filePath, 
				ParseEventListener jspHandler) 
	throws JasperException 
    {
	this.filePath = filePath;
        this.jspHandler = jspHandler;

        stack = new Stack();
        inEntity = false;
        inDtd = false;
        inCdata = false;
    }

    //*********************************************************************
    // Interface ContentHandler methods

    public void setDocumentLocator(Locator locator) {
	//p("\n*** setDocumentLocator");
	this.locator = locator;
    }

    public void startDocument() throws SAXException {
	//p("\n*** startDocument");
    }

    public void endDocument() throws SAXException {
	//p("\n*** endDocument");
    }

    public void startPrefixMapping(String prefix, String uri) 
	throws SAXException 
    {
	//p("\n*** startPrefixMapping");
	//p("prefix = " + prefix);
	//p("uri = " + uri);
	//p("BEWARE: Nothing done for this content.");
    }

    public void endPrefixMapping(String prefix)
	throws SAXException 
    {
	//p("\n*** endPrefixMapping");
	//p("prefix = " + prefix);
	//p("BEWARE: Nothing done for this content.");
    }

    public void characters(char[] ch, int start, int length)
        throws SAXException
    {
	//p("\n*** characters");
	//printLocation();
	//p("|" + String.valueOf(ch, start, length) + "|");

	Node node = (Node)stack.peek();

	// characters added to the body content of the element
	// at the top of the stack
	//p("these chars are added to node " + node.rawName + " in stack");
	node.addText(ch, start, length);
    }

    public void ignorableWhitespace(char[] ch,
				    int start,
				    int length)
	throws SAXException
    {
	//p("\n*** ignorableWhitespace");
	//printLocation();
	//p("|" + String.valueOf(ch, start, length) + "|");
	//p("BEWARE: Nothing done for this content.");
    }

    public void skippedEntity(String name)
	throws SAXException
    {
	//p("\n*** skippedEntity");
	//p("name = " + name);
	//p("BEWARE: Nothing done for this content.");
    }


    public void processingInstruction(String target, String data)
        throws SAXException 
    {
	//p("\n*** processingInstruction");
	//p("target = " + target);
	//p("data = " + data);
	//p("BEWARE: Nothing done for this content.");
    }
    
    public void startElement(String namespaceURI, String localName,
                             String rawName, Attributes attrs) 
	throws SAXException 
    {
	//p("\n*** startElement: " + rawName);
	//printLocation();
	//p("namespaceURI: " + namespaceURI);
	//p("localName: " + localName);
	//p("rawName: " + rawName);
	//p("attrs: " + attrs.toString());

	String name = rawName;
	Mark start = new Mark(filePath, locator.getLineNumber(),
			      locator.getColumnNumber());
	Node node = new Node(rawName, new AttributesImpl(attrs), start);

	if (name.equals("jsp:params")) {
	    // don't have it pushed on the stack...
	    // this is a noop...
	    return;
	}

        // If previous node is a custom tag, call its
        // begin tag handler
        Node prevNode = null;
        if (!stack.empty()) {
            prevNode = (Node) stack.peek();
            if (prevNode instanceof NodeTag) {
                try {
                    processCustomTagBeginDoIt((NodeTag) prevNode, true);
                } catch (Exception ex) {
                    throw new SAXException(ex);
                }
            }
        }

        // Simply push the new 'node' on the stack, and it will be
	// processed on the 'endElement' event.
	stack.push(node);

	// Those elements allow more 'parsed elements' in them.
	// They are therefore handled both at the start and at the
	// end.
	try {
	    if (name.equals("jsp:root")) {
		jspHandler.handleRootBegin(node.attrs);
	    } else if (name.equals("jsp:useBean")) {
		jspHandler.handleBean(node.start, node.start, node.attrs, true);
	    } else if (name.length()<4 || 
		       !name.substring(0,4).equals("jsp:")) {
		// custom tag or 'uninterpreted' tag
		int i = name.indexOf(':');
		boolean isCustomTag = false;
		if (i != -1) {
		    String prefix = name.substring(0, i);
		    String shortTagName = name.substring(i+1);
		    TagLibraries libraries = jspHandler.getTagLibraries();
		    if (libraries.isUserDefinedTag(prefix, shortTagName)) {
			// custom tag
			isCustomTag = true;
			processCustomTagBegin(prefix, shortTagName);
		    }
		}
		if (!isCustomTag) {
		    // uninterpreted tag
                    // If the parent tag is also uninterpreted, then the
                    // characters have to be flushed, to preserve the order
                    // of the tags and characters that may appear in a XML
                    // fragment, such as <a>xyz<b></b></a>.
                    if ((prevNode != null) && prevNode.isUninterpretedTag() &&
                        (prevNode.getText() != null)) {
                        jspHandler.handleCharData(prevNode.start, start,
                                                  prevNode.getText());
                        prevNode.clearText();
                    }
		    node.setUninterpreted(true);
		    jspHandler.handleUninterpretedTagBegin(node.start, 
                            node.start, node.rawName, node.attrs);
		}
	    }
	} catch (Exception ex) {
	    throw new SAXException(ex);
	}
    }

    public void endElement(String namespaceURI, String localName,
                           String rawName) 
	throws SAXException
    {
	//p("\n*** endElement: " + rawName);
	//printLocation();
	//p("namespaceURI: " + namespaceURI);
	//p("localName: " + localName);
	//p("rawName: " + rawName);

	Mark stop = new Mark(filePath, locator.getLineNumber(),
			     locator.getColumnNumber());

	String name = rawName;
	Node node = (Node)stack.pop();
	//p(node);

	try {
	    if (name.equals("jsp:root")) {
		node.validate(true, false);
		jspHandler.handleRootEnd();
            } else if (name.equals("jsp:text")) {
		node.validate(false, true);
		jspHandler.handleJspCdata(node.start, stop, node.getText());
	    } else if (name.equals("jsp:directive.include")) {
		node.validate(true, false);
		jspHandler.handleDirective("include", node.start, stop, 
					   node.attrs);
	    } else if (name.equals("jsp:directive.page")) {
		node.validate(true, false);
		jspHandler.handleDirective("page", node.start, stop, 
					   node.attrs);
	    } else if (name.equals("jsp:declaration")) {
		node.validate(false, true);
		jspHandler.handleDeclaration(
                        node.start, stop, null, node.getText());
	    } else if (name.equals("jsp:expression")) {
		node.validate(false, true);
		jspHandler.handleExpression(
                        node.start, stop, null, node.getText());
	    } else if (name.equals("jsp:scriptlet")) {
		node.validate(false, true);
		jspHandler.handleScriptlet(
                        node.start, stop, null, node.getText());
	    } else if (name.equals("jsp:param")) {
		node.validate(true, false);
		// push the node back, it will be needed by the container node
		stack.push(node);
	    } else if (name.equals("jsp:params")) {
                // don't have it pushed on the stack...
                // this is a noop...
                return;
	    } else if (name.equals("jsp:fallback")) {
		node.validate(false, true);
		// push the node back, it will be needed by the container node
		stack.push(node);
	    } else if (name.equals("jsp:include")) {
		Hashtable params = null;
		if (!node.rawName.equals("jsp:include")) {
		    // handle params
		    stack.push(node);
		    params = popParams();
		    node = (Node)stack.pop();
		}
		node.validate(true, false);
		jspHandler.handleInclude(node.start, stop, node.attrs, params, true);
	    } else if (name.equals("jsp:forward")) {
		Hashtable params = null;
		if (!node.rawName.equals("jsp:forward")) {
		    // handle params
		    stack.push(node);
		    params = popParams();
		    node = (Node)stack.pop();
		}
		node.validate(true, false);
		jspHandler.handleForward(node.start, stop, node.attrs, params, true);
	    } else if (name.equals("jsp:useBean")) {
		node.validate(true, false);
		jspHandler.handleBeanEnd(node.start, stop, node.attrs);
	    } else if (name.equals("jsp:getProperty")) {
		node.validate(true, false);
		jspHandler.handleGetProperty(node.start, stop, node.attrs);
	    } else if (name.equals("jsp:setProperty")) {
		node.validate(true, false);
		jspHandler.handleSetProperty(node.start, stop, node.attrs, true);
	    } else if (name.equals("jsp:plugin")) {
		//@@@ test jsp parser to see if fallback can come first?
		Hashtable params = null;
		String fallback = null;
		if (node.rawName.equals("jsp:fallback")) {
		    // handle fallback
		    fallback = String.valueOf(node.getText());
		    node = (Node)stack.pop();
		}
		if (!node.rawName.equals("jsp:plugin")) {
		    // handle params
		    stack.push(node);
		    params = popParams();
		    node = (Node)stack.pop();
		}
		node.validate(true, false);
		//p(node);
		jspHandler.handlePlugin(node.start, stop, node.attrs, params, 
                        fallback, true);
	    } else {
		if (node.isUninterpretedTag()) {
		    // this is an 'uninterpreted' tag
		    jspHandler.handleUninterpretedTagEnd(stop, stop, rawName,
							 node.getText());
		} else {
		    // this is a custom tag
		    node.validate(true, false);
		    //System.out.println("node is: " + node);
		    processCustomTagEnd((NodeTag)node, stop);
		}
	    }
	} catch (Exception ex) {
	    if (ex instanceof SAXException) {
		throw (SAXException)ex;
	    } else {
		throw new SAXException(ex);
	    }
	}
    }

    // Need to delay calling the begin handler until we know whether
    // the custom tag has a body or not. Must read ahead.
    private void processCustomTagBeginDoIt(NodeTag node, boolean hasBody)
        throws JasperException
    {
        if (!node.isBeginDone()) {
            jspHandler.handleTagBegin(node.start, node.start, node.attrs, 
    			              node.prefix, node.shortTagName, node.tli, 
                                      node.ti, hasBody, true);
            node.setBeginDone(true);
        }
    }

    private void processCustomTagBegin(String prefix, String shortTagName) 
	throws ParseException, JasperException 
    {
	Node node = (Node)stack.pop();
	TagLibraries libraries = jspHandler.getTagLibraries();
	
	if (shortTagName == null) {
	    throw new ParseException(node.start, "Nothing after the :");
	}
            
	TagLibraryInfo tli = libraries.getTagLibInfo(prefix);
	TagInfo ti = tli.getTag(shortTagName);
            
	if (ti == null) {
	    throw new ParseException(node.start, 
	       "Unable to locate TagInfo for " + prefix + ":" + shortTagName);
	}
	node = new NodeTag(node, prefix, shortTagName, tli, ti);

	stack.push(node);
    }
    
    private void processCustomTagEnd(NodeTag node, Mark stop) 
	throws ParseException, JasperException {
	String bc = node.ti.getBodyContent();
        boolean hasBody = true;

        if (node.getText() != null) {
            String charString = node.getText().toString();
            hasBody = (charString.trim().length() > 0);
        }

        // call begin tag processing with body info
        processCustomTagBeginDoIt(node, hasBody);

	if (node.getText() != null && bc.equalsIgnoreCase(TagInfo.BODY_CONTENT_EMPTY)) {
	    throw new ParseException(node.start, "Body is supposed to be empty for "
				     + node.rawName);
	}
	jspHandler.handleTagEnd(node.start, stop, node.prefix, 
                node.shortTagName, node.attrs, node.tli, node.ti, hasBody);
    }

    //*********************************************************************
    // Interface LexicalHandler methods

    public void startDTD(String name, String publicId, String systemId)
        throws SAXException 
    {
	//p("\n*** startDTD");
	//p("BEWARE: Nothing done for this content.");
    }

    public void endDTD() throws SAXException {
        inDtd = false;
	//p("\n*** endDTD");
	//p("BEWARE: Nothing done for this content.");
    }

    public void startEntity(String name)
        throws SAXException 
    {
	//p("\n*** startEntity");
	//p("name: " + name);
	//p("BEWARE: Nothing done for this content.");
    }

    public void endEntity(String name)
        throws SAXException 
    {
	//p("\n*** endEntity");
	//p("name: " + name);
	//p("BEWARE: Nothing done for this content.");
    }

    public void startCDATA() throws SAXException {
	//p("\n*** startCDATA");
        inCdata = true;
    }

    public void endCDATA() throws SAXException {
	//p("\n*** endCDATA");
        inCdata = false;
    }

    public void comment(char[] ch, int start, int length)
        throws SAXException 
    {
	//p("\n*** comment");
	//printLocation();
	//p("|" + String.valueOf(ch, start, length) + "|");
	//p("BEWARE: Nothing done for this content.");

	/*
        String commentText = new String(ch, start, end);
        if ((!inDtd) && (!commentText.equals(""))) {
	    if (stack.empty()) {
		document.addComment(
				    new Comment(commentText));
	    } else {
		((Element)stack.peek()).addChild(
						 new Comment(commentText));
	    }
        }
	*/
    }

    //*********************************************************************
    // Interface ErrorHandler methods

    public void fatalError(SAXParseException ex)
        throws SAXException 
    {
	printException("FATAL", ex);
        throw ex;
    }

    public void error(SAXParseException ex)
        throws SAXException 
    {
	printException("ERROR", ex);
        throw ex;
    }

    public void warning(SAXParseException ex)
	throws SAXException 
    {
	printException("WARNING", ex);
    }

    //*********************************************************************
    // DTD Handler

    public void notationDecl(java.lang.String name,
			     java.lang.String publicId,
			     java.lang.String systemId)
	throws SAXException
    {
	//p("\n*** notationDecl");
	//p("name = " + name);
	//p("publicId = " + publicId);
	//p("systemId = " + systemId);
	//p("BEWARE: Nothing done for this content.");
    }

    public void unparsedEntityDecl(java.lang.String name,
				   java.lang.String publicId,
				   java.lang.String systemId,
				   java.lang.String notationName)
	throws SAXException
    {
	//p("\n*** unparsedEntityDecl");
	//p("name = " + name);
	//p("publicId = " + publicId);
	//p("systemId = " + systemId);
	//p("notationName = " + notationName);
	//p("BEWARE: Nothing done for this content.");
    }

    //*********************************************************************
    // EntityResolver

    static final String JSP_PUBLIC_ID1 = 
        "-//Sun Microsystems Inc.//DTD JavaServer Pages Version 1.1//EN";
    static final String JSP_PUBLIC_ID2 = 
        "-//Sun Microsystems Inc.//DTD JavaServer Pages Version 1.2//EN";
    static final String JSP_SYSTEM_ID = 
        "http://java.sun.com/products/jsp/dtd/jspcore_1_2.dtd";
    static final String JSP_SYSTEM_ID_INTERNAL = 
        "/org/apache/jasper/resources/jsp12.dtd";
    
    public InputSource resolveEntity(String publicId, String systemId)
	throws SAXException 
    {
        //p("\n*** resolveEntity");
        //p("publicId: " + publicId);
        //p("systemId: " + systemId);
        
        if (publicId.equals(JSP_PUBLIC_ID1) ||
        publicId.equals(JSP_PUBLIC_ID2)) {
            InputStream is =
            this.getClass().getResourceAsStream(JSP_SYSTEM_ID_INTERNAL);
            //p("attempt to returning " + JSP_SYSTEM_ID_INTERNAL);
            InputSource isrc = new InputSource(is);
            //p("success on returning " + JSP_SYSTEM_ID_INTERNAL);
            return isrc;
        } else {
            System.out.println("ERROR: publicID does not match: " + publicId);
            return null;
        }
    }

    //*********************************************************************
    // class Node

    private class Node {
	String rawName;
	Attributes attrs;
	Mark start;

	boolean isRoot;
	boolean isUninterpretedTag = false;
	StringBuffer text = null;

	Node(Node node) {
	    this(node.rawName, node.attrs, node.start);
	}

	Node(String rawName, Attributes attrs, Mark start) {
	    this.rawName = rawName;
	    this.attrs = attrs;
	    this.start = start;
	    isRoot = rawName.equals("jsp:root");
	}

	void addText(char[] chars, int start, int length) {
	    if (text == null) text = new StringBuffer();
	    text.append(chars, start, length);
	}

        char[] getText() {
            return text == null 
	    ? null
	    : text.toString().toCharArray();
        }

        void clearText() {
            text = null;
        }

	boolean isRoot() {
	    return isRoot;
	}

	void setUninterpreted(boolean isUninterpretedTag) {
	    this.isUninterpretedTag = isUninterpretedTag;
	}

	boolean isUninterpretedTag() {
	    return isUninterpretedTag;
	}

	void validate(boolean canHaveAttributes,
		      boolean canHaveText) 
	    throws SAXException 
	{
	    if (!canHaveAttributes && attrs.getLength()!=0) {
		throw new SAXException("VALIDATE ERROR: " +
				       rawName + " cannot have attributes.");
	    }
	    if (!canHaveText && text != null) {
		String data = text.toString().trim();
		if (data.length() > 0) {
		throw new SAXException(
		    Constants.getString(
			"jspx.error.templateDataNotInJspCdata",
			new Object[]{rawName, JspUtil.escapeXml(data)}));
		}
	    }
	}

	public void display() {
	    p("NODE: " + rawName);
	    for (int i=0; i<attrs.getLength(); i++) {
		p("attrs[" + i + "] " + attrs.getQName(i) + " = " +
		  attrs.getValue(i));
	    }
	}
    }
	

    //*********************************************************************
    // class NodeTag

    private class NodeTag extends Node {
	String prefix;
	String shortTagName;
	TagLibraryInfo tli;
	TagInfo ti;
	boolean beginDone;

	NodeTag(Node node, String prefix, String shortTagName, 
		TagLibraryInfo tli, TagInfo ti)
	{
	    super(node);
	    this.prefix = prefix;
	    this.shortTagName = shortTagName;
	    this.tli = tli;
	    this.ti = ti;
	    beginDone = false;
	}

	boolean isBeginDone() {
	    return beginDone;
	}

	void setBeginDone(boolean beginDone) {
	    this.beginDone = beginDone;
	}
    }
	
    //*********************************************************************
    // Utility methods

    /**
     * <p>
     *  Trim the right spacing off of a <code>String</code>.
     * </p>
     *
     * @param orig <code>String</code> to rtrim.
     * @return <code>String</code> - orig with no right spaces
     */
    private String rtrim(String orig) {
        int len = orig.length();
        int st = 0;
        int off = 0;
        char[] val = orig.toCharArray();

        while ((st < len) && (val[off + len - 1] <= ' ')) {
            len--;
        }
        return ((st > 0) || (len < orig.length())) 
	    ? orig.substring(st, len) : orig;
    }

    /**
     * <p>
     *  Trim the left spacing off of a <code>String</code>.
     * </p>
     *
     * @param orig <code>String</code> to rtrim.
     * @return <code>String</code> - orig with no left spaces
     */
    private String ltrim(String orig) {
        int len = orig.length();
        int st = 0;
        int off = 0;
        char[] val = orig.toCharArray();

        while ((st < len) && (val[off + st] <= ' ')) {
            st++;
        }
        return ((st > 0) || (len < orig.length())) 
	    ? orig.substring(st, len) : orig;
    }

    private void printException(String level, SAXParseException ex) {
	p("\n*** SAXParseException: " + level);
	p("Public ID: " + ex.getPublicId());
	p("System ID: " + ex.getSystemId());
	p("line " + ex.getLineNumber() + 
	  ", col " + ex.getColumnNumber());
	ex.printStackTrace(System.out);
    }

    private char[] makeCharArray(char[] buf, int offset, int len) {
        char[] ret = new char[len];
        for (int i=0,j=offset; i<len; i++,j++) {
            ret[i] = buf[j];
        }
        return ret;
    }

    private Hashtable popParams() throws ParseException {
	// @@@ code duplicated with JspParseEventListener
        Hashtable params = new Hashtable();
	Node node;
	while ((node = (Node)stack.peek()).rawName.equals("jsp:param")) {
	    node = (Node)stack.pop();
	    //p("popped this param node");
	    //p(node);
            String name = node.attrs.getValue("name");
            String value = node.attrs.getValue("value");
            if (name == null) {
                throw new ParseException(node.start, Constants.getString("jsp.error.param.noname"));
            }
            if (value == null) {
                throw new ParseException(node.start, Constants.getString("jsp.error.param.novalue"));
            }
            // Put that new binding into the params hashtable:
            String oldval[] = (String[])params.get(name);
            if (oldval == null) {
                String newval[] = new String[1];
                newval[0] = value;
                params.put(name, newval);
            } else {
                String newval[] = new String[oldval.length+1];
                System.arraycopy(oldval, 0, newval, 0, oldval.length);
                newval[oldval.length] = value;
                params.put(name, newval);
            }
        }
	return params;
    }

    private void printLocation() {
	p("--- (" + locator.getSystemId() + ", " +
	  locator.getPublicId() + ") (" +
	  locator.getLineNumber() + ", " +
	  locator.getColumnNumber() + ")");
    }

    void p(String s) {
	//System.out.println("[ParserXJspSaxHandler] " + s);
	System.out.println(s);
    }

    void p(Node node) {
	node.display();
    }
}



