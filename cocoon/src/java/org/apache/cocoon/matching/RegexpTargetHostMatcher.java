/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.matching;

import org.apache.avalon.framework.activity.Initializable;

/**
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/12 01:14:33 $
 * @deprecated Renamed to RegexpHostMatcher
 */
public class RegexpTargetHostMatcher extends RegexpHostMatcher 
    implements Initializable
{

    public void initialize() {
        getLogger().warn("RegexpTargetHostMatcher is deprecated. Please use RegexpHostMatcher");
    }
}
