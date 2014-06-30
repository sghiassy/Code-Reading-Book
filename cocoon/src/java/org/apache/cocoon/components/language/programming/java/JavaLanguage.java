/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.language.programming.java;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.components.classloader.ClassLoaderManager;
import org.apache.cocoon.components.language.LanguageException;
import org.apache.cocoon.components.language.generator.CompiledComponent;
import org.apache.cocoon.components.language.programming.CompiledProgrammingLanguage;
import org.apache.cocoon.components.language.programming.CompilerError;
import org.apache.cocoon.util.ClassUtils;
import org.apache.cocoon.util.JavaArchiveFilter;

import java.io.File;
import java.io.IOException;
import java.util.List;
import java.util.StringTokenizer;

/**
 * The Java programming language processor
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:11 $
 */
public class JavaLanguage extends CompiledProgrammingLanguage implements ThreadSafe, Composable, Disposable {

  /** The class loader */
  private ClassLoaderManager classLoaderManager;

  /** The component manager */
  protected ComponentManager manager = null;

  /**
   * Return the language name
   *
   * @return The language name
   */
  public String getName() {
    return "java";
  }

  /**
   * Return the language's canonical source file extension.
   *
   * @return The source file extension
   */
  public String getSourceExtension() {
    return "java";
  }

  /**
   * Return the language's canonical object file extension.
   *
   * @return The object file extension
   */
  public String getObjectExtension() {
    return "class";
  }

  /**
   * Set the configuration parameters. This method instantiates the
   * sitemap-specified <code>ClassLoaderManager</code>
   *
   * @param params The configuration parameters
   * @exception Exception If the class loader manager cannot be instantiated
   */
  protected void setParameters(Parameters params) throws Exception {
    super.setParameters(params);

    String compilerClass = params.getParameter("class-loader", "org.apache.cocoon.components.classloader.ClassLoaderManagerImpl");
    if (compilerClass != null) {
        this.classLoaderManager = (ClassLoaderManager) ClassUtils.newInstance(compilerClass);
    }
  }

  /**
   * Set the global component manager. This methods initializes the class
   * loader manager if it was not (successfully) specified in the language
   * parameters
   *
   * @param manager The global component manager
   */
  public void compose(ComponentManager manager) {
    this.manager = manager;
    if (this.classLoaderManager == null) {
      try {
          getLogger().debug("Looking up " + ClassLoaderManager.ROLE);
          this.classLoaderManager =
            (ClassLoaderManager) manager.lookup(ClassLoaderManager.ROLE);
      } catch (Exception e) {
          getLogger().error("Could not find component", e);
      }
    }
  }

  /**
   * Actually load an object program from a class file.
   *
   * @param filename The object program base file name
   * @param baseDirectory The directory containing the object program file
   * @return The loaded object program
   * @exception LanguageException If an error occurs during loading
   */
  protected Class loadProgram(String name, File baseDirectory)
    throws LanguageException
  {
    try {
      this.classLoaderManager.addDirectory(baseDirectory);
      return
        this.classLoaderManager.loadClass(name.replace(File.separatorChar, '.'));
    } catch (Exception e) {
      getLogger().warn("Could not load class for program '" + name + "'", e);
      throw new LanguageException("Could not load class for program '" + name + "' due to a " + e.getClass().getName() + ": " + e.getMessage());
    }
  }

