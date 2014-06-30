/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.programming;

import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.Constants;
import org.apache.cocoon.components.language.LanguageException;
import org.apache.cocoon.util.ClassUtils;
import org.apache.cocoon.util.IOUtils;

import java.io.File;

/**
 * A compiled programming language. This class extends <code>AbstractProgrammingLanguage</code> adding support for compilation
 * and object program files
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:11 $
 */
public abstract class CompiledProgrammingLanguage extends AbstractProgrammingLanguage implements Contextualizable {
    /** The compiler */
    protected Class compilerClass;

    /** The local classpath */
    protected String classpath;

    /** The source deletion option */
    protected boolean deleteSources = false;

    /**
     * Set the configuration parameters. This method instantiates the sitemap-specified language compiler
     * @param params The configuration parameters
     * @exception Exception If the language compiler cannot be loaded
     */
    protected void setParameters(Parameters params) throws Exception {
        super.setParameters(params);
        String compilerClass = params.getParameter("compiler", null);
        if (compilerClass == null) {
            throw new LanguageException("Missing 'compiler' parameter for compiled language '" + this.getLanguageName() + "'");
        }
        this.compilerClass = ClassUtils.loadClass(compilerClass);
        this.deleteSources = params.getParameterAsBoolean("delete-sources", false);
    }

    /**
     * Set the global component manager
     * @param manager The global component manager
     */
    public void contextualize(Context context) throws ContextException {
        this.classpath = (String) context.get(Constants.CONTEXT_CLASSPATH);
    }

    /**
     * Return the language's canonical object file extension.
     * @return The object file extension
     */
    public abstract String getObjectExtension();

    /**
     * Unload a previously loaded program
     * @param program A previously loaded object program
     * @exception LanguageException If an error occurs during unloading
     */
    public abstract void doUnload(Object program) throws LanguageException;

    /**
     * Unload a previously loaded program given its original filesystem location
     * @param program The previously loaded object program
     * @param filename The base filename of the object program
     * @param baseDirectory The directory contaning the object program file
     * @return the value
     * @exception EXCEPTION_NAME If an error occurs
     */
    protected final void doUnload(Object program, String filename, File baseDirectory) throws LanguageException {
        int index = filename.lastIndexOf(File.separator);
        String dir = filename.substring(0, index);
        String file = filename.substring(index + 1);

        File baseDir = new File(baseDirectory, dir);
        File[] files = baseDir.listFiles();

        for (int i = 0; i < files.length; i++) {
            if (files[i].getName().startsWith(file)) {
                files[i].delete();
            }
        }
        this.doUnload(program);
    }

    /**
     * Actually load an object program from a file.
     * @param filename The object program base file name
     * @param baseDirectory The directory containing the object program file
     * @return The loaded object program
     * @exception LanguageException If an error occurs during loading
     */
    protected abstract Class loadProgram(String filename, File baseDirectory) throws LanguageException;

    /**
     * Compile a source file yielding a loadable object file.
     * @param filename The object program base file name
     * @param baseDirectory The directory containing the object program file
     * @param encoding The encoding expected in the source file or <code>null</code> if it is the platform's default encoding
     * @exception LanguageException If an error occurs during compilation
     */
    protected abstract void compile(String filename, File baseDirectory, String encoding) throws LanguageException;

    /**
     * Load an object program from a file. This method compiled the corresponding source file if necessary
     * @param filename The object program base file name
     * @param baseDirectory The directory containing the object program file
     * @param encoding The encoding expected in the source file or <code>null</code> if it is the platform's default encoding
     * @return The loaded object program
     * @exception LanguageException If an error occurs during compilation
     */
    public Class load(String filename, File baseDirectory, String encoding) throws LanguageException {
        // Does object file exist? Load and return instance
        File objectFile = new File(baseDirectory, new StringBuffer(filename).append(".").append(this.getObjectExtension()).toString());
        if (objectFile.exists() && objectFile.isFile() && objectFile.canRead()) {
            return this.loadProgram(filename, baseDirectory);
        }
        // Does source file exist?
        File sourceFile = new File(baseDirectory, new StringBuffer(filename).append(".").append(this.getSourceExtension()).toString());
        if (!sourceFile.exists()) {
            throw new LanguageException("Can't load program - File doesn't exist: " + IOUtils.getFullFilename(sourceFile));
        }
        if (!sourceFile.isFile()) {
            throw new LanguageException("Can't load program - File is not a normal file: " + IOUtils.getFullFilename(sourceFile));
        }
        if (!sourceFile.canRead()) {
            throw new LanguageException("Can't load program - File cannot be read: " + IOUtils.getFullFilename(sourceFile));
        }
        this.compile(filename, baseDirectory, encoding);
        if (this.deleteSources) {
            sourceFile.delete();
        }
        Class program = this.loadProgram(filename, baseDirectory);
        
        // Try to instantiate once to ensure there are no exceptions thrown in the constructor
        try {
            Object testInstance = program.newInstance();
            
        } catch(IllegalAccessException iae) {
            getLogger().debug("No public constructor for class " + program.getName());
            
        } catch(Exception e) {
            // Unload class and delete the object file, or it won't be recompiled
            // (leave the source file to allow examination).
            this.doUnload(program);
            objectFile.delete();
            
            String message = "Error while instantiating " + filename;
            getLogger().warn(message, e);
            throw new LanguageException(message, e);
        }
        
        if (program == null) {
            throw new LanguageException("Can't load program : " + baseDirectory.toString() + File.separator + filename);
        }
        return program;
    }
}
