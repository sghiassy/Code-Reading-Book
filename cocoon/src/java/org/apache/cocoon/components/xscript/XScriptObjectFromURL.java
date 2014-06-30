/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.xscript;

import java.net.URLConnection;
import java.net.URL;
import java.io.InputStream;
import java.io.IOException;

import org.apache.cocoon.components.url.URLFactory;
import org.apache.cocoon.ProcessingException;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.Component;

/**
 * An <code>XScriptObject</code> created from the contents of a URL.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since August 30, 2001
 */
public class XScriptObjectFromURL extends XScriptObject
{
  /**
   * The content obtained from this URL becomes the content of this
   * instance.
   */
  String systemId;

  /**
   * The content length.
   */
  int contentLength;

  /**
   * When was the content of the URL last modified.
   */
  long lastModified;
  
  public XScriptObjectFromURL(XScriptManager manager, String systemId)
  {
    super(manager);
    this.systemId = systemId;
  }

  public InputStream getInputStream()
    throws ProcessingException, IOException
  {
    try {
      URLFactory urlFactory = (URLFactory)componentManager.lookup(URLFactory.ROLE);
      URL url = urlFactory.getURL(systemId);
      URLConnection conn = url.openConnection();
      InputStream is = conn.getInputStream();
      contentLength = conn.getContentLength();
      componentManager.release((Component)urlFactory);

      return is;
    }
    catch (ComponentException ex) {
      throw new ProcessingException(ex);
    }
  }

  public long getContentLength()
  {
    return contentLength;
  }

  public long getLastModified()
  {
    return lastModified;
  }

  public String getSystemId()
  {
    // FIXME: generate a real system id to represent this object
    return "file:/";
  }

  public void recycle()
  {
  }

  public String toString()
  {
    return new StringBuffer("XScriptObjectFromURL(systemId = ").append(systemId).append(")").toString();
  }
}
