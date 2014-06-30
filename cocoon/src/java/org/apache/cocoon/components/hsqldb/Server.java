/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.hsqldb;

import org.apache.avalon.framework.component.Component;

/**
 *
 * @author <a href="mailto:dims@yahoo.org">Davanum Srinivas</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:09 $
 */
public interface Server extends Component {
    String ROLE = "org.apache.cocoon.components.hsqldb.Server";
}
