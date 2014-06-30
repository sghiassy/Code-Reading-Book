/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.xscript;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import javax.xml.transform.Result;
import javax.xml.transform.sax.SAXResult;
import javax.xml.transform.stream.StreamResult;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.url.URLFactory;

/**
 * An <code>XScriptObject</code> created from a JAXP
 * <code>Result</code> object.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since   August 30, 2001
 */
public class XScriptObjectResult extends XScriptObject
{
  /**
   * The XML content of this object.
   */
  String content;
  
  public XScriptObjectResult(XScriptManager manager, String content)
  {
    super(manager);
    this.content = content;
  }

  public InputStream getInputStream()
    throws ProcessingException, IOException
  {
    return new ByteArrayInputStream(content.getBytes());
  }

  public long getContentLength()
  {
    return content.length();
  }

  public String getSystemId()
  {
    // FIXME: Return a real URL that identifies this object
    return "file:/";
  }

  public String toString()
  {
    return "XScriptObjectResult: " + content;
  }
}
