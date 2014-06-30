/**
 * Trigger.java
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

/**
 * TriggerDef class declaration
 *
 * Definition and execution of triggers
 * Development of the trigger implementation sponsored by Logicscope Realisations Ltd
 *
 * @author Logicscope Realisations Ltd
 * @version 1.0.0.1
 */
class TriggerDef extends Thread {

    /**
     * member variables
     */
    static final int NUM_TRIGGER_OPS = 3;    // ie ins,del,upd
    static final int NUM_TRIGS = NUM_TRIGGER_OPS * 2 * 2;

    // indexes into the triggers Vector
    static final int  INSERT_AFTER = 0;
    static final int  DELETE_AFTER = 1;
    static final int  UPDATE_AFTER = 2;
    static final int  INSERT_BEFORE = INSERT_AFTER + NUM_TRIGGER_OPS;
    static final int  DELETE_BEFORE = DELETE_AFTER + NUM_TRIGGER_OPS;
    static final int  UPDATE_BEFORE = UPDATE_AFTER + NUM_TRIGGER_OPS;
    static final int  INSERT_AFTER_ROW = INSERT_AFTER + 2 * NUM_TRIGGER_OPS;
    static final int  DELETE_AFTER_ROW = DELETE_AFTER + 2 * NUM_TRIGGER_OPS;
    static final int  UPDATE_AFTER_ROW = UPDATE_AFTER + 2 * NUM_TRIGGER_OPS;
    static final int  INSERT_BEFORE_ROW = INSERT_BEFORE + 2 * NUM_TRIGGER_OPS;
    static final int  DELETE_BEFORE_ROW = DELETE_BEFORE + 2 * NUM_TRIGGER_OPS;
    static final int  UPDATE_BEFORE_ROW = UPDATE_BEFORE + 2 * NUM_TRIGGER_OPS;
    String	      name;
    String	      when;
    String	      operation;
    boolean	      forEachRow;
    Table	      table;
    Trigger	      trig;
    String	      fire;
    int		      vectorIndx;    // index into Vector[]
    Object	      rowForTrigger[];    // for firing thread to fire to pass to JSP
    protected boolean busy;	     // firing trigger in progress
    protected boolean valid;    // parsing valid

    /**
     * Constructor declaration
     *
     * create an object from the components of an SQL CREATE TRIGGER statement
     *
     * @param sName
     * @param sWhen
     * @param sOper
     * @param bForEach
     * @param pTab
     * @param pTrig
     * @param sFire
     */
    public TriggerDef(String sName, String sWhen, String sOper,
		      boolean bForEach, Table pTab, Trigger pTrig,
		      String sFire) {
	name = sName.toUpperCase();
	when = sWhen.toUpperCase();
	operation = sOper.toUpperCase();
	forEachRow = bForEach;
	table = pTab;
	trig = pTrig;
	fire = sFire;
	vectorIndx = SqlToIndex();
	busy = false;

	if (vectorIndx < 0) {
	    valid = false;
	} else {
	    valid = true;
	}
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    public StringBuffer toBuf() {
	StringBuffer a = new StringBuffer("CREATE TRIGGER ");

	a.append(name);
	a.append(" ");
	a.append(when);
	a.append(" ");
	a.append(operation);
	a.append(" ON ");
	a.append(table.getName());

	if (forEachRow) {
	    a.append(" FOR EACH ROW ");
	}

	a.append(" CALL ");
	a.append(fire);

	return a;
    }

    /**
     * SqlToIndex method declaration
     * <P>Given the SQL creating the trigger, say what the index to the Vector[] is
     *
     *
     * @return index to the Vector[]
     */
    public int SqlToIndex() {
	int indx;

	if (operation.equals("INSERT")) {
	    indx = INSERT_AFTER;
	} else if (operation.equals("DELETE")) {
	    indx = DELETE_AFTER;
	} else if (operation.equals("UPDATE")) {
	    indx = UPDATE_AFTER;
	} else {
	    indx = -1;
	}

	if (when.equals("BEFORE")) {
	    indx += NUM_TRIGGER_OPS;    // number of operations
	} else if (!when.equals("AFTER")) {
	    indx = -1;
	}

	if (forEachRow) {
	    indx += 2 * NUM_TRIGGER_OPS;
	}

	return indx;
    }

    /**
     * run method declaration
     * <P>the trigger JSP is run in its own thread here. Its job
     * is simply to wait until it is told by the main thread that
     * it should fire the trigger.
     *
     */
    public void run() {
	boolean keepGoing = true;

	while (keepGoing) {
	    pop();
	}
    }

    /**
     * pop method declaration
     * <P>The consumer (trigger) thread waits for an event to be queued
     * <P><B>Note: </B>This push/pop pairing assumes a single produced thread
     * and a single consumer thread _only_.
     */
    public synchronized void pop() {
	if (!busy) {
	    try {
		wait();    // this releases the lock monitor
	    } catch (InterruptedException e) { /* ignore and resume */
	    }
	}

	trig.fire(name, table.getName(), rowForTrigger);

	busy = false;

	notify();    // notify push's wait
    }

    /**
     * push method declaration
     * <P>The main thread tells the trigger thread to fire by this call
     *
     * @param row[]
     */
    public synchronized void push(Object row[]) {
	if (busy) {
	    try {
		wait();
	    } catch (InterruptedException e) { /* ignore and resume */
	    }
	}

	busy = true;
	rowForTrigger = row;

	notify();    // notify pop's wait
    }


    /**
     * Method declaration
     *
     *
     * @return
     */
    static public int numTrigs() {
	return NUM_TRIGS;
    }


    /**
     * Method declaration
     *
     *
     * @return
     */
    public boolean isBusy() {
	return busy;
    }


    /**
     * Method declaration
     *
     *
     * @return
     */
    public boolean isValid() {
	return valid;
    }

}
