package org.apache.cocoon.scheme.servlet;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.lang.NumberFormatException;
import java.util.Stack;
import java.util.zip.GZIPInputStream;
import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServlet;
import sisc.AppContext;
import sisc.ContinuationException;
import sisc.DynamicEnv;
import sisc.Interpreter;
import sisc.data.Expression;
import sisc.data.InputPort;
import sisc.data.OutputPort;
import sisc.data.Symbol;
import sisc.data.Value;
import sisc.exprs.AppExp;
import sisc.exprs.FreeReferenceExp;
import sisc.modules.J2S;

/**
 * Common functionality for REPL servlets. Reads the configuration
 * files and load the images specified in web.xml.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since December  8, 2001
 */
public class REPLGenericServlet extends HttpServlet
{
  public static final String appCtxAttrName = "sisc scheme interpreter context";
  protected ServletContext servletContext;
  protected String initExpression;
  protected String destroyExpression;
  protected Symbol mainFunction;
  protected Stack interPool;

  public void init(ServletConfig config)
    throws ServletException
  {
    super.init(config);
    servletContext = config.getServletContext();
    interPool = new Stack();

    Interpreter interp;

    synchronized (servletContext) {
      AppContext ctx = (AppContext)servletContext.getAttribute(appCtxAttrName);

      if (ctx == null) {
        ctx = new AppContext();
        servletContext.setAttribute(appCtxAttrName, ctx);

        interp = getInterpreter();
        // Read the heap file
        String realPath = servletContext.getRealPath("/");
        String heapFileName = realPath + config.getInitParameter("heap");
        System.out.println("loading heap " + heapFileName);
        File heapFile = new File(heapFileName);

        try {
          FileInputStream fis = new FileInputStream(heapFileName);
          BufferedInputStream bis
            = new BufferedInputStream(fis, (int)heapFile.length());
          GZIPInputStream gzis = new GZIPInputStream(bis);
          DataInputStream dis
            = new DataInputStream(new BufferedInputStream(gzis));
          ctx.loadEnv(interp, dis);
      
        } catch (IOException ex) {
          System.err.println("Error loading heap:" + ex);
          ex.printStackTrace();
          throw new ServletException(ex);
        }

        ctx.setEvaluator("eval");
      }
      else {
        interp = getInterpreter();
      }      
    }

    initExpression = config.getInitParameter("init-expression");
    destroyExpression = config.getInitParameter("destroy-expression");
    String main = config.getInitParameter("main-function");
    if (main != null)
      mainFunction = Symbol.get(main);
//         = new FreeReferenceExp(Symbol.get(main), -1, interp.ctx.toplevel_env);

    // Evaluate the init expression, discard the returned value and
    // any exception thrown
    try {
      if (initExpression != null && !initExpression.equals(""))
        interp.eval(initExpression);
    }
    catch (Exception ex) {
      System.out.println("Exception evaluating the init expression: " + ex);
    }

    releaseInterpreter(interp);
  }

  public void destroy()
  {
    // Evaluate the destroy expression, discard the returned value and
    // any exception thrown
    try {
      if (destroyExpression != null && !destroyExpression.equals(""))
        ((Interpreter)interPool.pop()).eval(destroyExpression);
    }
    catch (Exception ex) {
      System.out.println("Exception evaluating the destroy expression: " + ex);
    }
  }

  public Interpreter getInterpreter()
  {
    synchronized(interPool) {
      if (!interPool.empty())
        return (Interpreter)interPool.pop();

      // Create a new interpreter and return it
      AppContext ctx = (AppContext)servletContext.getAttribute(appCtxAttrName);
      DynamicEnv environment = new DynamicEnv(System.in, System.out);
      Interpreter interp = new Interpreter(ctx, environment);
      return interp;
    }
  }

  public void releaseInterpreter(Interpreter interp)
  {
    synchronized(interPool) {
      interPool.push(interp);
    }
  }
}
