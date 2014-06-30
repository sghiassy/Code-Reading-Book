/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
 
package org.apache.cocoon.components.notification;

import org.apache.avalon.framework.CascadingRuntimeException;

import java.util.Map;

import java.io.PrintStream;
import java.io.PrintWriter;

/**
 * A CascadingRuntimeException that is also Notifying.
 *
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @created 7 December 2001
 */
public class NotifyingCascadingRuntimeException 
  extends CascadingRuntimeException 
    implements Notifying{

  /**
   * The Notifying Object used internally to keep Notifying fields
   */
  Notifying n;

  /**
   * Construct a new <code>NotifyingCascadingRuntimeException</code> instance.
   */
  public NotifyingCascadingRuntimeException(String message) {
    super(message, null);
    n = new DefaultNotifyingBuilder().build(this, message);
  }

  /**
   * Creates a new <code>ProcessingException</code> instance.
   *
   * @param ex an <code>Exception</code> value
   */
  public NotifyingCascadingRuntimeException(Exception ex) {
    super(ex.getMessage(), ex);
    n = new DefaultNotifyingBuilder().build(this, ex);
  }

  /**
   * Construct a new <code>ProcessingException</code> that references
   * a parent Exception.
   */
  public NotifyingCascadingRuntimeException(String message, Throwable t) {
    super(message, t);
    n = new DefaultNotifyingBuilder().build(this, t);
  }

  /**
   *  Gets the Type attribute of the Notifying object
   */
  public String getType() {
    return n.getType();
  }

  /**
   *  Gets the Title attribute of the Notifying object
   */
  public String getTitle() {
    return n.getTitle();
  }

  /**
   *  Gets the Source attribute of the Notifying object
   */
  public String getSource() {
    return n.getSource();
  }

  /**
   *  Gets the Sender attribute of the Notifying object
   */
  public String getSender() {
    return n.getSender();
  }

  /**
   *  Gets the Message attribute of the Notifying object
   */
  public String getMessage() {
    return n.getMessage();
  }

  /**
   *  Gets the Description attribute of the Notifying object
   */
  public String getDescription() {
    return n.getDescription();
  }

  /**
   *  Gets the ExtraDescriptions attribute of the Notifying object
   */
  public Map getExtraDescriptions() {
    return n.getExtraDescriptions();
  }

}

