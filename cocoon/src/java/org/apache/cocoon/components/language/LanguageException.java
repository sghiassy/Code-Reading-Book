/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language;

import org.apache.avalon.framework.CascadingException;

import java.io.PrintStream;
import java.io.PrintWriter;

/**
 * The language exception.
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:09 $
 */
public class LanguageException extends CascadingException {
    /**
     * The default constructor.
     */
    public LanguageException(String message) {
        super(message);
    }
  
    public LanguageException(String message, Throwable t) {
        super(message, t);
    }
  
  
    // Stolen from ProcessingException...
    
    public String toString() {
        StringBuffer s = new StringBuffer();
        s.append(super.toString());
        if(getCause()!=null) {
            s.append(": ");
            s.append(getCause().toString());
        }
        return s.toString();
    }

    public void printStackTrace() {
        super.printStackTrace();
        if(getCause()!=null)
            getCause().printStackTrace();
    }

    public void printStackTrace( PrintStream s ) {
        super.printStackTrace(s);
        if(getCause()!=null)
            getCause().printStackTrace(s);
    }

    public void printStackTrace( PrintWriter s ) {
        super.printStackTrace(s);
        if(getCause()!=null)
            getCause().printStackTrace(s);
    }

}
