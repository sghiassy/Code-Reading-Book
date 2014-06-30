/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.hsqldb;

import org.apache.avalon.framework.activity.Startable;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.sql.SQLException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.Statement;
import java.util.Enumeration;

/**
 * This class runs an instance of HSQLDB Server.
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 */
public class ServerImpl
extends AbstractLoggable
implements Server,
           Parameterizable,
           Contextualizable,
           ThreadSafe,
           Runnable,
           Startable {

    /** Port which HSQLDB server will listen to */
    private String port;

    /** Arguments for running the server */
    private String arguments[] = new String[8];

    /** Check if the server has already been started */
    private boolean started = false;

    /**
     * Initialize the ServerImpl.
     * A few options can be used :
     * <UL>
     *  <LI>port = port where the server is listening</LI>
     *  <LI>silent = display all queries</LI>
     *  <LI>trace = print JDBC trace messages</LI>
     * </UL>
     */
    public void parameterize(Parameters params)  {
        this.getLogger().debug("Parameterize ServerImpl");

        arguments[0] = "-port";
        arguments[1] = this.port = params.getParameter("port", "9002");
        arguments[2] = "-silent";
        arguments[3] = params.getParameter("silent","true");
        arguments[4] = "-trace";
        arguments[5] = params.getParameter("trace","false");
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Configure ServerImpl with port: " + arguments[1]
                    + ", silent: " + arguments[3]
                    + ", trace: " +arguments[5]);
        }
    }

    /** Contextualize this class */
    public void contextualize(Context context) throws ContextException {
        org.apache.cocoon.environment.Context ctx =
                (org.apache.cocoon.environment.Context) context.get(Constants.CONTEXT_ENVIRONMENT_CONTEXT);
        try {
            arguments[6] = "-database";
            arguments[7] = new File(ctx.getRealPath("/WEB-INF/db")).getCanonicalPath();
            arguments[7] += File.separator + "cocoondb";
            getLogger().debug("database is " + arguments[7]);
        } catch (MalformedURLException e) {
            getLogger().error("MalformedURLException - Could not get database directory ", e);
        } catch (IOException e) {
            getLogger().error("IOException - Could not get database directory ", e);
        }
    }

    /** Start the server */
    public void start() {
        if (!started) {
            // FIXME (VG): This dirty hack here is till shutdown issue is resolved
            File file = new File(arguments[7] + ".backup");
            if (file.exists() && file.delete()) {
                getLogger().info("HSQLDB backup file has been deleted.");
            }
        
            Thread server = new Thread(this);
            this.getLogger().debug("Intializing hsqldb server thread");
            server.setPriority(Thread.currentThread().getPriority());
            server.setDaemon(true);
            server.setName("hsqldb server");
            server.start();
        }
    }

    /** Stop the server */
    public void stop() {
        if (started) {
            // TODO: Execute shutdown. Now HSQLDB have a System.exit() call
/*
            try {
                System.out.println("Shutting down HSQLDB");
                getLogger().debug("Shutting down HSQLDB");
                Connection connection = DriverManager.getConnection("jdbc:hsqldb:hsql://localhost:" + this.port, "sa", "");
                Statement statement = connection.createStatement();
                statement.executeQuery("SHUTDOWN");
                try {
                    connection.close();
                } catch (SQLException e) {
                    System.out.println("Shutting down HSQLDB: Ignoring exception: " + e);
                }
            } catch (Exception e){
                System.out.println("Shutting down HSQLDB: Exception.");
                e.printStackTrace();
            }
            System.out.println("Shutting down HSQLDB: Done");
*/
        }
    }

    public void run() {
        if(!started) {
            started = true;

            try {
                getLogger().debug("HSQLDB Server arguments are as follows:");
                for(int i=0;i<8;i++) {
                    getLogger().debug(i + " : " + arguments[i]);
                }

                org.hsqldb.Server.main(arguments);
            } catch(Exception e){
                System.out.println("run got exception: ");
                e.printStackTrace();
            } finally {
                started = false;
            }
        }
    }
}
