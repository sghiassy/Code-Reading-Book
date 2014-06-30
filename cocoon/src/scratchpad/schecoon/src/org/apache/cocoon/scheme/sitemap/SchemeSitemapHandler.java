package org.apache.cocoon.scheme.sitemap;

import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.Loggable;
import org.apache.cocoon.components.source.CocoonSourceFactory;

/**
 * The Scheme sitemap handler. Cocoon's SitemapManager class needs to
 * be configured to use this handler instead of the Cocoon's default
 * one, which compiles the XML sitemap to Java code.
 *
 * This handler uses an instance of {@link SchemeSitemap} instead.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since December 18, 2001
 */
public class SchemeSitemapHandler
  extends org.apache.cocoon.sitemap.Handler
  implements Contextualizable, Composable, Configurable
{
  Context context;
  Configuration confs;

  public void contextualize(Context context)
    throws ContextException
  {
    this.context = context;
  }

  /**
   * Obtain the ComponentManager instance and wrap it into a Scheme
   * object, that's passed later to the Scheme sitemap function.
   *
   * @param manager a <code>ComponentManager</code> value
   */
  public void compose(ComponentManager manager)
    throws ComponentException
  {
    super.compose(manager);
  }

  /**
   * Properly setup the Scheme interpreter, by loading the heap file.
   *
   * @param sconf a <code>Configuration</code> value
   * @exception ConfigurationException if an error occurs
   */
  public void configure(Configuration confs)
    throws ConfigurationException
  {
    this.confs = confs;
  }

  public void run()
  {
    try {
      SchemeSitemap smap = new SchemeSitemap();
      ((Loggable)smap).setLogger(getLogger());
      ((Composable)smap).compose(manager);
      ((Contextualizable)smap).contextualize(context);
      ((Configurable)smap).configure(confs);
      ((SchemeSitemap)smap).parseSitemap(resolve(sourceFileName));
      // add the source factory for the cocoon protocol
      sourceHandler
        .addFactory("cocoon",
                    new CocoonSourceFactory(smap, smap.getComponentManager()));
      sitemap = smap;
    }
    catch (Exception ex) {
      ex.printStackTrace();
      getLogger().debug("Cannot create sitemap: " + ex);
    }
    finally {
      this.regeneration = null;
      this.isRegenerationRunning = false;
      this.contextSource.recycle();
      this.contextSource = null;
    }
  }
}
