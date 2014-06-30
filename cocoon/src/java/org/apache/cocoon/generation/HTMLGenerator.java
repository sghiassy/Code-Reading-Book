/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.generation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.ResourceNotFoundException;
import org.apache.cocoon.caching.CacheValidity;
import org.apache.cocoon.caching.Cacheable;
import org.apache.cocoon.caching.TimeStampCacheValidity;
import org.apache.cocoon.components.xpath.XPathProcessor;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.util.HashUtil;
import org.apache.cocoon.xml.dom.DOMStreamer;
import org.apache.cocoon.xml.XMLUtils;
import org.w3c.dom.NodeList;
import org.w3c.tidy.Tidy;
import org.xml.sax.SAXException;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.sax.SAXResult;
import java.io.BufferedInputStream;
import java.io.StringWriter;
import java.io.PrintWriter;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Map;

/**
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/21 23:14:26 $
 */
public class HTMLGenerator extends ComposerGenerator implements Cacheable, Recyclable, Composable, Disposable {

    /** The  source */
    private Source inputSource;

    /** XPATH expression */
    private String xpath = null;

    /** XPath Processor */
    private XPathProcessor processor = null;

    public void compose(ComponentManager manager) {
        this.manager = manager;
        try {
            this.processor = (XPathProcessor)this.manager.lookup(XPathProcessor.ROLE);
        } catch (Exception e) {
            getLogger().error("cannot obtain XPathProcessor", e);
        }
    }

    /**
     * Recycle this component.
     * All instance variables are set to <code>null</code>.
     */
    public void recycle() {
        super.recycle();
        if (this.inputSource != null) {
            this.inputSource.recycle();
            this.inputSource = null;
        }
        this.xpath = null;
    }

    /**
     * Setup the html generator.
     * Try to get the last modification date of the source for caching.
     */
    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters par)
    throws ProcessingException, SAXException, IOException {
        super.setup(resolver, objectModel, src, par);

        Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
        xpath = request.getParameter("xpath");
        if(xpath == null)
            xpath = par.getParameter("xpath",null);

        // append the request parameter to the URL if necessary
        if (par.getParameterAsBoolean( "copy-parameters", false )) {
            StringBuffer query = new StringBuffer( 32 );

            Enumeration params = request.getParameterNames();
            while (params.hasMoreElements()) {
                String name = (String)params.nextElement();
                String[] values = request.getParameterValues( name );

                for (int i = 0; i < values.length; i++) {
                    query.append( name ).append( "=" ).append( values[i] ).append( "&" );
                }
            }

            if (query.length() > 0) {
                query = new StringBuffer(super.source);
                if (super.source.indexOf("?") == -1) {
                    query.append('?');
                } else {
                    query.append('&');
                }
                query.append( query.substring( 0, query.length() - 1) );
                super.source = query.toString();
            }
        }

        this.inputSource = resolver.resolve(super.source);
    }

    /**
     * Generate the unique key.
     * This key must be unique inside the space of this component.
     * This method must be invoked before the generateValidity() method.
     *
     * @return The generated key or <code>0</code> if the component
     *              is currently not cacheable.
     */
    public long generateKey() {
        if (this.inputSource.getLastModified() != 0) {
            if (this.xpath != null) {
                StringBuffer buffer = new StringBuffer(this.inputSource.getSystemId());
                buffer.append(':').append(this.xpath);
                return HashUtil.hash(buffer.toString());
            } else {
                return HashUtil.hash(this.inputSource.getSystemId());
            }
        }
        return 0;
    }

    /**
     * Generate the validity object.
     * Before this method can be invoked the generateKey() method
     * must be invoked.
     *
     * @return The generated validity object or <code>null</code> if the
     *         component is currently not cacheable.
     */
    public CacheValidity generateValidity() {
        if (this.inputSource.getLastModified() != 0) {
            return new TimeStampCacheValidity(this.inputSource.getLastModified());
        }
        return null;
    }

    /**
     * Generate XML data.
     */
    public void generate()
    throws IOException, SAXException, ProcessingException {
        try
        {
            // Setup an instance of Tidy.
            Tidy tidy = new Tidy();
            tidy.setXmlOut(true);
            tidy.setXHTML(true);
            //Set Jtidy warnings on-off
            tidy.setShowWarnings(getLogger().isWarnEnabled());
            //Set Jtidy final result summary on-off
            tidy.setQuiet(!getLogger().isInfoEnabled());
            //Set Jtidy infos to a String (will be logged) instead of System.out
            StringWriter stringWriter = new StringWriter();
            PrintWriter errorWriter = new PrintWriter(stringWriter);
            tidy.setErrout(errorWriter);

            // Extract the document using JTidy and stream it.
            org.w3c.dom.Document doc = tidy.parseDOM(new BufferedInputStream(this.inputSource.getInputStream()), null);

            // FIXME: Jtidy doesn't warn or strip duplicate attributes in same
            // tag; stripping.
            XMLUtils.stripDuplicateAttributes(doc, null);

            errorWriter.flush();
            errorWriter.close();
            if(getLogger().isWarnEnabled()){
               getLogger().warn(stringWriter.toString());
            }
            
            if(xpath != null)
            {
                Transformer serializer = TransformerFactory.newInstance().newTransformer();
                serializer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");

                NodeList nl = processor.selectNodeList(doc, xpath);
                int length = nl.getLength();
                for(int i=0;i<length;i++)
                {
                    SAXResult result = new SAXResult(this.contentHandler);
                    result.setLexicalHandler(this.lexicalHandler);
                    serializer.transform(new DOMSource(nl.item(i)), result);
                }
            } else {
                DOMStreamer streamer = new DOMStreamer(this.contentHandler,this.lexicalHandler);
                streamer.stream(doc);
            }
        } catch (IOException e){
            getLogger().warn("HTMLGenerator.generate()", e);
            throw new ResourceNotFoundException("Could not get resource "
                + this.inputSource.getSystemId(), e);
        } catch (SAXException e){
            getLogger().error("HTMLGenerator.generate()", e);
            throw e;
        } catch (ProcessingException e){
            throw e;
        } catch (Exception e){
            getLogger().error("Could not setup jtidy", e);
            throw new ProcessingException("Exception in HTMLGenerator.generate()",e);
        }
    }

    public void dispose()
    {
        this.manager.release((Component)this.processor);
    }
}
