package org.apache.cocoon.scheme.sitemap;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.zip.GZIPInputStream;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.http.HttpContext;
import org.apache.cocoon.scheme.servlet.REPLGenericServlet;
import sisc.AppContext;
import sisc.DynamicEnv;
import sisc.Interpreter;
import sisc.data.Procedure;
import sisc.data.Symbol;
import sisc.data.Value;
import org.apache.avalon.framework.thread.ThreadSafe;

public class SchemeInterpreterImpl
  implements SchemeInterpreter, Component, Contextualizable, Configurable, ThreadSafe
{
  protected Symbol mainFunction;
  protected Symbol sitemapParseFunction;
  protected ArrayList interPool;
  AppContext siscContext;
  HttpContext httpContext;
  Context cocoonContext;

  public void configure(Configuration confs)
    throws ConfigurationException
  {
    String mainFunctionString = confs.getAttribute("entry-point");
    if (mainFunctionString == null)
      throw new ConfigurationException("Scheme entry point not specified");

    String sitemapParseFunctionString = confs.getAttribute("sitemap-parse");
    if (sitemapParseFunctionString == null)
      throw new ConfigurationException("Scheme sitemap parse function not specified");

    String heapFileName = confs.getAttribute("heap");
    if (heapFileName == null)
      throw new ConfigurationException("Heap Scheme file not specified");

    synchronized (httpContext) {
      siscContext = (AppContext)httpContext.getAttribute(REPLGenericServlet.appCtxAttrName);

      if (siscContext == null) {
        siscContext = new AppContext();
        httpContext.setAttribute(REPLGenericServlet.appCtxAttrName, siscContext);

        interPool = new ArrayList();
        Interpreter interp = getInterpreter();

        try {
          org.apache.cocoon.environment.Context context =
            (org.apache.cocoon.environment.Context)cocoonContext.get(Constants.CONTEXT_ENVIRONMENT_CONTEXT);

          System.out.println("loading heap "
                           + context.getResource(heapFileName));

          InputStream is = context.getResource(heapFileName).openStream();
          BufferedInputStream bis = new BufferedInputStream(is);
          GZIPInputStream gzis = new GZIPInputStream(bis);
          DataInputStream dis
            = new DataInputStream(new BufferedInputStream(gzis));
          siscContext.loadEnv(interp, dis);
        } catch (Exception ex) {
          System.err.println("Error loading heap:" + ex);
          ex.printStackTrace();
          throw new ConfigurationException("Cannot load heap file: " + ex);
        }

        mainFunction = Symbol.get(mainFunctionString);
        sitemapParseFunction = Symbol.get(sitemapParseFunctionString);
        siscContext.setEvaluator("eval");
      }
    }
  }

  public void contextualize(Context context)
    throws ContextException
  {
    this.cocoonContext = context;
    httpContext = (HttpContext)context.get(Constants.CONTEXT_ENVIRONMENT_CONTEXT);
  }

  /**
   * Obtain a Scheme Interpreter from the internal pool of
   * interpreters.
   *
   * @return an <code>Interpreter</code> value
   */
  public Interpreter getInterpreter()
  {
    synchronized(interPool) {
      int size = interPool.size();
      if (size != 0)
        return (Interpreter)interPool.remove(size - 1);
    }

    // Create a new interpreter and return it
    DynamicEnv environment = new DynamicEnv(System.in, System.out);
    Interpreter interp = new Interpreter(siscContext, environment);
    return interp;
  }

  /**
   * Put back into the pool an Interpreter instance.
   *
   * @param interp an <code>Interpreter</code> value
   */
  public void releaseInterpreter(Interpreter interp)
  {
    synchronized(interPool) {
      interPool.add(interp);
    }
  }

  public Symbol getMainFunction()
  {
    return mainFunction;
  }

  public Symbol getSitemapParseFunction()
  {
    return sitemapParseFunction;
  }
}
