/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.notification;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Map;
import java.util.HashMap;

/**
 *  A simple bean implementation of Notifying.
 *
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @created 24 August 2000
 */

public class SimpleNotifyingBean implements Notifying {

  /**
   *  The type of the notification. Examples can be "warning" or "error"
   */
  private String type = "unknown";

  /**
   *  The title of the notification.
   */
  private String title = "";

  /**
   *  The source that generates the notification.
   */
  private String source = "";

  /**
   *  The sender of the notification.
   */
  private String sender = "";

  /**
   *  The notification itself.
   */
  private String message = "Message not known.";

  /**
   *  A more detailed description of the notification.
   */
  private String description = "No details available.";

  /**
   *  A HashMap containing extra notifications
   */
  private HashMap extraDescriptions = new HashMap();

    public SimpleNotifyingBean() {
    this.sender = "unknown";
    setSource(this.getClass().getName());
    setTitle("Generic notification");
  }
  
  public SimpleNotifyingBean(Object sender) {
    this.sender = sender.getClass().getName();
    setSource(this.getClass().getName());
    setTitle("Generic notification");
  }

  /**
   *  Sets the Type of the SimpleNotifyingBean object
   *
   *@param  type  The new Type value
   */
  public void setType(String type) {
    this.type = type;
  }

  /**
   *  Sets the Title of the SimpleNotifyingBean object
   *
   *@param  title  The new Title value
   */
  public void setTitle(String title) {
    this.title = title;
  }

  /**
   *  Sets the Source of the SimpleNotifyingBean object
   *
   *@param  source  The new Source value
   */
  public void setSource(String source) {
    this.source = source;
  }

  /**
   *  Sets the Sender of the SimpleNotifyingBean object
   *
   *@param  sender  The new sender value
   */
  private void setSender(Object sender) {
    this.sender = sender.getClass().getName();
  }

  /**
   *  Sets the Sender of the SimpleNotifyingBean object
   *
   *@param  sender  The new sender value
   */
  private void setSender(String sender) {
    this.sender = sender;
  }

  /**
   *  Sets the Message of the SimpleNotifyingBean object
   *
   *@param  message  The new Message value
   */
  public void setMessage(String message) {
    this.message = message;
  }

  /**
   *  Sets the Description of the SimpleNotifyingBean object
   *
   *@param  description  The new Description value
   */
  public void setDescription(String description) {
    this.description = description;
  }

  /**
   *  Sets the ExtraDescriptions of the SimpleNotifyingBean object
   *
   *@param  extraDescriptions  The new ExtraDescriptions value
   */
  public void addExtraDescription(String extraDescriptionDescription,
          String extraDescription) {
    this.extraDescriptions.put(extraDescriptionDescription, extraDescription);
  }

  /**
   *  replaces the ExtraDescriptions of the SimpleNotifyingBean object
   */
  protected void replaceExtraDescriptions(Map extraDescriptions) {
    this.extraDescriptions = new HashMap(extraDescriptions);
  }

  /**
   *  replaces the ExtraDescriptions of the SimpleNotifyingBean object
   */
  protected void replaceExtraDescriptions(HashMap extraDescriptions) {
    this.extraDescriptions = extraDescriptions;
  }
  
  /**
   *  Gets the Type of the SimpleNotifyingBean object
   */
  public String getType() {
    return type;
  }

  /**
   *  Gets the Title of the SimpleNotifyingBean object
   */
  public String getTitle() {
    return title;
  }

  /**
   *  Gets the Source of the SimpleNotifyingBean object
   */
  public String getSource() {
    return source;
  }

  /**
   *  Gets the Sender of the SimpleNotifyingBean object
   */
  public String getSender() {
    return sender;
  }

  /**
   *  Gets the Message of the SimpleNotifyingBean object
   */
  public String getMessage() {
    return message;
  }

  /**
   *  Gets the Description of the SimpleNotifyingBean object
   */
  public String getDescription() {
    return description;
  }

  /**
   *  Gets the ExtraDescriptions of the SimpleNotifyingBean object
   */
  public Map getExtraDescriptions() {
    return extraDescriptions;
  }
}



