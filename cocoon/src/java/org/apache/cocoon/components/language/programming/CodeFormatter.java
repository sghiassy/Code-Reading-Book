/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.programming;

/**
 * This interface defines source beautifying formatting
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:11 $
 */
public interface CodeFormatter {
  /**
   * Format and beautify a <code>String</code> containing source code
   *
   * @param code The input source code
   * @param encoding The encoding used for constant strings embedded in the
   * source code
   * @return The formatted source code
   */
  String format(String code, String encoding);
}
