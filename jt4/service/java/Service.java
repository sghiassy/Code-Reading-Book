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

/* @version $Id: Service.java,v 1.1 2001/06/26 01:31:58 pier Exp $ */
package org.apache.service;

/**
 * The <code>Service</code> interface provides a <b>simple</b> abstraction
 * for a Java Application running as a service (in UNIX words, a daemon).
 * <p>
 * This interface can be considered as an extension of the common invocation
 * mechanism used in Java of the <code>main(String argv[])</code> method,
 * and provides some extensions to it, allowing to perform certain operations
 * that before were not possible to obtain, and commonly used in native
 * services and daemons (such as user-id switching, signal and event handling
 * and so on).
 * </p>
 * <p>
 * This interface and related classes are <b>not</b> intended to be used in
 * other environment rather than low-level control of the VM environment and
 * its lifecycle (JMX, and JSR-111 will provide those APIs), and must always
 * be accompanied with a binary or wrapper that will be used instead of the
 * usual <b>java</b><i>(.exe)</i> invocator.
 * </p>
 *
 * @author Pier Fumagalli, Sun Microsystems INC.
 * @author Copyright &copy; The Apache Software Foundation.
 */
public interface Service {
    /**
     * Load the instance of this <code>Service</code>.
     * <p>
     * This method is called after the VM enviroment is started and a new
     * instance of this <code>Service</code> is constructed. This method is
     * the direct parallel to the <code>main(...)</code> method used by
     * common Java applications.
     * </p>
     * <p>
     * In some environments (many UNIX variants, for example) and when
     * specified, this method is the only one invoked as <b>super user</b>
     * and it is possible to perform operations normally restricted (for
     * example binding to a network port lesser than or equal to 1024).
     * After successful completion, and if specified, all calls to the other
     * methods will be performed as a <b>regular user</b>.
     * <br>
     * NOTE: For information on how to use this feature, please consult the
     * documentation of the invocation binary possibly distributed with these
     * classes.
     * </p>
     * <p>
     * Any exception thrown by this method will prevent any further execution
     * of the <code>start()</code> and <code>stop()</code> methods, and and
     * will result in a VM environment shutdown. If configured accordingly the
     * thrown exception will be logged.
     * </p>
     *
     * @param controller The <code>ServiceController</code> controlling this
     *            <code>Service</code> operation. A <code>Service</code> must
     *            never call its own methods as it would leave the controlling
     *            environment in an undefined state.
     *
     * @param arguments The arguments related to the execution of this
     *            <code>Service</code>, possibly from the operating system
     *            command line.
     *
     * @exception Throwable Any <code>Exception</code> or error which prevented
     *                the successful completion of this method.
     */
    public void load(ServiceController controller, String arguments[])
    throws Throwable;

    /**
     * Start this <code>Service</code> execution.
     * <p>
     * Any exception thrown by this method will prevent any further execution
     * of the <code>stop()</code> method, and will result in a VM environment
     * shutdown. If configured accordingly the thrown exception will be logged.
     * </p>
     *
     * @exception Throwable Any <code>Exception</code> or error which prevented
     *                the successful completion of this method.
     */
    public void start()
    throws Throwable;

    /**
     * Stop this <code>Service</code> execution.
     * <p>
     * As this method is invoked before the VM environment is shut down, any
     * exception thrown by this method will not alter this behaviour, but, if
     * configured accordingly, the exception will be logged.
     * </p>
     *
     * @exception Throwable Any <code>Exception</code> or error which prevented
     *                the successful completion of this method.
     */
    public void stop()
    throws Throwable;
}
