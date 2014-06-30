/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.xscript;

import org.apache.avalon.framework.component.ComponentManager;

/**
 * <code>XScriptManager</code> is the public interface used to
 * interact with the XScript component, which implements the
 * supporting code for the XScript language.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since August  4, 2001
 */
public interface XScriptManager
{
  String ROLE = "org.apache.cocoon.components.xscript.XScriptManager";

  String XSCRIPT_NS = "http://apache.org/xsp/xscript/1.0";

  /**
   * The variable's global scope. Each Cocoon instance has exactly one
   * global scope for variables.
   */
  int GLOBAL_SCOPE = 1;
  /**
   * The session scope. This scope is specific to a particular
   * activation of an XSP page, which is usually identified by the
   * session id of the user that sent the HTTP request. From XScript's
   * point of view however, the session identifier is not interpreted
   * in any way, so a client can define its own notion of "session".
   */
  int SESSION_SCOPE = 2;

  /**
   * The page scope. This is scope very specific to an XSP page, and
   * defines variables visible only within the context of that
   * page. No other XSP page can access these variables, unless it
   * knows the identifier used by that page to store its
   * variables. This identifier is not necessarily the URL or full
   * path of that page; it is up to the page to define it.
   */
  int PAGE_SCOPE = 3;

  /**
   * Search for a variable in all the accessible scopes. The variable
   * is first searched in the current session scope. If no variable is
   * found here, the current page scope is searched next. If nothing
   * is found either, the global scope is searched.
   */
  int ALL_SCOPES = 4;

  /**
   * Obtains the object value of the
   * <code>name</code> variable in <code>scope</code>. The
   * <code>context</code> parameter is interpreted differently
   * depending on the value of <code>scope</code>, as follows:
   *
   * <ul>
   *
   *  <li>if <code>scope</code> is <code>{@link #GLOBAL_SCOPE}</code>, the
   *  value of <code>context is ignored.
   *
   *  <li>if <code>scope</code> is <code>{@link
   *  #SESSION_SCOPE}</code>, the value of <code>context</code> is
   *  interpreted as the session identifier.
   *
   *  <li>if <code>scope</code> is <code>{@link #PAGE_SCOPE}</code>, the value
   *  of <code>context</code> is interpreted as an identifier of the
   *  page. This could be the URL of the page or the path of the file
   *  name in the file system.
   *
   * </ul>
   *
   * @param name a <code>String</code> value
   * @param scope an <code>int</code> value
   * @param context a <code>String</code> value whose interpretation
   * depends on <code>scope</code>
   * @return a <code>{@link XScriptObject}</code> value
   */
  XScriptObject get(String name, int scope, String context)
    throws IllegalArgumentException;

  /**
   * Search for the first occurence of the variable
   * <code>name</code>.
   *
   * <p>The search happens first in the session scope
   * identified by <code>sessionContext</code>. If no variable is
   * found here, the search continues in the page scope identified by
   * <code>pageContext</code>. If no variable is found here, it's
   * finally searched in the global scope.
   *
   * <p>The <code>XScriptObject</code> value of the variable is
   * returned if a variable is found in one of the scopes, otherwise
   * an exception is thrown.
   *
   * @param name a <code>String</code> value
   * @param sessionContext a <code>String</code> value
   * @param pageContext a <code>String</code> value
   * @return a <code>XScriptObject</code> value
   * @exception IllegalArgumentException if an error occurs
   */
  XScriptObject getFirst(String name,
                                String sessionContext,
                                String pageContext)
    throws IllegalArgumentException;

  /**
   * Defines or overwrites the value of variable
   * <code>name</code> in <code>scope</code>. The <code>context</code>
   * argument is interpreted as described in {@link #get(String, int,
   * String)}.
   *
   * @param name a <code>String</code> value
   * @param value a <code>XScriptObject</code> value
   * @param scope an <code>int</code> value
   * @param context a <code>String</code> value whose interpretation
   * depends on <code>scope</code>
   */
  void put(String name, XScriptObject value, int scope, String context)
    throws IllegalArgumentException;

  /**
   * Removes a variable previously declared in <code>scope</code>
   * within <code>context</code>. Such a variable could be declared
   * using the {@link #put(String, XScriptObject, int, String)}
   * method.
   *
   * @param name a <code>String</code> value
   * @param scope an <code>int</code> value
   * @param context a <code>String</code> value
   */
  void remove(String name, int scope, String context);

  /**
   * Remove the first appearance of <code>name</code> in the all the
   * currently accessible scopes. The search happens as described in
   * {@link #getFirst(String,String,String)}.
   *
   * @param name a <code>String</code> value
   * @param sessionContext a <code>String</code> value
   * @param pageContext a <code>String</code> value
   * @exception IllegalArgumentException if an error occurs
   */
  void removeFirst(String name, String sessionContext, String pageContext)
    throws IllegalArgumentException;
}
