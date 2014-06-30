/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.xslt;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.parameters.ParameterException;
import org.apache.cocoon.ResourceNotFoundException;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.store.Store;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.util.ClassUtils;
import org.apache.cocoon.util.TraxErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.XMLFilter;
import org.xml.sax.XMLReader;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.XMLReaderFactory;

import javax.xml.transform.URIResolver;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.Templates;
import javax.xml.transform.Result;
import javax.xml.transform.Transformer;
import javax.xml.transform.sax.SAXSource;
import javax.xml.transform.sax.SAXTransformerFactory;
import javax.xml.transform.sax.TemplatesHandler;
import javax.xml.transform.sax.TransformerHandler;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;
import java.io.File;
import java.io.InputStream;
import java.io.IOException;
import java.io.StringWriter;
import java.io.Writer;

/**
 * This class defines the implementation of the {@link XSLTProcessor}
 * component.
 *
 * To configure it, add the following lines in the
 * <file>cocoon.xconf</file> file:
 *
 * <pre>
 * &lt;xslt-processor class="org.apache.cocoon.components.xslt.XSLTProcessorImpl"&gt;
 *    &lt;parameter name="use-store" value="true"/&gt;
 *    &lt;parameter name="transformer-factory" value="org.apache.xalan.processor.TransformerFactoryImpl"/&gt;
 * &lt;/xslt-processor&gt;
 * </pre>
 *
 * The &lt;use-store&gt; configuration forces the transformer to put the
 * <code>Templates</code> generated from the XSLT stylesheet into the
 * <code>Store</code>. This property is true by default.
 * <p>
 * The &lt;transformer-factory&gt; configuration tells the transformer to use a particular
 * implementation of <code>javax.xml.transform.TransformerFactory</code>. This allows to force
 * the use of a given TRAX implementation (e.g. xalan or saxon) if several are available in the
 * classpath. If this property is not set, the transformer uses the standard TRAX mechanism
 * (<code>TransformerFactory.newInstance()</code>).
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @version 1.0
 * @since   July 11, 2001
 */
