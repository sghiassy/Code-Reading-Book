/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.profiler;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.saxconnector.SAXConnector;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.xml.AbstractXMLPipe;
import org.apache.cocoon.xml.XMLConsumer;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.util.Map;

/**
 * This SAX connector measures time taken by the following SAX handler.
 * @author <a href="vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:12 $
 */
public class ProfilingSAXConnector extends AbstractXMLPipe
	implements Recyclable, SAXConnector
{
	private ProfilerData data;
	private int index;
	private long time;
	private long total;

    /**
     * Setup this SAXConnector.
     */
    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters params)
    	throws ProcessingException, SAXException, IOException
	{
		this.data = (ProfilerData)objectModel.get("profiler");
    }

    /**
     * Recycle the SAXConnector
     */
    public void recycle()
	{
		this.data = null;
		this.time = this.total = 0;
        this.index = -1;
        super.recycle();
    }

	public void setConsumer(XMLConsumer consumer)
	{
        super.setConsumer(consumer);
		if(this.data != null)
			this.index = this.data.indexOf(consumer);
		else
			this.index = -1;	
    }


    public void startDocument() throws SAXException
	{
		time = System.currentTimeMillis();
        super.startDocument();
		total += System.currentTimeMillis() - time;
    }

    public void endDocument() throws SAXException
	{
		time = System.currentTimeMillis();
        super.endDocument();
		total += System.currentTimeMillis() - time;
		if(index != -1)
			data.setTime(index, total);
    }

    public void startPrefixMapping(String prefix, String uri) throws SAXException
	{
		time = System.currentTimeMillis();
		super.startPrefixMapping(prefix, uri);
		total += System.currentTimeMillis() - time;
    }

    public void endPrefixMapping(String prefix) throws SAXException
	{
		time = System.currentTimeMillis();
		super.endPrefixMapping(prefix);
		total += System.currentTimeMillis() - time;
    }

    public void startElement(String uri, String loc, String raw, Attributes a) throws SAXException
	{
		time = System.currentTimeMillis();
		super.startElement(uri, loc, raw, a);
		total += System.currentTimeMillis() - time;
    }

    public void endElement(String uri, String loc, String raw) throws SAXException
	{
		time = System.currentTimeMillis();
		super.endElement(uri, loc, raw);
		total += System.currentTimeMillis() - time;
    }

    public void characters(char c[], int start, int len) throws SAXException
	{
		time = System.currentTimeMillis();
		super.characters(c, start, len);
		total += System.currentTimeMillis() - time;
    }

    public void ignorableWhitespace(char c[], int start, int len) throws SAXException
	{
		time = System.currentTimeMillis();
		super.ignorableWhitespace(c, start, len);
		total += System.currentTimeMillis() - time;
    }

    public void processingInstruction(String target, String data) throws SAXException
	{
		time = System.currentTimeMillis();
		super.processingInstruction(target, data);
		total += System.currentTimeMillis() - time;
    }

    public void skippedEntity(String name) throws SAXException
	{
		time = System.currentTimeMillis();
		super.skippedEntity(name);
		total += System.currentTimeMillis() - time;
    }

    public void startDTD(String name, String publicId, String systemId) throws SAXException
	{
		time = System.currentTimeMillis();
		super.startDTD(name, publicId, systemId);
		total += System.currentTimeMillis() - time;
    }

    public void endDTD() throws SAXException
	{
		time = System.currentTimeMillis();
		super.endDTD();
		total += System.currentTimeMillis() - time;
    }

    public void startEntity(String name) throws SAXException
	{
		time = System.currentTimeMillis();
		super.startEntity(name);
		total += System.currentTimeMillis() - time;
    }

    public void endEntity(String name) throws SAXException
	{
		time = System.currentTimeMillis();
		super.endEntity(name);
		total += System.currentTimeMillis() - time;
    }

    public void startCDATA() throws SAXException
	{
		time = System.currentTimeMillis();
		super.startCDATA();
		total += System.currentTimeMillis() - time;
    }

    public void endCDATA() throws SAXException
	{
		time = System.currentTimeMillis();
		super.endCDATA();
		total += System.currentTimeMillis() - time;
    }

    public void comment(char ch[], int start, int len) throws SAXException
	{
		time = System.currentTimeMillis();
		super.comment(ch, start, len);
		total += System.currentTimeMillis() - time;
    }
}
