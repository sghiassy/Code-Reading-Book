/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.deli;

import org.apache.avalon.framework.activity.Initializable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.environment.http.HttpContext;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.Constants;
import org.apache.cocoon.xml.dom.DOMFactory;
import org.apache.cocoon.components.parser.Parser;

import java.util.Vector;
import java.util.Iterator;
import java.util.TreeSet;
import java.util.Enumeration;
import java.util.Locale;
import java.util.Map;
import java.util.Set;
import java.io.IOException;
import java.io.BufferedReader;
import java.net.MalformedURLException;
import java.security.Principal;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.Text;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.ServletRequest;
import javax.servlet.Servlet;
import javax.servlet.ServletException;
import javax.servlet.ServletContext;
import javax.servlet.ServletInputStream;
import javax.servlet.RequestDispatcher;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpSession;

import com.hp.hpl.deli.Profile;
import com.hp.hpl.deli.ProfileAttribute;
import com.hp.hpl.deli.Workspace;

/**
 * Allows the use of <a href="http://www-uk.hpl.hp.com/people/marbut/">DELI</a>
 * to provide <a href="http://www.w3.org/Mobile/CCPP/">CC/PP</a> or
 * <a href="http://www1.wapforum.org/tech/terms.asp?doc=WAP-248-UAProf-20010530-p.pdf">UAProf</a>
 * support. For more details of DELI see the Technical Report
 * <a href="http://www-uk.hpl.hp.com/people/marbut/DeliUserGuideWEB.htm">DELI:
 * A Delivery Context Library for CC/PP and UAProf</a>.
 *
 * @author <a href="mailto:marbut@hplb.hpl.hp.com">Mark H. Butler</a>
 * @version CVS $ $ $Date: 2002/01/30 17:13:24 $
 */

