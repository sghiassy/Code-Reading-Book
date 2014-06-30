/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.notification;

import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;

/**
 *  Interface for a List of Objects that can notify something.
 *  NKB: FIXME The type checks are not complete.
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @created 18 December 2001
 */
public class NotifyingObjects{

  private ArrayList notifyingObjects;
  
  public NotifyingObjects()
  {
    notifyingObjects = new ArrayList();
  }
  
  public synchronized void addNotifying(Notifying n)
  { 
      notifyingObjects.add(n);
  }
  
  public synchronized Notifying getNotifying(int i)
  { //NKB cast is safe here
    return (Notifying)notifyingObjects.get(i);
  }
  
  public synchronized Notifying getLastNotifying()
  { //NKB cast is safe here
    return (Notifying)notifyingObjects.get(notifyingObjects.size()-1);
  }
  
  public Iterator iterator()
  {
    return notifyingObjects.iterator();
  }
  
  public int size()
  {
     return notifyingObjects.size();
  }
  

}