  /**
   * Compile a source file yielding a loadable class file.
   *
   * @param filename The object program base file name
   * @param baseDirectory The directory containing the object program file
   * @param encoding The encoding expected in the source file or
   * <code>null</code> if it is the platform's default encoding
   * @exception LanguageException If an error occurs during compilation
   */
  protected void compile(
    String name, File baseDirectory, String encoding
  ) throws LanguageException {

    try {

      AbstractJavaCompiler compiler = (AbstractJavaCompiler) this.compilerClass.newInstance();
      // AbstractJavaCompiler is Loggable
      compiler.setLogger(getLogger());

      int pos = name.lastIndexOf(File.separatorChar);
      String filename = name.substring(pos + 1);
      String pathname =
        baseDirectory.getCanonicalPath() + File.separator +
        name.substring(0, pos).replace(File.separatorChar, '/');
      String filename_abs =
        new StringBuffer(pathname).append(File.separator).append(filename)
              .append(".").append(this.getSourceExtension()).toString();

      compiler.setFile(filename_abs);

      compiler.setSource(pathname);

      compiler.setDestination(baseDirectory.getCanonicalPath());

      String systemBootClasspath = System.getProperty("sun.boot.class.path");
      String systemClasspath = System.getProperty("java.class.path");
      String systemExtDirs = System.getProperty("java.ext.dirs");
      String systemExtClasspath = null;

      try {
        systemExtClasspath = expandDirs(systemExtDirs);
      } catch (Exception e) {
        getLogger().warn("Could not expand Directory:" + systemExtDirs, e);
      }

      compiler.setClasspath(
        baseDirectory.getCanonicalPath() +
        ((classpath != null) ? File.pathSeparator + classpath : "") +
        ((systemBootClasspath != null) ? File.pathSeparator + systemBootClasspath : "") +
        ((systemClasspath != null) ? File.pathSeparator + systemClasspath : "") +
        ((systemExtClasspath != null) ? File.pathSeparator + systemExtClasspath : "")
      );

      if (encoding != null) {
        compiler.setEncoding(encoding);
      }

      getLogger().debug("Compiling " + filename_abs);

      if (!compiler.compile()) {
        StringBuffer message = new StringBuffer("Error compiling ");
        message.append(filename);
        message.append(":\n");

        List errors = compiler.getErrors();
        int count = errors.size();
        for (int i = 0; i < count; i++) {
          CompilerError error = (CompilerError) errors.get(i);
          if (i > 0) message.append("\n");
          message.append("Line ");
          message.append(error.getStartLine());
          message.append(", column ");
          message.append(error.getStartColumn());
          message.append(": ");
          message.append(error.getMessage());
        }

        throw new LanguageException(message.toString());
      }

    } catch (InstantiationException e) {
      getLogger().warn("Could not instantiate the compiler", e);
      throw new LanguageException("Could not instantiate the compiler: " + e.getMessage());
    } catch (IllegalAccessException e) {
      getLogger().warn("Could not access the compiler class", e);
      throw new LanguageException("Could not access the compiler class: " + e.getMessage());
    } catch (IOException e) {
      getLogger().warn("Error during compilation", e);
      throw new LanguageException("Error during compilation: " + e.getMessage());
    }
  }

  /**
   * Create a new instance for the given class
   *
   * @param program The Java class
   * @return A new class instance
   * @exception LanguageException If an instantiation error occurs
   */
  public CompiledComponent instantiate(Class program) throws LanguageException {
    try {
      return (CompiledComponent) program.newInstance();
    } catch (Exception e) {
      getLogger().warn("Could not instantiate program instance", e);
      throw new LanguageException("Could not instantiate program instance due to a " + e.getClass().getName() + ": " + e.getMessage());
    }
  }

  /**
   * Unload a previously loaded class. This method simply reinstantiates the
   * class loader to ensure that a new version of the same class will be
   * correctly loaded in a future loading operation
   *
   * @param program A previously loaded class
   * @exception LanguageException If an error occurs during unloading
   */
  public void doUnload(Object program) throws LanguageException {
    this.classLoaderManager.reinstantiate();
  }

  /**
   * Escape a <code>String</code> according to the Java string constant
   * encoding rules.
   *
   * @param constant The string to be escaped
   * @return The escaped string
   */
  public String quoteString(String constant) {
    char chr[] = constant.toCharArray();
    StringBuffer buffer = new StringBuffer();

    for (int i = 0; i < chr.length; i++) {
      switch (chr[i]) {
        case '\t':
          buffer.append("\\t");
          break;
        case '\r':
          buffer.append("\\r");
          break;
        case '\n':
          buffer.append("\\n");
          break;
        case '"':
        case '\\':
          buffer.append('\\');
          // Fall through
        default:
          buffer.append(chr[i]);
          break;
      }
    }

    return buffer.toString();
  }

  /**
   * Expand a directory path or list of directory paths (File.pathSeparator
   * delimited) into a list of file paths of all the jar files in those
   * directories.
   *
   * @param dirPaths The string containing the directory path or list of
   * 		directory paths.
   * @return The file paths of the jar files in the directories. This is an
   *		empty string if no files were found, and is terminated by an
   *		additional pathSeparator in all other cases.
   */
  private String expandDirs(String dirPaths) throws LanguageException {
    StringTokenizer st = new StringTokenizer(dirPaths, File.pathSeparator);
    StringBuffer buffer = new StringBuffer();
    while (st.hasMoreTokens()) {
        String d = st.nextToken();
        File dir = new File(d);
        if ( ! dir.isDirectory() ) {
            // The absence of a listed directory may not be an error.
            if (getLogger().isWarnEnabled()) getLogger().warn("Attempted to retrieve directory listing of non-directory " + dir.toString());
        } else {
            File[] files = dir.listFiles(new JavaArchiveFilter());
            for (int i = 0; i < files.length; i++) {
                buffer.append(files[i]).append(File.pathSeparator);
            }
        }
    }
    return buffer.toString();
  }

  /**
   *  dispose
   */
  public void dispose() {
    manager.release((Component)this.classLoaderManager);
  }
}
