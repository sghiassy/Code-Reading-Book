/* ========================================================================= *
 *                                                                           *
 *                 The Apache Software License,  Version 1.1                 *
 *                                                                           *
 *          Copyright (c) 1999-2001 The Apache Software Foundation.          *
 *                           All rights reserved.                            *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * Redistribution and use in source and binary forms,  with or without modi- *
 * fication, are permitted provided that the following conditions are met:   *
 *                                                                           *
 * 1. Redistributions of source code  must retain the above copyright notice *
 *    notice, this list of conditions and the following disclaimer.          *
 *                                                                           *
 * 2. Redistributions  in binary  form  must  reproduce the  above copyright *
 *    notice,  this list of conditions  and the following  disclaimer in the *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. The end-user documentation  included with the redistribution,  if any, *
 *    must include the following acknowlegement:                             *
 *                                                                           *
 *       "This product includes  software developed  by the Apache  Software *
 *        Foundation <http://www.apache.org/>."                              *
 *                                                                           *
 *    Alternately, this acknowlegement may appear in the software itself, if *
 *    and wherever such third-party acknowlegements normally appear.         *
 *                                                                           *
 * 4. The names  "The  Jakarta  Project",  "WebApp",  and  "Apache  Software *
 *    Foundation"  must not be used  to endorse or promote  products derived *
 *    from this  software without  prior  written  permission.  For  written *
 *    permission, please contact <apache@apache.org>.                        *
 *                                                                           *
 * 5. Products derived from this software may not be called "Apache" nor may *
 *    "Apache" appear in their names without prior written permission of the *
 *    Apache Software Foundation.                                            *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES *
 * INCLUDING, BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY *
 * AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.  IN NO EVENT SHALL *
 * THE APACHE  SOFTWARE  FOUNDATION OR  ITS CONTRIBUTORS  BE LIABLE  FOR ANY *
 * DIRECT,  INDIRECT,   INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL *
 * DAMAGES (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS *
 * OR SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) *
 * HOWEVER CAUSED AND  ON ANY  THEORY  OF  LIABILITY,  WHETHER IN  CONTRACT, *
 * STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN *
 * ANY  WAY  OUT OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF  ADVISED  OF THE *
 * POSSIBILITY OF SUCH DAMAGE.                                               *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * This software  consists of voluntary  contributions made  by many indivi- *
 * duals on behalf of the  Apache Software Foundation.  For more information *
 * on the Apache Software Foundation, please see <http://www.apache.org/>.   *
 *                                                                           *
 * ========================================================================= */

/* @version $Id: ServiceController.java,v 1.1 2001/06/26 01:32:06 pier Exp $ */
package org.apache.service;

/**
 * The <code>ServiceController</code> interface defines a way in wich a
 * <code>Service</code> must control its operation and the one of the
 * VM environment in which it is running.
 * <p>
 * A <code>Service</code> must <b>never</b> call directly its own methods, but
 * rather rely on what is provided in this interface to control its execution.
 * Another method that should never be called is <code>System.exit()</code>
 * (with a notable exception of when the running environment is based on
 * version 1.3 of the Java 2 Platform). Calling ANY of those methods will
 * result in unpredicable behavior of the underlying native invocation
 * abstraction.
 * </p>
 *
 * @author Pier Fumagalli, Sun Microsystems INC.
 * @author Copyright &copy; The Apache Software Foundation.
 */
public interface ServiceController {
    /**
     * Stop the <code>Service</code> and shutdown the VM environment.
     * <p>
     * This method is equivalent as calling the <code>System.exit()</code>
     * method, but it will also notify the <code>Service</code> through the
     * invocation of its <code>stop()</code> method.
     * </p>
     *
     * @exception IllegalStateException If the <code>shutdown()</code> or
     *                <code>reload()</code> methods were already called, or
     *                if in any other way a shutdown is pending.
     */
    public void shutdown()
    throws IllegalStateException;

    /**
     * Stop the <code>Service</code>, shutdown the VM environment and attempt
     * to reload a new instance of it in a new VM environment.
     * <p>
     * This method is equivalent to the <code>shutdown()</code> method, but it
     * will also attempt to restart the VM environment.
     * </p>
     *
     * @exception IllegalStateException If the <code>shutdown()</code> or
     *                <code>reload()</code> methods were already called, or
     *                if in any other way a shutdown is pending.
     */
    public void reload()
    throws IllegalStateException;
}
