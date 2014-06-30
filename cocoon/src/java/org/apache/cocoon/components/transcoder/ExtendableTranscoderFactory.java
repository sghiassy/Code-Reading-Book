/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.transcoder;

import org.apache.batik.transcoder.Transcoder;
import org.apache.batik.transcoder.image.JPEGTranscoder;
import org.apache.batik.transcoder.image.PNGTranscoder;
import org.apache.batik.transcoder.image.TIFFTranscoder;

import java.util.HashMap;
import java.util.Map;

/**
 * An extendable Batik Transcoder factory.
 * When given a MIME type, find a Transcoder which supports that MIME
 * type. This factory is extendable as new <code>Transcoder</code>s can
 * be added at runtime.
 * @author Ross Burton
 * @version $Revision: 1.1 $, $Date: 2002/01/03 12:31:14 $
 */
public class ExtendableTranscoderFactory implements TranscoderFactory {

    protected static Map transcoders = new HashMap();

    protected final static TranscoderFactory singleton = new ExtendableTranscoderFactory();

    private ExtendableTranscoderFactory() {
        // Add the default transcoders which come with Batik
        addTranscoder("image/jpeg", JPEGTranscoder.class);
        addTranscoder("image/jpg", JPEGTranscoder.class);
        addTranscoder("image/png", PNGTranscoder.class);
        addTranscoder("image/tiff", TIFFTranscoder.class);
    }

    /**
     * Get a reference to this Transcoder Factory.
     */
    public final static TranscoderFactory getTranscoderFactoryImplementation() {
        return singleton;
    }

    /**
     * Create a transcoder for a specified MIME type.
     * @param mimeType The MIME type of the destination format
     * @return A suitable transcoder, or <code>null</code> if one cannot be found
     */
    public Transcoder createTranscoder(String mimeType) {
        Class transcoderClass = (Class) transcoders.get(mimeType);
        if (transcoderClass == null) {
            return null;
        } else {
            try {
                return (Transcoder) transcoderClass.newInstance();
            } catch (Exception ex) {
                return null;
            }
        }
    }

    /**
     * Add a mapping from the specified MIME type to a transcoder.
     * Note: The transcoder must have a no-argument constructor.
     * @param mimeType The MIME type of the Transcoder
     * @param transcoderClass The <code>Class</code> object for the Transcoder.
     */
    public void addTranscoder(String mimeType, Class transcoderClass) {
        transcoders.put(mimeType, transcoderClass);
    }

    /**
     * Remove the mapping from a specified MIME type.
     * @param mimeType The MIME type to remove from the mapping.
     */
    public void removeTranscoder(String mimeType) {
        transcoders.remove(mimeType);
    }
}
