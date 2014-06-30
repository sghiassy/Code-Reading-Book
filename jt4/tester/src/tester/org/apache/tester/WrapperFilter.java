/* ========================================================================= *
 *                                                                           *
 *                 The Apache Software License,  Version 1.1                 *
 *                                                                           *
 *         Copyright (c) 1999, 2000  The Apache Software Foundation.         *
 *                           All rights reserved.                            *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * Redistribution and use in source and binary forms,  with or without modi- *
 * fication, are permitted provided that the following conditions are met:   *
 *                                                                           *
 * 1. Redistributions of source code  must retain the above copyright notice *
 *    notice, this list of conditions and the following disclaimer.          *
 *                                                                           *
 * 2. Redistributions  in binary  form  must  reproduce the  above copyright *
 *    notice,  this list of conditions  and the following  disclaimer in the *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. The end-user documentation  included with the redistribution,  if any, *
 *    must include the following acknowlegement:                             *
 *                                                                           *
 *       "This product includes  software developed  by the Apache  Software *
 *        Foundation <http://www.apache.org/>."                              *
 *                                                                           *
 *    Alternately, this acknowlegement may appear in the software itself, if *
 *    and wherever such third-party acknowlegements normally appear.         *
 *                                                                           *
 * 4. The names  "The  Jakarta  Project",  "Tomcat",  and  "Apache  Software *
 *    Foundation"  must not be used  to endorse or promote  products derived *
 *    from this  software without  prior  written  permission.  For  written *
 *    permission, please contact <apache@apache.org>.                        *
 *                                                                           *
 * 5. Products derived from this software may not be called "Apache" nor may *
 *    "Apache" appear in their names without prior written permission of the *
 *    Apache Software Foundation.                                            *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES *
 * INCLUDING, BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY *
 * AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.  IN NO EVENT SHALL *
 * THE APACHE  SOFTWARE  FOUNDATION OR  ITS CONTRIBUTORS  BE LIABLE  FOR ANY *
 * DIRECT,  INDIRECT,   INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL *
 * DAMAGES (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS *
 * OR SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) *
 * HOWEVER CAUSED AND  ON ANY  THEORY  OF  LIABILITY,  WHETHER IN  CONTRACT, *
 * STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN *
 * ANY  WAY  OUT OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF  ADVISED  OF THE *
 * POSSIBILITY OF SUCH DAMAGE.                                               *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * This software  consists of voluntary  contributions made  by many indivi- *
 * duals on behalf of the  Apache Software Foundation.  For more information *
 * on the Apache Software Foundation, please see <http://www.apache.org/>.   *
 *                                                                           *
 * ========================================================================= */

package org.apache.tester;


import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

/**
 * Configurable filter that will wrap the request and/or response objects
 * it passes on with either generic or HTTP-specific wrappers.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.4 $ $Date: 2001/04/17 16:34:55 $
 */

public class WrapperFilter implements Filter {


    // ----------------------------------------------------------- Constructors



    // ----------------------------------------------------- Instance Variables


    /**
     * The filter configuration object for this filter.
     */
    protected FilterConfig config = null;


    /**
     * The type of wrapper for each request ("none", "generic", "http").
     */
    protected String requestWrapper = "none";


    /**
     * The type of wrapper for each response ("none", "generic", "http").
     */
    protected String responseWrapper = "none";


    // --------------------------------------------------------- Public Methods


    /**
     * Release this Filter instance from service.
     */
    public void destroy() {

        config = null;
        requestWrapper = "none";
        responseWrapper = "none";

    }


    /**
     * Wrap this request and/or response as configured and pass it on.
     */
    public void doFilter(ServletRequest inRequest, ServletResponse inResponse,
                         FilterChain chain)
        throws IOException, ServletException {

        // Create the appropriate wrappers
        ServletRequest outRequest = inRequest;
        ServletResponse outResponse = inResponse;
        if (requestWrapper.equals("generic")) {
            outRequest = new TesterServletRequestWrapper(inRequest);
        } else if (requestWrapper.equals("http")) {
            outRequest = new TesterHttpServletRequestWrapper
                ((HttpServletRequest) inRequest);
        }
        if (responseWrapper.equals("generic")) {
            outResponse = new TesterServletResponseWrapper(inResponse);
        } else if (responseWrapper.equals("http")) {
            outResponse = new TesterHttpServletResponseWrapper
                ((HttpServletResponse) inResponse);
        }

        // Perform this request
        chain.doFilter(outRequest, outResponse);

    }


    /**
     * Place this Filter instance into service.
     *
     * @param config The filter configuration object
     */
    public void init(FilterConfig config) throws ServletException {

        this.config = config;
        String value = null;
        value = config.getInitParameter("request");
        if (value != null)
            requestWrapper = value;
        value = config.getInitParameter("response");
        if (value != null)
            responseWrapper = value;

    }


}
