/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.xscript;

import java.util.Stack;
import java.io.StringReader;
import java.io.StringBufferInputStream;
import java.io.InputStream;
import java.io.IOException;

import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.SAXException;
import org.xml.sax.InputSource;
import org.xml.sax.ContentHandler;
import org.xml.sax.Attributes;

import org.apache.cocoon.ProcessingException;

/**
 * An <code>XScriptObject</code> created from an inline XML fragment.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since July 7, 2001
 */
public class XScriptObjectInlineXML extends XScriptObject
{
  StringBuffer stringBuffer;
  StringBufferContentHandler streamHandler;

  public XScriptObjectInlineXML(XScriptManager manager)
  {
    super(manager);
    stringBuffer = new StringBuffer();
    stringBuffer.append("<?xml version=\"1.0\"?>\n\n");
    streamHandler = new StringBufferContentHandler(stringBuffer);
  }
  
  public XScriptObjectInlineXML(XScriptManager manager,
                                StringBuffer stringBuffer)
  {
    super(manager);
    this.stringBuffer = stringBuffer;
    streamHandler = new StringBufferContentHandler(stringBuffer);
  }

  public XScriptObjectInlineXML(XScriptManager manager, String string)
  {
    super(manager);
    this.stringBuffer = new StringBuffer(string);
    streamHandler = new StringBufferContentHandler(stringBuffer);
  }

  public InputStream getInputStream()
    throws ProcessingException, IOException
  {
    return new StringBufferInputStream(stringBuffer.toString());
  }
  
  public ContentHandler getContentHandler()
  {
    return streamHandler;
  }

  public void setNextContentHandler(ContentHandler handler)
  {
    streamHandler.setNextContentHandler(handler);
  }

  public String toString()
  {
    return stringBuffer.toString();
  }

  public long getContentLength()
  {
    return stringBuffer.length();
  }

  public String getSystemId()
  {
    // FIXME: Implement a URI scheme to be able to refer to XScript
    // variables by URI
    return "file:/";
  }
}
