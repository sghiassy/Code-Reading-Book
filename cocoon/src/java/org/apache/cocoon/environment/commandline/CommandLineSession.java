/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment.commandline;

import org.apache.cocoon.environment.Session;

import java.util.Enumeration;
import java.util.Hashtable;

/**
 *
 * Command-line version of Http Session.
 *
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/22 00:17:12 $
 */
public final class CommandLineSession
implements Session {

    private long creationTime = System.currentTimeMillis();

    private Hashtable attributes = new Hashtable();

    public CommandLineSession() {
    }

    public long getCreationTime() {
        return this.creationTime;
    }

    public String getId() {
        return "1";
    }

    public long getLastAccessedTime() {
        return this.creationTime;
    }

    public void setMaxInactiveInterval(int interval) {
        // ignored
    }

    public int getMaxInactiveInterval() {
        return -1;
    }

    public Object getAttribute(String name) {
        return this.attributes.get(name);
    }

    public Enumeration getAttributeNames() {
        return this.attributes.keys();
    }

    public void setAttribute(String name, Object value) {
        this.attributes.put(name, value);
    }

    public void removeAttribute(String name) {
        this.attributes.remove(name);
    }

    public void invalidate() {
        this.attributes.clear();
        invalidateSession();
    }

    public boolean isNew() {
        return false;
    }

    protected static CommandLineSession session;

    /**
     * Get the current session object - if available
     */
    public static Session getSession(boolean create) {
        if (create && session == null) {
            session = new CommandLineSession();
        }
        return session;
    }

    /**
     * Invalidate the current session
     */
    public static void invalidateSession() {
        session = null;
    }

}

