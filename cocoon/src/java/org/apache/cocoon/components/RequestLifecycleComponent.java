/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components;

import org.apache.avalon.framework.component.Component;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.SourceResolver;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.util.Map;

/**
 * Objects implementing this marker interface have a lifecycle of one
 * request. This means if one request is looking up several times
 * an object implementing this interface, it's always the same object.
 * In addition, the first time this object is looked up during a request,
 * the setup() method is called
 *
 * @author <a href="mailto:cziegeler@org.com">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/07 12:34:12 $
 */
public interface RequestLifecycleComponent extends Component {

    /**
     * Set the <code>SourceResolver</code>, objectModel <code>Map</code>,
     * used to process the request.
     */
    void setup(SourceResolver resolver, Map objectModel)
    throws ProcessingException, SAXException, IOException;
}
