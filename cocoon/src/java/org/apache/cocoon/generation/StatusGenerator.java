/*****************************************************************************
* Copyright (C) The Apache Software Foundation. All rights reserved.        *
* ------------------------------------------------------------------------- *
* This software is published under the terms of the Apache Software License *
* version 1.1, a copy of which has been included  with this distribution in *
* the LICENSE file.                                                         *
*****************************************************************************/
package org.apache.cocoon.generation;

import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;

import org.apache.avalon.excalibur.pool.Recyclable;

import org.apache.cocoon.components.store.StoreJanitor;
import org.apache.cocoon.components.store.Store;

import org.xml.sax.Attributes;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.text.DateFormat;
import java.util.*;

/** Generates an XML representation of the current status of Cocoon.
 * Potted DTD:
 *
 * <code>
 * &lt;!ELEMENT statusinfo (group|value)*&gt;
 *
 * &lt;!ATTLIST statusinfo
 *     date CDATA #IMPLIED
 *     host CDATA #IMPLIED
 * &gt;
 *
 * &lt;!ELEMENT group (group|value)*&gt;
 * &lt;!ATTLIST group
 *     name CDATA #IMPLIED
 * &gt;
 *
 * &lt;!ELEMENT value (line)+&gt;
 * &lt;!ATTLIST value
 *     name CDATA #REQUIRED
 *
 * &lt;!ELEMENT line (#PCDATA)+&gt;
 * &gt;
 * </code>
 *
 * @author <a href="mailto:paul@luminas.co.uk">Paul Russell</a> (Luminas Limited)
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @author <a href="mailto:skoechlin@ivision.fr">S&eacute;bastien K&oelig;chlin</a> (iVision)
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */
public class StatusGenerator extends ComposerGenerator implements Recyclable {

    /** The StoreJanitor used to get cache statistics
     */
    protected StoreJanitor storejanitor;

    /** The XML namespace for the output document.
     */
    protected static final String namespace =
        "http://apache.org/cocoon/status/2.0";

    /** The XML namespace for xlink
     */
    protected static final String xlinkNamespace =
        "http://www.w3.org/1999/xlink";

    /**
     * Set the current <code>ComponentManager</code> instance used by this
     * <code>Composable</code>.
     * Need to get statistics about cache hits
     */
    public void compose(ComponentManager manager) throws ComponentException {
        super.compose(manager);
        try {
            this.storejanitor = (StoreJanitor)manager.lookup(StoreJanitor.ROLE);
        } catch(ComponentException ce) {
            getLogger().info("StoreJanitor is not available. Sorry, no cache statistics");
        }
    }

    /** Generate the status information in XML format.
     * @throws SAXException
     *         when there is a problem creating the output SAX events.
     */
    public void generate() throws SAXException {

        // Start the document and set the namespace.
        this.contentHandler.startDocument();
        this.contentHandler.startPrefixMapping("", namespace);
        this.contentHandler.startPrefixMapping("xlink", xlinkNamespace);

        genStatus(this.contentHandler);

        // End the document.
        this.contentHandler.endPrefixMapping("xlink");
        this.contentHandler.endPrefixMapping("");
        this.contentHandler.endDocument();
    }

    /** Generate the main status document.
     */
    private void genStatus(ContentHandler ch) throws SAXException {
        // Root element.

        // The current date and time.
        String dateTime = DateFormat.getDateTimeInstance().format(new Date());

        String localHost;

        // The local host.
        try {
            localHost = InetAddress.getLocalHost().getHostName();
        } catch ( UnknownHostException e ) {
            getLogger().debug("StatusGenerator:UnknownHost", e);
            localHost = "";
        } catch ( SecurityException e ) {
            getLogger().debug("StatusGenerator:Security", e);
            localHost = "";
        }

        AttributesImpl atts = new AttributesImpl();
        atts.addAttribute(namespace, "date", "date", "CDATA", dateTime);
        atts.addAttribute(namespace, "host", "host", "CDATA", localHost);
        ch.startElement(namespace, "statusinfo", "statusinfo", atts);

        genVMStatus(ch);

        // End root element.
        ch.endElement(namespace, "statusinfo", "statusinfo");
    }

