/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.generation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.http.HttpEnvironment;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.util.Enumeration;

/**
 * Allows PHP to be used as a generator.  Builds upon the PHP servlet
 * functionallity - overrides the output method in order to pipe the
 * results into SAX events.
 *
 * @author <a href="mailto:rubys@us.ibm.com">Sam Ruby</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */
public class PhpGenerator extends ServletGenerator implements Recyclable {

    /**
     * Stub implementation of Servlet Config
     */
    class config implements ServletConfig {
        ServletContext c;
        public config(ServletContext c) {this.c = c; }

        public String getServletName() { return "PhpGenerator"; }
        public Enumeration getInitParameterNames()
               { return c.getInitParameterNames(); }
        public ServletContext getServletContext() { return c; }
        public String getInitParameter(String name) { return null; }
    }

    /**
     * Subclass the PHP servlet implementation, overriding the method
     * that produces the output.
     */
    public class PhpServlet extends net.php.servlet implements Runnable {

        String input;
        OutputStream output;
        HttpServletRequest request;
        HttpServletResponse response;
        ServletException exception = null;

        public void setInput(String input) {
            this.input = input;
        }

        public void setOutput(OutputStream output) {
            this.output = output;
        }

        public void setRequest(HttpServletRequest request) {
            this.request = request;
        }

        public void setResponse(HttpServletResponse response) {
            this.response = response;
        }

        public void write(String data) {
            try {
                output.write(data.getBytes());
            } catch (IOException e) {
                PhpGenerator.this.getLogger().debug("PhpGenerator.write()", e);
                throw new RuntimeException(e.getMessage());
            }
        }

        /******************************************************************/
        /*                       runnable interface                       */
        /******************************************************************/

        public void run() {
            try {
                service(request, response, input);
            } catch (ServletException e) {
                PhpGenerator.this.getLogger().error("PhpGenerator.run()", e);
                this.exception = e;
            }

            try {
                output.close();
            } catch (IOException e) {
                PhpGenerator.this.getLogger().error("PhpGenerator.run():SHOULD NEVER HAPPEN", e);
                // should never happen
            }
        }

    }

    /**
     * Generate XML data from PHP.
     */
    public void generate() throws IOException, SAXException, ProcessingException {

        // ensure that we are running in a servlet environment
        HttpServletResponse httpResponse =
            (HttpServletResponse)this.objectModel.get(HttpEnvironment.HTTP_RESPONSE_OBJECT);
        HttpServletRequest httpRequest =
            (HttpServletRequest)this.objectModel.get(HttpEnvironment.HTTP_REQUEST_OBJECT);
        if (httpResponse == null || httpRequest == null) {
            throw new ProcessingException("HttpServletRequest or HttpServletResponse object not available");
        }

        // ensure that we are serving a file...
        Source inputSource = null;
        Parser parser = null;
        try {
            inputSource = this.resolver.resolve(this.source);
            String systemId = inputSource.getSystemId();
            if (!systemId.startsWith("file:/"))
                throw new IOException("protocol not supported: " + systemId);

            // construct both ends of the pipe
            PipedInputStream input = new PipedInputStream();

            // start PHP producing results into the pipe
            PhpServlet php = new PhpServlet();
            php.init(new config((ServletContext)this.objectModel.get(HttpEnvironment.HTTP_SERVLET_CONTEXT)));
            php.setInput(systemId.substring(6));
            php.setOutput(new PipedOutputStream(input));
            php.setRequest(httpRequest);
            php.setResponse(httpResponse);
            new Thread(php).start();

            // pipe the results into the parser
            parser = (Parser)this.manager.lookup(Parser.ROLE);
            parser.setConsumer(this.xmlConsumer);
            parser.parse(new InputSource(input));

            // clean up
            php.destroy();
        } catch (IOException e) {
            getLogger().debug("PhpGenerator.generate()", e);
            throw e;
        } catch (Exception e) {
            getLogger().debug("PhpGenerator.generate()", e);
            throw new IOException(e.toString());
        } finally {
            if (inputSource != null) inputSource.recycle();
            if (parser != null) this.manager.release(parser);
        }
    }
}
