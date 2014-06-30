/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.profiler;

import org.apache.avalon.framework.component.Component;

import java.util.Collection;

/**
 * Profiler component interface.
 *
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public interface Profiler extends Component
{
    String ROLE = "org.apache.cocoon.components.profiler.Profiler";

	void clearResults();
	void clearResult(Object key);

	void addResult(String uri, ProfilerData data);

	Collection getResultKeys();
	Collection getResults();
	ProfilerResult getResult(Object key);
}
