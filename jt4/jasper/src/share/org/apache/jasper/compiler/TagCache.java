/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/TagCache.java,v 1.2 2000/09/26 00:46:17 pierred Exp $
 * $Revision: 1.2 $
 * $Date: 2000/09/26 00:46:17 $
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

import java.util.Hashtable;

import java.lang.reflect.Method;

import java.beans.BeanInfo;
import java.beans.Introspector;
import java.beans.PropertyDescriptor;
import java.beans.PropertyEditor;
import java.beans.IntrospectionException;

import org.apache.jasper.JasperException;
import org.apache.jasper.Constants;

/**
 * A simple cache to hold results of one-time evaluation for a custom 
 * tag. 
 *
 * @author Anil K. Vijendran (akv@eng.sun.com)
 * @author Pierre Delisle
 */
public class TagCache {
    String shortTagName;
    Hashtable methodMaps;
    Hashtable propertyEditorMaps;
    BeanInfo tagClassInfo;
    Class tagHandlerClass;
    
    TagCache(String shortTagName) {
        this.shortTagName = shortTagName;
        this.methodMaps = new Hashtable();
        this.propertyEditorMaps = new Hashtable();
    }

    private void addSetterMethod(String attrName, Method m) {
        methodMaps.put(attrName, m);
    }
    
    Method getSetterMethod(String attrName) {
        return (Method) methodMaps.get(attrName);
    }

    private void addPropertyEditor(String attrName, Class peClass) {
        propertyEditorMaps.put(attrName, peClass);
    }
    
    Class getPropertyEditorClass(String attrName) {
        return (Class) propertyEditorMaps.get(attrName);
    }

    void setTagHandlerClass(Class tagHandlerClass) 
        throws JasperException
    {
        try {
            this.tagClassInfo = Introspector.getBeanInfo(tagHandlerClass);
            this.tagHandlerClass = tagHandlerClass;
            PropertyDescriptor[] pd = tagClassInfo.getPropertyDescriptors();
            for(int i = 0; i < pd.length; i++) {
                /* FIXME: why is the check for null below needed?? -akv */
                /* 
                   FIXME: should probably be checking for things like
                          pageContext, bodyContent, and parent here -akv
                */
                if (pd[i].getWriteMethod() != null)
                    addSetterMethod(pd[i].getName(), pd[i].getWriteMethod());
                if (pd[i].getPropertyEditorClass() != null)
                    addPropertyEditor(pd[i].getName(), pd[i].getPropertyEditorClass());
            }
        } catch (IntrospectionException ex) {
            throw new JasperException(Constants.getString("jsp.error.unable.to_introspect",
                                                          new Object[] {
                                                              tagHandlerClass.getName(),
                                                              ex.getMessage()
                                                          }
                                                          ));
        }
    }

    Class getTagHandlerClass() {
        return tagHandlerClass;
    }
    
    BeanInfo getTagClassInfo() {
        return tagClassInfo;
    }
}

    
