/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.transformation;

import org.apache.avalon.excalibur.pool.Poolable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.SourceResolver;
import org.xml.sax.Attributes;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;

import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;
import java.util.Map;

/**
 *
 * The <code>LogTransformer</code> is a class that can be plugged into a pipeline
 * to print the SAX events which passes thru this transformer in a readable form
 * to a file.
 * <br>
 * The file will be specified in a parameter tag in the sitemap pipeline to the
 * transformer as follows:
 * <p>
 * <pre>
 * &lt;map:transform type="log"&gt;
 * &nbsp;&nbsp;&lt;map:parameter name="logfile" value="logfile.log"/&gt;
 * &nbsp;&nbsp;&lt;map:parameter name="append" value="no"/&gt;
 * &lt;/map:transform&gt;
 * </pre>
 * </p>
 *
 * Because the log file will be hardcoded into the sitemap this LOGTransformer will
 * not be thread save!!
 * <br>
 * This transformations main purpose is debugging.
 *
 * @author <a href="mailto:giacomo.pati@pwr.ch">Giacomo Pati</a>
 *         (PWR Organisation &amp; Entwicklung)
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/07 13:09:02 $
 *
 */
public class LogTransformer extends AbstractTransformer implements Poolable {
    /** Wether we are forwarding XML data or not. */
    private boolean canReset=true;

    private String lf = System.getProperty("line.separator", "\n");

    /** true if filename is valid and writeable */
    private boolean isValid = true;
    /** filename for log file */
    private String logfilename = null;
    /** log file */
    private FileWriter logfile = null;
    /** should we append content to the log file */
    private boolean append = false;

    /** BEGIN SitemapComponent methods **/

    public void setup(SourceResolver resolver, Map objectModel,
                      String source, Parameters parameters)
            throws ProcessingException, SAXException, IOException {
        if (logfile == null) {
            String appends = parameters.getParameter("append", null);
            logfilename = parameters.getParameter("logfile", null);
            if ("yes".equals(appends)) {
                append = true;
            } else {
                append = false;
            }
            try {
                // Check for null, use System.out if logfile is not specified.
                if(logfilename != null)
                    logfile = new FileWriter(logfilename, append );
                else
                    logfile = new FileWriter(java.io.FileDescriptor.out);
            } catch (IOException e) {
                getLogger().debug("LogTransformer", e);
                isValid = false;
                throw e;
            }
        }
        Date date = new Date();
        StringBuffer logEntry = new StringBuffer();
        logEntry.append ( "---------------------------- [" );
        logEntry.append ( date.toString() );
        logEntry.append ( "] ----------------------------" );
        log("setup", logEntry.toString());
    }

    /** END SitemapComponent methods **/

    /**
     * Receive an object for locating the origin of SAX document events.
     */
    public void setDocumentLocator(Locator locator) {
        log("setDocumentLocator","");
        if (super.contentHandler!=null) {
            super.contentHandler.setDocumentLocator(locator);
        }
    }

    /**
     * Receive notification of the beginning of a document.
     */
    public void startDocument()
    throws SAXException {
        log("startDocument", "");
        if (super.contentHandler!=null) {
            super.contentHandler.startDocument();
        }

        this.canReset=false;
    }

    /**
     * Receive notification of the end of a document.
     */
    public void endDocument()
    throws SAXException {
        log ("endDocument", "");
        if (super.contentHandler!=null) {
            super.contentHandler.endDocument();
        }

        this.canReset=true;
    }

    /**
     * Begin the scope of a prefix-URI Namespace mapping.
     */
    public void startPrefixMapping(String prefix, String uri)
    throws SAXException {
        log ("startPrefixMapping", "prefix="+prefix+",uri="+uri);
        if (super.contentHandler!=null) {
            super.contentHandler.startPrefixMapping(prefix,uri);
        }
    }

    /**
     * End the scope of a prefix-URI mapping.
     */
    public void endPrefixMapping(String prefix)
    throws SAXException {
        log ("endPrefixMapping", "prefix="+prefix);
        if (super.contentHandler!=null) {
            super.contentHandler.endPrefixMapping(prefix);
        }
    }

    /**
     * Receive notification of the beginning of an element.
     */
    public void startElement(String uri, String loc, String raw, Attributes a)
    throws SAXException {
        log ("startElement", "uri="+uri+",local="+loc+",raw="+raw);
        for (int i = 0; i < a.getLength(); i++) {
            log ("            ", new Integer(i+1).toString()
                 +". uri="+a.getURI(i)
                 +",local="+a.getLocalName(i)
                 +",qname="+a.getQName(i)
                 +",type="+a.getType(i)
                 +",value="+a.getValue(i));
        }
        if (super.contentHandler!=null) {
            super.contentHandler.startElement(uri,loc,raw,a);
        }
    }


