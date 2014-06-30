/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/naming/factory/TyrexDataSourceFactory.java,v 1.3 2001/07/12 08:07:27 remm Exp $
 * $Revision: 1.3 $
 * $Date: 2001/07/12 08:07:27 $
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


package org.apache.naming.factory;

import java.util.Hashtable;
import java.sql.Driver;
import java.sql.DriverManager;
import javax.naming.Name;
import javax.naming.Context;
import javax.naming.NamingException;
import javax.naming.Reference;
import javax.naming.RefAddr;
import javax.naming.spi.ObjectFactory;
import org.apache.naming.ResourceRef;
import tyrex.jdbc.ServerDataSource;
import tyrex.jdbc.xa.EnabledDataSource;

/**
 * Object factory for Tyrex DataSources.<br>
 * Tyrex is an open-source transaction manager, developed by Assaf Arkin and
 * exolab.org. See the <a href="http://tyrex.exolab.org/">Tyrex homepage</a>
 * for more details about Tyrex and downloads.
 * <p>
 * This factory can produced either ServerDataSource objects (with integrated
 * connection pooling) or EnabledDataSource objects. If the requested type is
 * "tyrex.jdbc.ServerDataSource", a ServerDataSource will be instantiated.
 * Be aware that some specific runtime permissions have to be set to be able
 * to generate a ServerDataSource object (see the Tyrex documentation at the 
 * Tyrex website for more information).
 * <p>
 * Definition of the following additional properties is recommended :
 * <ul>
 * <li>driverName : Name of the JDBC driver to use ( = connection URL)</li>
 * <li>driverClassName : Class name of the JDBC driver</li>
 * <li>user : User name. Can also be specified later when the Connection
 * is retrieved.</li>
 * <li>password : Password. Can also be specified later when the Connection
 * is retrieved.</li>
 * <li>loginTimeout : Optional. Login timeout.</li>
 * </ul>
 * 
 * @author Remy Maucherat
 * @version $Revision: 1.3 $ $Date: 2001/07/12 08:07:27 $
 */

public class TyrexDataSourceFactory
    implements ObjectFactory {


    // ----------------------------------------------------------- Constructors


    // -------------------------------------------------------------- Constants


    // ServerDataSource address names
    public static final String LOGIN_TIMEOUT = "loginTimeout";
    public static final String DESCRIPTION = "description";
    public static final String DATA_SOURCE = "dataSource";


    // XADataSourceImpl
    public static final String USER = "user";
    public static final String PASSWORD = "password";
    public static final String DRIVER_NAME = "driverName";
    public static final String DRIVER_CLASS_NAME = "driverClassName";

    // Default values
    public static final String DEFAULT_DRIVER_NAME = "jdbc:HypersonicSQL:.";
    public static final String DEFAULT_DRIVER_CLASS_NAME = 
        "org.hsql.jdbcDriver";
    public static final String DEFAULT_USER = "sa";
    public static final String DEFAULT_PASSWORD = "";


    // ----------------------------------------------------- Instance Variables


    // --------------------------------------------------------- Public Methods


    // -------------------------------------------------- ObjectFactory Methods


    /**
     * Crete a new DataSource instance.
     * 
     * @param obj The reference object describing the DataSource
     */
    public Object getObjectInstance(Object obj, Name name, Context nameCtx,
                                    Hashtable environment)
        throws NamingException {
        
        if (obj instanceof ResourceRef) {
            Reference ref = (Reference) obj;
            if ((ref.getClassName().equals("javax.sql.DataSource")) ||
                (ref.getClassName().equals("tyrex.jdbc.ServerDataSource")) || 
                (ref.getClassName().equals
                 ("tyrex.jdbc.xa.EnabledDataSource"))) {
                
                try {
                    
                    // Getting a new EnabledDataSource instance
                    EnabledDataSource ds = new EnabledDataSource();
                    
                    // Retrieving parameters
                    RefAddr currentRefAddr = null;
                    currentRefAddr = ref.get(DESCRIPTION);
                    if (currentRefAddr != null)
                        ds.setDescription
                            (currentRefAddr.getContent().toString());
                    currentRefAddr = ref.get(LOGIN_TIMEOUT);
                    if (currentRefAddr != null) {
                        ds.setLoginTimeout
                            (Integer.parseInt
                             (currentRefAddr.getContent().toString()));
                    }
                    currentRefAddr = ref.get(USER);
                    if (currentRefAddr != null) {
                        ds.setUser(currentRefAddr.getContent().toString());
                    } else {
                        ds.setUser(DEFAULT_USER);
                    }
                    currentRefAddr = ref.get(PASSWORD);
                    if (currentRefAddr != null) {
                        ds.setPassword(currentRefAddr.getContent().toString());
                    } else {
                        ds.setPassword(DEFAULT_PASSWORD);
                    }
                    currentRefAddr = ref.get(DRIVER_NAME);
                    if (currentRefAddr != null) {
                        ds.setDriverName
                            (currentRefAddr.getContent().toString());
                    } else {
                        ds.setDriverName(DEFAULT_DRIVER_NAME);
                    }
                    currentRefAddr = ref.get(DRIVER_CLASS_NAME);
                    String driverClassName = null;
                    if (currentRefAddr != null) {
                        driverClassName = 
                            currentRefAddr.getContent().toString();
                    } else {
                        driverClassName = DEFAULT_DRIVER_CLASS_NAME;
                    }
                    ds.setDriverClassName(driverClassName);
                    
                    // Loading and registering JDBC driver
                    Class driverClass = Class.forName(driverClassName);
                    Driver databaseDriver = (Driver) driverClass.newInstance();
                    DriverManager.registerDriver(databaseDriver);
                    
                    if (ref.getClassName().equals
                        ("tyrex.jdbc.ServerDataSource")) {
                        
                        ServerDataSource sds = 
                            new ServerDataSource((javax.sql.XADataSource) ds);
                        
                        currentRefAddr = ref.get(DESCRIPTION);
                        if (currentRefAddr != null)
                            sds.setDescription
                                (currentRefAddr.getContent().toString());
                        
                        return sds;
                        
                    }
                    
                    return ds;
                    
                } catch (Throwable t) {
                    // Another factory could handle this, so just give up
                    return null;
                }
                
            } else {
                return null;
            }
            
        } else {
            return null;
        }
        
    }


}

