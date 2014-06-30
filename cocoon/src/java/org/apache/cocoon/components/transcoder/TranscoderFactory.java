/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.transcoder;

import org.apache.batik.transcoder.Transcoder;

/**
 * Apache Batik Transcoder factory.
 * When given a MIME type, find a Transcoder which supports that MIME type.
 * @author Ross Burton
 * @version $Revision: 1.1 $, $Date: 2002/01/03 12:31:14 $
 */
public interface TranscoderFactory {

  /**
   * Create a transcoder for a specified MIME type.
   * @param mimeType The MIME type of the destination format
   * @return A suitable transcoder, or <code>null> if one cannot be found
   */
  Transcoder createTranscoder(String mimeType) ;
}
