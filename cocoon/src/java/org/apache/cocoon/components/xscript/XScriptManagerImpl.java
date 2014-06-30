/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.xscript;

import java.util.HashMap;

import java.io.InputStream;

import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.parameters.ParameterException;

import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.avalon.excalibur.pool.Poolable;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;

import org.apache.cocoon.components.xscript.XScriptObjectFromURL;

/**
 * The actual implementation of the <code>XScriptManager</code> interface.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since August  4, 2001
 */
public class XScriptManagerImpl
  extends AbstractLoggable
  implements XScriptManager, Composable, Component, Configurable, Poolable
{
  /**
   * The global scope. All the global variables are indexed in this
   * space by their name.
   */
  XScriptVariableScope globalScope = new XScriptVariableScope();

  /**
   * The session scope. The key in this hash table is the session
   * identifier, which should be a <code>String</code> object. The
   * value is an {@link XScriptVariableScope} instance which holds the
   * variables in a session scope.
   */
  HashMap sessionScope = new HashMap();

  /**
   * Page specific variables. The key in the hash map is an identifier
   * for the page, usually the full path name to the page. The value
   * is an {@link XScriptVariableScope} instance which holds the
   * variables in a page scope.
   */
  HashMap pageScope = new HashMap();

  /**
   * The <code>ComponentManager</code> instance.
   */
  protected ComponentManager manager = null;

  public void compose(ComponentManager manager)
    throws ComponentException
  {
    this.manager = manager;
    getLogger().debug("XScriptManager component initialized.");
  }

  public void register(XScriptObject object)
  {
    try {
      object.compose(manager);
    }
    catch (ComponentException ex) {
    }
  }

  public void configure(Configuration conf)
    throws ConfigurationException
  {
    try {
      Parameters params = Parameters.fromConfiguration(conf);
      String[] names = params.getNames();
      for (int i = 0; i < names.length; i++) {
        String resourceString = params.getParameter(names[i]);
        XScriptObject resource = new XScriptObjectFromURL(this, resourceString);
        globalScope.put(names[i], resource);
      }
    }
    catch (ParameterException ex) {
      throw new ConfigurationException("Error configuring XScriptManager: ",
                                       ex);
    }
  }

  private IllegalArgumentException
    createAccessException(String msg, String name, int scope, String context)
  {
    StringBuffer message = new StringBuffer("Cannot ").append(msg)
                           .append(" variable named '").append(name)
                           .append("' in ");

    if (scope == XScriptManager.GLOBAL_SCOPE)
      message.append("global scope");
    else if (scope == XScriptManager.SESSION_SCOPE)
      message.append("session scope");
    else if (scope == XScriptManager.PAGE_SCOPE)
      message.append("page scope");
    else
      message.append("unknown scope (").append(scope).append(")");

    message.append(" with context '").append(context).append("'");
    return new IllegalArgumentException(message.toString());
  }
  
  public XScriptObject get(String name, int scope, String context)
    throws IllegalArgumentException
  {
    if (scope == XScriptManager.GLOBAL_SCOPE)
      return globalScope.get(name);
    else if (scope == XScriptManager.SESSION_SCOPE) {
      XScriptVariableScope s = (XScriptVariableScope)sessionScope.get(context);
      if (s != null)
        return s.get(name);
    }
    else if (scope == XScriptManager.PAGE_SCOPE) {
      XScriptVariableScope s = (XScriptVariableScope)pageScope.get(context);
      if (s != null)
        return s.get(name);
    }

    throw createAccessException("find", name, scope, context);
  }
  
  public XScriptObject getFirst(String name,
                                String sessionContext,
                                String pageContext)
    throws IllegalArgumentException
  {
    XScriptVariableScope scope;

    // Lookup in the session scope first.
    scope = (XScriptVariableScope)sessionScope.get(sessionContext);
    if (scope != null) {
      synchronized(scope) {
        if (scope.defines(name))
          return scope.get(name);
      }
    }

    // No luck finding `name' in session scope, try in page scope.
    scope = (XScriptVariableScope)pageScope.get(pageContext);
    if (scope != null) {
      synchronized(scope) {
        if (scope.defines(name))
          return scope.get(name);
      }
    }

    // No luck finding `name' in the page scope, try the global scope.
    synchronized(globalScope) {
      if (globalScope.defines(name))
        return globalScope.get(name);
    }

    // No variable `name' found, throw an exception.
    throw new IllegalArgumentException("getFirst: no variable '" + name
                                       + "' accessible from this scope!");
  }
  
  public void put(String name, XScriptObject value, int scope, String context)
  {
    if (scope == XScriptManager.GLOBAL_SCOPE) {
      globalScope.put(name, value);
    }
    else if (scope == XScriptManager.SESSION_SCOPE) {
      XScriptVariableScope s = (XScriptVariableScope)sessionScope.get(context);
      if (s == null) {
        s = new XScriptVariableScope();
        sessionScope.put(context, s);
      }
      s.put(name, value);
    }
    else if (scope == XScriptManager.PAGE_SCOPE) {
      XScriptVariableScope s = (XScriptVariableScope)pageScope.get(context);
      if (s == null) {
        s = new XScriptVariableScope();
        pageScope.put(context, s);
      }
      s.put(name, value);
    }
    else
      throw createAccessException("create", name, scope, context);
  }

  public void remove(String name, int scope, String context)
    throws IllegalArgumentException
  {
    if (scope == XScriptManager.GLOBAL_SCOPE) {
      globalScope.remove(name);
    }
    else if (scope == XScriptManager.SESSION_SCOPE) {
      XScriptVariableScope s = (XScriptVariableScope)sessionScope.get(context);
      if (s != null)
        s.remove(name);
    }
    else if (scope == XScriptManager.PAGE_SCOPE) {
      XScriptVariableScope s = (XScriptVariableScope)pageScope.get(context);
      if (s != null)
        s.remove(name);
    }
    else
      throw createAccessException("remove", name, scope, context);
  }

  public void removeFirst(String name,
                          String sessionContext,
                          String pageContext)
    throws IllegalArgumentException
  {
    XScriptVariableScope scope;

    // Lookup in the session scope first.
    scope = (XScriptVariableScope)sessionScope.get(sessionContext);
    if (scope != null) {
      synchronized(scope) {
        if (scope.defines(name)) {
          scope.remove(name);
          return;
        }
      }
    }

    // No luck finding `name' in session scope, try in page scope.
    scope = (XScriptVariableScope)pageScope.get(pageContext);
    if (scope != null) {
      synchronized(scope) {
        if (scope.defines(name)) {
          scope.remove(name);
          return;
        }
      }
    }

    // No luck finding `name' in the page scope, try the global scope.
    synchronized(globalScope) {
      if (globalScope.defines(name)) {
        globalScope.remove(name);
        return;
      }
    }

    // No variable `name' found, throw an exception.
    throw new IllegalArgumentException("removeFirst: no variable '" + name
                                       + "' accessible from this scope!");
  }
}
