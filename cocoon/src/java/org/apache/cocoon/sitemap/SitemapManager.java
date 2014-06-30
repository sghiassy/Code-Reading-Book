/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import org.apache.avalon.excalibur.component.RoleManager;
import org.apache.avalon.excalibur.logger.LogKitManager;
import org.apache.avalon.excalibur.component.DefaultRoleManager;

import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.configuration.SAXConfigurationHandler;

import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.Processor;
import org.apache.cocoon.components.language.generator.CompiledComponent;
import org.apache.cocoon.components.language.generator.ProgramGenerator;
import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.components.source.SourceHandler;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.util.ClassUtils;

import org.xml.sax.InputSource;

import java.io.InputStream;

/**
 * A <code>Processor</code> based on sitemap language files compiled
 * to Java code.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.4 $ $Date: 2002/01/25 03:49:57 $
 */
public class SitemapManager extends Manager implements Processor, Configurable {
    
    /** The sitemap file */
    protected String sitemapFileName;

    /** Check reloading of sitemap */
    private boolean checkSitemapReload = true;

    /** reload sitemap asynchron */
    private boolean reloadSitemapAsynchron = true;

    public void configure(Configuration sconf) throws ConfigurationException {

        super.configure(sconf);

        this.sitemapFileName = sconf.getAttribute("file");

        String value = sconf.getAttribute("check-reload", "yes");
        this.checkSitemapReload = !(value != null && value.equalsIgnoreCase("no"));
        
        value = sconf.getAttribute("reload-method", "asynchron");
        this.reloadSitemapAsynchron = !(value != null && value.equalsIgnoreCase("synchron"));

        long checkDelay = sconf.getAttributeAsLong("check-delay", 1L);
        Handler.setSitemapCheckDelay(checkDelay * 1000L);
        
        getLogger().debug("Sitemap location = " + this.sitemapFileName);
        getLogger().debug("Checking sitemap reload = " + this.checkSitemapReload);
        getLogger().debug("Reloading sitemap asynchron = " + this.reloadSitemapAsynchron);
        getLogger().debug("Sitemap check delay = " + checkDelay + " sec");
        
        System.err.println("Sitemap location = " + this.sitemapFileName);
        System.err.println("Checking sitemap reload = " + this.checkSitemapReload);
        System.err.println("Reloading sitemap asynchron = " + this.reloadSitemapAsynchron);
        System.err.println("Sitemap check delay = " + checkDelay + " sec");
        
        // Read sitemap roles
        Parser p = null;
        Configuration rolesConfig;
        try {
            p = (Parser)this.manager.lookup(Parser.ROLE);
            SAXConfigurationHandler b = new SAXConfigurationHandler();
            InputStream inputStream = ClassUtils.getResource("org/apache/cocoon/sitemap/sitemap.roles").openStream();
            InputSource is = new InputSource(inputStream);
            is.setSystemId("org/apache/cocoon/sitemap/sitemap.roles");
            p.setContentHandler(b);
            p.parse(is);
            rolesConfig = b.getConfiguration();
        } catch (Exception e) {
            getLogger().error("Could not configure Cocoon environment", e);
            throw new ConfigurationException("Error trying to load configurations", e);
        } finally {
            if (p != null) this.manager.release(p);
        }
    
        DefaultRoleManager sitemapRoleManager = new DefaultRoleManager();
        sitemapRoleManager.setLogger(getLogger());
        sitemapRoleManager.configure(rolesConfig);
    
        AbstractSitemap.setRoleManager(sitemapRoleManager, sconf);
        AbstractSitemap.setLogKitManager(this.sitemapLogKitManager);
    }

    /**
     * Process the given <code>Environment</code> to generate the sitemap.
     */
    public void generateSitemap(Environment environment)
    throws Exception {
        ProgramGenerator programGenerator = null;
        SourceHandler oldSourceHandler = environment.getSourceHandler();
        SourceHandler sourceHandler = null;
        try {
            programGenerator = (ProgramGenerator) this.manager.lookup(ProgramGenerator.ROLE);
            sourceHandler = (SourceHandler) this.manager.lookup(SourceHandler.ROLE);
            environment.setSourceHandler(sourceHandler);
            String markupLanguage = "sitemap";
            String programmingLanguage = "java";

            getLogger().debug("Sitemap regeneration begin:" + sitemapFileName);
            CompiledComponent smap = programGenerator.load(this.manager, sitemapFileName, markupLanguage, programmingLanguage, environment);
            getLogger().debug("Sitemap regeneration complete");

            if (smap != null) {
                getLogger().debug("Main: The sitemap has been successfully compiled!");
            } else {
                getLogger().debug("Main: No errors, but the sitemap has not been set.");
            }
        } catch (Exception e) {
            getLogger().error("Main: Error compiling sitemap", e);
            throw e;
        } finally {
            environment.setSourceHandler(oldSourceHandler);
            if (programGenerator != null) this.manager.release(programGenerator);
            if (sourceHandler != null) this.manager.release(sourceHandler);
        }
    }

    /**
     * Process the given <code>Environment</code> producing the output.
     */
    public boolean process(Environment environment) throws Exception {
        return this.invoke(this.manager, environment, "", this.sitemapFileName, this.checkSitemapReload, this.reloadSitemapAsynchron);
    }

    /**
     * Process the given <code>Environment</code> to assemble
     * a <code>StreamPipeline</code> and an <code>EventPipeline</code>.
     */
    public boolean process(Environment environment, StreamPipeline pipeline, EventPipeline eventPipeline) throws Exception {
        return this.invoke(this.manager, environment, "", this.sitemapFileName, this.checkSitemapReload, this.reloadSitemapAsynchron, pipeline, eventPipeline);
    }
}
