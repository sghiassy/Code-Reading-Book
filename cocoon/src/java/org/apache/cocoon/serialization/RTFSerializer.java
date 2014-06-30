/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.serialization;

import java.net.URL;
import java.net.MalformedURLException;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;

import org.apache.cocoon.components.url.URLFactory;

import org.apache.log.Priority;
import org.apache.log.util.OutputStreamLogger;

import org.jfor.jfor.converter.Converter;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.sax.TransformerHandler;
import javax.xml.transform.stream.StreamResult;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.io.BufferedWriter;

import org.xml.sax.SAXException;
import org.xml.sax.Attributes;
import org.xml.sax.helpers.AttributesImpl;

/**
 * This class uses the <a href="http://www.jfor.org">jfor</a> library
 * to serialize XSL:FO documents to RTF streams.
 *
 * @author <a href="mailto:gianugo@rabellino.it">Gianugo Rabellino</a>
 */

public class RTFSerializer extends AbstractTextSerializer
  implements Composable {

    private Writer rtfWriter = null;
    private Converter handler = null;
    private OutputStreamLogger cocoonLogger = null;
    private ComponentManager manager = null;


    public RTFSerializer() {
    }

    /**
     * Set the current <code>ComponentManager</code> instance used by this
     * <code>Composable</code>.
     */
    public void compose(ComponentManager manager) {
      this.manager = manager;
    }

    /**
     * Set the OutputStream where the serializer will write to.
     *
     * @param out the OutputStream
     */


    public void setOutputStream(OutputStream out) {
        try {
          cocoonLogger =
            new OutputStreamLogger(this.getLogger(), Priority.DEBUG);

          rtfWriter =
            new BufferedWriter(new OutputStreamWriter(out));
          handler = new Converter(rtfWriter,
            Converter.createConverterOption(cocoonLogger));
          super.setContentHandler(handler);

        } catch (Exception e) {
            getLogger().error("RTFSerializer.setOutputStream()", e);
            throw new RuntimeException(e.toString());
        }
    }

    /**
     * Recycle the serializer. GC instance variables
     */
    public void recycle() {
        super.recycle();
        rtfWriter = null;
        handler = null;
        cocoonLogger = null;
    }
}