    /**
     * Receive notification of the end of an element.
     */
    public void endElement(String uri, String loc, String qname)
    throws SAXException {
        log ("endElement", "uri="+uri+",local="+loc+",qname="+qname);
        if (super.contentHandler!=null) {
            super.contentHandler.endElement(uri,loc,qname);
        }
    }

    /**
     * Receive notification of character data.
     */
    public void characters(char ch[], int start, int len)
    throws SAXException {
        log ("characters", new String(ch,start,len));
        if (super.contentHandler!=null) {
            super.contentHandler.characters(ch,start,len);
        }
    }

    /**
     * Receive notification of ignorable whitespace in element content.
     */
    public void ignorableWhitespace(char ch[], int start, int len)
    throws SAXException {
        log ("ignorableWhitespace", new String(ch,start,len));
        if (super.contentHandler!=null) {
            super.contentHandler.ignorableWhitespace(ch,start,len);
        }
    }

    /**
     * Receive notification of a processing instruction.
     */
    public void processingInstruction(String target, String data)
    throws SAXException {
        log ("processingInstruction", "target="+target+",data="+data);
        if (super.contentHandler!=null) {
            super.contentHandler.processingInstruction(target,data);
        }
    }

    /**
     * Receive notification of a skipped entity.
     */
    public void skippedEntity(String name)
    throws SAXException {
        log ("skippedEntity", "name="+name);
        if (super.contentHandler!=null) {
            super.contentHandler.skippedEntity(name);
        }
    }

    /**
     * Report the start of DTD declarations, if any.
     */
    public void startDTD(String name, String publicId, String systemId)
    throws SAXException {
        log ("startDTD", "name="+name+",publicId="+publicId+",systemId="+systemId);
        if (super.lexicalHandler!=null) {
            super.lexicalHandler.startDTD(name,publicId,systemId);
        }
    }

    /**
     * Report the end of DTD declarations.
     */
    public void endDTD()
    throws SAXException {
        log ("endDTD", "");
        if (super.lexicalHandler!=null) {
            super.lexicalHandler.endDTD();
        }
    }

    /**
     * Report the beginning of an entity.
     */
    public void startEntity(String name)
    throws SAXException {
        log ("startEntity", "name="+name);
        if (super.lexicalHandler!=null) {
            super.lexicalHandler.startEntity(name);
        }
    }

    /**
     * Report the end of an entity.
     */
    public void endEntity(String name)
    throws SAXException {
        log ("endEntity", "name="+name);
        if (super.lexicalHandler!=null) {
            super.lexicalHandler.endEntity(name);
        }
    }

    /**
     * Report the start of a CDATA section.
     */
    public void startCDATA()
    throws SAXException {
        log ("startCDATA", "");
        if (super.lexicalHandler!=null) {
            super.lexicalHandler.startCDATA();
        }
    }

    /**
     * Report the end of a CDATA section.
     */
    public void endCDATA()
    throws SAXException {
        log ("endCDATA", "");
        if (super.lexicalHandler!=null) {
            super.lexicalHandler.endCDATA();
        }
    }

    /**
     * Report an XML comment anywhere in the document.
     */
    public void comment(char ch[], int start, int len)
    throws SAXException {
        log ("comment", new String(ch,start,len));
        if (super.lexicalHandler!=null) {
            super.lexicalHandler.comment(ch,start,len);
        }
    }

    /**
     * Report to logfile.
     */
    private void log (String location, String description) {
        if (isValid) {
            StringBuffer logEntry = new StringBuffer();
            logEntry.append ( "[" );
            logEntry.append ( location );
            logEntry.append ( "] " );
            logEntry.append ( description );
            logEntry.append ( lf );
            getLogger().info(logEntry.toString());
            synchronized (logfile) {
                try {
                    logfile.write( logEntry.toString(), 0, logEntry.length());
                    logfile.flush();
                }
                catch(IOException ioe) { getLogger().debug("LogTransformer.log", ioe); }
            }
        }
    }

    /**
     *  Attempt to close the log file when the class is GC'd
     */
    public void destroy() {
        try {
            logfile.close();
        } catch (Exception e) {getLogger().debug("LogTransformer.destroy()", e);}
    }
}
