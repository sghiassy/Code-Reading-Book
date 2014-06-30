/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup;

import org.apache.avalon.framework.component.Component;
import org.apache.cocoon.components.language.programming.ProgrammingLanguage;
import org.apache.cocoon.environment.SourceResolver;
import org.xml.sax.InputSource;

/**
 * This interface defines a markup language whose SAX producer's instance are to
 * be translated into an executable program capable or transforming the original
 * document augmenting it with dynamic content
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:09 $
 */
public interface MarkupLanguage extends Component {

    String ROLE = "org.apache.cocoon.components.language.markup.MarkupLanguage";
    /**
    * Return the input document's encoding or <code>null</code> if it is the
    * platform's default encoding.
    * This method should be called after <code>generateCode<code> method.
    *
    * @return The input document's encoding
    */
    String getEncoding();

    /**
    * Generate source code from the input source for the target
    * <code>ProgrammingLanguage</code>.
    *
    * @param input The input source document
    * @param filename The input document's original filename
    * @param programmingLanguage The target programming language
    * @return The generated source code
    * @exception Exception If an error occurs during code generation
    */
    String generateCode(
        InputSource input, String filename,
        ProgrammingLanguage programmingLanguage,
        SourceResolver resolver
    ) throws Exception;
}
