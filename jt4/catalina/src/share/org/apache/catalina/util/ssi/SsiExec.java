/*
 * SsiExec.java
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/util/ssi/SsiExec.java,v 1.3 2001/07/22 20:25:15 pier Exp $
 * $Revision: 1.3 $
 * $Date: 2001/07/22 20:25:15 $
 *
 * ====================================================================
 *
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowlegement:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowlegement may appear in the software itself,
 *    if and wherever such third-party acknowlegements normally appear.
 *
 * 4. The names "The Jakarta Project", "Tomcat", and "Apache Software
 *    Foundation" must not be used to endorse or promote products derived
 *    from this software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 * [Additional notices, if required by prior licensing conditions]
 *
 */

package org.apache.catalina.util.ssi;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import javax.servlet.ServletException;
import javax.servlet.ServletContext;
import javax.servlet.ServletOutputStream;
import javax.servlet.RequestDispatcher;

/**
 * @author Bip Thelin
 * @author Amy Roh
 * @version $Revision: 1.3 $, $Date: 2001/07/22 20:25:15 $
 *
 */
public final class SsiExec
    extends SsiMediator implements SsiCommand {

    public SsiExec() {}

    public final String getStream(String[] strParamType,
                                  String[] strParam) {

        String retString = "";
        String path = "";

        if(strParamType[0].equals("cgi")) {
            path = super.getCGIPath(strParam[0]);
            if (path != null) {
                try {
                    URL url = new URL(path);
                    InputStream istream = url.openStream();
                    int i = istream.read();
                    while (i != -1) {
                        super.out.write(i);
                        i = istream.read();
                    }
                } catch (IOException e) {
                    retString = new String(super.getError());
                }
            } else {
                retString = new String(super.getError());
            }
        } else if(strParamType[0].equals("cmd"))

            path = super.getCommandPath(strParam[0]);

            if (path!=null) {
                    BufferedReader commandsStdOut = null;
                    BufferedReader commandsStdErr = null;
                    BufferedOutputStream commandsStdIn = null;
                BufferedWriter out = new BufferedWriter(new OutputStreamWriter(super.out));
                    //byte[] bBuf = new byte[1024];
                    char[] cBuf = new char[1024];
                    int bufRead = -1;

                Runtime rt = null;
                Process proc = null;

                try {
                    rt = Runtime.getRuntime();
                    proc = rt.exec(path);

                commandsStdIn = new BufferedOutputStream(proc.getOutputStream());
                    //boolean isRunning = true;
                    commandsStdOut = new BufferedReader(new InputStreamReader(proc.getInputStream()));
                    commandsStdErr = new BufferedReader(new InputStreamReader(proc.getErrorStream()));
                    BufferedWriter servletContainerStdout = null;

                        while ((bufRead = commandsStdErr.read(cBuf)) != -1) {
                                out.write(cBuf, 0, bufRead);
                            }

                    cBuf = new char[1024];
                    while ((bufRead = commandsStdOut.read(cBuf)) != -1) {
                                out.write(cBuf, 0, bufRead);
                            }

                        super.out.flush();

                        proc.exitValue();
                } catch (IOException ex) {
                }
        } else {
            retString = new String(super.getError());
        }

        return retString;
    }

    public final void process(String[] strParamType, String[] strParam) {}

    public final boolean isPrintable() { return true; }

    public final boolean isModified() { return false; }
}
