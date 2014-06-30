/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.sax;

import org.apache.cocoon.xml.AbstractSAXFragment;
import org.apache.cocoon.xml.EmbeddedXMLPipe;
import org.apache.cocoon.xml.XMLConsumer;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;

/**
 * An XMLByteStream wrapped by an XMLFragment implementation. This allows to
 * store SAX events and insert them in an XSP result using &lt;xsp:expr&gt;.
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:13 $
 */

public class XMLByteStreamFragment extends AbstractSAXFragment {
    
    /** The XML byte stream */
    private Object xmlBytes;
    
    /**
     * Creates a new <code>XMLByteStreamFragment</code> defined by the given
     * XML byte stream.
     *
     * @param bytes the XML byte stream representing the document fragment
     */
    public XMLByteStreamFragment(Object bytes) {
        xmlBytes = bytes;
    }
    
    /**
     * Output the fragment. If the fragment is a document, start/endDocument
     * events are discarded.
     */
    public void toSAX(ContentHandler ch)
      throws SAXException {
        
        // Stream bytes and discard start/endDocument
        XMLByteStreamInterpreter interp = new XMLByteStreamInterpreter();
        EmbeddedXMLPipe pipe = new EmbeddedXMLPipe(ch);

        // If ch is an XMLConsumer, set it as such so that XML comments are
        // also deserialized.
        if (ch instanceof XMLConsumer)
            pipe.setConsumer((XMLConsumer) ch);
        
        interp.setContentHandler(pipe);
        
        interp.deserialize(xmlBytes);
    }
}