public class XSLTProcessorImpl
  extends AbstractLoggable
  implements XSLTProcessor, Composable, Disposable, Parameterizable, URIResolver, Component
{
  protected ComponentManager manager;

  /** The store service instance */
  Store store;

  /** The trax TransformerFactory */
  SAXTransformerFactory tfactory;

  /** The factory class used to create tfactory */
  Class tfactoryClass;

  /** Is the store turned on? (default is on) */
  boolean useStore = true;

  /** Is incremental processing turned on? (default for Xalan: no) */
  boolean incrementalProcessing = false;

  SourceResolver resolver;

    /**
     * Compose. Try to get the store
     */
    public void compose(ComponentManager manager)
    throws ComponentException {
        this.manager = manager;
        getLogger().debug("XSLTProcessorImpl component initialized.");
        store = (Store)manager.lookup(Store.ROLE);
    }

    /**
     * Dispose
     */
    public void dispose() {
        if (this.manager != null) {
            this.manager.release((Component)store);
        }
    }

    /**
     * Configure the component
     */
  public void parameterize(Parameters params)
    throws ParameterException
  {
    useStore = params.getParameterAsBoolean("use-store", true);
    incrementalProcessing = params.getParameterAsBoolean("incremental-processing", false);

    String factoryName = params.getParameter("transformer-factory", null);
    if (factoryName == null) {
      // Will use default TRAX mechanism
      this.tfactoryClass = null;

    } else {
      // Will use specific class
      getLogger().debug("Using factory " + factoryName);
      try {
        this.tfactoryClass = ClassUtils.loadClass(factoryName);
      } catch(ClassNotFoundException cnfe) {
        throw new ParameterException("Cannot load TransformerFactory class", cnfe);
      }

      if (! TransformerFactory.class.isAssignableFrom(tfactoryClass)) {
        throw new ParameterException("Class " + factoryName + " isn't a TransformerFactory");
      }
    }
  }

  public TransformerHandler getTransformerHandler(Source stylesheet)
    throws ProcessingException
  {
    return getTransformerHandler(stylesheet, null);
  }

  public TransformerHandler getTransformerHandler(Source stylesheet,
                                                  XMLFilter filter)
    throws ProcessingException
  {
    try {
      final String id = stylesheet.getSystemId();
      Templates templates = getTemplates(stylesheet, id);
      if (templates == null) {
        if (this.getLogger().isDebugEnabled()) {
          getLogger().debug("Creating new Templates for " + id);
        }

        // Create a Templates ContentHandler to handle parsing of the
        // stylesheet.
        TemplatesHandler templatesHandler
          = getTransformerFactory().newTemplatesHandler();

        if (filter != null) {
          filter.setContentHandler(templatesHandler);
        }

        if (this.getLogger().isDebugEnabled()) {
          getLogger().debug("Source = " + stylesheet
                            + ", templatesHandler = " + templatesHandler);
        }

        // Process the stylesheet.
        stylesheet.toSAX(filter != null ?
            (ContentHandler)filter : (ContentHandler)templatesHandler);

        // Get the Templates object (generated during the parsing of
        // the stylesheet) from the TemplatesHandler.
        templates = templatesHandler.getTemplates();
        putTemplates (templates, stylesheet, id);
      } else {
        if (this.getLogger().isDebugEnabled()) {
          getLogger().debug("Reusing Templates for " + id);
        }
      }

      TransformerHandler handler
        = getTransformerFactory().newTransformerHandler(templates);

      /* (VG)
         From http://java.sun.com/j2se/1.4/docs/api/javax/xml/transform/TransformerFactory.html#newTemplates(javax.xml.transform.Source)
         Or http://xml.apache.org/xalan-j/apidocs/javax/xml/transform/TransformerFactory.html#newTemplates(javax.xml.transform.Source)

         "Returns: Templates object capable of being used for transformation
          purposes, never null."
      if (handler == null) {
        if (this.getLogger().isDebugEnabled()) {
          getLogger().debug("Re-creating new Templates for " + id);
        }

        templates = getTransformerFactory().newTemplates(new SAXSource(stylesheet.getInputSource()));
        putTemplates (templates, stylesheet, id);
        handler = getTransformerFactory().newTransformerHandler(templates);
      }
      */

      handler.getTransformer()
        .setErrorListener(new TraxErrorHandler(getLogger()));
      return handler;
    } catch (ProcessingException e) {
        throw e;
    } catch (SAXException e) {
        getLogger().debug("Got SAXException. Rethrowing cause exception.", e);
        throw new ProcessingException("Exception in creating Transform Handler", e.getException());
    } catch (Exception e) {
        throw new ProcessingException("Exception in creating Transform Handler", e);
    }
  }

  public void transform(Source source, Source stylesheet, Parameters params,
                        Result result)
    throws ProcessingException
  {
    try {
      if (this.getLogger().isDebugEnabled()) {
        getLogger().debug("XSLTProcessorImpl: transform source = " + source
                          + ", stylesheet = " + stylesheet
                          + ", parameters = " + params
                          + ", result = " + result);
      }
      TransformerHandler handler = getTransformerHandler(stylesheet);
      Transformer transformer = handler.getTransformer();

      if (params != null) {
        transformer.clearParameters();
        String[] names = params.getNames();
        for (int i = names.length -1 ; i >= 0; i--) {
          transformer.setParameter(names[i], params.getParameter(names[i]));
        }
      }
      InputSource is = source.getInputSource();
      InputStream bs = is.getByteStream();
      getLogger().debug("XSLTProcessorImpl: starting transform");
      // FIXME (VG): Is it possible to use Source's toSAX method?
      try {
        transformer.transform(new StreamSource(bs, is.getSystemId()),
                              result);
      } finally {
        bs.close();
      }
      getLogger().debug("XSLTProcessorImpl: transform done");
    } catch (Exception e) {
      throw new ProcessingException("Error in running Transformation", e);
    }
  }

  /**
   * Helper for TransformerFactory.
   */
  private SAXTransformerFactory getTransformerFactory() throws Exception
  {
    if(tfactory == null) {
      if (tfactoryClass == null) {
        tfactory = (SAXTransformerFactory)TransformerFactory.newInstance();
      } else {
        tfactory = (SAXTransformerFactory)tfactoryClass.newInstance();
      }
      tfactory.setErrorListener(new TraxErrorHandler(getLogger()));
      tfactory.setURIResolver(this);
      // TODO: If we will support this feature with a different
      // transformer than Xalan we'll have to set that corresponding
      // feature
      if (tfactory.getClass().getName().equals("org.apache.xalan.processor.TransformerFactoryImpl")) {
         tfactory.setAttribute("http://xml.apache.org/xalan/features/incremental",
                               new Boolean (incrementalProcessing));
      }
    }
    return tfactory;
  }

  private Templates getTemplates(Source stylesheet, String id)
    throws IOException, ProcessingException
  {
    if (!useStore)
      return null;

    getLogger().debug("XSLTProcessorImpl getTemplates: stylesheet " + id);

    Templates templates = null;
    // only stylesheets with a last modification date are stored
    if (stylesheet.getLastModified() != 0) {

      // Stored is an array of the template and the caching time
      if (store.containsKey(id)) {
        Object[] templateAndTime = (Object[])store.get(id);

        if(templateAndTime != null && templateAndTime[1] != null) {
          long storedTime = ((Long)templateAndTime[1]).longValue();

          if (storedTime < stylesheet.getLastModified()) {
            store.remove(id);
          } else {
            templates = (Templates)templateAndTime[0];
          }
        }
      }
    } else if (store.containsKey(id)) {
        // remove an old template if it exists
        store.remove(id);
    }
    return templates;
  }

  private void putTemplates (Templates templates, Source stylesheet, String id)
    throws IOException, ProcessingException
  {
    if (!useStore)
      return;

    // only stylesheets with a last modification date are stored
    if (stylesheet.getLastModified() != 0) {

      // Stored is an array of the template and the current time
      Object[] templateAndTime = new Object[2];
      templateAndTime[0] = templates;
      templateAndTime[1] = new Long(stylesheet.getLastModified());
      store.hold(id, templateAndTime);
    }
  }

  /**
   * Called by the processor when it encounters
   * an xsl:include, xsl:import, or document() function.
   *
   * @param href An href attribute, which may be relative or absolute.
   * @param base The base URI in effect when the href attribute
   * was encountered.
   *
   * @return A Source object, or null if the href cannot be resolved,
   * and the processor should try to resolve the URI itself.
   *
   * @throws TransformerException if an error occurs when trying to
   * resolve the URI.
   */
  public javax.xml.transform.Source resolve(String href, String base)
    throws TransformerException
  {
    if (this.getLogger().isDebugEnabled()) {
      getLogger().debug("resolve(href = " + href
                        + ", base = " + base + "); resolver = " + resolver);
    }

    Source xslSource = null;
    try {
      if (href.indexOf(":") > 1) {
        xslSource = resolver.resolve(href);
      } else {
        // patch for a null pointer passed as base
        if (base == null)
          throw new IllegalArgumentException("Null pointer passed as base");

        // is the base a file or a real url
        if (!base.startsWith("file:")) {
          int lastPathElementPos = base.lastIndexOf('/');
          if (lastPathElementPos == -1) {
            // this should never occur as the base should
            // always be protocol:/....
            return null; // we can't resolve this
          } else {
            xslSource = resolver.resolve(new StringBuffer(base.substring(0, lastPathElementPos))
                                         .append("/").append(href).toString());
          }
        } else {
          File parent = new File(base.substring(5));
          File parent2 = new File(parent.getParentFile(), href);
          xslSource = resolver.resolve(parent2.toURL().toExternalForm());
        }
      }

      InputSource is = xslSource.getInputSource();
      if (this.getLogger().isDebugEnabled()) {
        getLogger().debug("xslSource = " + xslSource
                          + ", system id = " + is.getSystemId());
      }

      return new StreamSource(is.getByteStream(), is.getSystemId());

    } catch (ResourceNotFoundException rnfe) {
        // to obtain the same behaviour as when the resource is
        // transformed by the XSLT Transformer we should return null here.
        return null;
    } catch (java.net.MalformedURLException mue) {
        return null;
    } catch (IOException ioe) {
        return null;
    } catch (SAXException se) {
        throw new TransformerException(se);
    } catch (ProcessingException pe) {
        throw new TransformerException(pe);
    } finally {
      if (xslSource != null) xslSource.recycle();
    }
  }

  public void setSourceResolver(SourceResolver resolver)
  {
    this.resolver = resolver;
  }
}
