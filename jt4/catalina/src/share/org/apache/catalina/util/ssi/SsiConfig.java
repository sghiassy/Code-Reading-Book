/*
 * SsiConfig.java
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/util/ssi/SsiConfig.java,v 1.3 2001/07/22 20:25:15 pier Exp $
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

import java.util.Properties;

/**
 * Implementation of the SsiCommand config, example of usage:
 * &lt;!--#config sizefmt="abbrev" errmsg="An error occured!"--&gt;
 *
 * @author Bip Thelin
 * @version $Revision: 1.3 $, $Date: 2001/07/22 20:25:15 $
 */
public final class SsiConfig
    extends SsiMediator implements SsiCommand {

    /**
     * Variable to hold if this SsiCommand modified or not
     */
    private boolean modified = false;

    /**
     * Variable to hold the errmsg to return
     */
    private byte[] errmsg;

    /**
     * Variable to hold which sizefmt to use
     */
    private String sizefmt;

    /**
     * Variable to hold which timefmt to use
     */
    private String timefmt;

    /**
     * Variable to hold the patterns for translation
     */
    private static Properties translate;

    /**
     * Initialize our pattern translation
     */
    static {
        translate = new Properties();
        translate.put("a","EEE");
        translate.put("A","EEEE");
        translate.put("b","MMM");
        translate.put("h","MMM");
        translate.put("B","MMMM");
        translate.put("d","dd");
        translate.put("D","MM/dd/yy");
        translate.put("e","d");
        translate.put("H","HH");
        translate.put("I","hh");
        translate.put("j","E");
        translate.put("m","M");
        translate.put("M","m");
        translate.put("p","a");
        translate.put("r","hh:mm:ss a");
        translate.put("S","s");
        translate.put("T","HH:mm:ss");
        translate.put("U","w");
        translate.put("W","w");
        translate.put("w","E");
        translate.put("y","yy");
        translate.put("Y","yyyy");
        translate.put("z","z");
    }

    /**
     * Initialize this SsiCommand
     *
     */
    public SsiConfig() {
        init();
    }

    /**
     * <code>process()</code> should be used since this SsiCommand
     * does not return anything.
     *
     * @param strParamType a value of type 'String[]'
     * @param strParam a value of type 'String[]'
     * @return a value of type 'String'
     */
    public String getStream(String[] strParamType, String[] strParam) {
        return "";
    }

    /**
     * Process request.
     *
     * @param strParamType a value of type 'String[]'
     * @param strParam a value of type 'String[]'
     */
    public final void process(String[] strParamType, String[] strParam) {
        modified = true;

        for(int i=0;i<strParamType.length;i++) {
            if(strParamType[i].equals("errmsg"))
                errmsg = strParam[i].getBytes();
            else if(strParamType[i].equals("sizefmt"))
                sizefmt = strParam[i];
            else if(strParamType[i].equals("timefmt"))
                timefmt = convertFormat(strParam[i]);
        }
    }

    /**
     * Return the current error message.
     *
     * @return a value of type 'byte[]'
     */
    public final byte[] getError() {
        return errmsg;
    }

    /**
     * Return the current Size format.
     *
     * @return a value of type 'String'
     */
    public final String getSizefmt() {
        return sizefmt;
    }

    /**
     * Return the current Time format.
     *
     * @return a value of type 'String'
     */
    public final String getTimefmt() {
        return timefmt;
    }

    /**
     * Initialize, run once per page being parsed.
     *
     */
    public final void flush() {
        init();
    }

    /**
     * Returns true if SSI Command does any output.
     *
     * @return a value of type 'boolean'
     */
    public final boolean isPrintable() { return false;}

    /**
     * Return true if we're modified.
     *
     * @return a value of type 'boolean'
     */
    public final boolean isModified() { return modified; }

    /**
     * Search the provided pattern and get the C standard
     * Date/Time formatting rules and convert them to the
     * Java equivalent.
     *
     * @param pattern The pattern to search
     * @return The modified pattern
     */
    private String convertFormat(String pattern) {
        boolean inside = false;
        boolean mark = false;
        StringBuffer retString = new StringBuffer();
        String sRetString = "";

        for(int i = 0; i<pattern.length();i++) {
            if(pattern.charAt(i)=='%'&&!mark) {
                mark=true;
                continue;
            }

            if(pattern.charAt(i)=='%'&&mark) {
                mark=false;
            }

            if(mark) {
                if(inside) {
                    retString.append("'");
                    inside=false;
                }

                retString.append(translateCommand(pattern.charAt(i)));
                mark=false;
                continue;
            }

            if(!inside) {
                retString.append("'");
                inside = true;
            }

            retString.append(pattern.charAt(i));
        }

        sRetString = retString.toString();

        if(!sRetString.endsWith("'")&&inside)
            sRetString = sRetString.concat("'");

        return sRetString;
    }

    /**
     * try to get the Java Date/Time formating associated with
     * the C standard provided
     *
     * @param c The C equivalent to translate
     * @return The Java formatting rule to use
     */
    private String translateCommand(char c) {
        String retCommand = translate.getProperty("".valueOf(c));

        return retCommand==null?"":retCommand;
    }

    /**
     * Called from <code>flush</code> Initialize internal parameters
     * in their default setting.
     *
     */
    private void init() {
        errmsg =
            "[an error occurred while processing this directive]".getBytes();
        sizefmt = "abbrev";
        timefmt = "EEE, dd MMM yyyyy HH:mm:ss z";
    }
}
