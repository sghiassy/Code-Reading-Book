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
import org.apache.cocoon.generation.ComposerGenerator;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.text.DateFormat;
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;

/**
 * Generates an XML representation of the current status of Profiler.
 *
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public class ProfilerGenerator extends ComposerGenerator
implements Recyclable, Composable, Disposable {

    /**
	 * The XML namespace for the output document.
     */
    protected static final String namespace = "http://apache.org/cocoon/profiler/1.0";

	private Profiler profiler;


	public void compose(ComponentManager manager) throws ComponentException
	{
		super.compose(manager);

		try{
			this.profiler = (Profiler)super.manager.lookup(Profiler.ROLE);
		}catch(Exception e){
			e.printStackTrace();
		}
	}

	public void dispose()
    {
		if(this.profiler != null){
			super.manager.release(this.profiler);
			this.profiler = null;
		}
	}

    /**
	 * Generate the status information in XML format.
     * @throws SAXException
     *         when there is a problem creating the output SAX events.
     */
    public void generate() throws SAXException {

        // Start the document and set the namespace.
        this.contentHandler.startDocument();
        this.contentHandler.startPrefixMapping("", namespace);

        generate(this.contentHandler);

        // End the document.
        this.contentHandler.endPrefixMapping("");
        this.contentHandler.endDocument();
    }

    /** Generate the main status document. */
    private void generate(ContentHandler ch) throws SAXException {
        // Root element.

        // The current date and time.
        String dateTime = DateFormat.getDateTimeInstance().format(new Date());

        AttributesImpl atts = new AttributesImpl();
        atts.addAttribute(namespace, "date", "date", "CDATA", dateTime);
        ch.startElement(namespace, "profilerinfo", "profilerinfo", atts);

        Collection results = profiler.getResults();
		for(Iterator i = results.iterator(); i.hasNext();)
			generateResult(ch, (ProfilerResult)i.next());

        // End root element.
        ch.endElement(namespace, "profilerinfo", "profilerinfo");
    }

    private void generateResult(ContentHandler ch, ProfilerResult result) throws SAXException {
        AttributesImpl atts = new AttributesImpl();

		int count = result.getCount();
		String[] roles = result.getRoles();
		String[] source = result.getSources();
		long[] avgTime = result.getTotalTime();
		long avgTotal = 0;
		for(int i=0; i < avgTime.length; i++)
			avgTotal += avgTime[i];

		atts.addAttribute(namespace, "uri", "uri", "CDATA", result.getURI());
		atts.addAttribute(namespace, "count", "count", "CDATA", Integer.toString(result.getCount()));
		atts.addAttribute(namespace, "time", "time", "CDATA", Long.toString(avgTotal));
        ch.startElement(namespace, "pipeline", "pipeline", atts);
		atts.clear();

		if(count > 0){
			atts.addAttribute(namespace, "time", "time", "CDATA", Long.toString(avgTotal / count));
			ch.startElement(namespace, "average", "average", atts);
			atts.clear();
			for(int i=0; i<roles.length; i++){
				if(roles[i] != null)
					atts.addAttribute(namespace, "role", "role", "CDATA", roles[i]);
				if(source[i] != null)
					atts.addAttribute(namespace, "source", "source", "CDATA", source[i]);
				atts.addAttribute(namespace, "time", "time", "CDATA", Long.toString(avgTime[i] / count));
				ch.startElement(namespace, "element", "element", atts);
				atts.clear();
				ch.endElement(namespace, "element", "element");
			}
			ch.endElement(namespace, "average", "average");

			long[][] last = result.getLastTimes();
			for(int j=0; j<last.length; j++){
				if(last[j] != null){
					long[] curTime = last[j];
					long curTotal = 0;
					for(int i=0; i < curTime.length; i++)
						curTotal += curTime[i];

					atts.addAttribute(namespace, "time", "time", "CDATA", Long.toString(curTotal));
					ch.startElement(namespace, "result", "result", atts);
					atts.clear();
					for(int i=0; i<roles.length; i++){
						if(roles[i] != null)
							atts.addAttribute(namespace, "role", "role", "CDATA", roles[i]);
						if(source[i] != null)
							atts.addAttribute(namespace, "source", "source", "CDATA", source[i]);
						atts.addAttribute(namespace, "time", "time", "CDATA", Long.toString(curTime[i]));
						ch.startElement(namespace, "element", "element", atts);
						atts.clear();
						ch.endElement(namespace, "element", "element");
					}
					ch.endElement(namespace, "result", "result");
				}
			}
		}

		ch.endElement(namespace, "pipeline", "pipeline");
    }
}

