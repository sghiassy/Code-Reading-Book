/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 **************************************************************************** *
 */

package org.apache.cocoon.components.notification;

import org.apache.avalon.framework.CascadingThrowable;
import org.apache.avalon.framework.component.Component;

import org.xml.sax.SAXException;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Map;

/**
 *  Generates an Notifying representation of widely used objects.
 *
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @created 24 August 2000
 */
public class DefaultNotifyingBuilder implements NotifyingBuilder, Component {

  /** Builds a Notifying object (SimpleNotifyingBean in this case)
   *  that tries to explain what the Object o can reveal.
   * @param sender who sent this Object.
   * @param o the object to use when building the SimpleNotifyingBean
   * @return the  Notifying Object that was build
   * @see org.apache.cocoon.components.notification.Notifying   
   */
  public Notifying build (Object sender, Object o) {
    if (o instanceof Notifying) {
      return (Notifying) o;
    } else if (o instanceof Throwable) {
      Throwable t = (Throwable) o;
      SimpleNotifyingBean n = new SimpleNotifyingBean(sender);
      n.setType("error");
      n.setTitle("An error occurred");
      if (t != null) {
        n.setSource(t.getClass().getName());
        n.setMessage(t.getMessage());
        n.setDescription(t.toString());

        Throwable rootT = getRootCause(t);
        n.addExtraDescription("original message", rootT.toString());

        // get the stacktrace: if the exception is a SAXException,
        // the stacktrace of the embedded exception is used as the
        // SAXException does not append it automatically
        Throwable stackTraceException;
        if (t instanceof SAXException && ((SAXException) t).getException() != null) {
          stackTraceException = ((SAXException) t).getException();
        } else {
          stackTraceException = t;
        }
        //org.apache.avalon.framework.ExceptionUtil.captureStackTrace();
        StringWriter sw = new StringWriter();
        stackTraceException.printStackTrace(new PrintWriter(sw));
        n.addExtraDescription("stacktrace", sw.toString());
        // Add nested throwables description
        sw = new StringWriter();
        appendCauses(new PrintWriter(sw), stackTraceException);
        String causes = sw.toString();
        if (causes != null && causes.length() != 0) {
          n.addExtraDescription("full exception chain stacktrace", causes);
        }
      }
      return n;
    } else {
      SimpleNotifyingBean n = new SimpleNotifyingBean(sender);
      n.setType("unknown");
      n.setTitle("Object notification");
      n.setSource(o.getClass().getName());
      n.setMessage(o.toString());
      n.setDescription("No details available.");
      return n;
    }
  }

  /** Builds a Notifying object (SimpleNotifyingBean in this case)
   *  that explains a notification.
   * @param sender who sent this Object.
   * @param o the object to use when building the SimpleNotifyingBean
   * @param type see the Notifying apidocs
   * @param title see the Notifying apidocs
   * @param source see the Notifying apidocs
   * @param message see the Notifying apidocs
   * @param description see the Notifying apidocs
   * @param extra see the Notifying apidocs   
   * @return the  Notifying Object that was build
   * @see org.apache.cocoon.components.notification.Notifying
   */
  public Notifying build(Object sender, Object o, String type, String title,
          String source, String message, String description, Map extra) {
    //NKB Cast here is secure, the method is of this class
    SimpleNotifyingBean n = (SimpleNotifyingBean) build (sender, o);

    if (type != null)
      n.setType(type);
    if (title != null)
      n.setTitle(title);
    if (source != null)
      n.setSource(source);
    if (message != null)
      n.setMessage(message);
    if (description != null)
      n.setDescription(description);
    if (extra != null)
      n.replaceExtraDescriptions(extra);

    return n;
  }


  /**
   * Print recursively all nested causes of a Throwable in a PrintWriter.
   */
  private static void appendCauses (PrintWriter out, Throwable t) {
    Throwable cause = null;
    if (t instanceof CascadingThrowable) {
      cause = ((CascadingThrowable) t).getCause();
    } else if (t instanceof SAXException) {
      cause = ((SAXException) t).getException();
    } else if (t instanceof java.sql.SQLException) {
      cause = ((java.sql.SQLException) t).getNextException();
    }
    if (cause != null) {
      out.print("Original exception : ");
      cause.printStackTrace(out);
      out.println();
      // Recurse
      appendCauses(out, cause);
    }
  }

  /**
   * Get root Exception.
   */
  private static Throwable getRootCause (Throwable t) {
    Throwable cause = null;
    if (t instanceof CascadingThrowable) {
      cause = ((CascadingThrowable) t).getCause();
    } else if (t instanceof SAXException) {
      cause = ((SAXException) t).getException();
    } else if (t instanceof java.sql.SQLException) {
      cause = ((java.sql.SQLException) t).getNextException();
    }
    if (cause == null) {
      return t;
    } else {
      // Recurse
      return getRootCause(cause);
    }
  }
}

