/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.xscript;

import java.util.HashMap;

/**
 * <code>XScriptVariableScope</code> maintains variables in a given
 * scope. A variable has a unique name within a scope, but multiple
 * variables with the same name can exist within different scopes.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since August 4, 2001
 */
public class XScriptVariableScope
{
  /**
   * The variables store; each entry is <code>String</code>
   * representing the name of the variable, with the corresponding
   * value an {@link XScriptObject}.
   */
  HashMap variables = new HashMap();

  /**
   * Define a new variable or overwrite the value of an existing
   * variable in this scope.
   *
   * @param name a <code>String</code> value
   * @param value a <code>{@link XScriptObject}</code> value
   */
  public synchronized void put(String name, XScriptObject value)
  {
    variables.put(name, value);
  }

  /**
   * Obtains the value of the XScript <code>name</code> variable.
   *
   * @param name a <code>String</code> value
   * @return a <code>{@link XScriptObject}</code> value
   */
  public synchronized XScriptObject get(String name)
    throws IllegalArgumentException
  {
    if (!variables.containsKey(name))
      throw new IllegalArgumentException("Cannot find variable '"
                                         + name + "'!");
    return (XScriptObject)variables.get(name);
  }

  /**
   * Removes the XScript variable that's accessible via
   * <code>name</code>.
   *
   * @param name a <code>String</code> value
   */
  public synchronized void remove(String name)
  {
    variables.remove(name);
  }

  public synchronized boolean defines(String name)
  {
    return variables.containsKey(name);
  }
}
