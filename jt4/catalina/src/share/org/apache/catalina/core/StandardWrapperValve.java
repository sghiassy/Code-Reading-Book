/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/core/StandardWrapperValve.java,v 1.30 2001/09/14 20:30:01 craigmcc Exp $
 * $Revision: 1.30 $
 * $Date: 2001/09/14 20:30:01 $
 *
 * ====================================================================
 *
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999-2001 The Apache Software Foundation.  All rights
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


package org.apache.catalina.core;


import java.io.IOException;
import java.io.PrintWriter;
import javax.servlet.FilterConfig;
import javax.servlet.RequestDispatcher;
import javax.servlet.Servlet;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.UnavailableException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.catalina.Container;
import org.apache.catalina.Context;
import org.apache.catalina.Globals;
import org.apache.catalina.HttpRequest;
import org.apache.catalina.HttpResponse;
import org.apache.catalina.Logger;
import org.apache.catalina.Request;
import org.apache.catalina.Response;
import org.apache.catalina.ValveContext;
import org.apache.catalina.Wrapper;
import org.apache.catalina.deploy.ErrorPage;
import org.apache.catalina.deploy.FilterDef;
import org.apache.catalina.deploy.FilterMap;
import org.apache.catalina.util.InstanceSupport;
import org.apache.catalina.util.RequestUtil;
import org.apache.catalina.util.StringManager;
import org.apache.catalina.valves.ValveBase;


/**
 * Valve that implements the default basic behavior for the
 * <code>StandardWrapper</code> container implementation.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.30 $ $Date: 2001/09/14 20:30:01 $
 */

