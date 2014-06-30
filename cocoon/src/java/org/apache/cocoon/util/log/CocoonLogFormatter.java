/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.util.log;

import org.apache.avalon.framework.CascadingThrowable;
import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Request;
import org.apache.log.ContextMap;
import org.apache.log.LogEvent;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Map;

/**
 * An extended pattern formatter. New patterns are defined by this class are :
 * <ul>
 * <li><code>class</code> : outputs the name of the class that has logged the
 *     message. The optional <code>short</code> subformat removes the
 *     package name. Warning : this pattern works only if formatting occurs in
 *     the same thread as the call to Logger, i.e. it won't work with
 *     <code>AsyncLogTarget</code>.</li>
 * <li><code>thread</code> : outputs the name of the current thread (first element
 *     on the context stack).</li>
 * <li><code>uri</code> : outputs the request URI.<li>
 * </ul>
 *
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:22 $
 */

public class CocoonLogFormatter extends ExtensiblePatternFormatter
{
    protected final static int     TYPE_CLASS  = MAX_TYPE + 1;
    protected final static int     TYPE_URI    = MAX_TYPE + 2;
    protected final static int     TYPE_THREAD = MAX_TYPE + 3;

    protected final static String  TYPE_CLASS_STR       = "class";
    protected final static String  TYPE_CLASS_SHORT_STR = "short";

    protected final static String  TYPE_URI_STR         = "uri";
    protected final static String  TYPE_THREAD_STR      = "thread";

    protected final SimpleDateFormat dateFormatter = new SimpleDateFormat("(yyyy-MM-dd) HH:mm.ss:SSS");

    /**
     * Hack to get the call stack as an array of classes. The
     * SecurityManager class provides it as a protected method, so
     * change it to public through a new method !
     */
    static public class CallStack extends SecurityManager
    {
        /**
         * Returns the current execution stack as an array of classes.
         * The length of the array is the number of methods on the execution
         * stack. The element at index 0 is the class of the currently executing
         * method, the element at index 1 is the class of that method's caller,
         * and so on.
         */

        public Class[] get()
        {
            return getClassContext();
        }
    }

    /** The class that we will search for in the call stack */
    private Class loggerClass = org.apache.log.Logger.class;

    private CallStack callStack = new CallStack();


    protected int getTypeIdFor(String type) {

        // Search for new patterns defined here, or else delegate
        // to the parent class
        if (type.equalsIgnoreCase(TYPE_CLASS_STR))
            return TYPE_CLASS;
        else if (type.equalsIgnoreCase(TYPE_URI_STR))
            return TYPE_URI;
        else if (type.equalsIgnoreCase(TYPE_THREAD_STR))
            return TYPE_THREAD;
        else
            return super.getTypeIdFor( type );
    }

    protected String formatPatternRun(LogEvent event, PatternRun run) {

        // Format new patterns defined here, or else delegate to
        // the parent class
        switch (run.m_type) {
            case TYPE_CLASS :
                return getClass(run.m_format);

            case TYPE_URI :
                return getURI(event.getContextMap());

            case TYPE_THREAD :
                return getThread(event.getContextMap());
        }

        return super.formatPatternRun(event, run);
    }

    /**
     * Finds the class that has called Logger.
     */
    private String getClass(String format) {

        Class[] stack = this.callStack.get();

        // Traverse the call stack in reverse order until we find a Logger
        for (int i = stack.length-1; i >= 0; i--) {
            if (this.loggerClass.isAssignableFrom(stack[i])) {

                // Found : the caller is the previous stack element
                String className = stack[i+1].getName();

                // Handle optional format
                if (TYPE_CLASS_SHORT_STR.equalsIgnoreCase(format))
                {
                    int pos = className.lastIndexOf('.');
                    if (pos >= 0)
                        className = className.substring(pos + 1);
                }

                return className;
            }
        }

        // No Logger found in call stack : can occur with AsyncLogTarget
        // where formatting takes place in a different thread.
        return "Unknown-class";
    }

    /**
     * Find the URI that is being processed.
     */
    private String getURI(ContextMap ctxMap) {
        String result = "Unknown-URI";

        // Get URI from the the object model.
        if (ctxMap != null) {
            Object context = ctxMap.get("objectModel");
            if (context != null && context instanceof Map) {
                // Get the request
                Request request = (Request)((Map)context).get(Constants.REQUEST_OBJECT);
                if (request != null) {
                    result = request.getRequestURI();
                }
            }
        }

        return result;
    }


    /**
     * Find the thread that is logged this event.
     */
    private String getThread(ContextMap ctxMap) {
        if (ctxMap != null && ctxMap.get("threadName") != null)
            return (String)ctxMap.get("threadName");
        else
            return "Unknown-thread";
    }

    /**
     * Utility method to format stack trace so that CascadingExceptions are
     * formatted with all nested exceptions.
     *
     * FIXME : copied from AvalonFormatter, to be removed if ExtensiblePatternFormatter
     * replaces PatternFormatter.
     *
     * @param throwable the throwable instance
     * @param format ancilliary format parameter - allowed to be null
     * @return the formatted string
     */
    protected String getStackTrace( final Throwable throwable, final String format )
    {
        final StringBuffer sb = new StringBuffer();
        sb.append( super.getStackTrace( throwable, format ) );

        if( throwable instanceof CascadingThrowable )
        {
            final Throwable t = ((CascadingThrowable)throwable).getCause();

            sb.append( getStackTrace( t, format ) );
        }

        return sb.toString();
    }

    /**
     * Utility method to format time.
     *
     * @param time the time
     * @param format ancilliary format parameter - allowed to be null
     * @return the formatted string
     */
    protected String getTime( final long time, final String format )
    {
        return this.dateFormatter.format(new Date());
    }

}
