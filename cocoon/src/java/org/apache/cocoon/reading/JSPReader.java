/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.reading;

import org.apache.avalon.excalibur.pool.Poolable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.jsp.JSPEngine;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.http.HttpEnvironment;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;

/**
 * @author <a href="mailto:kpiroumian@flagship.ru">Konstantin Piroumian</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:18 $
 *
 * The <code>JSPReader</code> component is used to serve JSP page output data
 * in a sitemap pipeline.
 */
public class JSPReader extends AbstractReader
            implements Composable, Poolable{

    private ComponentManager manager;

    public void compose (ComponentManager manager) {
        this.manager = manager;
    }

    /** Contextualize this class
    public void contextualize(Context context) throws ContextException {
        if (this.contextDir == null) {
            org.apache.cocoon.environment.Context ctx =
                (org.apache.cocoon.environment.Context) context.get(Constants.CONTEXT_ENVIRONMENT_CONTEXT);
            try {
                this.contextDir = new File(ctx.getRealPath("/")).toURL().toExternalForm();
                getLogger().debug("JSPReader: Context directory is " + this.contextDir);
            } catch (MalformedURLException e) {
                getLogger().error("JSPReader: Could not get context directory", e);
                throw new ContextException("JSPReader: Could not get context directory", e);
            }
        }
    }
    */
    /**
     * Generates the output from JSP page.
     */
    public void generate() throws IOException, ProcessingException {
        // ensure that we are running in a servlet environment
        HttpServletResponse httpResponse =
            (HttpServletResponse)this.objectModel.get(HttpEnvironment.HTTP_RESPONSE_OBJECT);
        HttpServletRequest httpRequest =
            (HttpServletRequest)this.objectModel.get(HttpEnvironment.HTTP_REQUEST_OBJECT);
        ServletContext httpContext =
            (ServletContext)this.objectModel.get(HttpEnvironment.HTTP_SERVLET_CONTEXT);

        if (httpResponse == null || httpRequest == null || httpContext == null) {
            throw new ProcessingException("JSPReader can be used only in a Servlet/JSP environment");
        }

        JSPEngine engine = null;
        Source src = null;
        try {
            // KP: A hacky way of source resolving.
            // Why context:// protocol returns not a string in URL format,
            // but a system-dependent path with 'file:' prefix?
            String contextDir = new File(httpContext.getRealPath("/")).toURL().toExternalForm();
            src = this.resolver.resolve(this.source);
            String url = src.getSystemId();
            if(url.startsWith(contextDir)) {
                // File is located under contextDir, using relative file name
                url = url.substring(contextDir.length());
            }
            if (url.startsWith("file:")) {
                // we need a relative path
                url = url.substring(5);
            }

            getLogger().debug("JSPReader executing JSP:" + url);

            engine = (JSPEngine)this.manager.lookup(JSPEngine.ROLE);
            byte[] bytes = engine.executeJSP(url, httpRequest, httpResponse, httpContext);

            byte[] buffer = new byte[8192];
            int length = -1;

            ByteArrayInputStream bais = new ByteArrayInputStream(bytes);
            while ((length = bais.read(buffer)) > -1) {
                out.write(buffer, 0, length);
            }
            bais.close();
            bais = null;
            out.flush();
            //
        } catch (ServletException e) {
            getLogger().debug("ServletException in JSPReader.generate()", e);
            getLogger().debug("Embedded ServletException JSPReader.generate()", e.getRootCause());
            throw new ProcessingException("ServletException in JSPReader.generate()",e.getRootCause());
        } catch (IOException e) {
            getLogger().debug("IOException in JSPReader.generate()", e);
            throw new ProcessingException("IOException JSPReader.generate()",e);
        } catch (ProcessingException e) {
            throw e;
        } catch (Exception e) {
            getLogger().debug("Exception in JSPReader.generate()", e);
            throw new ProcessingException("Exception JSPReader.generate()",e);
        } finally {
            if (src != null) src.recycle();
            if (engine != null) this.manager.release(engine);
        }
    }
}
