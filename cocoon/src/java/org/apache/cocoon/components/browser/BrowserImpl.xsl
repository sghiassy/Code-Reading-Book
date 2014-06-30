<?xml version="1.0"?>

<!-- Author: Ovidiu Predescu "ovidiu@cup.hp.com" -->
<!-- Author: Davanum Srinivas "dims@yahoo.com" -->
<!-- Date: April 13, 2001 -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="text" omit-xml-declaration="yes"/>

 <xsl:template match="browser-categories">
/* WARNING: This class is automatically generated from Browser.xml
   using Browser.xsl */
package org.apache.cocoon.components.browser;

import java.util.Comparator;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.StringTokenizer;
import java.util.Map.Entry;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.Text;

import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.xml.dom.DOMFactory;
import org.apache.cocoon.components.parser.Parser;
import java.util.TreeSet;

<![CDATA[
/**
 * <code>Browser</code> is an automatically generated class that
 * represents various user agent types. The original file this class
 * is generated from is <file>Browser.xml</file>, using
 * <file>Browser.xsl</file>.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version 2.0
 * @since April 13, 2001 */
public class BrowserImpl implements Component, Browser, Composable, Disposable, ThreadSafe
{
  /**
   * <code>byMimeType</code> describes how to map from a generic MIME
   * type to a browser description, which maintains information about
   * the media type and the formatter type used to output that
   * data. The values in this hash table are hash tables that contain
   * the above information. The keys in the browser description hash
   * table are:
   <ul>
    <li><code>media</code> - the media type associated with this MIME type</li>
    <li><code>formatter-type</code> - the formatter used to output</li>
    markup for this MIME type
    </ul>
  */
  HashMap byMimeType = new HashMap();

  /**
   * <code>byUserAgent</code> has as keys strings that are used to
   * match a particular user agent. The value is a
   * <code>HashMap</code> that describes a browser. Below is a
   * description of the keys that can appear in this hash map. Any tag
   * you add to the description of a browser in the
   * <code>Browser.xml</code> file, is added to the dictionary.

   <ul>

    <li><code>name</code> - a generic name used to identify the
    browser. It is useful only to the developer and should not be used
    to differentiate between browsers.</li>

    <li><code>user-agent</code> - the user agent string used to
    identify the browser. This can be a partial name, not necessarily
    the full user agent. If multiple browsers have similar substrings
    in their user agent string, the longest match wins.

    <li><code>formatter-type</code> - the formatter to be used to
    output markup for this browser. Note that this may also affect the
    DOCTYPE that's placed in the resulting XML document.</li>

    <li><code>has-accesskey</code> - whether the browser understands
    the <code>accesskey</code> attribute as part of a
    <code>anchor</code> tag.</li>

    <li><code>binds-call-to-send</code> - whether the browser on a
    cell phone is able, when displaying a page, to bind a <i>"make a
    phone call"</i> action to the <i>Send</i> (or <i>Call</i>) button
    on the cell phone. If so the value of this tag should describe the
    type used in the <code>do</code> tag that does this.</li>

    <li><code>ua-capabilities</code> - the value of this key is a
    <code>Document</code> object which is a description of this hash
    table. This <code>Document</code> instance is passed to the XSLT
    processor as an argument so that the generic stylesheets can make
    use of it to generate content appropriate for the device.</li>

   </ul>

  */
  TreeSet byUserAgent = new TreeSet(new UserAgentComparator());
  //  Vector byUserAgent = new Vector();

  /**
   * <code>mimeTypeSearchOrder</code> specifies the order in which the
   * MIME types are searched in the <code>Accept:</code> header.  */
  ArrayList mimeTypeSearchOrder;

  Parser parser;

  /** The component manager instance */
  protected ComponentManager manager=null;

  /**
   * Set the current <code>ComponentManager</code> instance used by this
   * <code>Composable</code>.
   */
]]>

  public void compose(ComponentManager manager)
    throws ComponentException {
      this.manager=manager;
      parser = (Parser)this.manager.lookup(Parser.ROLE);

      HashMap browser0 = new HashMap();
      Document document;

      <xsl:apply-templates/>
  }

  public void dispose() {
      if(parser != null) this.manager.release((Component)parser);
  }

<![CDATA[
  /**
   * <code>getMedia</code> returns a browser description based on the user
   * agent or on the Accept: header. This is used later by the XSLT
   * processor to select the stylesheet to apply on a given XML file.
   *
   * @param userAgent a <code>String</code> value
   * @param acceptHeader a <code>String</code> value
   * @return a <code>HashMap</code> value containing the browser
   *         description or <code>null</code>
   */
  public HashMap getBrowser(String userAgent, String acceptHeader)
  {
    if (userAgent == null || userAgent.equals(""))
      return new HashMap();

    /* First search for a matching user agent */
    Iterator iter = byUserAgent.iterator();
    while (iter.hasNext()) {
      HashMap browserInfo = (HashMap)iter.next();
      String ua = (String)browserInfo.get("user-agent");
      if (ua.indexOf(userAgent) != -1 || userAgent.indexOf(ua) != -1) {
        /* We found a user agent. Return the browser dictionary
         information. */
        return browserInfo;
      }
    }

    /* The user agent didn't match any of the ones described in the
       database. Search on the Accept header to find a matching MIME
       type. Once we found a matching MIME type, look it up in the
       byMimeType dictionary and return the appropriate browser
       dictionary. */
    String exactMatch = null;
    String partialMatch = null;
    String globalMatch = null;
    int length = mimeTypeSearchOrder.size();

loop:
    for (int i = 0; i < length; i++) {
      String currentType = (String)mimeTypeSearchOrder.get(i);
      int index = currentType.indexOf("/");
      String genericType = currentType.substring(0, index) + "/*";
      StringTokenizer tokenizer = new StringTokenizer(acceptHeader, ", ");
      while (tokenizer.hasMoreElements()) {
        String mediaType = ((String)tokenizer.nextElement()).trim();

        if (currentType.equals(mediaType)) {
          exactMatch = currentType;
          break loop;
        }
        else if (genericType.equals(mediaType) && partialMatch == null)
          partialMatch = currentType;
        else if (mediaType.equals("*/*") && globalMatch == null)
          globalMatch = currentType;
      }
    }

    String type = null;

    if (exactMatch != null)
      type = exactMatch;
    else if (partialMatch != null)
      type = partialMatch;
    else if (globalMatch != null)
      type = globalMatch;

    return type != null ? (HashMap)byMimeType.get(type) : null;
  }

  protected Document createDocumentForBrowserInfo(HashMap browserInfo)
  {
    Document document;
    Element element;
    Element node;
    Text text;

    document = ((DOMFactory)parser).newDocument();
    element = document.createElementNS(null, "browser");
    document.appendChild(element);

    Iterator iter = browserInfo.entrySet().iterator();
    while (iter.hasNext()) {
      Entry entry = (Entry)iter.next();
      String nodeName = (String)entry.getKey();
      if (nodeName.equals("ua-capabilities"))
        continue;
      String nodeValue = (String)entry.getValue();

      if (nodeName.indexOf(":") < 0) {
          node = document.createElementNS(null, nodeName);
      } else {
          node = document.createElementNS("http://www.wapforum.org/UAPROF/ccppschema-20000405", 
                                          nodeName);
      }
      text = document.createTextNode(nodeValue);
      node.appendChild(text);
      element.appendChild(node);
    }

    return document;
  }

  public String getMedia(HashMap browserInfo)
  {
    return (String)browserInfo.get("media");
  }

  public Document getUACapabilities(HashMap browserInfo)
  {
    return (Document)browserInfo.get("ua-capabilities");
  }
}

class UserAgentComparator implements Comparator
{
  public int compare(Object a, Object b)
  {
    String ua1 = (String)((HashMap)a).get("user-agent");
    String ua2 = (String)((HashMap)b).get("user-agent");

    if (ua1 == null)
      return -1;
    else if (ua2 == null)
      return 1;
    else
      return ua2.compareTo(ua1);
  }

  public boolean equals(Object obj)
  {
    return false;
  }
}
]]>
  </xsl:template>


 <xsl:template match="browser-category">
  <xsl:param name="depth" select="0"/>
  <xsl:param name="current" select="$depth + 1"/>

  {
    HashMap browser<xsl:value-of select="$current"/> = (HashMap)browser<xsl:value-of select="$depth"/>.clone();
  <!-- Put the browser category in the byMediaType dictionary under the
  "mime-type" key. This allows us to identify the generic category in
  case a user agent is not specified in the user agents database (the
  Browser.xml file). -->
    byMimeType.put("<xsl:value-of select="mime-type"/>", browser<xsl:value-of select="$current"/>);
  <xsl:apply-templates select="*[name() != 'browser'] | @*">
   <xsl:with-param name="depth" select="$current"/>
  </xsl:apply-templates>
    document = createDocumentForBrowserInfo(browser<xsl:value-of select="$current"/>);
    browser<xsl:value-of select="$current"/>.put("ua-capabilities", document);
  <xsl:apply-templates select="browser">
   <xsl:with-param name="depth" select="$current"/>
  </xsl:apply-templates>
  }
 </xsl:template>


 <xsl:template match="browser-category//*[name() != 'browser'
                    and name() != 'user-agent']
                      | browser-category//@*">
  <xsl:param name="depth" select="0"/>
    browser<xsl:value-of select="$depth"/>.put("<xsl:value-of select="name()"/>", "<xsl:value-of select="."/>");
 </xsl:template>


 <xsl:template match="browser">
  <xsl:param name="depth" select="0"/>
  <xsl:param name="current" select="$depth + 1"/>

  {
    HashMap browser<xsl:value-of select="$current"/> = (HashMap)browser<xsl:value-of select="$depth"/>.clone();
  /* Add the user agents as a vector in the browser hash map */
  <xsl:apply-templates select="*[name() != 'browser'] | @*">
   <xsl:with-param name="depth" select="$current"/>
  </xsl:apply-templates>
    document = createDocumentForBrowserInfo(browser<xsl:value-of select="$current"/>);
    browser<xsl:value-of select="$current"/>.put("ua-capabilities", document);
  <xsl:apply-templates select="browser">
   <xsl:with-param name="depth" select="$current"/>
  </xsl:apply-templates>
  }
 </xsl:template>


 <xsl:template match="user-agent">
  <xsl:param name="depth" select="0"/>

  browser<xsl:value-of select="$depth"/>.put("<xsl:value-of select="name()"/>", "<xsl:value-of select="."/>");
  byUserAgent.add(browser<xsl:value-of select="$depth"/>);
 </xsl:template>


 <xsl:template match="search-order">
  mimeTypeSearchOrder = new ArrayList();
  <xsl:apply-templates/>
 </xsl:template>


 <xsl:template match="search-order/type">
  mimeTypeSearchOrder.add("<xsl:value-of select="."/>");
 </xsl:template>


</xsl:stylesheet>
