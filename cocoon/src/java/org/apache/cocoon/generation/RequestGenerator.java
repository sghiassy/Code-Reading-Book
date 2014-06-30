/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.generation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.parameters.ParameterException;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.SourceResolver;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.IOException;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.Map;

/**
 * Generates an XML representation of the incoming request.
 * <p>
 * <b>Configuration options:</b>
 * <dl>
 * <dt> <i>container-encoding</i> (optional)
 * <dd> The encoding used by container. Default value is ISO-8859-1.
 * <dt> <i>form-encoding</i> (optional)
 * <dd> The supposed encoding of the request parameter. Default is null.
 * </dl>
 * These configuration options supported in both declaration and use time.
 *
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */
public class RequestGenerator extends ServletGenerator implements Parameterizable, Recyclable {

    /** The URI of the namespace of this generator. */
    private String URI="http://xml.apache.org/cocoon/requestgenerator/2.0";
    private String global_container_encoding;
    private String global_form_encoding;
    private String container_encoding;
    private String form_encoding;

    public void parameterize(Parameters parameters)
    throws ParameterException {
        // super.parameterize(parameters);

        global_container_encoding = parameters.getParameter("container-encoding", "ISO-8859-1");
        global_form_encoding = parameters.getParameter("form-encoding", null);
    }

    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters par)
    throws ProcessingException, SAXException, IOException {
        super.setup(resolver, objectModel, src, par);

        container_encoding = par.getParameter("container-encoding", global_container_encoding);
        form_encoding = par.getParameter("form-encoding", global_form_encoding);
    }

    /**
     * Generate XML data.
     */
    public void generate()
    throws SAXException {
        Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
        this.contentHandler.startDocument();
        this.contentHandler.startPrefixMapping("",URI);
        AttributesImpl attr=new AttributesImpl();

        this.attribute(attr,"target", request.getRequestURI());
        this.attribute(attr,"source", (this.source != null ? this.source : ""));
        this.start("request", attr);
        this.data("\n");
        this.data("\n");

        this.data("  ");
        this.start("requestHeaders", attr);
        this.data("\n");
        Enumeration headers=request.getHeaderNames();
        while (headers.hasMoreElements()) {
            String header=(String)headers.nextElement();
            this.attribute(attr,"name",header);
            this.data("    ");
            this.start("header",attr);
            this.data(request.getHeader(header));
            this.end("header");
            this.data("\n");
        }
        this.data("  ");
        this.end("requestHeaders");
        this.data("\n");
        this.data("\n");

        this.data("  ");
        this.start("requestParameters",attr);
        this.data("\n");
        Enumeration parameters=request.getParameterNames();
        while (parameters.hasMoreElements()) {
            String parameter=(String)parameters.nextElement();
            this.attribute(attr,"name",parameter);
            this.data("    ");
            this.start("parameter",attr);
            this.data("\n");
            String values[]=request.getParameterValues(parameter);
            if (values!=null) for (int x=0; x<values.length; x++) {
                this.data("      ");
                this.start("value",attr);
                if (form_encoding != null) {
                    try {
                        this.data(new String(values[x].getBytes(container_encoding),
                            form_encoding));
                    } catch(java.io.UnsupportedEncodingException uee) {
                        throw new RuntimeException("Unsupported Encoding Exception: " +
                            uee.getMessage());
                    }
                } else {
                    this.data(values[x]);
                }
                this.end("value");
                this.data("\n");
            }
            this.data("    ");
            this.end("parameter");
            this.data("\n");
        }
        this.data("  ");
        this.end("requestParameters");
        this.data("\n");
        this.data("\n");

        this.data("  ");
        this.start("configurationParameters",attr);
        this.data("\n");
        String[] confparams=super.parameters.getNames();
        for (int i=0; i<confparams.length; i++) {
            this.attribute(attr, "name", confparams[i]);
            this.data("    ");
            this.start("parameter",attr);
            this.data(super.parameters.getParameter(confparams[i], ""));
            this.end("parameter");
            this.data("\n");
        }
        this.data("  ");
        this.end("configurationParameters");
        this.data("\n");
        this.data("\n");

        this.end("request");

        // Finish
        this.contentHandler.endPrefixMapping("");
        this.contentHandler.endDocument();
    }

    private void attribute(AttributesImpl attr, String name, String value) {
        attr.addAttribute("",name,name,"CDATA",value);
    }

    private void start(String name, AttributesImpl attr)
    throws SAXException {
        super.contentHandler.startElement(URI,name,name,attr);
        attr.clear();
    }

    private void end(String name)
    throws SAXException {
        super.contentHandler.endElement(URI,name,name);
    }

    private void data(String data)
    throws SAXException {
        super.contentHandler.characters(data.toCharArray(),0,data.length());
    }
}
