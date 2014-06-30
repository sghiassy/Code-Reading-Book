/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup.xsp;

import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.xml.XMLizable;
import org.apache.cocoon.xml.dom.DOMStreamer;
import org.w3c.dom.Node;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.util.Collection;
import java.util.Iterator;

/**
 * Base class for XSP's object model manipulation logicsheets
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @author <a href="mailto:sylvain.wallez@anyware-tech.com">Sylvain Wallez</a>
 *         (Cocoon1 <code>xspExpr()</code> methods port)
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:10 $
 */
public class XSPObjectHelper {
  /**
   * Empty attributes used for contentHandler.startElement()
   */
  protected static final AttributesImpl emptyAttr = new AttributesImpl();

  /**
   * Output an element containing text only and no attributes
   *
   * @param contentHandler The SAX content handler
   * @param name The element name
   * @param data The data contained by the element
   */
  protected static void elementData(String uri, String prefix,
    ContentHandler contentHandler,
    String name,
    String data
  )
    throws SAXException
  {
    start(uri, prefix, contentHandler, name);
    data(contentHandler, data);
    end(uri, prefix, contentHandler, name);
  }

  /**
   * Output an element containing text only and attributes
   *
   * @param contentHandler The SAX content handler
   * @param name The element name
   * @param data The data contained by the element
   * @param attr The element attributes
   */
  protected static void elementData(String uri, String prefix,
    ContentHandler contentHandler,
    String name,
    String data,
    AttributesImpl attr
  )
    throws SAXException
  {
    start(uri, prefix, contentHandler, name, attr);
    data(contentHandler, data);
    end(uri, prefix, contentHandler, name);
  }

  /**
   * Start an element with the proper object's uri and prefix and no
   * attributes
   *
   * @param contentHandler The SAX content handler
   * @param name The element name
   */
  protected static void start(String uri, String prefix,
    ContentHandler contentHandler,
    String name
  )
    throws SAXException
  {
    contentHandler.startElement(uri, name, new StringBuffer(prefix).append(":").append(name).toString(), emptyAttr);
  }

  /**
   * Start an element with the proper object's uri and prefix and with
   * attributes
   *
   * @param contentHandler The SAX content handler
   * @param name The element name
   * @param attr The element attributes
   */
  protected static void start(String uri, String prefix,
    ContentHandler contentHandler,
    String name,
    AttributesImpl attr
  )
    throws SAXException
  {
    contentHandler.startElement(uri, name, new StringBuffer(prefix).append(":").append(name).toString(), attr);
  }

  /**
   * End an element with the proper object's uri and prefix
   *
   * @param contentHandler The SAX content handler
   * @param name The element name
   */
  protected static void end(String uri, String prefix,
    ContentHandler contentHandler,
    String name
  )
    throws SAXException
  {
    contentHandler.endElement(uri, name, new StringBuffer(prefix).append(":").append(name).toString());
  }

  /**
   * Add an attribute
   *
   * @param attr The attribute list
   * @param name The attribute name
   * @param value The attribute value
   */
  protected static void addAttribute(
    AttributesImpl attr,
    String name,
    String value
  )
    throws SAXException
  {
    attr.addAttribute("", name, name, "CDATA", value);
  }

  /**
   * Add string data
   *
   * @param contentHandler The SAX content handler
   * @param data The string data
   */
  protected static void data(
    ContentHandler contentHandler,
    String data
  )
    throws SAXException
  {
    contentHandler.characters(data.toCharArray(), 0, data.length());
  }

