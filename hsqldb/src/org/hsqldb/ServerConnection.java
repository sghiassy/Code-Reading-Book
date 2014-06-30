/*
 * ServerConnection.java
 *
 * Copyright (c) 2001, The HSQL Development Group
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 *
 * Neither the name of the HSQL Development Group nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This package is based on HypersonicSQL, originally developed by Thomas Mueller.
 *
 */
package org.hsqldb;

import java.sql.*;
import java.net.*;
import java.io.*;
import java.util.*;

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
class ServerConnection extends Thread {
    private Database	     mDatabase;
    private Socket	     mSocket;
    private Server	     mServer;
    private DataInputStream  mInput;
    private DataOutputStream mOutput;
    private static int       mCurrentThread = 0;
    private int		     mThread;

    /**
     * Constructor declaration
     *
     *
     * @param socket
     * @param server
     */
    ServerConnection(Socket socket, Server server) {
	mSocket = socket;
	mDatabase = server.mDatabase;
	mServer = server;

	synchronized (this) {
	    mThread = mCurrentThread++;
	}
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    private Channel init() {
	try {
	    mSocket.setTcpNoDelay(true);

	    mInput =
		new DataInputStream(new BufferedInputStream(mSocket.getInputStream()));
	    mOutput =
		new DataOutputStream(new BufferedOutputStream(mSocket.getOutputStream()));

	    String  user = mInput.readUTF();
	    String  password = mInput.readUTF();
	    Channel c;

	    try {
		mServer.trace(mThread + ":trying to connect user " + user);

		return mDatabase.connect(user, password);
	    } catch (SQLException e) {
		write(new Result(e.getMessage()).getBytes());
	    }
	} catch (Exception e) {}

	return null;
    }

    /**
     * Method declaration
     *
     */
    public void run() {
	Channel c = init();

	if (c != null) {
	    try {
		while (true) {
		    String sql = mInput.readUTF();

		    mServer.trace(mThread + ":" + sql);

		    if (sql == null) {
			break;
		    }

		    write(mDatabase.execute(sql, c).getBytes());
		}
	    } catch (Exception e) {}
	}

	try {
	    mSocket.close();
	} catch (IOException e) {}

	if (mDatabase.isShutdown()) {
	    System.out.println("The database is shutdown");
	    System.exit(0);
	}
    }

    /**
     * Method declaration
     *
     *
     * @param b
     *
     * @throws IOException
     */
    void write(byte b[]) throws IOException {
	mOutput.writeInt(b.length);
	mOutput.write(b);
	mOutput.flush();
    }

}
