/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/Connector.java,v 1.10 2001/08/23 22:32:10 craigmcc Exp $
 * $Revision: 1.10 $
 * $Date: 2001/08/23 22:32:10 $
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
 * [Additional notices, if required by prior licensing conditions]
 *
 */


package org.apache.catalina;


import org.apache.catalina.net.ServerSocketFactory;


/**
 * A <b>Connector</b> is a component responsible receiving requests from,
 * and returning responses to, a client application.  A Connector performs
 * the following general logic:
 * <ul>
 * <li>Receive a request from the client application.
 * <li>Create (or allocate from a pool) appropriate Request and Response
 *     instances, and populate their properties based on the contents of
 *     the received request, as described below.
 *     <ul>
 *     <li>For all Requests, the <code>connector</code>,
 *         <code>protocol</code>, <code>remoteAddr</code>,
 *         <code>response</code>, <code>scheme</code>,
 *         <code>secure</code>, <code>serverName</code>,
 *         <code>serverPort</code> and <code>stream</code>
 *         properties <b>MUST</b> be set. The <code>contentLength</code>
 *         and <code>contentType</code> properties are also generally set.
 *     <li>For HttpRequests, the <code>method</code>, <code>queryString</code>,
 *         <code>requestedSessionCookie</code>,
 *         <code>requestedSessionId</code>, <code>requestedSessionURL</code>,
 *         <code>requestURI</code>, and <code>secure</code> properties
 *         <b>MUST</b> be set.  In addition, the various <code>addXxx</code>
 *         methods must be called to record the presence of cookies, headers,
 *         and locales in the original request.
 *     <li>For all Responses, the <code>connector</code>, <code>request</code>,
 *         and <code>stream</code> properties <b>MUST</b> be set.
 *     <li>No additional headers must be set by the Connector for
 *         HttpResponses.
 *     </ul>
 * <li>Identify an appropriate Container to use for processing this request.
 *     For a stand alone Catalina installation, this will probably be a
 *     (singleton) Engine implementation.  For a Connector attaching Catalina
 *     to a web server such as Apache, this step could take advantage of
 *     parsing already performed within the web server to identify the
 *     Context, and perhaps even the Wrapper, to utilize in satisfying this
 *     Request.
 * <li>Call the <code>invoke()</code> method of the selected Container,
 *     passing the initialized Request and Response instances as arguments.
 * <li>Return any response created by the Container to the client, or
 *     return an appropriate error message if an exception of any type
 *     was thrown.
 * <li>If utilizing a pool of Request and Response objects, recycle the pair
 *     of instances that was just used.
 * </ul>
 * It is expected that the implementation details of various Connectors will
 * vary widely, so the logic above should considered typical rather than
 * normative.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.10 $ $Date: 2001/08/23 22:32:10 $
 */

public interface Connector {


    // ------------------------------------------------------------- Properties


    /**
     * Return the Container used for processing requests received by this
     * Connector.
     */
    public Container getContainer();


    /**
     * Set the Container used for processing requests received by this
     * Connector.
     *
     * @param container The new Container to use
     */
    public void setContainer(Container container);


    /**
     * Return the "enable DNS lookups" flag.
     */
    public boolean getEnableLookups();


    /**
     * Set the "enable DNS lookups" flag.
     *
     * @param enableLookups The new "enable DNS lookups" flag value
     */
    public void setEnableLookups(boolean enableLookups);


    /**
     * Return the server socket factory used by this Container.
     */
    public ServerSocketFactory getFactory();


    /**
     * Set the server socket factory used by this Container.
     *
     * @param factory The new server socket factory
     */
    public void setFactory(ServerSocketFactory factory);


    /**
     * Return descriptive information about this Connector implementation.
     */
    public String getInfo();


    /**
     * Return the port number to which a request should be redirected if
     * it comes in on a non-SSL port and is subject to a security constraint
     * with a transport guarantee that requires SSL.
     */
    public int getRedirectPort();


    /**
     * Set the redirect port number.
     *
     * @param redirectPort The redirect port number (non-SSL to SSL)
     */
    public void setRedirectPort(int redirectPort);


    /**
     * Return the scheme that will be assigned to requests received
     * through this connector.  Default value is "http".
     */
    public String getScheme();


    /**
     * Set the scheme that will be assigned to requests received through
     * this connector.
     *
     * @param scheme The new scheme
     */
    public void setScheme(String scheme);


    /**
     * Return the secure connection flag that will be assigned to requests
     * received through this connector.  Default value is "false".
     */
    public boolean getSecure();


    /**
     * Set the secure connection flag that will be assigned to requests
     * received through this connector.
     *
     * @param secure The new secure connection flag
     */
    public void setSecure(boolean secure);


    /**
     * Return the <code>Service</code> with which we are associated (if any).
     */
    public Service getService();


    /**
     * Set the <code>Service</code> with which we are associated (if any).
     *
     * @param service The service that owns this Engine
     */
    public void setService(Service service);


    // --------------------------------------------------------- Public Methods


    /**
     * Create (or allocate) and return a Request object suitable for
     * specifying the contents of a Request to the responsible Container.
     */
    public Request createRequest();


    /**
     * Create (or allocate) and return a Response object suitable for
     * receiving the contents of a Response from the responsible Container.
     */
    public Response createResponse();

    /**
     * Invoke a pre-startup initialization. This is used to allow connectors
     * to bind to restricted ports under Unix operating environments.
     *
     * @exception LifecycleException If this server was already initialized.
     */
    public void initialize()
    throws LifecycleException;

}