  // <xsp:expr> methods

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>char</code> :
   * outputs characters representing the value.
   *
   * @param contentHandler the SAX content handler
   * @param v the value
   */
  public static void xspExpr(ContentHandler contentHandler, char v) throws SAXException
  {
    data(contentHandler, String.valueOf(v));
  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>byte</code> :
   * outputs characters representing the value.
   *
   * @param contentHandler the SAX content handler
   * @param v the value
   */
  public static void xspExpr(ContentHandler contentHandler, byte v) throws SAXException
  {
    data(contentHandler, String.valueOf(v));
  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>boolean</code> :
   * outputs characters representing the value (true / false).
   *
   * @param contentHandler the SAX content handler
   * @param v the value
   */
  public static void xspExpr(ContentHandler contentHandler, boolean v) throws SAXException
  {
    data(contentHandler, String.valueOf(v));
  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>int</code> :
   * outputs characters representing the value.
   *
   * @param contentHandler the SAX content handler
   * @param v the value
   */
  public static void xspExpr(ContentHandler contentHandler, int v) throws SAXException
  {
    data(contentHandler, String.valueOf(v));
  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>long</code> :
   * outputs characters representing the value.
   *
   * @param contentHandler the SAX content handler
   * @param v the value
   */
  public static void xspExpr(ContentHandler contentHandler, long v) throws SAXException
  {
    data(contentHandler, String.valueOf(v));
  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>long</code> :
   * outputs characters representing the value.
   *
   * @param contentHandler the SAX content handler
   * @param v the value
   */
  public static void xspExpr(ContentHandler contentHandler, float v) throws SAXException
  {
    data(contentHandler, String.valueOf(v));
  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>double</code> :
   * outputs characters representing the value.
   *
   * @param contentHandler the SAX content handler
   * @param v the value
   */
  public static void xspExpr(ContentHandler contentHandler, double v) throws SAXException
  {
    data(contentHandler, String.valueOf(v));
  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>String</code> :
   * outputs characters representing the value.
   *
   * @param contentHandler the SAX content handler
   * @param v the value
   */
  public static void xspExpr(ContentHandler contentHandler, String text) throws SAXException
  {
    if (text != null)
    {
      data(contentHandler, text);
    }
  }

// Now handled by XMLizable
//  /**
//   * Implementation of &lt;xsp:expr&gt; for <code>XMLFragment</code> :
//   * outputs the value by calling <code>v.toSax(contentHandler)</code>.
//   *
//   * @param contentHandler the SAX content handler
//   * @param v the XML fragment
//   */
//  public static void xspExpr(ContentHandler contentHandler, XMLFragment v) throws SAXException
//  {
//    if (v != null)
//    {
//      v.toSAX(contentHandler);
//    }
//  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>XMLizable</code> :
   * outputs the value by calling <code>v.toSax(contentHandler)</code>.
   *
   * @param contentHandler the SAX content handler
   * @param v the XML fragment
   */
  public static void xspExpr(ContentHandler contentHandler, XMLizable v) throws SAXException
  {
    if (v != null)
    {
      try{
        v.toSAX(contentHandler);
      }catch(ProcessingException e){
        throw new SAXException(e);
      }
    }
  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>org.w3c.dom.Node</code> :
   * converts the Node to a SAX event stream.
   *
   * @param contentHandler the SAX content handler
   * @param v the value
   */
  public static void xspExpr(ContentHandler contentHandler, Node v) throws SAXException
  {
    if (v != null)
    {
      DOMStreamer streamer = new DOMStreamer(contentHandler);
      streamer.stream(v);
    }
  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>java.util.Collection</code> :
   * outputs the value by calling <code>xspExpr()</code> on each element of the
   * collection.
   *
   * @param contentHandler the SAX content handler
   * @param v the XML fragment
   */
  public static void xspExpr(ContentHandler contentHandler, Collection v) throws SAXException
  {
    if (v != null)
    {
      Iterator iterator = v.iterator();
      while (iterator.hasNext())
      {
        xspExpr(contentHandler, iterator.next());
      }
    }
  }

  /**
   * Implementation of &lt;xsp:expr&gt; for <code>Object</code> depending on its class :
   * <ul>
   * <li>if it's an array, call <code>xspExpr()</code> on all its elements,</li>
   * <li>if it's class has a specific <code>xspExpr()</code>implementation, use it,</li>
   * <li>else, output it's string representation.</li>
   * </ul>
   *
   * @param contentHandler the SAX content handler
   * @param v the value
   */
  public static void xspExpr(ContentHandler contentHandler, Object v) throws SAXException
  {
    if (v == null)
    {
      return;
    }

    // Array: recurse over each element
    if (v.getClass().isArray())
    {
      Object[] elements = (Object[]) v;

      for (int i = 0; i < elements.length; i++)
      {
        xspExpr(contentHandler, elements[i]);
      }
      return;
    }

    // Check handled object types in case they were not typed in the XSP

    // XMLizable
    if (v instanceof XMLizable)
    {
        xspExpr(contentHandler, (XMLizable)v);
    }

// Now handled by XMLizable
//    // XMLFragment
//    if (v instanceof XMLFragment)
//    {
//      xspExpr(contentHandler, (XMLFragment)v);
//      return;
//    }

    // Node
    if (v instanceof Node)
    {
      xspExpr(contentHandler, (Node)v);
      return;
    }

    // Collection
    if (v instanceof Collection)
    {
      xspExpr(contentHandler, (Collection)v);
      return;
    }

    // Give up: hope it's a string or has a meaningful string representation
    data(contentHandler, String.valueOf(v));
  }
}
