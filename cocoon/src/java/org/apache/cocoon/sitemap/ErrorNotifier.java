/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import java.util.Map;
import java.io.IOException;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.components.notification.Notifying;
import org.apache.cocoon.components.notification.DefaultNotifyingBuilder;
import org.apache.cocoon.generation.ComposerGenerator;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.Constants;

import org.xml.sax.SAXException;

/**
 * Generates an XML representation of the current notification.
 * @deprecated Now using NotifyingGenerator in sitemap
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @created 31 July 2000
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:20 $
 */
public class ErrorNotifier extends NotifyingGenerator {
  /**
   * The <code>Notification</code> to report.
   */
  private Notifying notification = null;

  public void setup(SourceResolver resolver, Map objectModel, String src,
          Parameters par) throws ProcessingException, SAXException, IOException {
    try
    {
      super.setup(resolver, objectModel, src, par);
    }
    catch(ProcessingException pe)
    {
      //do nothing, thrown by superclass that didn't find the object to notify
      //it will be set in setThrowable in this class
    }
  }

  /**
   * Set the Notification to report.
   * @deprecated Now it gets the Exception from the ObjectModel
   * @param exception The Exception to report
   */
  public void setThrowable(Throwable throwable) {
    if (throwable instanceof Notifying) {
      this.notification = (Notifying) throwable;
    } else {
      notification = new DefaultNotifyingBuilder().build("@deprecated method setThrowable() in ErrorNotifier", throwable);
    }
  }

}

