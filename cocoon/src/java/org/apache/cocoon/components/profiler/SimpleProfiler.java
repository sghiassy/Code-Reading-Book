/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.profiler;

import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.thread.ThreadSafe;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

/**
 * Profiler component implementation. Stores profiler data for
 * all pipelines.
 *
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public class SimpleProfiler extends AbstractLoggable
	implements Profiler, ThreadSafe //, Composable, Configurable, Disposable
{
	private static final int RESULTS_COUNT = 10;
	private Map results;

	public SimpleProfiler()
	{
		results = new HashMap();
	}

	public void clearResults()
	{
		results.clear();
	}

	public void clearResult(Object key)
	{
		results.remove(key);
	}

	public Collection getResultKeys()
	{
		return results.keySet();
	}

	public Collection getResults()
	{
		return results.values();
	}

	public ProfilerResult getResult(Object key)
	{
		return (ProfilerResult)results.get(key);
	}

	public void addResult(String uri, ProfilerData data)
	{
		Long key = new Long(data.getKey(uri));
		ProfilerResult result = (ProfilerResult)results.get(key);
		if(result == null){
			synchronized(results){
				if((result = (ProfilerResult)results.get(key)) == null)
					results.put(key, result = new ProfilerResult(uri, RESULTS_COUNT));
			}
		}

		result.addData(data);
//		result.report();
	}
}
