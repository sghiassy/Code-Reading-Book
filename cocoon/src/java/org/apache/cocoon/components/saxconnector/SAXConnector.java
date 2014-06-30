/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.saxconnector;

import org.apache.avalon.framework.component.Component;
import org.apache.cocoon.sitemap.SitemapModelComponent;
import org.apache.cocoon.xml.XMLPipe;

/**
 * Provides a connection between SAX components.
 * @author <a href="mailto:prussell@apache.org">Paul Russell</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:13 $
 */
public interface SAXConnector extends XMLPipe, Component, SitemapModelComponent {

    String ROLE = "org.apache.cocoon.components.saxconnector.SAXConnector";
}
