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
import java.io.IOException;

import org.xml.sax.InputSource;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.Attributes;
import org.xml.sax.helpers.DefaultHandler;


/**
 * A <code>ContentHandler</code> that accumulates the SAX stream into
 * a <code>StringBuffer</code> object.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since August 30, 2001
 */
public class StringBufferContentHandler extends DefaultHandler
{
  private Stack namespaces = new Stack();
  ContentHandler contentHandler;
  StringBuffer stringBuffer;
  static Marker marker = new Marker();

  public StringBufferContentHandler(StringBuffer stringBuffer)
  {
    this.stringBuffer = stringBuffer;
  }

  public void setNextContentHandler(ContentHandler handler)
  {
    this.contentHandler = handler;
  }

  public void startPrefixMapping(String prefix, String uri)
    throws SAXException
  {
    namespaces.push(new NPU(prefix, uri));
  }

  public void endPrefixMapping(String prefix)
    throws SAXException
  {
    namespaces.pop();
  }

  public void startElement(String uri, String loc, String qName, Attributes a)
    throws SAXException
  {
    int lastNamespaceIndex = 0;

    lastNamespaceIndex = namespaces.size() - 1;
    while(lastNamespaceIndex >= 0
          && namespaces.elementAt(lastNamespaceIndex) != marker)
      lastNamespaceIndex--;

    if (lastNamespaceIndex < 0)
      lastNamespaceIndex = 0;
    else if (namespaces.elementAt(lastNamespaceIndex) == marker)
      lastNamespaceIndex++;

    namespaces.push(marker);

    stringBuffer.append("<").append(qName);

    for (int i = 0, len = a.getLength(); i < len; i++) {
      // Check if the attribute is a namespace declaration. Some
      // parsers (Xerces) sometimes pass the namespace declaration
      // as an attribute. We need to catch this case so that we
      // don't end up generating the namespace declaration twice.
      String attrName = a.getQName(i);

      if (attrName.startsWith("xmlns:")) {
        // We have a namespace declaration
        String name = a.getLocalName(i);

        // Check whether this namespace has been already declared
        boolean found = false;
        for (int j = namespaces.size() - 1;
             j >= lastNamespaceIndex;
             j--) {
          Object obj = namespaces.elementAt(j);
          if (obj == marker)
            continue;
          NPU npu = (NPU)obj;
          
          if (name.equals(npu.prefix)) {
            found = true;
            break;
          }
        }

        if (!found) {
          namespaces.push(new NPU(name, a.getValue(i)));
        }
      }
      else {
        // Normal attribute
        stringBuffer.append(" ").append(a.getQName(i))
                            .append("=\"").append(a.getValue(i)).append("\"");
      }
    }

    if (namespaces.size() != 0) {
      for (int i = namespaces.size() - 1; i >= lastNamespaceIndex; i--) {
        Object obj = namespaces.elementAt(i);
        if (obj == marker)
          continue;
        NPU npu = (NPU)obj;
        stringBuffer.append(" xmlns:").append(npu.prefix).append("=\"").append(npu.uri).append("\"");
      }
    }
    stringBuffer.append(">");
  }

  public void endElement(String uri, String loc, String qName)
    throws SAXException 
  {
    stringBuffer.append("</").append(qName).append(">");

    Object obj;
    do {
      obj = namespaces.pop();
    } while (obj != marker);
  }
    
  public void characters(char ch[], int start, int len)
    throws SAXException 
  {
    stringBuffer.append(ch, start, len);
  }
}

class NPU
{
  public String prefix;
  public String uri;

  NPU(String prefix, String uri)
  {
    this.prefix = prefix;
    this.uri = uri;
  }

  public String toString()
  {
    return new StringBuffer(this.prefix).append("=").append(this.uri).toString();
  }
}

class Marker
{
}
