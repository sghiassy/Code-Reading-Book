package org.apache.cocoon.scheme.servlet;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import sisc.ContinuationException;
import sisc.Interpreter;
import sisc.data.Value;

/**
 * Interactive Scheme evaluator. Use it only during development as it
 * is a big security risk.
 *
 * Receive the Scheme forms to evaluate via a POST message. Use the
 * Emacs minor mode to automatically submit a buffer or a region to
 * evaluate in the servlet.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since December  8, 2001
 */
public class REPLEvalServlet extends REPLGenericServlet
{
  protected boolean isEnabled = false;
  
  public void init(ServletConfig config)
    throws ServletException
  {
    super.init(config);

    String enabledString = config.getInitParameter("enabled");
    if (enabledString != null)
      isEnabled = enabledString.equalsIgnoreCase("true");
  }

  public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException
  {
    if (!isEnabled) {
      response.sendError(HttpServletResponse.SC_FORBIDDEN,
                         "You're not authorized to access this URL!");
      return;
    }

    response.setContentType("text/plain");

    OutputStream os = response.getOutputStream();
    PrintStream out = new PrintStream(os);
    System.setErr(out);

    try {
      InputStream is = request.getInputStream();
      InputStreamReader isr = new InputStreamReader(is);
      int size = 4096;
      int len;
      char[] buffer = new char[size];
      StringBuffer sbuf = new StringBuffer();

      while ((len = isr.read(buffer, 0, size)) > 0) {
        sbuf.append(buffer, 0, len);
      }

      System.out.println("executing '" + sbuf + "'");

      Interpreter interp = getInterpreter();
      out.println(interp.eval(sbuf.toString()));
      releaseInterpreter(interp);
    }
    catch (Exception ex) {
      out.println("ERROR: " + ex);
      throw new ServletException(ex);
    }
  }
}
