package org.apache.cocoon.scheme.sitemap;

import org.apache.cocoon.xml.AbstractXMLConsumer;
import org.xml.sax.Attributes;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;

/**
 * SAX content handler for translating an XML document in the SXML
 * (Scheme XML) representation.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since January 16, 2002
 */
public class XMLtoSXML extends AbstractXMLConsumer
{
  protected StringBuffer content = new StringBuffer(100000);
  Locator locator;

  public XMLtoSXML() {}

  public String getContent()
  {
    return content.toString();
  }

  public void startDocument()
    throws SAXException
  {
    content.append("(quote ");
  }

  public void endDocument()
    throws SAXException
  {
    content.append(")");
  }

  public void startElement(String uri, String loc, String qName, Attributes a)
    throws SAXException
  {
    content.append("(" + loc + " (@ ");
    for (int i = 0, size = a.getLength(); i < size; i++) {
      String attrName = a.getQName(i);

      if (!attrName.startsWith("xmlns:"))
        content.append("(" +  attrName + " \"" + a.getValue(i) + "\") ");
    }
    content.append("(*line* " + locator.getLineNumber() + ") ");
    content.append(") ");
  }

  public void endElement(String uri, String loc, String raw)
    throws SAXException
  {
    content.append(") ");
  }

  public void characters(char ch[], int start, int len)
    throws SAXException
  {
    content.append(escape(ch, start, len));
  }

  public void setDocumentLocator(Locator locator)
  {
    this.locator = locator;
  }

  /**
   * Escapes characters in <code>chars</code> as follows:
   *
   * <ul>
   *  <li><pre>\  -> \\</pre></li>
   *  <li><pre>"  -> \"</pre></li>
   * </ul>
   *
   * @param chars a <code>char[]</code> value
   */
  public static String escape(char[] chars, int start, int len)
  {
    if (chars == null)
      return "";
    
    // Assume about 1% of the characters need to be escaped; probably
    // a safe bet in most cases. Otherwise the buffer is increased
    // automatically by the StringBuffer.
    StringBuffer escaped = new StringBuffer((int)(chars.length * 1.01));

    for (int i = start; i < len; i++) {
      char ch = chars[i];
      switch (ch) {
      case '\\':
        escaped.append("\\\\\\\\");
        break;
      case '"':
        escaped.append("\\\\\"");
        break;
      default:
        escaped.append(ch);
        break;
      }
    }

    return escaped.toString();
  }
}
