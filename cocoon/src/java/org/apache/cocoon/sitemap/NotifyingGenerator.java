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
import org.apache.cocoon.components.notification.Notifier;
import org.apache.cocoon.generation.ComposerGenerator;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.Constants;

import org.xml.sax.SAXException;

/**
 * Generates an XML representation of the current notification.
 *
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @author <a href="mailto:proyal@managingpartners.com">Peter Royal</a>
 * @created 31 July 2000
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:20 $
 */
public class NotifyingGenerator extends ComposerGenerator implements Recyclable {
  /**
   * The <code>Notification</code> to report.
   */
  private Notifying notification = null;

  public void setup(SourceResolver resolver, Map objectModel, String src,
          Parameters par) throws ProcessingException, SAXException, IOException {
    super.setup(resolver, objectModel, src, par);

    notification  = (Notifying)objectModel.get(Constants.NOTIFYING_OBJECT);
    
    if ( notification  == null) {
      throw new ProcessingException("Expected Constants.NOTIFYING_OBJECT not found in object model");
    }

  }

  /**
   * Generate the notification information in XML format.
   *
   * @exception  SAXException  Description of problem there is creating the output SAX events.
   * @throws SAXException when there is a problem creating the
   *      output SAX events.
   */
  public void generate() throws SAXException {
    Notifier.notify(notification, this.contentHandler);
  }

  /**
   * Recycle
   */
  public void recycle() {
    super.recycle();
    this.notification = null;
  }
}

