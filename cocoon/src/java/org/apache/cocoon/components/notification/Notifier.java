/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.notification;

import org.apache.cocoon.Constants;

import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Map;
import java.util.Iterator;

/**
 * Generates a representations of the specified Notifying Object.
 *
 * @author <a href="mailto:nicolaken@supereva.it">Nicola Ken Barozzi</a> Aisa
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */

public class Notifier {

  /**
   * Generate notification information as a response.
   * The notification is directly written to the OutputStream.
   * @param  n The <code>Notifying</code> object
   * @param outputStream The output stream the notification is written to
   *        This could be <code>null</code>.
   * @return The content type for this notification
   *         (currently always text/html)
   */
  public static String notify(Notifying n,
          OutputStream outputStream) throws IOException {

    StringBuffer sb = new StringBuffer();

    sb.append("<html><head><title>").append(n.getTitle()).append("</title>");
    sb.append("<STYLE><!--H1{font-family : sans-serif,Arial,Tahoma;color : white;background-color : #0086b2;} ");
    sb.append("BODY{font-family : sans-serif,Arial,Tahoma;color : black;background-color : white;} ");
    sb.append("B{color : white;background-color : #0086b2;} ");
    sb.append("HR{color : #0086b2;} ");
    sb.append("--></STYLE> ");
    sb.append("</head><body>");
    sb.append("<h1>Cocoon 2 - ").append(n.getTitle()).append("</h1>");
    sb.append("<HR size=\"1\" noshade=\"noshade\">");
    sb.append("<p><b>type</b> ").append(n.getType()).append("</p>");
    sb.append("<p><b>message</b> <u>").append(n.getMessage()).append("</u></p>");
    sb.append("<p><b>description</b> <u>").append(n.getDescription()).append("</u></p>");
    sb.append("<p><b>sender</b> ").append(n.getSender()).append("</p>");
    sb.append("<p><b>source</b> ").append(n.getSource()).append("</p>");

    Map extraDescriptions = n.getExtraDescriptions();
    Iterator keyIter = extraDescriptions.keySet().iterator();

    while (keyIter.hasNext()) {
      String key = (String) keyIter.next();

      sb.append("<p><b>").append(key).append("</b><pre>").append(
              extraDescriptions.get(key)).append("</pre></p>");
    }

    sb.append("<HR size=\"1\" noshade>");
    sb.append("</body></html>");

    if (outputStream != null)
      outputStream.write(sb.toString().getBytes());
    // FIXME (SM) how can we send the error with the proper content type?

    return "text/html";
  }

  /**
   * Generate notification information in XML format.
   */
  public static void notify(Notifying n, ContentHandler ch) throws SAXException {

    final String PREFIX = Constants.ERROR_NAMESPACE_PREFIX;
    final String URI = Constants.ERROR_NAMESPACE_URI;

    // Start the document
    ch.startDocument();
    ch.startPrefixMapping(PREFIX, URI);

    // Root element.
    AttributesImpl atts = new AttributesImpl();

    atts.addAttribute(URI, "type", PREFIX + ":type", "CDATA", n.getType());
    atts.addAttribute(URI, "sender", PREFIX + ":sender", "CDATA", n.getSender());
    ch.startElement(URI, "notify", PREFIX + ":notify", atts);
    ch.startElement(URI, "title", PREFIX + ":title", new AttributesImpl());
    ch.characters(n.getTitle().toCharArray(), 0, n.getTitle().length());
    ch.endElement(URI, "title", PREFIX + ":title");
    ch.startElement(URI, "source", PREFIX + ":source", new AttributesImpl());
    ch.characters(n.getSource().toCharArray(), 0, n.getSource().length());
    ch.endElement(URI, "source", PREFIX + ":source");
    ch.startElement(URI, "message", PREFIX + ":message", new AttributesImpl());

    if (n.getMessage() != null) {
      ch.characters(n.getMessage().toCharArray(), 0, n.getMessage().length());
    }

    ch.endElement(URI, "message", PREFIX + ":message");
    ch.startElement(URI, "description", PREFIX + ":description",
            new AttributesImpl());
    ch.characters(n.getDescription().toCharArray(), 0, n.getDescription().length());
    ch.endElement(URI, "description", PREFIX + ":description");

    Map extraDescriptions = n.getExtraDescriptions();
    Iterator keyIter = extraDescriptions.keySet().iterator();

    while (keyIter.hasNext()) {
      String key = (String) keyIter.next();

      atts = new AttributesImpl();

      atts.addAttribute(URI, "description", PREFIX + ":description", "CDATA", key);
      ch.startElement(URI, "extra", PREFIX + ":extra", atts);
      ch.characters(extraDescriptions.get(key).toString().toCharArray(), 0,
              (extraDescriptions.get(key).toString()).length());
      ch.endElement(URI, "extra", PREFIX + ":extra");
    }

    // End root element.
    ch.endElement(URI, "notify", PREFIX + ":notify");

    // End the document.
    ch.endPrefixMapping(PREFIX);
    ch.endDocument();
  }
}

