/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.xml.dom;

import org.apache.cocoon.xml.AbstractXMLPipe;
import org.xml.sax.SAXException;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import javax.xml.transform.dom.DOMResult;
import javax.xml.transform.sax.SAXTransformerFactory;
import javax.xml.transform.sax.TransformerHandler;

/**
 * The <code>DOMBuilder</code> is a utility class that will generate a W3C
 * DOM Document from SAX events.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/18 12:19:19 $
 */
public class DOMBuilder
extends AbstractXMLPipe {

    /** The transformer factory shared by all instances */
    protected static final SAXTransformerFactory factory = (SAXTransformerFactory)SAXTransformerFactory.newInstance();

    /** The listener */
    protected Listener listener;

    /** The result */
    protected DOMResult result;

    /**
     * Construct a new instance of this TreeGenerator.
     */
    public DOMBuilder() {
        this( (Listener)null, (Node)null );
    }

    /**
     * Construct a new instance of this TreeGenerator.
     * @deprecated Use DOMBuilder() instead.
     */
    public DOMBuilder(DOMFactory factory) {
        this( (Listener)null, (Node)null );
    }

    /**
     * Construct a new instance of this TreeGenerator.
     */
    public DOMBuilder( Listener listener ) {
        this(listener, null);
    }

    /**
     * Construct a new instance of this TreeGenerator.
     * @deprecated Use DOMBuilder(listener) instead.
     */
    public DOMBuilder( DOMFactory factory, Listener listener ) {
        this(listener, null);
    }

    /**
     * Construct a new instance of this TreeGenerator.
     * @deprecated Use DOMBuilder(listener, parentNode) instead.
     */
    public DOMBuilder( DOMFactory domFactory, Listener listener, Node parentNode ) {
        this(listener, parentNode);
    }

    /**
     * Construct a new instance of this TreeGenerator.
     */
    public DOMBuilder( Listener listener, Node parentNode ) {
        super();
        this.listener = listener;
        try {
            TransformerHandler handler = factory.newTransformerHandler();
            this.setContentHandler(handler);
            this.setLexicalHandler(handler);
            if (parentNode != null) {
                this.result = new DOMResult( parentNode );
            } else {
                this.result = new DOMResult();
                }
            handler.setResult(this.result);
        } catch (javax.xml.transform.TransformerException local) {
            throw new RuntimeException("Fatal-Error: Unable to get transformer handler");
        }
    }

    /**
     * Constructs a new instance that appends nodes to the given parent node.<br/>
     * Note : you cannot use a <code>Listener<code> when appending to a
     * <code>Node</code>, because the notification occurs at <code>endDocument()</code>
     * which does not happen here.
     */
    public DOMBuilder( Node parentNode ) {
        this(null, null, parentNode);
    }

    /**
     * Recycling
     */
    public void recycle() {
        super.recycle();
        this.result = null;
    }

    /**
     * Return the newly built Document.
     */
    public Document getDocument() {
        if (this.result.getNode().getNodeType() == Node.DOCUMENT_NODE) {
            return ( (Document)this.result.getNode() );
        } else {
            return ( this.result.getNode().getOwnerDocument() );
        }
    }

    /**
     * Receive notification of the beginning of a document.
     *
     * @exception SAXException If this method was not called appropriately.
     */
    public void endDocument()
    throws SAXException {
        super.endDocument();

        // Notify the listener
        this.notifyListener();
    }

    /**
     * Receive notification of a successfully completed DOM tree generation.
     */
    protected void notifyListener()
    throws SAXException {
        if ( this.listener != null ) this.listener.notify( this.getDocument() );
    }

    /**
     * The Listener interface must be implemented by those objects willing to
     * be notified of a successful DOM tree generation.
     */
    public interface Listener {

        /**
         * Receive notification of a successfully completed DOM tree generation.
         */
        void notify(Document doc)
        throws SAXException;
    }
}
