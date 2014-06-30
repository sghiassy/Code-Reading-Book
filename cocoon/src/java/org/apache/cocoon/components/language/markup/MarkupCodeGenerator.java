/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup;

import org.xml.sax.InputSource;
import org.xml.sax.XMLReader;

/**
 * This interfaces defines the functionality of a source code generator
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:09 $
 */
public interface MarkupCodeGenerator {
    /**
    * Generate source code from the input reader. Filename information may be
    * needed by certain code-generation approaches and programming languages
    *
    * @param reader The input reader
    * @param input The input source
    * @param filename The input source original filename
    * @return The generated source code
    * @exception Exception If an error occurs during code generation
    */
    String generateCode(XMLReader reader, InputSource input, String filename)
        throws Exception;
}
