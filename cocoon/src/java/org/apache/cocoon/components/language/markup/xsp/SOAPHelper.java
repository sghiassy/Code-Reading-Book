/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup.xsp;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.xscript.XScriptManager;
import org.apache.cocoon.components.xscript.XScriptObject;
import org.apache.cocoon.components.xscript.XScriptObjectInlineXML;

import org.apache.commons.httpclient.Header;
import org.apache.commons.httpclient.HttpConnection;
import org.apache.commons.httpclient.HttpState;
import org.apache.commons.httpclient.methods.PostMethod;
import org.xml.sax.InputSource;

/**
 * Helper for the SOAP logicsheet.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since July 16, 2001
 */
public class SOAPHelper
{
  XScriptManager xscriptManager;
  URL url;
  String action = "";
  XScriptObject xscriptObject;

  public SOAPHelper(ComponentManager manager, String urlContext, String url,
                    String action, XScriptObject xscriptObject)
    throws MalformedURLException, ComponentException
  {
    this.xscriptManager = (XScriptManager)manager.lookup(XScriptManager.ROLE);
    URL context = new URL(urlContext);
    this.url = new URL(context, url);
    this.action = action;
    this.xscriptObject = xscriptObject;
  }

  public XScriptObject invoke()
    throws ProcessingException
  {
    HttpConnection conn = null;

    try {
      if (action == null || action.equals(""))
        action = "\"\"";

      String host = url.getHost();
      int port = url.getPort();

      if (System.getProperty("http.proxyHost") != null) {
        String proxyHost = System.getProperty("http.proxyHost");
        int proxyPort = Integer.parseInt(System.getProperty("http.proxyPort"));
        conn = new HttpConnection(proxyHost, proxyPort, host, port);
     }
      else
        conn = new HttpConnection(host, port);

      PostMethod method = new PostMethod(url.getFile()) {
          protected String generateRequestBody(HashMap params)
          {
            try {
              StringBuffer bodyBuffer
                = new StringBuffer(super.generateRequestBody(params));

              // Write the SOAP request
              InputSource saxInputStream = xscriptObject.getInputSource();
              InputStream is = saxInputStream.getByteStream();
              InputStreamReader isr = new InputStreamReader(is);

              char[] buffer = new char[1024];
              int len;
              while ((len = isr.read(buffer)) > 0)
                bodyBuffer.append(buffer, 0, len);
              isr.close();
              is.close();
              return bodyBuffer.toString();
            }
            catch (Exception ex) {
              return null;
            }
          }
        };

        method.setRequestHeader(
                new Header("Content-type", "text/xml; charset=\"utf-8\""));
        method.setRequestHeader(new Header("SOAPAction", action));
        method.setUseDisk(false);

        method.execute(new HttpState(), conn);
        return new XScriptObjectInlineXML(xscriptManager,
                                          method.getResponseBodyAsString());
    }
    catch (Exception ex) {
      throw new ProcessingException("Error invoking remote service: " + ex,
                                    ex);
    }
    finally {
      try {
        if (conn != null)
          conn.close();
      }
      catch (Exception ex) {
      }
    }
  }
}
