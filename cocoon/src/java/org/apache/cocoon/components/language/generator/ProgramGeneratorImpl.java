/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.language.generator;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.ComponentSelector;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.parameters.ParameterException;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.classloader.ClassLoaderManager;
import org.apache.cocoon.components.language.LanguageException;
import org.apache.cocoon.components.language.markup.MarkupLanguage;
import org.apache.cocoon.components.language.markup.sitemap.SitemapMarkupLanguage;
import org.apache.cocoon.components.language.programming.CodeFormatter;
import org.apache.cocoon.components.language.programming.ProgrammingLanguage;
import org.apache.cocoon.components.store.Store;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.util.IOUtils;
import org.xml.sax.InputSource;

import java.io.File;
import java.net.MalformedURLException;

/**
 * The default implementation of <code>ProgramGenerator</code>
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/29 15:33:26 $
 */
public class ProgramGeneratorImpl extends AbstractLoggable
    implements ProgramGenerator, Contextualizable, Composable, Parameterizable,
               Disposable, ThreadSafe {

    /** The auto-reloading option */
    protected boolean autoReload = false;

    /** The pre-loading option */
    protected boolean preload = false;

    /** The ComponentSelector for CompiledPages */
    protected GeneratorSelector cache;

    /** The repository store */
    protected Store repository;

    /** The component manager */
    protected ComponentManager manager;

    /** The markup language component selector */
    protected ComponentSelector markupSelector;

    /** The programming language component selector */
    protected ComponentSelector languageSelector;

    /** The working directory */
    protected File workDir;

    /** The ClassLoaderManager */
    protected ClassLoaderManager classManager;

    /** The root package */
    protected String rootPackage;

    /** Servlet Context Directory */
    protected String contextDir;

    /** Contextualize this class */
    public void contextualize(Context context) throws ContextException {
        if (this.workDir == null) {
            this.workDir = (File) context.get(Constants.CONTEXT_WORK_DIR);
        }

        if (this.contextDir == null) {
            org.apache.cocoon.environment.Context ctx =
                (org.apache.cocoon.environment.Context) context.get(Constants.CONTEXT_ENVIRONMENT_CONTEXT);

            // Determine the context directory, preferably as a file
            // FIXME (SW) - this is purposely redundant with some code in CocoonServlet
            //              to have the same rootPath. How to avoid this ?
            try {
                String rootPath = ctx.getRealPath("/");
                if (rootPath != null) {
                    this.contextDir = new File(rootPath).toURL().toExternalForm();
                } else {
                    String webInf = ctx.getResource("/WEB-INF").toExternalForm();
                    this.contextDir = webInf.substring(0, webInf.length() - "WEB-INF".length());
                }
                getLogger().debug("Context directory is " + this.contextDir);
            } catch (MalformedURLException e) {
                getLogger().warn("Could not get context directory", e);
                this.contextDir = "";
            }
        }
    }

    /**
     * Set the global component manager. This method also sets the
     * <code>ComponentSelector</code> used as language factory for both markup
     * and programming languages.
     * @param manager The global component manager
     */
    public void compose(ComponentManager manager) throws ComponentException {
        if ((this.manager == null) && (manager != null)) {
            this.manager = manager;
            this.cache = (GeneratorSelector) this.manager.lookup(GeneratorSelector.ROLE + "Selector");
            this.repository = (Store) this.manager.lookup(Store.ROLE + "/Filesystem");
            this.markupSelector = (ComponentSelector)this.manager.lookup(MarkupLanguage.ROLE + "Selector");
            this.languageSelector = (ComponentSelector)this.manager.lookup(ProgrammingLanguage.ROLE + "Selector");
            this.classManager = (ClassLoaderManager)this.manager.lookup(ClassLoaderManager.ROLE);
        }
    }

    /**
     * Set the sitemap-provided configuration. This method sets the persistent code repository and the auto-reload option
     * @param conf The configuration information
     * @exception ConfigurationException Not thrown here
     */
    public void parameterize(Parameters params) throws ParameterException {
        this.autoReload = params.getParameterAsBoolean("auto-reload", autoReload);
        this.rootPackage = params.getParameter("root-package", "org.apache.cocoon.www");
        this.preload = params.getParameterAsBoolean("preload", preload);
    }

    /**
     * Load a program built from an XML document written in a <code>MarkupLanguage</code>
     * @param file The input document's <code>File</code>
     * @param markupLanguage The <code>MarkupLanguage</code> in which the input document is written
     * @param programmingLanguage The <code>ProgrammingLanguage</code> in which the program must be written
     * @return The loaded program instance
     * @exception Exception If an error occurs during generation or loading
     */
    public CompiledComponent load(ComponentManager newManager,
                                  String fileName,
                                  String markupLanguageName,
                                  String programmingLanguageName,
                                  SourceResolver resolver)
        throws Exception {

        Source source = resolver.resolve(fileName);
        try {
            // Set filenames
            StringBuffer contextFilename = new StringBuffer(this.rootPackage.replace('.', File.separatorChar));
            contextFilename.append(File.separator);
            String id = source.getSystemId();
            if(id.startsWith(this.contextDir)) {
                // VG: File is located under contextDir, using relative file name
                contextFilename.append(id.substring(this.contextDir.length()));
            } else {
                // VG: File is located outside of contextDir, using systemId
                getLogger().debug("Loading from external source " + id);
                contextFilename.append(id);
            }
            String normalizedName = IOUtils.normalizedFilename(contextFilename.toString());

            // Ensure no 2 requests for the same file overlap
            Class program = null;
            CompiledComponent programInstance = null;

            // Attempt to load program object from cache
            try {
                programInstance = (CompiledComponent) select(normalizedName);
            } catch (Exception e) {
                getLogger().debug("The instance was not accessible from the internal cache. Proceeding.");
            }

            if ((programInstance == null) && this.preload) {
                String className = normalizedName.replace(File.separatorChar, '.');
                try {
                    program = this.classManager.loadClass(className);
                    this.addCompiledComponent(newManager, normalizedName, program);
                    programInstance = (CompiledComponent) select(normalizedName);
                } catch (Exception e) {
                    getLogger().debug("The class was not preloaded");
                }
            }

            if (programInstance == null) {
	            programInstance =
                    this.createResource(
                        newManager, fileName, normalizedName,
                        markupLanguageName, programmingLanguageName, resolver
                    );
            }

            if (!this.autoReload) {
                return programInstance;
            }

            /*
             * FIXME: It's the program (not the instance) that must
             * be queried for changes!!!
             */
            long lastModified = source.getLastModified();
            if (programInstance != null &&
                (lastModified == 0 || programInstance.modifiedSince(lastModified)))
            {
                // Release the component.
                release(programInstance);

                // Unload program
                ProgrammingLanguage programmingLanguage = (ProgrammingLanguage)this.languageSelector.select(programmingLanguageName);
                programmingLanguage.setLanguageName(programmingLanguageName);
                programmingLanguage.unload(program, normalizedName, this.workDir);
                this.cache.removeGenerator(normalizedName);

                // Invalidate previous program/instance pair
                program = null;
                programInstance = null;
            }

            if (programInstance == null) {
                if (program == null) {
                    programInstance =
                        this.createResource(
                            newManager, fileName, normalizedName,
                            markupLanguageName, programmingLanguageName,
                            resolver
                        );
                } else {
                    programInstance = (CompiledComponent) select(normalizedName);
                }
            }

            return programInstance;
        } finally {
            source.recycle();
        }
    }

    /**
     * Helper method to create resources in a threadsafe manner.
     */
    private CompiledComponent createResource(
        ComponentManager newManager,
        String fileName,
        String normalizedName,
        String markupLanguageName,
        String programmingLanguageName,
        SourceResolver resolver
    )
    throws Exception {

        CompiledComponent programInstance = null;
        MarkupLanguage markupLanguage = null;
        ProgrammingLanguage programmingLanguage = null;
        Class program = null;

        // prevent 2 requests from generating this resource simultaneously
        synchronized (this) {
            try {
                programInstance = (CompiledComponent) select(normalizedName);
            } catch (Exception e) {

                getLogger().debug(
                     "Creating resource " +
                     normalizedName.replace(File.separatorChar, '.') +
                     ", using generator " + this
                );

                try {
                    // Get markup and programming languages
                    markupLanguage = (MarkupLanguage)this.markupSelector.select(markupLanguageName);
                    programmingLanguage = (ProgrammingLanguage)this.languageSelector.select(programmingLanguageName);
                    programmingLanguage.setLanguageName(programmingLanguageName);
                    program = this.generateResource(newManager, fileName, normalizedName, markupLanguage, programmingLanguage, resolver);
                } catch (LanguageException le) {
                    getLogger().debug("Language Exception", le);
                    throw new ProcessingException("Language Exception", le);
                } finally {
                    if (this.markupSelector != null) {
                        this.markupSelector.release(markupLanguage);
                    }

                    if (this.languageSelector != null) {
                        this.languageSelector.release(programmingLanguage);
                    }
                }

                try {
                    programInstance = (CompiledComponent) select(normalizedName);
                } catch (Exception cme) {
                    getLogger().debug("Can't load ServerPage", cme);
                }
            }
        }

	return programInstance;
    }

    private Class generateResource(ComponentManager newManager,
                                   String fileName,
                                   String normalizedName,
                                   MarkupLanguage markupLanguage,
                                   ProgrammingLanguage programmingLanguage,
                                   SourceResolver resolver)
        throws Exception {

        Source source = resolver.resolve(fileName);
        try {
            // Input Source
            InputSource is = source.getInputSource();
            // Generate code
            String code = markupLanguage.generateCode(is, normalizedName, programmingLanguage, resolver);
            String encoding = markupLanguage.getEncoding();
            // Format source code if applicable
            CodeFormatter codeFormatter = programmingLanguage.getCodeFormatter();
            if (codeFormatter != null) {
                code = codeFormatter.format(code, encoding);
            }
            // Store generated code
            String sourceFilename = new StringBuffer(normalizedName).append(".")
                                    .append(programmingLanguage.getSourceExtension()).toString();
            repository.store(sourceFilename, code);
            // [Compile]/Load generated program
            Class program = programmingLanguage.load(normalizedName, this.workDir, markupLanguage.getEncoding());
            // Store generated program in cache
            this.addCompiledComponent(newManager, normalizedName, program);

            // FIXME: Do we want this functionality?  All analysis says no.
            if (markupLanguage.getClass().equals(SitemapMarkupLanguage.class)) {
                try {
                    select("sitemap");
                } catch (Exception e) {
                    // If the root sitemap has not been compiled, add an alias here.
                    this.addCompiledComponent(newManager, "sitemap", program);
                }
            }

            return program;
        } finally {
            source.recycle();
        }
    }

    private final void addCompiledComponent(ComponentManager newManager,
                                            String normalizedName,
                                            Class program)
    throws Exception {
        this.cache.addGenerator(newManager, normalizedName, program);
    }

    public CompiledComponent select(String componentName)
        throws Exception {
        CompiledComponent component = (CompiledComponent)this.cache.select(componentName);
        return component;
    }

    public void release(CompiledComponent component) {
        this.cache.release(component);
    }

    /**
     *  dispose
     */
    public void dispose() {
        this.manager.release(this.cache);
        this.cache = null;
        this.manager.release(this.repository);
        this.repository = null;
        this.manager.release(this.markupSelector);
        this.markupSelector = null;
        this.manager.release(this.languageSelector);
        this.languageSelector = null;
        this.manager.release(this.classManager);
        this.classManager = null;

        this.manager = null;

        this.workDir = null;
        this.contextDir = null;
    }
}
