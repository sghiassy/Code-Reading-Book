/*
 * SsiFsize.java
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/util/ssi/SsiFsize.java,v 1.3 2001/07/22 20:25:15 pier Exp $
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

import java.io.IOException;
import java.net.URL;
import java.net.MalformedURLException;
import javax.servlet.ServletContext;

/**
 * Return the size of a given file, subject of formatting.
 *
 * @author Bip Thelin
 * @version $Revision: 1.3 $, $Date: 2001/07/22 20:25:15 $
 */
public final class SsiFsize
    extends SsiMediator implements SsiCommand {

    /**
     * Figure out the length/size of a given file.
     *
     * @param strParamType The parameter type
     * @param strParam The value, only "var" accepted
     * @return The result
     */
    public final String getStream(String[] strParamType,
                                  String[] strParam) {
        String length = "";
        String retLength = "";
        String path = "";
        URL url = null;
        long lLength = -1;

        if(strParamType[0].equals("file")) {
            path = super.getFilePath(strParam[0]);
        } else if(strParamType[0].equals("virtual")) {
            path = super.getVirtualPath(strParam[0]);
        }

        try {
            url = super.servletContext.getResource(path);
            lLength = url.openConnection().getContentLength();
            length = (new Long(lLength)).toString();
        } catch (MalformedURLException e){
            length = null;
        } catch (IOException e) {
            length = null;
        } catch (NullPointerException e) {
            length = null;
        }

        if(length == null)
            retLength = (new String(super.getError()));
        else
            retLength =
                formatSize(length,
                           ((SsiConfig)super.getCommand("config")).getSizefmt());

        return retLength;
    }

    /**
     * Not used since this SsiCommand return a stream, use
     * <code>getStream()</code> instead.
     *
     * @param strParamType a value of type 'String[]'
     * @param strParam a value of type 'String[]'
     */
    public final void process(String[] strParamType, String[] strParam) {}

    /**
     * Returns <code>true</code> this SsiCommand is always prnitable
     * and should therefore be accsessed through <code>getStream()</code>
     *
     * @return a value of type 'boolean'
     */
    public final boolean isPrintable() { return true; }

    /**
     * Returns <code>false</code>, this SsiCommands is never buffered.
     *
     * @return a value of type 'boolean'
     */
    public final boolean isModified() { return false; }

    //----------------- Private methods

    /**
     * Format the size with the correct format, either
     * abbrev or bytes.
     *
     * @param length The variable to format
     * @param format The pattern to use when formatting
     * @return The result
     */
    private String formatSize(String length, String format) {
        String retString = "";

        if(format.equalsIgnoreCase("bytes")) {
            retString = commaFormat(length);
        } else {
            double lParse = (new Long(length)).doubleValue();

            if(lParse>=1048576) {
                double abbrevSize = lParse/1048576;
                long splitSize = (long)abbrevSize;
                int  catSize = (int)(100 * (abbrevSize - splitSize));

                retString =
                    commaFormat((new Long(splitSize)).toString())+
                    "."+catSize+" MB";
            } else if(lParse>=1024) {
                double abbrevSize = lParse/1024;
                long splitSize = (long)abbrevSize;
                int  catSize = (int)(100 * (abbrevSize - splitSize));

                retString =
                    commaFormat((new Long(splitSize)).toString())+
                    "."+catSize+" KB";
            } else {
                retString =
                    commaFormat(length)+" bytes";
            }
        }

        return retString;
    }

    /**
     * Modify the supplied variable to be returned comma formated.
     *
     * @param length The variable to modify
     * @return The modified result
     */
    private String commaFormat(String length) {
        String retString = "";

        for(int i = length.length();i-1>=0;i--) {
            retString = (length.substring(i-1,i)).concat(retString);
            if((length.length()-(i-1))%3==0&&
               retString.length()<length.length())
                retString = ",".concat(retString);
        }

        return retString;
    }
}
