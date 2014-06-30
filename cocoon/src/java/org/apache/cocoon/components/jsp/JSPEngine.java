/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.jsp;

import org.apache.avalon.framework.component.Component;
import org.xml.sax.SAXException;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

/**
 * A component for loading and running JSP.
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:09 $
 */
public interface JSPEngine extends Component {

    String ROLE = "org.apache.cocoon.components.jsp.JSPEngine";

    /**
    * execute the JSP and return the output
    *
    * @param context The Servlet Context
    * @exception IOException 
    * @exception ServletException 
    * @exception SAXException 
    * @exception Exception 
    */
    byte[] executeJSP(String url, 
                       HttpServletRequest request, 
                       HttpServletResponse response, 
                       ServletContext context) 
        throws IOException, ServletException, SAXException, Exception;
}
