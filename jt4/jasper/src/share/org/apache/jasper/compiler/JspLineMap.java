/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/JspLineMap.java,v 1.1 2001/07/12 21:41:10 horwat Exp $
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

import java.util.ArrayList;

/**
 * Data structure to store the line and file map information.
 * The line map has a mapping of which jsp lines correspond to
 * the generated servlet. The file map has all of jsp files
 * that are included in the servlet.
 *
 * @author Justyna Horwat
 */
public class JspLineMap {

    private ArrayList lineMap;
    private ArrayList fileMap;

    public JspLineMap() {
        lineMap = new ArrayList();
        fileMap = new ArrayList();
    }

    /**
     * Add an item to the line map data structure
     */
    public void add(JspLineMapItem lineMapItem) {
        lineMap.add(lineMapItem);
    }

    /**
     * Get an item to the line map data structure
     */
    public JspLineMapItem get(int index) {
        return (JspLineMapItem) lineMap.get(index);
    }

    /**
     * Get line map data structure size
     */
    public int size() {
        return lineMap.size();
    }

    public void clear() {
        lineMap.clear();
        fileMap.clear();
    }

    /**
     * Add a file to the file map data structure. The index is
     * stored in the line map to associate a file with the line
     * of code.
     */
    public int addFileName(String fileName) {
        int idx = fileMap.indexOf(fileName);

        if (idx>=0) return idx;

        fileName = fileName.replace( '\\', '/' );
        fileMap.add(fileName);
        idx = fileMap.size() - 1 ; // added item

        return idx;
    }

    /**
     * Get a file from the file map data structure. Use the index
     * to grab the right file name.
     */
    public String getFileName(int index) {
        return (String) fileMap.get(index);
    }

    /**
     * Convert data structures to a string
     */
    public String toString() {
        int i;
        JspLineMapItem lineMapItem;
        StringBuffer out = new StringBuffer();

        out.append("JspLineMap Debugging:\n");
        out.append("lineMap: \n");
        for (i=0; i < lineMap.size(); i++) {
            lineMapItem = (JspLineMapItem) lineMap.get(i);
            out.append("#" + i + ": ");
            out.append(lineMapItem.toString());
        }

        out.append("fileMap: \n");

        for (i=0; i < fileMap.size(); i++) {
            out.append("#" + i + ": " + fileMap.get(i) + "\n");
        }

        return out.toString();
    }

}
