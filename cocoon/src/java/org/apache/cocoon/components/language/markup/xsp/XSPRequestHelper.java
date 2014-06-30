/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup.xsp;

import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Session;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.Map;

/**
 * The <code>Request</code> object helper
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:10 $
 */
public class XSPRequestHelper {
  /**
   * Assign values to the object's namespace uri and prefix
   */
  private static final String URI = Constants.XSP_REQUEST_URI;
  private static final String PREFIX = Constants.XSP_REQUEST_PREFIX;

  /**
   * Output the uri associated with the given <code>Request</code>
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getRequestURI(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.elementData(URI, PREFIX, contentHandler, "request-uri", request.getRequestURI());
    }

  /**
   * Output the uri associated with the given objectModel
   *
   * @param objectModel The Map objectModel
   */
  public static String getURI(
      Map objectModel
      )
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getRequestURI();
  }

  /**
   * Output the uri associated with the given objectModel
   *
   * @param objectModel The Map objectModel
   */
  public static void getURI(
      Map objectModel,
      ContentHandler handler
      )
    throws SAXException
    {
      XSPObjectHelper.elementData(URI, PREFIX, handler, "uri", 
          getURI(objectModel));
    }

  /**
   * Outputs the sitemap uri associated with the given objectModel
   *
   * @param objectModel the Map objectModel
   */
  public static String getSitemapURI(Map objectModel) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getSitemapURI();
  }

  /**
   * Outputs the sitemap uri associated with the given objectModel
   *
   * @param objectModel the Map objectModel
   */
  public static void getSitemapURI(Map objectModel, ContentHandler handler)
    throws SAXException {
      XSPObjectHelper.elementData(URI, PREFIX, handler, "sitemap-uri", 
          getSitemapURI(objectModel));
    }

  /**
   * Return the given request parameter value or a user-provided default if
   * none was specified.
   *
   * @param objectModel The Map objectModel
   * @param name The parameter name
   * @param defaultValue Value to substitute in absence of a parameter value
   */

  public static String getParameter(
      Map objectModel,
      String name,
      String defaultValue
      ) {
    return getParameter(objectModel, name, defaultValue, null, null);
  }

  /**
   * Return the given request parameter value or a user-provided default if
   * none was specified.
   *
   * @param objectModel The Map objectModel
   * @param name The parameter name
   * @param defaultValue Value to substitute in absence of a parameter value
   * @param form_encoding The supposed encoding of the request parameter.
   * @param container_encoding The encoding used by container.
   */
  public static String getParameter(
      Map objectModel,
      String name,
      String defaultValue,
      String form_encoding,
      String container_encoding
      ) {
    if(container_encoding == null) 
      container_encoding = "ISO-8859-1"; // default per Servlet spec

    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    String value = request.getParameter(name);
    if(form_encoding != null && value != null && value.length() > 0) {
      try {
        value = new String(value.getBytes(container_encoding), form_encoding);
      } catch(java.io.UnsupportedEncodingException uee) {
        throw new RuntimeException("Unsupported Encoding Exception: " +
            uee.getMessage());
      }
    }

    if (value == null) {
      value = defaultValue;
    }

    return value;
  }

  /**
   * Output the given request parameter value or a user-provided default if
   * none was specified.
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @param name The parameter name
   * @param defaultValue Value to substitute in absence of a parameter value
   * @exception SAXException If a SAX error occurs
   */
  public static void getParameter(
      Map objectModel,
      ContentHandler contentHandler,
      String name,
      String defaultValue
      )
    throws SAXException
    {
      AttributesImpl attr = new AttributesImpl();
      XSPObjectHelper.addAttribute(attr, "name", name);

      XSPObjectHelper.elementData(
          URI,
          PREFIX,
          contentHandler,
          "parameter",
          getParameter(objectModel, name, defaultValue, null, null),
          attr
          );
    }

  /**
   * Output the given request parameter value or a user-provided default if
   * none was specified.
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @param name The parameter name
   * @param defaultValue Value to substitute in absence of a parameter value
   * @param form_encoding The supposed encoding of the request parameter.
   * @param container_encoding The encoding used by container.
   * @exception SAXException If a SAX error occurs
   */
  public static void getParameter(
      Map objectModel,
      ContentHandler contentHandler,
      String name,
      String defaultValue,
      String form_encoding,
      String container_encoding
      )
    throws SAXException
    {
      AttributesImpl attr = new AttributesImpl();
      XSPObjectHelper.addAttribute(attr, "name", name);

      XSPObjectHelper.elementData(
          URI,
          PREFIX,
          contentHandler,
          "parameter",
          getParameter(objectModel, name, defaultValue, 
            form_encoding, container_encoding),
          attr
          );
    }

  /**
   * Output the request parameter values for a given name
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getParameterValues(
      Map objectModel,
      ContentHandler contentHandler,
      String name
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      AttributesImpl attr = new AttributesImpl();
      XSPObjectHelper.addAttribute(attr, "name", name);

      XSPObjectHelper.start(URI, PREFIX, contentHandler, 
          "parameter-values", attr);

      String[] values = request.getParameterValues(name);

      if (values != null) {
        for (int i = 0; i < values.length; i++) {
          XSPObjectHelper.elementData(URI, PREFIX, contentHandler, 
              "value", values[i]);
        }
      }

      XSPObjectHelper.end(URI, PREFIX, contentHandler, "parameter-values");
    }

  /**
   * Output the request parameter values for a given name
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @param form_encoding The supposed encoding of the request parameter.
   * @param container_encoding The encoding used by container.
   * @exception SAXException If a SAX error occurs
   */
  public static void getParameterValues(
      Map objectModel,
      ContentHandler contentHandler,
      String name,
      String form_encoding,
      String container_encoding
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      AttributesImpl attr = new AttributesImpl();
      XSPObjectHelper.addAttribute(attr, "name", name);

      XSPObjectHelper.start(URI, PREFIX, contentHandler, 
          "parameter-values", attr);

      String[] values = request.getParameterValues(name);

      if (values != null) {
        for (int i = 0; i < values.length; i++) {
          if(form_encoding != null && values[i] != null && 
              values[i].length() > 0) {
            try {
              values[i] = new String(values[i].getBytes(container_encoding), 
                  form_encoding);
            } catch(java.io.UnsupportedEncodingException uee) {
              throw new RuntimeException("Unsupported Encoding Exception: " +
                  uee.getMessage());
            }
          }
          XSPObjectHelper.elementData(URI, PREFIX, contentHandler, 
              "value", values[i]);
        }
      }

      XSPObjectHelper.end(URI, PREFIX, contentHandler, "parameter-values");
    }

  /**
   * return the request parameter values for a given name as array
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @param form_encoding The supposed encoding of the request parameter.
   * @param container_encoding The encoding used by container.
   * @return Array containing requested values.
   */
  public static String[] getParameterValues(
      Map objectModel,
      String name,
      String form_encoding,
      String container_encoding
      )
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);

      String[] values = request.getParameterValues(name);

      if (values != null) {
        for (int i = 0; i < values.length; i++) {
          if(form_encoding != null && values[i] != null && 
              values[i].length() > 0) {
            try {
              values[i] = new String(values[i].getBytes(container_encoding), 
                  form_encoding);
            } catch(java.io.UnsupportedEncodingException uee) {
              throw new RuntimeException("Unsupported Encoding Exception: " +
                  uee.getMessage());
            }
          }
        }
      }
      return values;
    }

  /**
   * Output parameter names for a given request
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getParameterNames(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.start(URI, PREFIX, contentHandler, "parameter-names");

      Enumeration e = request.getParameterNames();
      while (e.hasMoreElements()) {
        String name = (String) e.nextElement();
        XSPObjectHelper.elementData(URI, PREFIX, contentHandler, "name", name);
      }

      XSPObjectHelper.end(URI, PREFIX, contentHandler, "parameter-names");
    }

  /**
   * Return the request header value for a given name
   *
   * @param objectModel The Map objectModel
   * @param name The parameter name
   */
  public static String getHeader(
      Map objectModel,
      String name
      ) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getHeader(name);
  }

  /**
   * Output the request header value for a given name
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @param name The parameter name
   * @exception SAXException If a SAX error occurs
   */
  public static void getHeader(
      Map objectModel,
      ContentHandler contentHandler,
      String name
      )
    throws SAXException
    {
      AttributesImpl attr = new AttributesImpl();
      XSPObjectHelper.addAttribute(attr, "name", name);

      String value = getHeader(objectModel, name);
      if (value == null) {
        value = "";
      }

      XSPObjectHelper.elementData(
          URI,
          PREFIX,
          contentHandler,
          "header",
          value,
          attr
          );
    }

  /**
   * Output the header names for a given request
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getHeaderNames(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.start(URI, PREFIX, contentHandler, "header-names");

      Enumeration e = request.getHeaderNames();
      while (e.hasMoreElements()) {
        String name = (String) e.nextElement();
        XSPObjectHelper.elementData(URI, PREFIX, contentHandler, "name", name);
      }

      XSPObjectHelper.end(URI, PREFIX, contentHandler, "header-names");
    }



  /**
   * Sets the given session attribute value
   *
   * @param objectModel The Map objectModel
   * @param name The parameter name
   * @param content The parameter value
   */
  public static void setSessionAttribute(
      Map objectModel, 
      String name, 
      Object content
      ) 
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    Session session = request.getSession(false);
    session.setAttribute(name, content);
  }

  /**
   * Return the given session attribute value or a user-provided default if
   * none was specified.
   *
   * @param objectModel The Map objectModel
   * @param name The parameter name
   */
  public static Object getSessionAttribute(
      Map objectModel,
      String name) {

    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    Session session = request.getSession(false);
    return session.getAttribute(name);
  }

  /**
   * Return the given session attribute value or a user-provided default if
   * none was specified.
   *
   * @param objectModel The Map objectModel
   * @param name The parameter name
   * @param defaultValue Value to substitute in absence of a parameter value
   */
  public static Object getSessionAttribute(
      Map objectModel,
      String name,
      Object defaultValue
      ) 
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    Session session = request.getSession(false);
    Object value = null;

    if (session != null) {
      value = session.getAttribute(name);
    }

    if (value == null) {
      value = defaultValue;
    }

    return value;
  }

  /**
   * Remove the specified attribute
   *
   * @param objectModel The Map objectModel
   * @param name The parameter name
   */
  public static void removeAttribute(
      Map objectModel,
      String name
      ) 
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    request.removeAttribute(name);
  }

  /**
   * Get the specified attribute
   *
   * @param objectModel The Map objectModel
   * @param name The parameter name
   */
  public static Object getAttribute(
      Map objectModel,
      String name) {

    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getAttribute(name);
  }

  /**
   * Output attribute names for a given request
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getAttributeNames(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.start(URI, PREFIX, contentHandler, "attribute-names");

      Enumeration e = request.getAttributeNames();
      while (e.hasMoreElements()) {
        String name = (String) e.nextElement();
        XSPObjectHelper.elementData(URI, PREFIX, contentHandler, "name", name);
      }

      XSPObjectHelper.end(URI, PREFIX, contentHandler, "attribute-names");
    }

  /**
   * Set the specified attribute
   *
   * @param objectModel The Map objectModel
   * @param name The parameter name
   */
  public static void setAttribute(
      Map objectModel,
      String name,
      Object value) {

    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    request.setAttribute(name, value);
  }



  /**
   * Output the login of the user making the request
   * Could be null if user is not authenticated.
   *
   * @param objectModel The Map objectModel
   */
  public static String getRemoteUser(
      Map objectModel
      )
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getRemoteUser();
  }

  /**
   * Output the login of the user making the request
   * Could be null if user is not authenticated.
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getRemoteUser(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.elementData(URI, PREFIX, contentHandler, 
          "remote-user", request.getRemoteUser());
    }


  /**
   * Output the name of the HTTP method with which the request was made,
   *
   * @param objectModel The Map objectModel
   */
  public static String getMethod(
      Map objectModel
      )
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getMethod();
  }

  /**
   * Output the name of the HTTP method with which the request was made,
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getMethod(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.elementData(URI, PREFIX, contentHandler, 
          "method", request.getMethod());
    }

  /**
   * Output the query string that is contained in the request URL after the path,
   *
   *
   * @param objectModel The Map objectModel
   */
  public static String getQueryString(
      Map objectModel
      )
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getQueryString();
  }

  /**
   * Output the query string that is contained in the request URL after the path,
   *
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getQueryString(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.elementData(URI, PREFIX, contentHandler, 
          "query-string", request.getQueryString());
    }

  /**
   * Output the name and version of the protocol the request uses in the form of
   * protocol/majorVersion.minorVersion,
   *
   * @param objectModel The Map objectModel
   */
  public static String getProtocol(
      Map objectModel
      )
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getProtocol();
  }

  /**
   * Output the name and version of the protocol the request uses in the form of
   * protocol/majorVersion.minorVersion,
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getProtocol(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.elementData(URI, PREFIX, contentHandler, "protocol", request.getProtocol());
    }

  /**
   * Output the fully qualified name of the client that sent the request, or
   * the IP address of the client if the name cannot be determined, given
   * <code>Request</code>
   *
   * @param objectModel The Map objectModel
   */
  public static String getRemoteHost(
      Map objectModel
      )
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getRemoteHost();
  }

  /**
   * Output the fully qualified name of the client that sent the request, or
   * the IP address of the client if the name cannot be determined, given
   * <code>Request</code>
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getRemoteHost(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.elementData(URI, PREFIX, contentHandler, "remote-host", request.getRemoteHost());
    }

  /**
   * Output the IP address of the client that sent the request
   *
   * @param objectModel The Map objectModel
   */
  public static String getRemoteAddr(
      Map objectModel
      )
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getRemoteAddr();
  }

  /**
   * Output the IP address of the client that sent the request
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getRemoteAddr(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.elementData(URI, PREFIX, contentHandler, 
          "remote-address", request.getRemoteAddr());
    }


  /**
   * Checks the secure flag
   *
   * @param objectModel The Map objectModel
   */
  public static boolean isSecure(
      Map objectModel) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.isSecure();
  }


  /**
   * Remove the specified attribute
   *
   * @param objectModel The Map objectModel
   */
  public static String getServerName(
      Map objectModel) {

    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getServerName();
  }

  /**
   * Remove the specified attribute
   *
   * @param objectModel The Map objectModel
   */
  public static int getServerPort(
      Map objectModel) {

    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getServerPort();
  }

  public static String getRequestedURL(Map objectModel) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    StringBuffer uribuf = null;
    boolean isSecure = request.isSecure();
    int port = request.getServerPort();

    if (isSecure) {
      uribuf = new StringBuffer("https://");
    } else {
      uribuf = new StringBuffer("http://");
    }

    uribuf.append(request.getServerName());
    if (isSecure) {
      if (port != 443) {
        uribuf.append(":").append(port);
      }
    } else {
      if (port != 80) {
        uribuf.append(":").append(port);
      }
    }

    uribuf.append(request.getRequestURI());
    return uribuf.toString();
  }

  /**
   * Get the session attribute names.
   *
   * @param objectModel The Map objectModel
   */
  public static List getSessionAttributeNames (
      Map objectModel) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    ArrayList v = new ArrayList();
    Enumeration e = request.getSession().getAttributeNames();

    while (e.hasMoreElements()) {
      v.add(e.nextElement());
    }
    return v;
  }

  /**
   * Output the given session attribute value or a user-provided default if
   * none was specified.
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @param name The parameter name
   * @param defaultValue Value to substitute in absence of a parameter value
   * @exception SAXException If a SAX error occurs
   */
  public static void getSessionAttribute(
      Map objectModel,
      ContentHandler contentHandler,
      String name,
      Object defaultValue
      )
    throws SAXException
    {
      AttributesImpl attr = new AttributesImpl();
      XSPObjectHelper.addAttribute(attr, "name", name);

      XSPObjectHelper.elementData(
          URI,
          PREFIX,
          contentHandler,
          "attribute",
          (String) getSessionAttribute(objectModel, name, defaultValue),
          attr
          );
    }


  /**
   * Get the session creation time
   *
   * @param objectModel The Map objectModel
   */
  public static long getSessionCreationTime (
      Map objectModel) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getSession().getCreationTime();
  }

  /**
   * Get the session id
   *
   * @param objectModel The Map objectModel
   */
  public static String getSessionId (
      Map objectModel) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getSession().getId();
  }

  /**
   * Get the session last accessed time
   *
   * @param objectModel The Map objectModel
   */
  public static long getSessionLastAccessedTime (
      Map objectModel) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getSession().getLastAccessedTime();
  }

  /**
   * Get the session max inactive interval
   *
   * @param objectModel The Map objectModel
   */
  public static long getSessionMaxInactiveInterval (
      Map objectModel) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getSession().getMaxInactiveInterval();
  }

  /**
   * Set the session max inactive interval
   *
   * @param objectModel The Map objectModel
   * @param interval max inactive interval
   */
  public static void setSessionMaxInactiveInterval (
      Map objectModel,
      int interval) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    request.getSession().setMaxInactiveInterval(interval);
  }

  /**
   * Invalidate the session
   *
   * @param objectModel The Map objectModel
   */
  public static void invalidateSession (
      Map objectModel) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    request.getSession().invalidate();
  }


  /**
   * Checks the isNew flag
   *
   * @param objectModel The Map objectModel
   */
  public static boolean isSessionNew(
      Map objectModel) {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getSession().isNew();
  }

  /**
   * Remove the specified attribute
   *
   * @param objectModel The Map objectModel
   * @param name The parameter name
   */
  public static void removeSessionAttribute(
      Map objectModel,
      String name) {

    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    request.getSession().removeAttribute(name);
  }

  /**
   * Obtain the portion of the request URI that indicates the context
   * of the request
   *
   * @param objectModel The Map objectModel
   */
  public static String getContextPath(
      Map objectModel
      )
  {
    Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
    return request.getContextPath();
  }

  /**
   * Obtain the portion of the request URI that indicates the context
   * of the request
   *
   * @param objectModel The Map objectModel
   * @param contentHandler The SAX content handler
   * @exception SAXException If a SAX error occurs
   */
  public static void getContextPath(
      Map objectModel,
      ContentHandler contentHandler
      )
    throws SAXException
    {
      Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
      XSPObjectHelper.elementData(URI, PREFIX, contentHandler, 
          "context-path", request.getContextPath());
    }
}
