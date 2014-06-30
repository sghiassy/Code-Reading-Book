/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.xml;

import org.apache.cocoon.xml.dom.DOMStreamer;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

/**
 * Abstract implementation of {@link XMLFragment} for objects that are more easily represented
 * as a DOM.
 * <br/>
 * The toSAX() method is implemented by streaming (using a <code>DOMStreamer</code>)
 * the results of <code>toDOM()</code> that must be implemented by concrete subclasses.
 * 
 * @author <a href="mailto:sylvain.wallez@anyware-tech.com">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:22 $
 */

public abstract class AbstractDOMFragment implements XMLFragment {

    /**
     * Generates SAX events representing the object's state by serializing the
     * result of <code>toDOM()</code>.
     */
    
    public void toSAX(ContentHandler handler) throws SAXException {
        // The ComponentManager is unknown here : use JAXP to create a document
        DocumentBuilder builder;
        try {
            builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
        }
        catch (ParserConfigurationException pce) {
            throw new SAXException("Couldn't get a DocumentBuilder", pce);
        }
        
        Document doc = builder.newDocument();
        
        // Create a DocumentFragment that will hold the results of toDOM()
        // (which can create several top-level elements)
        Node df = doc.createDocumentFragment();
        
        // Build the DOM representation of this object
        try {
            toDOM(df);
        }
        catch(Exception e) {
            throw new SAXException("Exception while converting object to DOM", e);
        }
        
        // Stream the document fragment
        DOMStreamer streamer = new DOMStreamer(handler);
        streamer.stream(df);
    }
}
