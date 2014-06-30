/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.notification;

import java.util.Map;

import org.apache.avalon.framework.component.Component;

/**
 *  Generates an Notifying representation of widely used objects.
 *
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @created 24 August 2000
 */

public interface NotifyingBuilder extends Component{

  /**
  * The role implemented by a <code>NotifyingBuilder</code>.
  */
  String ROLE = "org.apache.cocoon.components.notification.NotifyingBuilder";

  /** Builds a Notifying object (SimpleNotifyingObject in this case)
   *  that tries to explain what the Object o can reveal.
   * @param sender who sent this Object.
   * @param o the object to use when building the SimpleNotifyingObject
   * @return the  Notifying Object that was build
   * @see org.apache.cocoon.components.notification.Notifying
   */
  Notifying build(Object sender, Object o);


  /** Builds a Notifying object (SimpleNotifyingObject in this case)
   *  that explains a notification.
   * @param sender who sent this Object.
   * @param o the object to use when building the SimpleNotifyingObject
   * @param type see the Notifying apidocs
   * @param title see the Notifying apidocs
   * @param source see the Notifying apidocs
   * @param message see the Notifying apidocs
   * @param description see the Notifying apidocs
   * @param extra see the Notifying apidocs
   * @return the  Notifying Object that was build
   * @see org.apache.cocoon.components.notification.Notifying
   */
  Notifying build(Object sender, Object o, String type, String title,
          String source, String message, String description, Map extra);
}


