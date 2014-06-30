/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.renderer;

import org.apache.fop.render.Renderer;

/**
 * Apache FOP Renderer factory.
 * When given a MIME type, find a Renderer which supports that MIME type.
 * @author Davanum Srinivas
 * @version $Revision: 1.1 $, $Date: 2002/01/03 12:31:13 $
 */
public interface RendererFactory {

  /**
   * Create a transcoder for a specified MIME type.
   * @param mimeType The MIME type of the destination format
   * @return A suitable renderer, or <code>null> if one cannot be found
   */
  Renderer createRenderer(String mimeType) ;
}