final class StandardWrapperValve
    extends ValveBase {


    // ----------------------------------------------------- Instance Variables


    /**
     * The debugging detail level for this component.
     */
    private int debug = 0;


    /**
     * The filter definition for our container-provided filter.
     */
    private FilterDef filterDef = null;


    /**
     * The descriptive information related to this implementation.
     */
    private static final String info =
        "org.apache.catalina.core.StandardWrapperValve/1.0";


    /**
     * The string manager for this package.
     */
    private static final StringManager sm =
        StringManager.getManager(Constants.Package);


    // ------------------------------------------------------------- Properties


    /**
     * Return descriptive information about this Valve implementation.
     */
    public String getInfo() {

        return (info);

    }


    // --------------------------------------------------------- Public Methods


    /**
     * Invoke the servlet we are managing, respecting the rules regarding
     * servlet lifecycle and SingleThreadModel support.
     *
     * @param request Request to be processed
     * @param response Response to be produced
     * @param valveContext Valve context used to forward to the next Valve
     *
     * @exception IOException if an input/output error occurred
     * @exception ServletException if a servlet error occurred
     */
    public void invoke(Request request, Response response,
                       ValveContext valveContext)
        throws IOException, ServletException {

        // Initialize local variables we may need
        boolean unavailable = false;
        Throwable throwable = null;
        StandardWrapper wrapper = (StandardWrapper) getContainer();
        ServletRequest sreq = request.getRequest();
        ServletResponse sres = response.getResponse();
        Servlet servlet = null;
        HttpServletRequest hreq = null;
        if (sreq instanceof HttpServletRequest)
            hreq = (HttpServletRequest) sreq;
        HttpServletResponse hres = null;
        if (sres instanceof HttpServletResponse)
            hres = (HttpServletResponse) sres;

        // Check for the application being marked unavailable
        if (!((Context) wrapper.getParent()).getAvailable()) {
            hres.sendError(HttpServletResponse.SC_SERVICE_UNAVAILABLE,
                           sm.getString("standardContext.isUnavailable"));
            unavailable = true;
        }

        // Check for the servlet being marked unavailable
        if (!unavailable && wrapper.isUnavailable()) {
            log(sm.getString("standardWrapper.isUnavailable",
                             wrapper.getName()));
            if (hres == null) {
                ;       // NOTE - Not much we can do generically
            } else {
                long available = wrapper.getAvailable();
                if ((available > 0L) && (available < Long.MAX_VALUE))
                    hres.setDateHeader("Retry-After", available);
                hres.sendError(HttpServletResponse.SC_SERVICE_UNAVAILABLE,
                               sm.getString("standardWrapper.isUnavailable",
                                            wrapper.getName()));
            }
            unavailable = true;
        }

        // Allocate a servlet instance to process this request
        try {
            if (!unavailable) {
                servlet = wrapper.allocate();
            }
        } catch (ServletException e) {
            log(sm.getString("standardWrapper.allocateException",
                             wrapper.getName()), e);
            throwable = e;
            exception(request, response, e);
            servlet = null;
        } catch (Throwable e) {
            log(sm.getString("standardWrapper.allocateException",
                             wrapper.getName()), e);
            throwable = e;
            exception(request, response, e);
            servlet = null;
        }

        // Create the filter chain for this request
        ApplicationFilterChain filterChain =
          createFilterChain(request, servlet);

        // Call the filter chain for this request
        // NOTE: This also calls the servlet's service() method
        try {
            String jspFile = wrapper.getJspFile();
            if (jspFile != null)
                sreq.setAttribute(Globals.JSP_FILE_ATTR, jspFile);
            else
                sreq.removeAttribute(Globals.JSP_FILE_ATTR);
            if ((servlet != null) && (filterChain != null)) {
                filterChain.doFilter(sreq, sres);
            }
            sreq.removeAttribute(Globals.JSP_FILE_ATTR);
        } catch (IOException e) {
            sreq.removeAttribute(Globals.JSP_FILE_ATTR);
            log(sm.getString("standardWrapper.serviceException",
                             wrapper.getName()), e);
            ;   // No reporting to the response
            ;   // No change in availability status
        } catch (UnavailableException e) {
            sreq.removeAttribute(Globals.JSP_FILE_ATTR);
            log(sm.getString("standardWrapper.serviceException",
                             wrapper.getName()), e);
            //            throwable = e;
            //            exception(request, response, e);
            wrapper.unavailable(e);
            long available = wrapper.getAvailable();
            if ((available > 0L) && (available < Long.MAX_VALUE))
                hres.setDateHeader("Retry-After", available);
            hres.sendError(HttpServletResponse.SC_SERVICE_UNAVAILABLE,
                           sm.getString("standardWrapper.isUnavailable",
                                        wrapper.getName()));
            // Do not save exception in 'throwable', because we
            // do not want to do exception(request, response, e) processing
        } catch (ServletException e) {
            sreq.removeAttribute(Globals.JSP_FILE_ATTR);
            log(sm.getString("standardWrapper.serviceException",
                             wrapper.getName()), e);
            throwable = e;
            exception(request, response, e);
        } catch (Throwable e) {
            sreq.removeAttribute(Globals.JSP_FILE_ATTR);
            log(sm.getString("standardWrapper.serviceException",
                             wrapper.getName()), e);
            throwable = e;
            exception(request, response, e);
        }

        // Release the filter chain (if any) for this request
        try {
            if (filterChain != null)
                filterChain.release();
        } catch (Throwable e) {
            log(sm.getString("standardWrapper.releaseFilters",
                             wrapper.getName()), e);
            if (throwable == null) {
                throwable = e;
                exception(request, response, e);
            }
        }

        // Deallocate the allocated servlet instance
        try {
            if (servlet != null) {
                wrapper.deallocate(servlet);
            }
        } catch (Throwable e) {
            log(sm.getString("standardWrapper.deallocateException",
                             wrapper.getName()), e);
            if (throwable == null) {
                throwable = e;
                exception(request, response, e);
            }
        }

        // If this servlet has been marked permanently unavailable,
        // unload it and release this instance
        try {
            if ((servlet != null) &&
                (wrapper.getAvailable() == Long.MAX_VALUE)) {
                wrapper.unload();
            }
        } catch (Throwable e) {
            log(sm.getString("standardWrapper.unloadException",
                             wrapper.getName()), e);
            if (throwable == null) {
                throwable = e;
                exception(request, response, e);
            }
        }


        // Generate a response for the generated HTTP status and message
        if (throwable == null) {
            status(request, response);
        }

    }


    // -------------------------------------------------------- Private Methods


    /**
     * Construct and return a FilterChain implementation that will wrap the
     * execution of the specified servlet instance.  If we should not execute
     * a filter chain at all, return <code>null</code>.
     * <p>
     * <strong>FIXME</strong> - Pool the chain instances!
     *
     * @param request The servlet request we are processing
     * @param servlet The servlet instance to be wrapped
     */
    private ApplicationFilterChain createFilterChain(Request request,
                                                     Servlet servlet) {

        // If there is no servlet to execute, return null
        if (servlet == null)
            return (null);

        // Create and initialize a filter chain object
        ApplicationFilterChain filterChain =
          new ApplicationFilterChain();
        filterChain.setServlet(servlet);
        StandardWrapper wrapper = (StandardWrapper) getContainer();
        filterChain.setSupport(wrapper.getInstanceSupport());

        // Acquire the filter mappings for this Context
        StandardContext context = (StandardContext) wrapper.getParent();
        FilterMap filterMaps[] = context.findFilterMaps();

        // If there are no filter mappings, we are done
        if ((filterMaps == null) || (filterMaps.length == 0))
            return (filterChain);
//        if (debug >= 1)
//            log("createFilterChain:  Processing " + filterMaps.length +
//                " filter map entries");

        // Acquire the information we will need to match filter mappings
        String requestPath = null;
        if (request instanceof HttpRequest) {
            HttpServletRequest hreq =
                (HttpServletRequest) request.getRequest();
            String contextPath = hreq.getContextPath();
            if (contextPath == null)
                contextPath = "";
            String requestURI = hreq.getRequestURI();
            if (requestURI.length() >= contextPath.length())
                requestPath = requestURI.substring(contextPath.length());
        }
        String servletName = wrapper.getName();
//        if (debug >= 1) {
//            log(" requestPath=" + requestPath);
//            log(" servletName=" + servletName);
//        }
        int n = 0;

        // Add the relevant path-mapped filters to this filter chain
        for (int i = 0; i < filterMaps.length; i++) {
//            if (debug >= 2)
//                log(" Checking path-mapped filter '" +
//                    filterMaps[i] + "'");
            if (!matchFiltersURL(filterMaps[i], requestPath))
                continue;
            ApplicationFilterConfig filterConfig = (ApplicationFilterConfig)
                context.findFilterConfig(filterMaps[i].getFilterName());
            if (filterConfig == null) {
//                if (debug >= 2)
//                    log(" Missing path-mapped filter '" +
//                        filterMaps[i] + "'");
                ;       // FIXME - log configuration problem
                continue;
            }
//            if (debug >= 2)
//                log(" Adding path-mapped filter '" +
//                    filterConfig.getFilterName() + "'");
            filterChain.addFilter(filterConfig);
            n++;
        }

        // Add filters that match on servlet name second
        for (int i = 0; i < filterMaps.length; i++) {
//            if (debug >= 2)
//                log(" Checking servlet-mapped filter '" +
//                    filterMaps[i] + "'");
            if (!matchFiltersServlet(filterMaps[i], servletName))
                continue;
            ApplicationFilterConfig filterConfig = (ApplicationFilterConfig)
                context.findFilterConfig(filterMaps[i].getFilterName());
            if (filterConfig == null) {
//                if (debug >= 2)
//                    log(" Missing servlet-mapped filter '" +
//                        filterMaps[i] + "'");
                ;       // FIXME - log configuration problem
                continue;
            }
//            if (debug >= 2)
//                log(" Adding servlet-mapped filter '" +
//                     filterMaps[i] + "'");
            filterChain.addFilter(filterConfig);
            n++;
        }

        // Return the completed filter chain
//        if (debug >= 2)
//            log(" Returning chain with " + n + " filters");
        return (filterChain);

    }


    /**
     * Handle an HTTP status code or Java exception by forwarding control
     * to the location included in the specified errorPage object.  It is
     * assumed that the caller has already recorded any request attributes
     * that are to be forwarded to this page.  Return <code>true</code> if
     * we successfully utilized the specified error page location, or
     * <code>false</code> if the default error report should be rendered.
     *
     * @param request The request being processed
     * @param response The response being generated
     * @param errorPage The errorPage directive we are obeying
     */
    private boolean custom(Request request, Response response,
                           ErrorPage errorPage) {

        if (debug >= 1)
            log("Processing " + errorPage);

        // Validate our current environment
        if (!(request instanceof HttpRequest)) {
            if (debug >= 1)
                log(" Not processing an HTTP request --> default handling");
            return (false);     // NOTE - Nothing we can do generically
        }
        HttpServletRequest hreq =
            (HttpServletRequest) request.getRequest();
        if (!(response instanceof HttpResponse)) {
            if (debug >= 1)
                log("Not processing an HTTP response --> default handling");
            return (false);     // NOTE - Nothing we can do generically
        }
        HttpServletResponse hres =
            (HttpServletResponse) response.getResponse();

        try {

            // Reset the response if possible (else IllegalStateException)
            hres.reset();

            // Forward control to the specified location
            ServletContext servletContext =
                ((Context) container.getParent()).getServletContext();
            RequestDispatcher rd =
                servletContext.getRequestDispatcher(errorPage.getLocation());
            rd.forward(hreq, hres);

            // Indicate that we have successfully processed this custom page
            return (true);

        } catch (Throwable t) {

            // Report our failure to process this custom page
            log("Exception Processing " + errorPage, t);
            return (false);

        }

    }


    /**
     * Handle the specified ServletException encountered while processing
     * the specified Request to produce the specified Response.  Any
     * exceptions that occur during generation of the exception report are
     * logged and swallowed.
     *
     * @param request The request being processed
     * @param response The response being generated
     * @param exception The exception that occurred (which possibly wraps
     *  a root cause exception
     */
    private void exception(Request request, Response response,
                           Throwable exception) {

        // Handle a custom error page for this status code
        if (debug >= 1)
            log("Handling exception: " + exception);
        Context context = (Context) container.getParent();
        Throwable realError = exception;
        ErrorPage errorPage = findErrorPage(context, realError);
        if ((errorPage == null) && (realError instanceof ServletException)) {
            realError = ((ServletException) exception).getRootCause();
            if (realError != null)
                errorPage = findErrorPage(context, realError);
            else
                realError = exception;
        }
        if (errorPage != null) {
            //            if (debug >= 1)
            //                log(" Sending to custom error page " + errorPage);
            ServletRequest sreq = request.getRequest();
            sreq.setAttribute(Globals.ERROR_MESSAGE_ATTR,
                              exception.getMessage());
            sreq.setAttribute(Globals.EXCEPTION_ATTR,
                              exception);
            Wrapper wrapper = (Wrapper) getContainer();
            sreq.setAttribute(Globals.SERVLET_NAME_ATTR,
                              wrapper.getName());
            if (sreq instanceof HttpServletRequest)
                sreq.setAttribute(Globals.EXCEPTION_PAGE_ATTR,
                                  ((HttpServletRequest) sreq).getRequestURI());
            sreq.setAttribute(Globals.EXCEPTION_TYPE_ATTR,
                              exception.getClass());
            if (custom(request, response, errorPage))
                return;
        }

        // The response is an error
        response.setError();

        // Reset the response (if possible)
        //        if (debug >= 1)
        //            log(" Resetting response");
        try {
            response.getResponse().reset();
        } catch (IllegalStateException e) {
            //            if (debug >= 1)
            //                log("  IllegalStateException: " + e.toString());
            ;
        }

        // Indicate an INTERNAL SERVER ERROR status (if possible)
        try {
            //            if (debug >= 1)
            //                log(" Sending INTERNAL_SERVER_ERROR");
            ServletResponse sresponse = response.getResponse();
            if (sresponse instanceof HttpServletResponse)
                ((HttpServletResponse) sresponse).sendError
                    (HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
        } catch (IllegalStateException e) {
            //            if (debug >= 1)
            //                log("  IllegalStateException: " + e.toString());
            ;
        } catch (IOException e) {
            //            if (debug >= 1)
            //                log("  IOException: " + e.toString());
            ;
        }

        // Render a default HTML exception report page
        Throwable rootCause = null;
        if (exception instanceof ServletException)
            rootCause = ((ServletException) exception).getRootCause();
        try {
            //            if (debug >= 1)
            //                log(" Setting content type to text/html");
            try {
                response.getResponse().setContentType("text/html");
            } catch (Throwable t) {
                //                if (debug >= 1)
                //                    log("  Throwable: " + t.toString());
                ;
            }
            //            if (debug >= 1)
            //                log(" Getting reporter writer");
            PrintWriter writer = response.getReporter();
            //            if (debug >= 1)
            //                log(" Writing standard error report page");
            writer.println("<html>");
            writer.println("<head>");
            writer.println("<title>" +
                           sm.getString("standardWrapper.exception0") +
                           "</title>");
            writer.println("</head>");
            writer.println("<body bgcolor=\"white\">");
            writer.println("<br><br>");
            writer.println("<h1>" +
                           sm.getString("standardWrapper.exception1") +
                           "</h1>");
            if (rootCause != null)
                writer.println("<h3>" +
                               sm.getString("standardWrapper.exception2") +
                               "</h3>");
            writer.println("<pre>");
            exception.printStackTrace(writer);
            writer.println("</pre>");
            if (rootCause != null) {
                writer.println("<h3>" +
                               sm.getString("standardWrapper.exception3") +
                               "</h3>");
                writer.println("<pre>");
                rootCause.printStackTrace(writer);
                writer.println("</pre>");
            }
            writer.println("</body>");
            writer.println("</html>");
            writer.flush();
        } catch (IllegalStateException e) {
            //            if (debug >= 1)
            //                log("  IllegalStateException:", e);
            ;
        }
        //        if (debug >= 1)
        //            log(" Finished with exception() report");

    }


    /**
     * Find and return the ErrorPage instance for the specified exception's
     * class, or an ErrorPage instance for the closest superclass for which
     * there is such a definition.  If no associated ErrorPage instance is
     * found, return <code>null</code>.
     *
     * @param context The Context in which to search
     * @param exception The exception for which to find an ErrorPage
     */
    private ErrorPage findErrorPage(Context context, Throwable exception) {

        if (exception == null)
            return (null);
        Class clazz = exception.getClass();
        String name = clazz.getName();
        while (!"java.lang.Object".equals(clazz)) {
            ErrorPage errorPage = context.findErrorPage(name);
            if (errorPage != null)
                return (errorPage);
            clazz = clazz.getSuperclass();
            if (clazz == null)
                break;
            name = clazz.getName();
        }
        return (null);

    }


    /**
     * Log a message on the Logger associated with our Container (if any)
     *
     * @param message Message to be logged
     */
    private void log(String message) {

        Logger logger = null;
        if (container != null)
            logger = container.getLogger();
        if (logger != null)
            logger.log("StandardWrapperValve[" + container.getName() + "]: "
                       + message);
        else {
            String containerName = null;
            if (container != null)
                containerName = container.getName();
            System.out.println("StandardWrapperValve[" + containerName
                               + "]: " + message);
        }

    }


    /**
     * Log a message on the Logger associated with our Container (if any)
     *
     * @param message Message to be logged
     * @param throwable Associated exception
     */
    private void log(String message, Throwable throwable) {

        Logger logger = null;
        if (container != null)
            logger = container.getLogger();
        if (logger != null)
            logger.log("StandardWrapperValve[" + container.getName() + "]: "
                       + message, throwable);
        else {
            String containerName = null;
            if (container != null)
                containerName = container.getName();
            System.out.println("StandardWrapperValve[" + containerName
                               + "]: " + message);
            System.out.println("" + throwable);
            throwable.printStackTrace(System.out);
        }

    }


    /**
     * Return <code>true</code> if the specified servlet name matches
     * the requirements of the specified filter mapping; otherwise
     * return <code>false</code>.
     *
     * @param filterMap Filter mapping being checked
     * @param servletName Servlet name being checked
     */
    private boolean matchFiltersServlet(FilterMap filterMap,
                                        String servletName) {

//      if (debug >= 3)
//          log("  Matching servlet name '" + servletName +
//              "' against mapping " + filterMap);

        if (servletName == null)
            return (false);
        else
            return (servletName.equals(filterMap.getServletName()));

    }


    /**
     * Return <code>true</code> if the context-relative request path
     * matches the requirements of the specified filter mapping;
     * otherwise, return <code>null</code>.
     *
     * @param filterMap Filter mapping being checked
     * @param requestPath Context-relative request path of this request
     */
    private boolean matchFiltersURL(FilterMap filterMap,
                                    String requestPath) {

//      if (debug >= 3)
//          log("  Matching request path '" + requestPath +
//              "' against mapping " + filterMap);

        if (requestPath == null)
            return (false);

        // Match on context relative request path
        String testPath = filterMap.getURLPattern();
        if (testPath == null)
            return (false);

        // Case 1 - Exact Match
        if (testPath.equals(requestPath))
            return (true);

        // Case 2 - Path Match ("/.../*")
        if (testPath.equals("/*"))
            return (true);      // Optimize a common case
        if (testPath.endsWith("/*")) {
            String comparePath = requestPath;
            while (true) {
                if (testPath.equals(comparePath + "/*"))
                    return (true);
                int slash = comparePath.lastIndexOf('/');
                if (slash < 0)
                    break;
                comparePath = comparePath.substring(0, slash);
            }
            return (false);
        }

        // Case 3 - Extension Match
        if (testPath.startsWith("*.")) {
            int slash = requestPath.lastIndexOf('/');
            int period = requestPath.lastIndexOf('.');
            if ((slash >= 0) && (period > slash))
                return (testPath.equals("*." +
                                        requestPath.substring(period + 1)));
        }

        // Case 4 - "Default" Match
        return (false); // NOTE - Not relevant for selecting filters

    }


    /**
     * Handle the HTTP status code (and corresponding message) generated
     * while processing the specified Request to produce the specified
     * Response.  Any exceptions that occur during generation of the error
     * report are logged and swallowed.
     *
     * @param request The request being processed
     * @param response The response being generated
     */
    private void status(Request request, Response response) {

        // Do nothing on non-HTTP responses
        if (!(response instanceof HttpResponse))
            return;
        HttpResponse hresponse = (HttpResponse) response;
        if (!(response.getResponse() instanceof HttpServletResponse))
            return;
        HttpServletResponse hres =
            (HttpServletResponse) response.getResponse();
        int statusCode = hresponse.getStatus();
        String message = RequestUtil.filter(hresponse.getMessage());
        if (message == null)
            message = "";

        // Do nothing on a 1xx status
        if (statusCode < 200)
            return;
        // Do nothing on an OK status
        if (statusCode == HttpServletResponse.SC_OK)
            return;
        // Do nothing on a NO MODIFIED status
        if (statusCode == HttpServletResponse.SC_NOT_MODIFIED)
            return;
        // Do nothing on a NO CONTENT status
        if (statusCode == HttpServletResponse.SC_NO_CONTENT)
            return;

        // Handle a custom error page for this status code
        Context context = (Context) container.getParent();
        ErrorPage errorPage = context.findErrorPage(statusCode);
        if (errorPage != null) {
            ServletRequest sreq = request.getRequest();
            sreq.setAttribute(Globals.STATUS_CODE_ATTR,
                              new Integer(statusCode));
            sreq.setAttribute(Globals.ERROR_MESSAGE_ATTR,
                              message);
            Wrapper wrapper = (Wrapper) getContainer();
            sreq.setAttribute(Globals.SERVLET_NAME_ATTR,
                              wrapper.getName());
            if (sreq instanceof HttpServletRequest)
                sreq.setAttribute(Globals.EXCEPTION_PAGE_ATTR,
                                  ((HttpServletRequest) sreq).getRequestURI());
            if (custom(request, response, errorPage))
                return;
        }

        // Do nothing if there is no report for the specified status code
        String report = null;
        try {
            report = sm.getString("http." + statusCode, message);
        } catch (Throwable t) {
            ;
        }
        if (report == null)
            return;

        // Reset the response data buffer (if possible)
        try {
            if (hresponse.isError())
                hresponse.reset(statusCode, message);
        } catch (Throwable e) {
            if (debug >= 1)
                log("status.reset", e);
        }

        // Render a default HTML status report page
        try {
            try {
                hres.setContentType("text/html");
            } catch (Throwable t) {
                if (debug >= 1)
                    log("status.setContentType", t);
            }
            PrintWriter writer = response.getReporter();
            if (writer != null) {
                writer.println("<html>");
                writer.println("<head>");
                writer.println("<title>" +
                               sm.getString("standardWrapper.statusTitle") +
                               "</title>");
                writer.println("</head>");
                writer.println("<body bgcolor=\"white\">");
                writer.println("<br><br>");
                writer.println("<h1>" +
                               sm.getString("standardWrapper.statusHeader",
                                            "" + statusCode, message) +
                               "</h1>");
                writer.println(report);
                writer.println("</body>");
                writer.println("</html>");
                writer.flush();
            }
        } catch (IllegalStateException e) {
            if (debug >= 1)
                log("status.write", e);
        }


    }


}
