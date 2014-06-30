/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.matching;

import org.apache.avalon.framework.logger.Loggable;
import org.apache.log.Logger;

/**
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @deprecated Renamed to RequestParameterMatcher
 * @version CVS $Revision: 1.3 $
 */
public class RequestParamMatcher extends RequestParameterMatcher
    implements Loggable
{
    public void setLogger(Logger logger) {
        logger.warn("RequestParamMatcher is deprecated. Please use RequestParameterMatcher");
    }
}
