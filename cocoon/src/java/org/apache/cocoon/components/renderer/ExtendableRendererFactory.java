/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.renderer;

import org.apache.fop.render.Renderer;
import org.apache.fop.render.pcl.PCLRenderer;
import org.apache.fop.render.pdf.PDFRenderer;
import org.apache.fop.render.ps.PSRenderer;

import java.util.HashMap;
import java.util.Map;

/**
 * An extendable FOP Renderer factory.
 * When given a MIME type, find a Renderer which supports that MIME
 * type. This factory is extendable as new <code>Renderer</code>s can
 * be added at runtime.
 * @author Davanum Srinivas
 * @version $Revision: 1.1 $, $Date: 2002/01/03 12:31:13 $
 */
public class ExtendableRendererFactory implements RendererFactory {

  protected static Map renderers = new HashMap();

  protected final static RendererFactory singleton = new ExtendableRendererFactory();

  private ExtendableRendererFactory() {
    // Add the default renderers which come with Apache FOP.
    addRenderer("application/pdf", PDFRenderer.class);
    addRenderer("application/postscript", PSRenderer.class);
    addRenderer("application/vnd.hp-PCL", PCLRenderer.class);
  }

  /**
   * Get a reference to this Renderer Factory.
   */
  public final static RendererFactory getRendererFactoryImplementation() {
    return singleton;
  }

  /**
   * Create a renderer for a specified MIME type.
   * @param mimeType The MIME type of the destination format
   * @return A suitable renderer, or <code>null</code> if one cannot be found
   */
  public Renderer createRenderer(String mimeType) {
    Class rendererClass = (Class)renderers.get(mimeType);
    if (rendererClass == null) {
      return null;
    } else {
      try {
        return (Renderer)rendererClass.newInstance();
      } catch (Exception ex) {
        return null;
      }
    }
  }

  /**
   * Add a mapping from the specified MIME type to a renderer.
   * Note: The renderer must have a no-argument constructor.
   * @param mimeType The MIME type of the Renderer
   * @param rendererClass The <code>Class</code> object for the Renderer.
   */
  public void addRenderer(String mimeType, Class rendererClass) {
    renderers.put(mimeType, rendererClass);
  }

  /**
   * Remove the mapping from a specified MIME type.
   * @param mimeType The MIME type to remove from the mapping.
   */
  public void removeRenderer(String mimeType) {
    renderers.remove(mimeType);
  }
}
