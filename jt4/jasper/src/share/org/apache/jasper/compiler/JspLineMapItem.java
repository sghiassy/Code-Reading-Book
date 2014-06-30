/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/JspLineMapItem.java,v 1.1 2001/07/12 21:41:10 horwat Exp $
 * $Revision: 1.1 $
 * $Date: 2001/07/12 21:41:10 $
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
 */ 
package org.apache.jasper.compiler;

/**
 * Data structure used for each individual line map item.
 * This data structure has a set of numbers representing the 
 * beginning and ending jsp line numbers and the associated 
 * generated servlet lines.
 *
 * @author Justyna Horwat
 */
public class JspLineMapItem {

    private int[] itemArray;

    public JspLineMapItem() {
        itemArray = new int[8];
    }

    /**
     * Set the beginning servlet line number
     */
    public void setBeginServletLnr(int item) {
        itemArray[0] = item;
    }

    /**
     * Get the beginning servlet line number
     */
    public int getBeginServletLnr() {
        return itemArray[0];
    }

    /**
     * Set the ending servlet line number
     */
    public void setEndServletLnr(int item) {
        itemArray[1] = item;
    }

    /**
     * Get the ending servlet line number
     */
    public int getEndServletLnr() {
        return itemArray[1];
    }

    /**
     * Set the index of the starting jsp file
     */
    public void setStartJspFileNr(int item) {
        itemArray[2] = item;
    }

    /**
     * Get the index of the starting jsp file
     */
    public int getStartJspFileNr() {
        return itemArray[2];
    }

    /**
     * Set the beginning jsp line number
     */
    public void setBeginJspLnr(int item) {
        itemArray[3] = item;
    }

    /**
     * Get the beginning jsp line number
     */
    public int getBeginJspLnr() {
        return itemArray[3];
    }

    /**
     * Set the beginning jsp column number
     */
    public void setBeginJspColNr(int item) {
        itemArray[4] = item;
    }

    /**
     * Get the beginning jsp column number
     */
    public int getBeginJspColNr() {
        return itemArray[4];
    }

    /**
     * Set the index of the stopping jsp file
     */
    public void setStopJspFileNr(int item) {
        itemArray[5] = item;
    }

    /**
     * Get the index of the stopping jsp file
     */
    public int getStopJspFileNr() {
        return itemArray[5];
    }

    /**
     * Set the ending jsp line number
     */
    public void setEndJspLnr(int item) {
        itemArray[6] = item;
    }

    /**
     * Get the ending jsp line number
     */
    public int getEndJspLnr() {
        return itemArray[6];
    }

    /**
     * Set the ending jsp column number
     */
    public void setEndJspColNr(int item) {
        itemArray[7] = item;
    }

    /**
     * Get the ending jsp column number
     */
    public int getEndJspColNr() {
        return itemArray[7];
    }

    /**
     * Convert data structure to a string
     */
    public String toString() {
        StringBuffer out = new StringBuffer();

        for (int j=0; j < itemArray.length; j++) {
            out.append("["+j+"] " + itemArray[j]+" ");
        }
        out.append("\n");

        return out.toString();
    }
}
