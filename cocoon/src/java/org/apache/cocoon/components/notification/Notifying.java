/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.notification;

import java.util.Map;

/**
 *  Interface for Objects that can notify something.
 *
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @created 24 August 2000
 */

public interface Notifying {

  /**
   *  Proposed types of notifications
   */
  String UNKNOWN_NOTIFICATION = "unknown";
  String DEBUG_NOTIFICATION = "debug";
  String INFO_NOTIFICATION = "info" ;
  String WARN_NOTIFICATION = "warn" ;
  String ERROR_NOTIFICATION = "error";
  String FATAL_NOTIFICATION = "fatal";

  /**
   *  Gets the Type of the Notifying object
   */
  String getType();

  /**
   *  Gets the Title of the Notifying object
   */
  String getTitle();

  /**
   *  Gets the Source of the Notifying object
   */
  String getSource();

  /**
   *  Gets the Sender of the Notifying object
   */
  String getSender();

  /**
   *  Gets the Message of the Notifying object
   */
  String getMessage();

  /**
   *  Gets the Description of the Notifying object
   */
  String getDescription();

  /**
   *  Gets the ExtraDescriptions of the Notifying object
   */
  Map getExtraDescriptions();
}

