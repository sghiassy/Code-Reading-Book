/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.Processor;
import org.apache.cocoon.components.CocoonComponentManager;
import org.apache.cocoon.components.language.generator.CompiledComponent;
import org.apache.cocoon.components.language.generator.ProgramGenerator;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.components.source.CocoonSourceFactory;
import org.apache.cocoon.components.source.SourceHandler;
import org.apache.cocoon.components.source.DelayedRefreshSourceWrapper;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.xml.sax.SAXException;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URL;

/**
 * Handles the manageing and stating of one <code>Sitemap</code>
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @version CVS $Revision: 1.4 $ $Date: 2002/01/25 03:49:57 $
 */
public class Handler extends AbstractLoggable
implements Runnable, Contextualizable, Composable, Processor, Disposable, SourceResolver {

    /** the component manager */
    protected ComponentManager manager;

    /** the source of this sitemap */
    protected String sourceFileName;
    protected Source source;

    /** the last error */
    protected Exception exception;

    /** the managed sitemap */
    protected Sitemap sitemap = null;
    protected boolean check_reload = true;

    /** the regenerating thread */
    protected Thread regeneration;
    protected volatile boolean isRegenerationRunning = false;
    protected Source contextSource;

    /** the sitemaps base path */
    protected String basePath;

    /** The source handler for the sitemap components */
    protected SourceHandler sourceHandler;

    // FIXME : ugly hack to pass delay information from the main sitemap configuration
    // (the way to pass it cleanly from SitemapManager isn't obvious).
    protected static long sitemapCheckDelay = 10000L; // default is 10 secs.

    static void setSitemapCheckDelay(long delay) {
        sitemapCheckDelay = delay;
    }

  public Handler() {}

    /**
     * Contextualizable
     */
    public void contextualize(Context context)
    throws ContextException {
    }

    /**
     * Composable
     */
    public void compose(ComponentManager manager)
    throws ComponentException {
        this.manager = manager;
        this.sourceHandler = (SourceHandler)manager.lookup(SourceHandler.ROLE);
    }

  public void setSourceFileName(String sourceFileName)
  {
    this.sourceFileName = sourceFileName;
  }

  public void setCheckReload(boolean check_reload)
  {
    this.check_reload = check_reload;
  }

    protected boolean available() {
        return (sitemap != null);
    }

    protected boolean hasChanged() {
        if (available()) {
            if (check_reload) {
                return sitemap.modifiedSince(this.source.getLastModified());
            }
            return false;
        }
        return true;
    }

    protected boolean isRegenerating() {
        return isRegenerationRunning;
    }

    protected void regenerateAsynchronously(Environment environment) throws Exception {
        if (this.sourceFileName.charAt(this.sourceFileName.length() - 1) == '/') {
            this.sourceFileName = this.sourceFileName + "sitemap.xmap";
        }
        SourceHandler oldSourceHandler = environment.getSourceHandler();
        try {
            environment.setSourceHandler(this.sourceHandler);

            if (this.source != null) this.source.recycle();
            this.source = new DelayedRefreshSourceWrapper(
                environment.resolve(this.sourceFileName), this.sitemapCheckDelay);

            if (this.contextSource != null) this.contextSource.recycle();
            this.contextSource = environment.resolve("");
        } finally {
            environment.setSourceHandler(oldSourceHandler);
        }
        if (!this.isRegenerationRunning) {
            isRegenerationRunning = true;
            regeneration = new Thread(this);

            /* HACK for reducing class loader problems. */
            /* example: xalan extensions fail if someone adds xalan jars in tomcat3.2.1/lib */
            try {
                regeneration.setContextClassLoader(Thread.currentThread().getContextClassLoader());
            } catch (Exception e) {
            }

            /* clear old exception if any */
            this.exception = null;

            /* start the thread */
            regeneration.start();
        }
    }

    protected void regenerate(Environment environment) throws Exception {
        getLogger().debug("Beginning sitemap regeneration");
        this.regenerateAsynchronously(environment);
        if (regeneration != null) {
            this.regeneration.join();
        }
        throwEventualException();
    }

    public boolean process(Environment environment) throws Exception {
        checkSanity();
        SourceHandler oldSourceHandler = environment.getSourceHandler();
        CocoonComponentManager.enterEnvironment(environment, environment.getObjectModel());
        try {
            environment.setSourceHandler(this.sourceHandler);
            return sitemap.process(environment);
        } finally {
            CocoonComponentManager.leaveEnvironment();
            environment.setSourceHandler(oldSourceHandler);
        }
    }

    public boolean process(Environment environment, StreamPipeline pipeline,
        EventPipeline eventPipeline) throws Exception {
        checkSanity();
        SourceHandler oldSourceHandler = environment.getSourceHandler();
        CocoonComponentManager.enterEnvironment(environment, environment.getObjectModel());
        try {
            environment.setSourceHandler(this.sourceHandler);
            return sitemap.process(environment, pipeline, eventPipeline);
        } finally {
            CocoonComponentManager.leaveEnvironment();
            environment.setSourceHandler(oldSourceHandler);
        }
    }

    private void checkSanity() throws Exception {
        throwEventualException();
        if (sitemap == null) {
            getLogger().fatalError("Sitemap is not set for the Handler!!!!");
            throw new RuntimeException("The Sitemap is null, this should never be!");
        }
    }

    public void setBasePath(String basePath) {
        this.basePath = basePath;
    }

    /** Generate the Sitemap class */
    public void run() {
        Sitemap smap = null;
        String markupLanguage = "sitemap";
        String programmingLanguage = "java";
        ProgramGenerator programGenerator = null;
        try {
            /* FIXME: Workaround -- set the logger XSLTFactoryLoader used to generate source
             * within the sitemap generation phase.
             * Needed because we never have the opportunity to handle the lifecycle of the
             * XSLTFactoryLoader, since it is created by the Xalan engine.
             */

            XSLTFactoryLoader.setLogger(getLogger());
            programGenerator = (ProgramGenerator)this.manager.lookup(ProgramGenerator.ROLE);
            smap = (Sitemap)programGenerator.load(this.manager, this.sourceFileName, markupLanguage,
                    programmingLanguage, this);
            if (this.sitemap != null) {
                programGenerator.release((CompiledComponent)this.sitemap);
            }
            this.sitemap = smap;
            // add the source factory for the cocoon protocol
            this.sourceHandler.addFactory("cocoon", new CocoonSourceFactory(this.sitemap,
                                                                            this.sitemap.getComponentManager()));
            getLogger().debug("Sitemap regeneration complete");
            if (this.sitemap != null) {
                getLogger().debug("The sitemap has been successfully compiled!");
            } else {
                getLogger().debug("No errors, but the sitemap has not been set.");
            }
        } catch (Throwable t) {
            getLogger().error("Error compiling sitemap", t);
            if (t instanceof Exception) {
                this.exception = (Exception)t;
            }
        } finally {
            if (programGenerator != null) {
                this.manager.release(programGenerator);
            }
            this.regeneration = null;
            this.isRegenerationRunning = false;
            this.contextSource.recycle();
            this.contextSource = null;
        }
    }

    public void throwEventualException() throws Exception {
        if (this.exception != null) {
            throw this.exception;
       }
    }

    public Exception getException() {
        return this.exception;
    }

    /**
     * dispose
     */
    public void dispose() {
        if (this.source != null) {
            this.source.recycle();
            this.source = null;
        }
        if (this.sourceHandler != null) {
            manager.release((Component)this.sourceHandler);
            this.sourceHandler = null;
        }
    }

    /**
     * Resolve an entity. Interface SourceResolver
     */
    public Source resolve(String systemId)
    throws ProcessingException, SAXException, IOException {
        if (systemId == null) throw new SAXException("Invalid System ID");

        URL context = new URL(this.contextSource.getSystemId());

        if (systemId.length() == 0)
            return this.sourceHandler.getSource(null, context, systemId);
        if (systemId.indexOf(":") > 1)
            return this.sourceHandler.getSource(null, systemId);
        if (systemId.charAt(0) == '/')
            return this.sourceHandler.getSource(null, new StringBuffer(context.getProtocol())
                                                .append(":").append(systemId).toString());
        return this.sourceHandler.getSource(null, context, systemId);
    }
}
