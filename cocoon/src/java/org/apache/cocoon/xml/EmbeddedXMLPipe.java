/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.xml;

import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;

/**
 * This class implements a ContentHandler for embedding a full SAX
 * event stream into an existing stream of SAX events. An instance of
 * this class will pass unmodified all the SAX events to the linked
 * ContentHandler, but will ignore the startDocument and endDocument
 * events.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:22 $
 */
public class EmbeddedXMLPipe extends AbstractXMLPipe
{
    /**
     * Creates an EmbeddedXMLPipe that writes into the given ContentHandler.
     */
    public EmbeddedXMLPipe(ContentHandler handler) {
        setContentHandler(handler);
    }
     
    /**
     * Ignore the <code>startDocument</code> event: this method does nothing.
     *
     * @exception SAXException if an error occurs
     */
    public void startDocument()
    throws SAXException
    {
    }
    
    /**
     * Ignore the <code>endDocument</code> event: this method does nothing.
     *
     * @exception SAXException if an error occurs
     */
    public void endDocument()
    throws SAXException
    {
    }
}
