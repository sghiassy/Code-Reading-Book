/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup;

import org.apache.avalon.framework.component.ComponentManager;
import org.apache.cocoon.components.url.URLFactory;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.SourceResolver;

import org.xml.sax.SAXException;

import java.io.IOException;
import java.net.MalformedURLException;

/**
 * An extension to <code>Logicsheet</code> that is associated with a namespace.
 * Named logicsheets are implicitly declared (and automagically applied) when
 * the markup language document's root element declares the same logichseet's
 * namespace
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/26 04:42:39 $
 */
public class NamedLogicsheet extends Logicsheet {
    /**
    * The namespace uri
    */
    // FIXME: NOT USED: protected String uri;

    /**
    * The namespace prefix
    */
    private String prefix;

    public NamedLogicsheet(String systemId, ComponentManager manager, SourceResolver resolver)
        throws IOException, ProcessingException, SAXException
    {
        super(systemId, manager, resolver);
    }
  
    /**
    * Set the logichseet's namespace prefix
    *
    * @param prefix The namespace prefix
    */
    public void setPrefix(String prefix) {
        this.prefix = prefix;
    }

    /**
    * Return the logicsheet's namespace prefix
    *
    * @return The logicsheet's namespace prefix
    */
    public String getPrefix() {
        return this.prefix;
    }
}
