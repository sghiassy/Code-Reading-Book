/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.profiler;

import java.util.Arrays;

/**
 * Represents data collected about one pipeline.
 *
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public class ProfilerResult
{
	private String uri;
	private String[] roles;
	private String[] sources;

	private int count;
	private long[] totalTime;
	private long[][] latestTimes;
	private int latestCurrent;

	public ProfilerResult(String uri, int latestResultsCount)
 	{
		this.uri = uri;
		if(latestResultsCount > 0)
			this.latestTimes = new long[latestResultsCount][];
	}

	public void addData(ProfilerData data)
	{
		ProfilerData.Entry[] entries = data.getEntries();
		synchronized(this){
			if(totalTime == null || totalTime.length != entries.length){
				// Reinitialize arrays
				totalTime = new long[entries.length];
				roles = new String[entries.length];
				sources = new String[entries.length];
				for(int i=0; i<entries.length; i++){
					roles[i] = entries[i].role;
					if(roles[i] == null)
						roles[i] = entries[i].component.getClass().getName();
					sources[i] = entries[i].source;
				}

				// Clear latest times
				if(latestTimes != null)
					Arrays.fill(latestTimes, 0, latestTimes.length, null);
				latestCurrent = 0;

				// Clear counter
				count = 0;
			}

			// Adjust time. It's cumulative towards 0 index in data object
			for(int i=0; i<entries.length-1; i++)
				entries[i].time -= entries[i+1].time;

			long[] latest = new long[entries.length];
			for(int i=0; i<entries.length; i++)
				totalTime[i] += latest[i] = entries[i].time;

			if(latestTimes != null){
				latestTimes[latestCurrent++] = latest;
				if(latestCurrent >= latestTimes.length) latestCurrent = 0;
			}

			count++;
		}
	}

	public String getURI()
	{
		return uri;
	}

	public String[] getRoles()
	{
		return roles;
	}

	public String[] getSources()
	{
		return sources;
	}

	public int getCount()
	{
		return count;
	}

	public long[] getTotalTime()
	{
		return totalTime;
	}

	public long[][] getLastTimes()
	{
		return latestTimes;
	}
/*
	public void report()
	{
		System.err.println("-------------------------------------------------------------------------------");
		System.err.println("PROFILER TOTALS FOR: " + uri);

		if(totalTime != null){
			long time = 0;
			for(int i=0; i<totalTime.length; i++){
				System.err.println("PROFILER TOTALS: " + totalTime[i] + "\tFOR " + roles[i] + "\t" + sources[i]);
				time += totalTime[i];
			}
			System.err.println("PROFILER TOTALS: " + time + " TIMES");
			System.err.println("PROFILER TOTALS: " + count + " REQUESTS");
		}
	}
*/
}
