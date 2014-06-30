/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

import org.apache.avalon.framework.logger.AbstractLoggable;

import java.util.Collections;
import java.util.Map;
import java.util.TreeMap;

/**
 * AbstractAction gives you the infrastructure for easily deploying more
 * Actions.  In order to get at the Logger, use getLogger().
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @author <a href="mailto:haul@informatik.tu-darmstadt.de">Christian Haul</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $
 */
public abstract class AbstractAction extends AbstractLoggable
implements Action {

    /**
     * Empty unmodifiable map. Replace with Collections.EMPTY_MAP when
     * pre-jdk1.3 support is dropped.
     */
    protected static final Map EMPTY_MAP = Collections.unmodifiableMap(new TreeMap());

}
