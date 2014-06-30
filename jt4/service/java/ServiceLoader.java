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

/* @version $Id: ServiceLoader.java,v 1.1 2001/06/26 01:32:15 pier Exp $ */
package org.apache.service.support;

import org.apache.service.Service;
import org.apache.service.ServiceController;

public final class ServiceLoader {

    private static Controller controller=null;
    private static Service service=null;

    public static void version() {
        System.err.println("java version \""+
                           System.getProperty("java.version")+
                           "\"");
        System.err.println(System.getProperty("java.runtime.name")+
                           " (build "+
                           System.getProperty("java.runtime.version")+
                           ")");
        System.err.println(System.getProperty("java.vm.name")+
                           " (build "+
                           System.getProperty("java.vm.version")+
                           ", "+
                           System.getProperty("java.vm.info")+
                           ")");
    }

    public static boolean check(String cn) {
        try {
            /* Check the class name */
            if (cn==null)
                throw new NullPointerException("Null class name specified");

            /* Get the ClassLoader loading this class */
            ClassLoader cl=ServiceLoader.class.getClassLoader();
            if (cl==null) {
                System.err.println("Cannot retrieve ClassLoader instance");
                return(false);
            }

            /* Find the required class */
            Class c=cl.loadClass(cn);

            /* This should _never_ happen, but doublechecking doesn't harm */
            if (c==null) throw new ClassNotFoundException(cn);

            /* Create a new instance of the service */
            Service s=(Service)c.newInstance();

        } catch (Throwable t) {
            /* In case we encounter ANY error, we dump the stack trace and
               return false (load, start and stop won't be called). */
            t.printStackTrace(System.err);
            return(false);
        }
        /* The class was loaded and instantiated correctly, we can return */
        return(true);
    }

    public static boolean load(String cn, String ar[]) {
        try {
            /* Make sure any previous instance is garbage collected */
            System.gc();

            /* Check if the underlying libray supplied a valid list of
               arguments */
            if (ar==null) ar=new String[0];

            /* Check the class name */
            if (cn==null)
                throw new NullPointerException("Null class name specified");

            /* Get the ClassLoader loading this class */
            ClassLoader cl=ServiceLoader.class.getClassLoader();
            if (cl==null) {
                System.err.println("Cannot retrieve ClassLoader instance");
                return(false);
            }

            /* Find the required class */
            Class c=cl.loadClass(cn);

            /* This should _never_ happen, but doublechecking doesn't harm */
            if (c==null) throw new ClassNotFoundException(cn);

            /* Create a new instance of the service */
            service=(Service)c.newInstance();

            /* Create a new controller instance */
            controller=new Controller();

            /* Set the availability flag in the controller */
            controller.setAvailable(false);

            /* Now we want to call the load method in the class */
            service.load(controller,ar);

        } catch (Throwable t) {
            /* In case we encounter ANY error, we dump the stack trace and
               return false (load, start and stop won't be called). */
            t.printStackTrace(System.err);
            return(false);
        }
        /* The class was loaded and instantiated correctly, we can return */
        return(true);
    }

    public static boolean start() {
        try {
            /* Attempt to start the service */
            service.start();

            /* Set the availability flag in the controller */
            controller.setAvailable(true);

        } catch (Throwable t) {
            /* In case we encounter ANY error, we dump the stack trace and
               return false (load, start and stop won't be called). */
            t.printStackTrace(System.err);
            return(false);
        }
        return(true);
    }

    public static boolean stop() {
        try {
            /* Set the availability flag in the controller */
            controller.setAvailable(false);

            /* Attempt to start the service */
            service.stop();

            /* Run garbage collector */
            service=null;
            controller=null;
            System.gc();

        } catch (Throwable t) {
            /* In case we encounter ANY error, we dump the stack trace and
               return false (load, start and stop won't be called). */
            t.printStackTrace(System.err);
            return(false);
        }
        return(true);
    }

    private static native void shutdown(boolean reload);

    public static class Controller implements ServiceController {

        boolean available=false;

        private Controller() {
            super();
            this.setAvailable(false);
        }

        private boolean isAvailable() {
            synchronized (this) {
                return(this.available);
            }
        }

        private void setAvailable(boolean available) {
            synchronized (this) {
                this.available=available;
            }
        }

        public void shutdown() throws IllegalStateException {
            synchronized (this) {
                if (!this.isAvailable()) {
                    throw new IllegalStateException();
                } else {
                    this.setAvailable(false);
                    ServiceLoader.shutdown(false);
                }
            }
        }

        public void reload() throws IllegalStateException {
            synchronized (this) {
                if (!this.isAvailable()) {
                    throw new IllegalStateException();
                } else {
                    this.setAvailable(false);
                    ServiceLoader.shutdown(true);
                }
            }
        }
    }
}
