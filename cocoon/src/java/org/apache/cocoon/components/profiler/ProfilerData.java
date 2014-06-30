/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.profiler;

import org.apache.cocoon.util.HashUtil;

import java.util.ArrayList;

/**
 * Request-time profiler information.
 *
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public class ProfilerData
{
	class Entry
	{
		Object component;
		String role;
		String source;
		long time;

		Entry(Object c, String r, String s)
 		{
			component = c;
			role = r;
			source = s;
		}
	}

	private ArrayList a = null;

	public ProfilerData()
 	{
		a = new ArrayList();
		a.add(null);
	}

	public void setGenerator(Object generator, String role, String source)
 	{
		a.set(0, new Entry(generator, role, source));
	}

	public void addComponent(Object component, String role, String source)
 	{
		a.add(new Entry(component, role, source));
	}

	public int indexOf(Object component)
 	{
		for(int i=0; i<a.size(); i++){
			if(((Entry)a.get(i)).component == component)
				return i;
		}
		return -1;
	}

	public void setTime(int i, long time)
 	{
		((Entry)a.get(i)).time = time;
	}

	Entry[] getEntries()
	{
		return (Entry[])a.toArray(new Entry[a.size()]);
	}

	public long getKey(String uri)
	{
		StringBuffer key = new StringBuffer(uri);
		for(int i=0; i<a.size(); i++){
			Entry entry = (Entry)a.get(i);
			key.append(':');
			key.append(entry.role);
			key.append(':');
			key.append(entry.source);
		}
		return HashUtil.hash(key);
	}
/*
	public void report(String uri)
	{
		System.err.println("-------------------------------------------------------------------------------");
		System.err.println("PROFILER DATA FOR: " + uri);

		Entry[] entries = getEntries();
		for(int i=0; i<entries.length; i++){
			long time = entries[i].time;
			if(i < entries.length - 1)
				time -= entries[i+1].time;
			if(entries[i].role == null || entries[i].role.length() == 0)
				System.err.println("PROFILER DATA: " + time
					+ "\tFOR " + entries[i].component);
			else
				System.err.println("PROFILER DATA: " + time
					+ "\tFOR " + entries[i].role + "\t" + entries[i].source);
		}
		System.err.println("PROFILER DATA: " + entries[0].time + " TOTAL");
	}
*/
}
