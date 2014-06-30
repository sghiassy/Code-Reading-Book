/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.generation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.jsp.JSPEngine;
import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.http.HttpEnvironment;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.ByteArrayInputStream;
import java.io.IOException;

/**
 * Allows JSP to be used as a generator.  Builds upon the JSP servlet
 * functionality - overrides the output method in order to pipe the
 * results into SAX events.
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */
public class JspGenerator extends ServletGenerator implements Recyclable, Configurable {

    public void configure(Configuration conf) throws ConfigurationException
    {
    }

    /**
     * Generate XML data from JSP.
     */
    public void generate() throws ProcessingException {

        // ensure that we are running in a servlet environment
        HttpServletResponse httpResponse =
            (HttpServletResponse)this.objectModel.get(HttpEnvironment.HTTP_RESPONSE_OBJECT);
        HttpServletRequest httpRequest =
            (HttpServletRequest)this.objectModel.get(HttpEnvironment.HTTP_REQUEST_OBJECT);
        ServletContext httpContext = 
            (ServletContext)this.objectModel.get(HttpEnvironment.HTTP_SERVLET_CONTEXT);

        if (httpResponse == null || httpRequest == null || httpContext == null) {
            throw new ProcessingException("HttpServletRequest or HttpServletResponse or ServletContext object not available");
        }

        JSPEngine engine = null;
        Parser parser = null;
        Source src = null;
        try {
            src = this.resolver.resolve(this.source);
            String url = src.getSystemId();
            // Guarantee src parameter is a file
            if (!url.startsWith("file:/"))
                throw new IOException("Protocol not supported: " + url);

            url = url.substring(5);
            getLogger().debug("JspGenerator executing JSP:" + url);

            engine = (JSPEngine)this.manager.lookup(JSPEngine.ROLE);
            byte[] bytes = engine.executeJSP(url, httpRequest, httpResponse, httpContext);

            // explicitly specify bytestream encoding
            InputSource input = new InputSource(new ByteArrayInputStream(bytes));
            input.setEncoding("utf-8");

            // pipe the results into the parser
            parser = (Parser)this.manager.lookup(Parser.ROLE);
            parser.setConsumer(this.xmlConsumer);
            parser.parse(input);
        } catch (ServletException e) {
            getLogger().debug("ServletException in JspGenerator.generate()", e);
            getLogger().debug("Embedded ServletException JspGenerator.generate()", e.getRootCause());
            throw new ProcessingException("ServletException in JspGenerator.generate()",e.getRootCause());
        } catch (SAXException e) {
            getLogger().debug("SAXException JspGenerator.generate()", e);
            getLogger().debug("Embedded SAXException JspGenerator.generate()", e.getException());
            throw new ProcessingException("SAXException JspGenerator.generate()",e.getException());
        } catch (IOException e) {
            getLogger().debug("IOException in JspGenerator.generate()", e);
            throw new ProcessingException("IOException JspGenerator.generate()",e);
        } catch (ProcessingException e) {
            throw e;
        } catch (Exception e) {
            getLogger().debug("Exception in JspGenerator.generate()", e);
            throw new ProcessingException("Exception JspGenerator.generate()",e);
        } finally {
            if (src != null) src.recycle();
            if (parser != null) this.manager.release(parser);
            if (engine != null) this.manager.release(engine);
        }
    }
}
