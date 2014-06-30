/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon;

import org.apache.cocoon.components.notification.SimpleNotifyingBean;

import org.apache.avalon.framework.CascadingThrowable;
import org.xml.sax.SAXException;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.HashMap;
import java.util.Map;


/**
 *  Generates an XML representation of the current notification.
 *
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @deprecated Now another interface is used
 * @see org.apache.components.notification.SimpleNotifyingBean
 * @created 24 August 2000
 */

public class Notification extends SimpleNotifyingBean {
  
    public Notification(Object sender) {
      super(sender);
  }

}