public final class DeliImpl
extends AbstractLoggable
implements Parameterizable,
           Deli,
           Composable,
           Disposable,
           Initializable,
           ThreadSafe,
           Contextualizable {

    /** The path to the Cocoon configuration files */
    private String defaultPath = null;

    /** The name of the main DELI configuration file */
    private String deliConfig = "resources/deli/config/deliConfig.xml";

    /** The DELI workspace */
    private Workspace workspace;

    /** The component manager */
    protected ComponentManager manager = null;

    /** Parser used to construct the DOM tree to import the profile to a stylesheet */
    Parser parser;

    /** A context, used to retrieve the path to the configuration file */
    protected CocoonServletContext servletContext;

    /** Contextualize this class */
    public void contextualize(Context context)
    throws ContextException {
        org.apache.cocoon.environment.Context ctx = (org.apache.cocoon.environment.Context) context.get(Constants.CONTEXT_ENVIRONMENT_CONTEXT);
        this.servletContext = new CocoonServletContext(ctx);
    }

    /** Compose this class */
    public void compose(ComponentManager manager)
    throws ComponentException {
        this.manager = manager;
        this.parser = (Parser)this.manager.lookup(Parser.ROLE);
    }

     /** Configure this class */
     public void parameterize(Parameters params) {
         this.deliConfig = params.getParameter("deli-config-file", this.deliConfig);
     }

    /**
     * Initialize
     */
    public void initialize()
    throws Exception {
        this.workspace = new Workspace(this.servletContext, this.deliConfig);
    }

    /** Dispose of this class */
    public void dispose() {
        if (parser != null) this.manager.release((Component)parser);
        this.parser = null;
    }

    /** Process a HttpServletRequest and either extract
     *  CC/PP or UAProf information from it and use this information
     *  to resolve a profile or examine the user agent string, match
     *  this using the DELI legacy device database, and use this
     *  information to retrieve the appropriate CC/PP profile.
     *
     *@param	theRequest	The Request.
     *@return	The profile as a vector of profile attributes.
     *@exception	IOException.
     *@exception	ServletException.
     *@exception	Exception.
     */
    public Profile getProfile(Request theRequest)
    throws IOException, ServletException, Exception {
        CocoonServletRequest servletRequest = new CocoonServletRequest(theRequest);
        Profile theProfile = new Profile(this.workspace, (HttpServletRequest) servletRequest);
        return theProfile;
    }

    /** Convert a profile stored as a vector of profile attributes
     *  to a DOM tree.
     *
     *@param	theProfile	The profile as a vector of profile attributes.
     *@return	The DOM tree.
     */
    public Document getUACapabilities(Profile theProfile) {
        Document document;
        Element rootElement;
        Element attributeNode;
        Element complexAttributeNode;
        Text text;

        document = ((DOMFactory)parser).newDocument();
        rootElement = document.createElementNS(null, "browser");
        document.appendChild(rootElement);

        Iterator profileIter = theProfile.iterator();
        while (profileIter.hasNext()) {
            ProfileAttribute p = (ProfileAttribute) profileIter.next();
            attributeNode = document.createElementNS(null, p.getAttribute());
            rootElement.appendChild(attributeNode);
            Vector attributeValue = p.get();
            Iterator complexValueIter = attributeValue.iterator();
            if (p.getCollectionType().equals("Simple")) {
                // Simple attribute
                String value = (String) complexValueIter.next();
                text = document.createTextNode(value);
                attributeNode.appendChild(text);
            } else {
                // Complex attribute e.g. Seq or Bag
                while (complexValueIter.hasNext()) {
                    String value = (String) complexValueIter.next();
                    complexAttributeNode = document.createElementNS(null, "li");
                    attributeNode.appendChild(complexAttributeNode);
                    text = document.createTextNode(value);
                    complexAttributeNode.appendChild(text);
                }
            }
        }
        return document;
    }

    public Document getUACapabilities(Request theRequest)
    throws IOException, Exception {
        return this.getUACapabilities(this.getProfile(theRequest));
    }

    /**
     * Stub implementation of Servlet Context
     */
    public class CocoonServletContext implements ServletContext {

        org.apache.cocoon.environment.Context envContext;

        public CocoonServletContext(org.apache.cocoon.environment.Context context) {
            this.envContext = context;
        }

        public Object getAttribute(String name) { return envContext.getAttribute(name); }
        public void setAttribute(String name, Object value) { envContext.setAttribute(name, value); }
        public Enumeration getAttributeNames() { return envContext.getAttributeNames(); }
        public java.net.URL getResource(String path) throws MalformedURLException { return envContext.getResource(path);}
        public String getRealPath(String path) { return envContext.getRealPath(path); }
        public String getMimeType(String file) { return envContext.getMimeType(file); }
        public String getInitParameter(String name) { return envContext.getInitParameter(name); }
        public java.io.InputStream getResourceAsStream(String path){ return envContext.getResourceAsStream(path);}

        public ServletContext getContext(String uripath) { return (null); }
        public Enumeration getInitParameterNames() {return (null); }
        public int getMajorVersion() { return (2); }
        public int getMinorVersion() { return (3); }
        public RequestDispatcher getNamedDispatcher(String name) { return (null); }
        public RequestDispatcher getRequestDispatcher(String path) { return (null); }
        public Set getResourcePaths(String path) { return null; }
        public String getServerInfo() { return (null); }
        public Servlet getServlet(String name) throws ServletException { return (null); }
        public String getServletContextName() { return (null); }
        public Enumeration getServletNames() { return (null); }
        public Enumeration getServlets() {  return (null); }
        public void log(String message) {}
        public void log(Exception exception, String message) {}
        public void log(String message, Throwable exception) {}
        public void removeAttribute(String name) {}
   }

    /**
     * Stub implementation of HttpServletRequest
     */
    public class CocoonServletRequest implements HttpServletRequest {
        Request request;

        public CocoonServletRequest(Request request) {
            this.request = request;
        }

        public String getAuthType(){ return request.getAuthType(); }
        public long getDateHeader(String s){ return request.getDateHeader(s); }
        public String getHeader(String s){ return request.getHeader(s); }
        public Enumeration getHeaders(String s){ return request.getHeaders(s); }
        public Enumeration getHeaderNames(){ return request.getHeaderNames(); }
        public String getMethod(){ return request.getMethod(); }
        public String getPathInfo(){ return request.getPathInfo(); }
        public String getPathTranslated(){ return request.getPathTranslated(); }
        public String getContextPath(){ return request.getContextPath(); }
        public String getQueryString(){ return request.getQueryString(); }
        public String getRemoteUser(){ return request.getRemoteUser(); }
        public boolean isUserInRole(String s){ return request.isUserInRole(s); }
        public String getRequestedSessionId(){ return request.getRequestedSessionId(); }
        public String getRequestURI(){ return request.getRequestURI(); }
        public String getServletPath(){ return request.getServletPath(); }
        public boolean isRequestedSessionIdValid(){ return request.isRequestedSessionIdValid(); }
        public boolean isRequestedSessionIdFromCookie(){ return request.isRequestedSessionIdFromCookie(); }
        public Object getAttribute(String s){ return request.getAttribute(s); }
        public Enumeration getAttributeNames(){ return request.getAttributeNames(); }
        public String getCharacterEncoding(){ return request.getCharacterEncoding(); }
        public int getContentLength(){ return request.getContentLength(); }
        public String getContentType(){ return request.getContentType(); }
        public String getParameter(String s){ return request.getParameter(s); }
        public Enumeration getParameterNames(){ return request.getParameterNames(); }
        public String[] getParameterValues(String s){ return request.getParameterValues(s); }
        public String getProtocol(){ return request.getProtocol(); }
        public String getScheme(){ return request.getScheme(); }
        public String getServerName(){ return request.getServerName(); }
        public int getServerPort(){ return request.getServerPort(); }
        public String getRemoteAddr(){ return request.getRemoteAddr(); }
        public String getRemoteHost(){ return request.getRemoteHost(); }
        public void setAttribute(String s, Object obj){ request.setAttribute(s,obj); }
        public void removeAttribute(String s){ request.removeAttribute(s); }
        public boolean isSecure(){ return request.isSecure(); }
        public StringBuffer getRequestURL() { return null; }
        public Map getParameterMap() { return null; }
        public void setCharacterEncoding(String s) { }
        public Principal getUserPrincipal(){ return request.getUserPrincipal(); }
        public Locale getLocale(){ return request.getLocale(); }
        public Enumeration getLocales(){ return request.getLocales(); }

        public String getRealPath(String s){ return null; }
        public Cookie[] getCookies(){ return null; }
        public RequestDispatcher getRequestDispatcher(String s){ return null; }
        public BufferedReader getReader() throws IOException{ return null; }
        public ServletInputStream getInputStream() throws IOException{ return null; }
        public HttpSession getSession(boolean flag){ return null; }
        public HttpSession getSession(){ return null; }
        public boolean isRequestedSessionIdFromURL(){ return false; }
        public boolean isRequestedSessionIdFromUrl(){ return false; }
        public int getIntHeader(String s){ return 0; }
    }

}

