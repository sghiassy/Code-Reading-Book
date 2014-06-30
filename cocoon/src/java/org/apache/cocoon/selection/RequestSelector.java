/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.selection;

import org.apache.avalon.framework.activity.Initializable;

/**
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/11 17:07:06 $
 * @deprecated Renamed to RequestParameterSelector
 */
public class RequestSelector extends RequestParameterSelector implements Initializable {

    public void initialize() {
        getLogger().warn("RequestSelector is deprecated. Please use RequestParameterSelector");
    }
}
