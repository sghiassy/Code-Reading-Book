/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup;

import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.cocoon.util.TraxErrorHandler;
import org.apache.cocoon.ProcessingException;

import org.xml.sax.ContentHandler;
import org.xml.sax.InputSource;
import org.xml.sax.XMLReader;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;
import org.xml.sax.Attributes;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.sax.SAXResult;
import javax.xml.transform.sax.SAXTransformerFactory;
import javax.xml.transform.sax.TransformerHandler;
import javax.xml.transform.stream.StreamResult;
import java.io.StringWriter;
import java.io.IOException;
import java.util.Properties;

/**
 * A logicsheet-based implementation of <code>MarkupCodeGenerator</code>
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version CVS $Revision: 1.6 $ $Date: 2002/01/30 19:21:26 $
 */
public class LogicsheetCodeGenerator extends AbstractLoggable implements MarkupCodeGenerator {

    private Logicsheet corelogicsheet;

    private ContentHandler serializerContentHandler;

    private XMLReader rootReader;

    private TransformerHandler currentParent;

    private StringWriter writer;

    /** The trax TransformerFactory */
    private SAXTransformerFactory tfactory = null;

    /**
     * Initialize the LogicsheetCodeGenerator.
     */
    public void initialize() {
        Properties format = new Properties();
        try {
            // Set the serializer which would act as ContentHandler for the last transformer
            // FIXME (SSA) change a home made content handler, that extract the PCDATA
            // from the last remaining element
            TransformerHandler handler = getTransformerFactory().newTransformerHandler();

            // Set the output properties
            format.put(OutputKeys.METHOD,"text");
            // FIXME (SSA) remove the nice identing. For debug purpose only.
            format.put(OutputKeys.INDENT,"yes");
            handler.getTransformer().setOutputProperties(format);

            this.writer = new StringWriter();
            handler.setResult(new StreamResult(writer));
            this.serializerContentHandler = handler;
        } catch (TransformerConfigurationException tce) {
            getLogger().error("LogicsheetCodeGenerator: unable to get TransformerHandler", tce);
        }
    }

    /**
     * Helper for TransformerFactory.
     */
    private SAXTransformerFactory getTransformerFactory()
    {
        if(tfactory == null)  {
            tfactory = (SAXTransformerFactory) TransformerFactory.newInstance();
            tfactory.setErrorListener(new TraxErrorHandler(getLogger()));
        }
        return tfactory;
    }

    /**
    * Add a logicsheet to the logicsheet list
    *
    * @param logicsheet The logicsheet to be added
    */
    public void addLogicsheet(Logicsheet logicsheet) throws ProcessingException {
        if (this.currentParent == null) {
            // Setup the first transformer of the chain.
            this.currentParent = logicsheet.getTransformerHandler();

            // the parent is the rootReader
            this.rootReader.setContentHandler(this.currentParent);

            // Set content handler for the end of the chain : serializer
            this.currentParent.setResult(new SAXResult(this.serializerContentHandler));
        } else {
            // Build the transformer chain on the fly
            TransformerHandler newParent = logicsheet.getTransformerHandler();

            // the currentParent is the parent of the new logicsheet filter
            this.currentParent.setResult(new SAXResult(newParent));

            // reset the new parent and the contentHanlder
            this.currentParent = newParent;
            this.currentParent.setResult(new SAXResult(this.serializerContentHandler));
        }
    }

    /**
    * Generate source code from the input document. Filename information is
    * ignored in the logicsheet-based code generation approach.
    *
    * @param reader The reader
    * @param input The input source
    * @param filename The input source original filename
    * @return The generated source code
    * @exception Exception If an error occurs during code generation
    */
    public String generateCode(XMLReader reader, InputSource input, String filename) throws Exception {
        try {
            // set the root XMLReader of the transformer chain
            this.rootReader = reader;
            // start the parsing
            this.rootReader.parse(input);
            return this.writer.toString();
        } catch (SAXException e) {
            if(e.getException() != null) {
                getLogger().debug("Got SAXException; Rethrowing cause exception", e);
                throw e.getException();
            }
            getLogger().debug("Got SAXException", e);
            throw e;
        }
    }
}
