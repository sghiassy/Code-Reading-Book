/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.programming;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.logger.Loggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.components.language.LanguageException;
import org.apache.cocoon.util.ClassUtils;

import java.io.File;

/**
 * Base implementation of <code>ProgrammingLanguage</code>. This class sets the
 * <code>CodeFormatter</code> instance and deletes source program files after
 * unloading.
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:11 $
 */
public abstract class AbstractProgrammingLanguage extends AbstractLoggable
  implements ProgrammingLanguage, Configurable
{
  /** The source code formatter */
  protected Class codeFormatter;

  protected String languageName;

  /**
   * Configure the language
   */
  public void configure(Configuration conf) throws ConfigurationException {
      try {
          getLogger().debug("Setting the parameters");
          this.setParameters( Parameters.fromConfiguration(conf) );
      } catch (Exception e) {
          getLogger().error("Could not set Parameters", e);
          throw new ConfigurationException("Could not get parameters because: " +
                                           e.getMessage());
      }
  }

  /**
   * Set the configuration parameters. This method instantiates the
   * sitemap-specified source code formatter
   *
   * @param params The configuration parameters
   * @exception Exception If the language compiler cannot be loaded
   */
  protected void setParameters(Parameters params) throws Exception
  {
    try {
      String className = params.getParameter("code-formatter", null);
      if (className != null) {
        this.codeFormatter = ClassUtils.loadClass(className);
      }
    } catch (Exception e) {
       getLogger().error("Error with \"code-formatter\" parameter", e);
       throw e;
    }
  }

  /**
   * Return this language's source code formatter. A new formatter instance is
   * created on each invocation.
   *
   * @return The language source code formatter
   */
  public CodeFormatter getCodeFormatter() {
    if (this.codeFormatter != null) {
      try {
        CodeFormatter formatter = (CodeFormatter) this.codeFormatter.newInstance();
        if (formatter instanceof Loggable) {
            ((Loggable)formatter).setLogger(this.getLogger());
        }
        return formatter;
      } catch (Exception e) {
          getLogger().error("Error instantiating CodeFormatter", e);
      }
    }

    return null;
  }

  /**
   * Unload a previously loaded program
   *
   * @param program A previously loaded object program
   * @exception LanguageException If an error occurs during unloading
   */
  protected abstract void doUnload(
    Object program, String filename, File baseDirectory
  )
    throws LanguageException;

  public final void unload(
    Object program, String filename, File baseDirectory
  )
    throws LanguageException
  {
    File file = new File (
      baseDirectory, new StringBuffer(filename).append(".").append(this.getSourceExtension()).toString()
    );

    file.delete();

    this.doUnload(program, filename, baseDirectory);
  }

  public final void setLanguageName(String name) {
    this.languageName = name;
  }

  public final String getLanguageName() {
    return this.languageName;
  }
}
