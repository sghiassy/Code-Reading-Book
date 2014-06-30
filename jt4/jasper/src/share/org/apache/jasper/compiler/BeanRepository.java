/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/jasper/src/share/org/apache/jasper/compiler/BeanRepository.java,v 1.2 2001/06/19 04:17:42 remm Exp $
 * $Revision: 1.2 $
 * $Date: 2001/06/19 04:17:42 $
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


import java.util.Vector;
import java.util.Enumeration;
import java.util.Hashtable;

import java.beans.Beans;
import java.beans.BeanInfo;
import java.beans.PropertyDescriptor;

import javax.servlet.ServletException;
import org.apache.jasper.JasperException;

/**
 * Holds instances of {session, application, page}-scoped beans 
 *
 * @author Mandar Raje
 */
public class BeanRepository {

    Vector sessionBeans;
    Vector pageBeans;
    Vector appBeans;
    Vector requestBeans;
    Hashtable beanTypes;
    
    public BeanRepository (ClassLoader loader) {
	sessionBeans = new Vector(11);
	pageBeans    = new Vector(11);
	appBeans = new Vector(11);
	requestBeans    = new Vector(11);
	beanTypes    = new Hashtable ();
    }
    
    public boolean checkSessionBean (String s) {
	return sessionBeans.contains (s);
    }
    
    public void addSessionBean (String s, String type) throws JasperException {
	sessionBeans.addElement (s);
	putBeanType (s, type);
    }
    
    public boolean hasSessionBeans () {
	return !sessionBeans.isEmpty ();
    }
    
    public Enumeration getSessionBeans () {
	return sessionBeans.elements ();
    }
    
    public boolean checkApplicationBean (String s) {
	return appBeans.contains (s);
    }
    
    public void addApplicationBean (String s, String type) throws JasperException 
    {
	appBeans.addElement (s);
	putBeanType (s, type);
    }
    
    public boolean hasApplicationBeans () {
	return !appBeans.isEmpty ();
    }
    
    public Enumeration getApplicationBeans () {
	return appBeans.elements ();
    }
    
    public boolean checkRequestBean (String s) {
	return requestBeans.contains (s);
    }
    
    public void addRequestBean (String s, String type) throws JasperException {
	requestBeans.addElement (s);
	putBeanType (s, type);
    }
    
    public boolean hasRequestBeans () {
	return !requestBeans.isEmpty ();
    }
    
    public Enumeration getRequestBeans () {
	return requestBeans.elements ();
    }
    
    public boolean checkPageBean (String s) {
	return pageBeans.contains (s);
    }
    
    public void addPageBean (String s, String type) throws JasperException {
	pageBeans.addElement (s);
	putBeanType (s, type);
    }
    
    public boolean hasPageBeans () {
	return !pageBeans.isEmpty ();
    }
    
    public Enumeration getPageBeans () {
	return pageBeans.elements ();
    }

    public boolean ClassFound (String clsname)
    throws ClassNotFoundException {
	Class cls = null;
	//try {
	    cls = Thread.currentThread().getContextClassLoader().loadClass
                (clsname) ;
	    //} catch (ClassNotFoundException ex) {
	    //return false;
	    //}
	return !(cls == null);	
    }
    
    public Class getBeanType (String bean) throws JasperException {
	Class cls = null;
	try {
	    cls = Thread.currentThread().getContextClassLoader().loadClass
                ((String)beanTypes.get(bean)) ;
	} catch (ClassNotFoundException ex) {
	    throw new JasperException (ex);
	}
	return cls;
    }
  
    public void putBeanType (String bean, String type) throws JasperException {
	try {
	    beanTypes.put (bean, type);
	} catch (Exception ex) {
	    throw new JasperException (ex);
	}
    }
    
    //public void putBeanType (String bean, Class type) {
    //beanTypes.put (bean, type);
    //}
    
    public void removeBeanType (String bean) {
	beanTypes.remove (bean);
    }
    
    // Not sure if this is the correct way.
    // After pageContext is finalised this will change.
    public boolean checkVariable (String bean) {
	return (checkPageBean(bean) || checkSessionBean(bean) ||
		checkRequestBean(bean) || checkApplicationBean(bean));
    }

    // Ideally this method should belong to the utils.
    public Class getClass (String clsname)
	throws ClassNotFoundException {
	    Class cls = null;
	    if (clsname != null) {
		cls = Thread.currentThread().getContextClassLoader().loadClass (clsname);
	    }
	    return cls;
    }

    public boolean beanFound (String beanName)
	throws ClassNotFoundException {
	    try {
		Beans.instantiate(Thread.currentThread().getContextClassLoader(), beanName);
		return true;
	    } catch (java.io.IOException ex) {
		// Ignore it for the time being.
		return false;
	    }
    }
}




