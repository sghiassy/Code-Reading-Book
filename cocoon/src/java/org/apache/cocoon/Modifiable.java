/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon;

/**
 * This interface is implemented by those classes that change
 * their behavior/results over time (non-ergodic).
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 */
public interface Modifiable
{
    /**
     * Queries the class to estimate its ergodic period termination.
     * <br>
     * This method is called to ensure the validity of a cached product. It
     * is the class responsibility to provide the fastest possible
     * implementation of this method or, whether this is not possible and the
     * costs of the change evaluation is comparable to the production costs,
     * to return <b>true</b> directly with no further delay, thus reducing
     * the evaluation overhead to a minimum.
     *
     * @return <b>true</b> if the class ergodic period is over and the class
     *         would behave differently if processed again, <b>false</b> if the
     *         resource is still ergodic so that it doesn't require
     *         reprocessing.
     */
    boolean modifiedSince( long date );
}
