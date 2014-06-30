/*
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

import java.lang.reflect.Constructor;

import java.util.Hashtable;
import java.util.Enumeration;

import javax.servlet.jsp.tagext.TagLibraryInfo;
import javax.servlet.jsp.tagext.TagInfo;
import javax.servlet.jsp.tagext.Tag;

import org.apache.jasper.Constants;
import org.apache.jasper.JasperException;


/**
 * A container for all tag libraries that have been imported using
 * the taglib directive. 
 *
 * @author Anil K. Vijendran
 * @author Mandar Raje
 */
public class TagLibraries {
    
    public TagLibraries(ClassLoader cl) {
        this.tagLibInfos = new Hashtable();
	this.tagCaches = new Hashtable();
        this.cl = cl;
    }
    
    public void addTagLibrary(String prefix, TagLibraryInfo tli) {
        tagLibInfos.put(prefix, tli);
    }
    
    public boolean isUserDefinedTag(String prefix, String shortTagName) 
        throws JasperException
    {
        TagLibraryInfo tli = (TagLibraryInfo) tagLibInfos.get(prefix);
        if (tli == null)
            return false;
        else if (tli.getTag(shortTagName) != null)
            return true;
        throw new JasperException(Constants.getString("jsp.error.bad_tag",
                                                      new Object[] {
                                                          shortTagName,
                                                          prefix
                                                      }
                                                      ));
    }
    
    public TagLibraryInfo getTagLibInfo(String prefix) {
        return (TagLibraryInfo) tagLibInfos.get(prefix);
    }

    public TagCache getTagCache(String prefix, String shortTagName) {
	return (TagCache) tagCaches.get(new TagID(prefix, shortTagName));
    }

    public void putTagCache(String prefix, String shortTagName, TagCache tc) {
	tagCaches.put(new TagID(prefix, shortTagName), tc);
    }

    public Enumeration getTagLibInfos() {
	return tagLibInfos.elements();
    }

    private Hashtable tagLibInfos;
    private Hashtable tagCaches;
    private ClassLoader cl;

    private static class TagID {

	private String prefix;
	private String shortTagName;

	public TagID(String prefix, String shortTagName) {
	    this.prefix = prefix;
	    this.shortTagName = shortTagName;
	}

	public boolean equals(Object obj) {
	    return (prefix.equals(((TagID)obj).prefix)) &&
		(shortTagName.equals(((TagID)obj).shortTagName));
	}

	public int hashCode() {
	    return prefix.hashCode() + shortTagName.hashCode();
	}
    }
}

