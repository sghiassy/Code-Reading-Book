/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.xslt;

import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.xml.sax.XMLFilter;

import javax.xml.transform.Result;
import javax.xml.transform.sax.TransformerHandler;

/**
 * This is the interface of the XSLT processor in Cocoon.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @version 1.0
 * @since   July 11, 2001
 */
public interface XSLTProcessor
{
  /**
   * The role implemented by an <code>XSLTProcessor</code>.
   */
  String ROLE = "org.apache.cocoon.components.xslt.XSLTProcessor";

  /**
   * Set the {@link org.apache.cocoon.environment.SourceResolver} for
   * this instance. The <code>resolver</code> is invoked to return a
   * <code>Source</code> object, given an HREF.
   *
   * @param resolver a <code>SourceResolver</code> value
   */
  void setSourceResolver(SourceResolver resolver);
  
  /**
   * <p>Return a <code>TransformerHandler</code> for a given
   * stylesheet <code>Source</code>. This can be used in a pipeline to
   * handle the transformation of a stream of SAX events. See {@link
   * org.apache.cocoon.transformation.TraxTransformer#setConsumer} for
   * an example of how to use this method.
   *
   * <p>The additional <code>filter</code> argument, if it's not
   * <code>null</code>, is inserted in the chain SAX events as an XML
   * filter during the parsing or the source document.
   *
   * <p>This method caches the Source object and performs a reparsing
   * only if this changes.
   *
   * @param stylesheet a <code>Source</code> value
   * @param filter a <code>XMLFilter</code> value
   * @return a <code>TransformerHandler</code> value
   * @exception ProcessingException if an error occurs
   */
  TransformerHandler getTransformerHandler(Source stylesheet,
                                           XMLFilter filter)
    throws ProcessingException;

  /**
   * Same as {@link #getTransformerHandler(Source,XMLFilter)}, with
   * <code>filter</code> set to <code>null</code>.
   *
   * @param stylesheet a <code>Source</code> value
   * @return a <code>TransformerHandler</code> value
   * @exception ProcessingException if an error occurs
   * @see org.apache.cocoon.transformation.TraxTransformer#setConsumer
   */
  TransformerHandler getTransformerHandler(Source stylesheet)
    throws ProcessingException;

  /**
   * Applies an XSLT stylesheet to an XML document. The source and
   * stylesheet documents are specified as <code>Source</code>
   * objects. The result of the transformation is placed in
   * <code>result</code>, which should be properly initialized before
   * invoking this method. Any additional parameters passed in
   * <code>params</code> will become arguments to the stylesheet.
   *
   * @param source a <code>Source</code> value
   * @param stylesheet a <code>Source</code> value
   * @param params a <code>Parameters</code> value
   * @param result a <code>Result</code> value
   * @exception ProcessingException if an error occurs
   */
  void transform(Source source, Source stylesheet, Parameters params,
                 Result result)
    throws ProcessingException;
}
