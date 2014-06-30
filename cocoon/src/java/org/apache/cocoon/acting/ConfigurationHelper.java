/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
/**
 * Keep book on configurations together with their modification times
 *
 * @author Christian Haul &lt;haul@informatik.tu-darmstadt.de&gt;
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $ 
 */
package org.apache.cocoon.acting;
import org.apache.avalon.framework.configuration.Configuration;

public class ConfigurationHelper {
    public long lastModified = 0;
    public Configuration configuration = null;
}
