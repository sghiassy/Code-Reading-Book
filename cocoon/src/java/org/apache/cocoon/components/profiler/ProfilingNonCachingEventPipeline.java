/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.profiler;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.components.pipeline.NonCachingEventPipeline;
import org.apache.cocoon.environment.Environment;

/**
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public class ProfilingNonCachingEventPipeline extends NonCachingEventPipeline
implements Recyclable, Composable, Disposable {

	private ComponentManager manager;
	private Profiler profiler;

   	private ProfilerData data = null;


	public void compose(ComponentManager manager) throws ComponentException
	{
		super.compose(manager);

		try{
			this.manager = manager;
			this.profiler = (Profiler)manager.lookup(Profiler.ROLE);
		}catch(Exception e){
			e.printStackTrace();
		}
	}

	public void dispose()
    {
		super.dispose();

		if(this.profiler != null){
			this.manager.release(this.profiler);
			this.profiler = null;
		}
		this.manager = null;
	}

    public void setGenerator (String role, String source, Parameters param, Exception e)
    throws Exception {
    	super.setGenerator(role, source, param, e);

		if(this.data == null)
			this.data = new ProfilerData();
       	this.data.setGenerator(super.generator, role, source);
	}

    public void setGenerator (String role, String source, Parameters param)
    throws Exception {
    	super.setGenerator(role, source, param);

		if(this.data == null)
			this.data = new ProfilerData();
       	this.data.setGenerator(super.generator, role, source);
    }

    public void addTransformer (String role, String source, Parameters param)
    throws Exception {
        super.addTransformer(role, source, param);

		if(this.data == null)
			this.data = new ProfilerData();
       	this.data.addComponent(super.transformers.get(super.transformers.size()-1), role, source);
    }

    public boolean process(Environment environment) throws Exception {
		this.data.addComponent(super.xmlConsumer, null, null);
        environment.getObjectModel().put("profiler", data);

        // Execute pipeline
       	long time = System.currentTimeMillis();
        boolean result = super.process(environment);
		this.data.setTime(0, System.currentTimeMillis() - time);

		// Report
		profiler.addResult(environment.getURI(), data);
        return result;
    }

    public void recycle() {
        this.data = null;
        super.recycle();
    }
}
