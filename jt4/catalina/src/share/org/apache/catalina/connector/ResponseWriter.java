/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/connector/ResponseWriter.java,v 1.1 2000/12/13 23:45:38 craigmcc Exp $
 * $Revision: 1.1 $
 * $Date: 2000/12/13 23:45:38 $
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


package org.apache.catalina.connector;


import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;


/**
 * Wrapper around the standard <code>java.io.PrintWriter</code> that keeps
 * track of whether or not any characters have ever been written (even if they
 * are still buffered inside the PrintWriter or any other Writer that it uses
 * above the underlying TCP/IP socket).  This is required by the semantics of
 * several calls on ServletResponse, which are required to throw an
 * <code>IllegalStateException</code> if output has ever been written.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.1 $ $Date: 2000/12/13 23:45:38 $
 */

public class ResponseWriter extends PrintWriter {


    // ------------------------------------------------------------ Constructor


    /**
     * Construct a new ResponseWriter, wrapping the specified writer and
     * attached to the specified response.
     *
     * @param writer OutputStreamWriter to which we are attached
     * @param stream ResponseStream to which we are attached
     */
    public ResponseWriter(OutputStreamWriter writer, ResponseStream stream) {

        super(writer);
        this.stream = stream;
        this.stream.setCommit(false);

    }


    // ----------------------------------------------------- Instance Variables


    /**
     * The response stream to which we are attached.
     */
    protected ResponseStream stream = null;


    // --------------------------------------------------------- Public Methods


    /**
     * Flush this stream, and cause the response to be committed.
     */
    public void flush() {

        stream.setCommit(true);
        super.flush();
        stream.setCommit(false);

    }


    /**
     * Print a boolean value.
     *
     * @param b The value to be printed
     */
    public void print(boolean b) {

        super.print(b);
        super.flush();

    }


    /**
     * Print a character value.
     *
     * @param c The value to be printed
     */
    public void print(char c) {

        super.print(c);
        super.flush();

    }


    /**
     * Print a character array value.
     *
     * @param ca The value to be printed
     */
    public void print(char ca[]) {

        super.print(ca);
        super.flush();

    }


    /**
     * Print a double value.
     *
     * @param d The value to be printed
     */
    public void print(double d) {

        super.print(d);
        super.flush();

    }


    /**
     * Print a float value.
     *
     * @param f The value to be printed
     */
    public void print(float f) {

        super.print(f);
        super.flush();

    }


    /**
     * Print an integer value.
     *
     * @param i The value to be printed.
     */
    public void print(int i) {

        super.print(i);
        super.flush();

    }



    /**
     * Print a long value.
     *
     * @param l The value to be printed
     */
    public void print(long l) {

        super.print(l);
        super.flush();

    }


    /**
     * Print an object value.
     *
     * @param o The value to be printed
     */
    public void print(Object o) {

        super.print(o);
        super.flush();

    }


    /**
     * Print a String value.
     *
     * @param s The value to be printed
     */
    public void print(String s) {

        super.print(s);
        super.flush();

    }


    /**
     * Terminate the current line by writing the line separator string.
     */
    public void println() {

        super.println();
        super.flush();

    }


    /**
     * Print a boolean value and terminate the current line.
     *
     * @param b The value to be printed
     */
    public void println(boolean b) {

        super.println(b);
        super.flush();

    }


    /**
     * Print a character value and terminate the current line.
     *
     * @param c The value to be printed
     */
    public void println(char c) {

        super.println(c);
        super.flush();

    }


    /**
     * Print a character array value and terminate the current line.
     *
     * @param ca The value to be printed
     */
    public void println(char ca[]) {

        super.println(ca);
        super.flush();

    }


    /**
     * Print a double value and terminate the current line.
     *
     * @param d The value to be printed
     */
    public void println(double d) {

        super.println(d);
        super.flush();

    }


    /**
     * Print a float value and terminate the current line.
     *
     * @param f The value to be printed
     */
    public void println(float f) {

        super.println(f);
        super.flush();

    }


    /**
     * Print an integer value and terminate the current line.
     *
     * @param i The value to be printed.
     */
    public void println(int i) {

        super.println(i);
        super.flush();

    }



    /**
     * Print a long value and terminate the current line.
     *
     * @param l The value to be printed
     */
    public void println(long l) {

        super.println(l);
        super.flush();

    }


    /**
     * Print an object value and terminate the current line.
     *
     * @param o The value to be printed
     */
    public void println(Object o) {

        super.println(o);
        super.flush();

    }


    /**
     * Print a String value and terminate the current line.
     *
     * @param s The value to be printed
     */
    public void println(String s) {

        super.println(s);
        super.flush();

    }


    /**
     * Write a single character.
     *
     * @param c The value to be written
     */
    public void write(char c) {

        super.write(c);
        super.flush();

    }


    /**
     * Write an array of characters.
     *
     * @param ca The value to be written
     */
    public void write(char ca[]) {

        super.write(ca);
        super.flush();

    }


    /**
     * Write a portion of an array of characters.
     *
     * @param ca The array from which to write
     * @param off Starting offset
     * @param len Number of characters to write
     */
    public void write(char ca[], int off, int len) {

        super.write(ca, off, len);
        super.flush();

    }


    /**
     * Write a String.
     *
     * @param s The value to be written
     */
    public void write(String s) {

        super.write(s);
        super.flush();

    }


    /**
     * Write a portion of a String.
     *
     * @param s The String from which to write
     * @param off Starting offset
     * @param len Number of characters to write
     */
    public void write(String s, int off, int len) {

        super.write(s, off, len);
        super.flush();

    }


}