    private void genVMStatus(ContentHandler ch) throws SAXException {
        AttributesImpl atts = new AttributesImpl();

        startGroup(ch, "vm");
        // BEGIN Memory status
        startGroup(ch, "memory");
        addValue(ch, "total", String.valueOf(Runtime.getRuntime().totalMemory()));
        addValue(ch, "free", String.valueOf(Runtime.getRuntime().freeMemory()));
        endGroup(ch);
        // END Memory status

        // BEGIN JRE
        startGroup(ch, "jre");
        addValue(ch, "version", System.getProperty("java.version"));
        atts.clear();
        atts.addAttribute(xlinkNamespace, "type", "type", "CDATA", "simple");
        atts.addAttribute(xlinkNamespace, "href", "href", "CDATA",
            System.getProperty("java.vendor.url") );
        addValue(ch, "java-vendor", System.getProperty("java.vendor"), atts);
        endGroup(ch);
        // END JRE

        // BEGIN Operating system
        startGroup(ch, "operating-system");
        addValue(ch, "name", System.getProperty("os.name"));
        addValue(ch, "architecture", System.getProperty("os.arch"));
        addValue(ch, "version", System.getProperty("os.version"));
        endGroup(ch);
        // END operating system

        String classpath = System.getProperty("java.class.path");
        List paths = new ArrayList();
        StringTokenizer tokenizer = new StringTokenizer(classpath, System.getProperty("path.separator"));
        while (tokenizer.hasMoreTokens()) {
            paths.add(tokenizer.nextToken());
        }
        addMultilineValue(ch, "classpath", paths);
        // END ClassPath

        // BEGIN Cache
        startGroup(ch, "Store-Janitor");

        // For each element in StoreJanitor
        Iterator i = this.storejanitor.iterator();
        while( i.hasNext() ) {
            Store store = (Store) i.next();
            startGroup(ch, store.getClass().getName()+" (hash = 0x"+Integer.toHexString(store.hashCode())+")" );
            int size = 0;
            int empty = 0;
            atts.addAttribute(namespace, "name", "name", "CDATA", "cached");
            ch.startElement(namespace, "value", "value", atts);
            // For each element in Store
            Enumeration e = store.keys();
            atts.clear();
            while( e.hasMoreElements() ) {
                size++;
                Object key  = e.nextElement();
                Object val  = store.get( key );
                String line = null;
                if( val == null ) {
                    empty++;
		} else {
                    line = key.toString() + " (class: " +
                           val.getClass().getName() +
                        ")" ;
                    ch.startElement(namespace, "line", "line", atts);
                    ch.characters(line.toCharArray(), 0, line.length());
                ch.endElement(namespace, "line", "line");
                };


            };
    
            if (size == 0) {
                atts.clear();
                ch.startElement(namespace, "line", "line", atts);
                String value = "[empty]";
                ch.characters(value.toCharArray(), 0, value.length());
                ch.endElement(namespace, "line", "line");
            }
            
            ch.endElement(namespace, "value", "value");
                
            addValue(ch, "size", String.valueOf(size) + " items in cache (" + empty + " are empty)");
            endGroup(ch);
        };
        endGroup(ch);
        // END Cache    

        // BEGIN OS info
        endGroup(ch);
    }

    /** Utility function to begin a <code>group</code> tag pair. */
    private void startGroup(ContentHandler ch, String name) throws SAXException {
        startGroup(ch, name, null);
    }

    /** Utility function to begin a <code>group</code> tag pair with added attributes. */
    private void startGroup(ContentHandler ch, String name, Attributes atts) throws SAXException {
        AttributesImpl ai;
        if ( atts == null ) {
            ai = new AttributesImpl();
        } else {
            ai = new AttributesImpl(atts);
        }
        ai.addAttribute(namespace, "name", "name", "CDATA", name);
        ch.startElement(namespace, "group", "group", ai);
    }

    /** Utility function to end a <code>group</code> tag pair. */
    private void endGroup(ContentHandler ch) throws SAXException {
        ch.endElement(namespace, "group", "group");
    }

    /** Utility function to begin and end a <code>value</code> tag pair. */
    private void addValue(ContentHandler ch, String name, String value) throws SAXException {
        addValue(ch, name, value, null);
    }

    /** Utility function to begin and end a <code>value</code> tag pair with added attributes. */
    private void addValue(ContentHandler ch, String name, String value, Attributes atts) throws SAXException {
        AttributesImpl ai;
        if ( atts == null ) {
            ai = new AttributesImpl();
        } else {
            ai = new AttributesImpl(atts);
        }
        ai.addAttribute(namespace, "name", "name", "CDATA", name);
        ch.startElement(namespace, "value", "value", ai);
        ch.startElement(namespace, "line", "line", new AttributesImpl());

        if ( value != null ) {
            ch.characters(value.toCharArray(), 0, value.length());
        }

        ch.endElement(namespace, "line", "line");
        ch.endElement(namespace, "value", "value");
    }

    /** Utility function to begin and end a <code>value</code> tag pair. */
    private void addMultilineValue(ContentHandler ch, String name, List values) throws SAXException {
        addMultilineValue(ch, name, values, null);
    }

    /** Utility function to begin and end a <code>value</code> tag pair with added attributes. */
    private void addMultilineValue(ContentHandler ch, String name, List values, Attributes atts) throws SAXException {
        AttributesImpl ai;
        if ( atts == null ) {
            ai = new AttributesImpl();
        } else {
            ai = new AttributesImpl(atts);
        }
        ai.addAttribute(namespace, "name", "name", "CDATA", name);
        ch.startElement(namespace, "value", "value", ai);

        for (int i = 0; i < values.size(); i++) {
            String value = (String) values.get(i);
            if ( value != null ) {
                ch.startElement(namespace, "line", "line", new AttributesImpl());
                ch.characters(value.toCharArray(), 0, value.length());
                ch.endElement(namespace, "line", "line");
            }
        }

        ch.endElement(namespace, "value", "value");

    }
}

