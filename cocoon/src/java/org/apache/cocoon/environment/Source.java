/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.xml.XMLizable;
import org.xml.sax.InputSource;

import java.io.IOException;
import java.io.InputStream;

/**
 * Description of a source. This interface provides a simple interface
 * for accessing a source of data. The source of data is assumed to
 * <b>not change</b> during the lifetime of the Source object. If you
 * have a data source that can change its content and you want it to
 * reflect in Cocoon, use a {@link ModifiableSource} object instead.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:15 $
 */

public interface Source extends Recyclable, XMLizable {
  /**
   * Get the last modification date of the source or 0 if it
   * is not possible to determine the date.
   */
  long getLastModified();

  /**
   * Get the content length of the source or -1 if it
   * is not possible to determine the length.
   */
  long getContentLength();

  /**
   * Return an <code>InputStream</code> object to read from the source.
   */
  InputStream getInputStream()
    throws ProcessingException, IOException;

  /**
   * Return an <code>InputSource</code> object to read the XML
   * content.
   *
   * @return an <code>InputSource</code> value
   * @exception ProcessingException if an error occurs
   * @exception IOException if an error occurs
   */
  InputSource getInputSource()
    throws ProcessingException, IOException;

  /**
   * Return the unique identifer for this source
   */
  String getSystemId();
}
