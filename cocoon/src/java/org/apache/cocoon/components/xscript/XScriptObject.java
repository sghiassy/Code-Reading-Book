/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.xscript;

import javax.xml.transform.stream.StreamResult;
import java.util.Date;
import java.io.IOException;
import java.io.CharArrayWriter;

import org.xml.sax.SAXException;
import org.xml.sax.InputSource;
import org.xml.sax.ContentHandler;
import org.apache.cocoon.xml.EmbeddedXMLPipe;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.components.xslt.XSLTProcessor;
import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.ProcessingException;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.Composable;

/**
 * <code>XScriptObject</code> is the root class implemented by all the
 * object types in XScript. Every XScriptObject is essentially a
 * Source object.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since August  4, 2001
 */
public abstract class XScriptObject implements Source, Composable
{
  /**
   * The creation date of this <code>XScriptObject</code>.
   */
  Date lastModifiedDate = new Date();

  /**
   * The <code>XScriptManager</code> object that's managing this
   * <code>XScriptObject</code> value.
   */
  XScriptManager xscriptManager;
  
  protected ComponentManager componentManager;

  /**
   * Creates a new <code>XScriptObject</code> instance.
   *
   * @param manager a <code>XScriptManager</code> value
   */
  public XScriptObject(XScriptManager manager)
  {
    this.xscriptManager = manager;
    ((XScriptManagerImpl)this.xscriptManager).register(this);
  }

  public void compose(ComponentManager manager)
    throws ComponentException
  {
    this.componentManager = manager;
  }
  
  /**
   * Apply the XSLT stylesheet defined by the <code>stylesheet</code>
   * variable to this instance. Return the result of the
   * transformation as an <code>XScriptObject</code>.
   *
   * @param source a <code>XScriptObject</code> value
   * @param stylesheet a <code>XScriptObject</code> value
   * @param params a <code>Parameters</code> value containing optional
   * arguments to be passed to the XSLT processor.
   * @param result a <code>Result</code> value. This is where the
   * result of the XSLT processing will be placed.
   * @exception IllegalArgumentException if an error occurs
   * @exception ProcessingException if an error occurs
   */
  public XScriptObject transform(XScriptObject stylesheet, Parameters params)
    throws IllegalArgumentException, ProcessingException
  {
    try {
      CharArrayWriter writer = new CharArrayWriter();
      StreamResult result = new StreamResult(writer);
      
      XSLTProcessor transformer
        = (XSLTProcessor)componentManager.lookup(XSLTProcessor.ROLE);
      
      transformer.transform(this, stylesheet, params, result);

      componentManager.release((Component)transformer);

      return new XScriptObjectResult(xscriptManager, writer.toString());
    }
    catch (ComponentException ex) {
      throw new ProcessingException(ex);
    }
  }

  public void toSAX(ContentHandler handler)
    throws SAXException
  {
    Parser parser = null;
    try {
      EmbeddedXMLPipe newHandler = new EmbeddedXMLPipe(handler);

      parser = (Parser)componentManager.lookup(Parser.ROLE);
      
      parser.setContentHandler(newHandler);
      InputSource source = getInputSource();
      parser.parse(source);

    } catch (Exception ex) {
      throw new SAXException(ex);
    } finally {
      if (parser != null) componentManager.release(parser);
    }
  }

  /* The Source interface methods. */

  public long getLastModified()
  {
    return lastModifiedDate.getTime();
  }

  public abstract long getContentLength();

  public InputSource getInputSource()
    throws ProcessingException, IOException
  {
    return new InputSource(getInputStream());
  }

  public void recycle()
  {
  }
}
