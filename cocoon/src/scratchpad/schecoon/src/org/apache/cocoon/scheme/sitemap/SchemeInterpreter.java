package org.apache.cocoon.scheme.sitemap;

import sisc.Interpreter;
import sisc.data.Symbol;
import sisc.data.Value;

/**
 * The interface of the component used to obtain a Scheme interpreter.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since January 18, 2002
 */
public interface SchemeInterpreter
{
  public static String ROLE = "org.apache.cocoon.scheme.sitemap.SchemeInterpreter";

  /**
   * Obtain a Scheme interpreter from the pool of interpreters.
   *
   * @return a Scheme <code>Interpreter</code> instance
   */
  public Interpreter getInterpreter();

  /**
   * Release the Scheme interpreter passed as value. This adds the
   * interpreter back into the pool of interpreters.
   *
   * @param interpreter a Scheme <code>Interpreter</code> instance
   */
  public void releaseInterpreter(Interpreter interpreter);

  /**
   * Return the main function as a SISC Scheme symbol.
   *
   * @return the SISC Scheme <code>Symbol</code> corresponding to the
   * main function.
   */
  public Symbol getMainFunction();

  /**
   * Return the function used to parse the XML representation of the
   * sitemap, and convert it to SXML. This function should accept two
   * parameters: an Avalon <code>ComponentManager</code> and a Cocoon
   * <code>Source</code> object.
   *
   * @return a <code>Symbol</code> value
   */
  public Symbol getSitemapParseFunction();
}
