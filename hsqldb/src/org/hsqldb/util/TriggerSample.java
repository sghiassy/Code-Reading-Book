/**
 * TriggerSample.java
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

// use the hsql package here; but will be in your own package usually
package org.hsqldb.util;

/**
 * TriggerSample class declaration
 * <P>Sample code for use of triggers in hsql.
 *
 * SQL to invoke is:
 * CREATE TRIGGER triggerSample BEFORE|AFTER INSERT|UPDATE|DELETE \
 * ON myTable [FOR EACH ROW] CALL "myPackage.trigClass"
 *
 * This will create a thread that will wait for its firing event to occur;
 * when this happens, the trigger's thread runs the 'trigClass.fire'
 * Note that this is still in the same Java Virtual Machine as the
 * database, so make sure the fired method does not hang.
 *
 * Ensure that "myPackage.trigClass" is present in the classpath which
 * you use to start hsql.
 *
 * If the method wants to access the database, it must establish
 * a JDBC connection.
 *
 * When the 'fire' method is called, it is passed the following arguments:
 * fire (String trigName, String tabName, Object row[])
 *
 * where 'row' represents the row acted on, with each column being
 * a member of the array. The mapping of row classes to database
 * types is specified in doc/internet/hSqlSyntax.html#Datatypes
 *
 * For implementation at a later date:
 * 1.	jdbc:default:connection: URL for JDBC trigger method connections to
 * the database.
 * 2.	arguments to the trigger method.
 * 3.	Because they run in different threads, it is possible for an
 * 'after' trigger to run before its corresponding 'before' trigger;
 * the acceptability of this needs to be investigated.
 *
 * @author Peter Hudson
 * @version 1.0.0.1
 */
public class TriggerSample implements org.hsqldb.Trigger {

    /**
     * fire method declaration
     * <P> This is a sample implementation that simply prints information
     * about the trigger firing.
     *
     * @param trigName
     * @param tabName
     * @param row
     */
    public void fire(String trigName, String tabName, Object row[]) {
	System.out.println(trigName + " trigger fired on " + tabName);
	System.out.print("col 0 value <");
	System.out.print(row[0]);
	System.out.println(">");

	// you can cast row[i] given your knowledge of what the table
	// format is.
    }

}

/**
 * **************************************************************************
 *
 * test SQL
 * CREATE CACHED TABLE trig_test (int_field	integer)
 * CREATE TRIGGER ins_before BEFORE INSERT ON trig_test CALL "org.hsqldb.util.TriggerSample"
 * CREATE TRIGGER ins_after  AFTER  INSERT ON trig_test CALL "org.hsqldb.util.TriggerSample"
 * CREATE TRIGGER upd_before BEFORE UPDATE ON trig_test CALL "org.hsqldb.util.TriggerSample"
 * CREATE TRIGGER upd_after  AFTER  UPDATE ON trig_test CALL "org.hsqldb.util.TriggerSample"
 * CREATE TRIGGER upd_before_row BEFORE UPDATE ON trig_test FOR EACH ROW CALL "org.hsqldb.util.TriggerSample"
 * CREATE TRIGGER upd_after_row  AFTER  UPDATE ON trig_test FOR EACH ROW CALL "org.hsqldb.util.TriggerSample"
 * CREATE TRIGGER del_before BEFORE DELETE ON trig_test CALL "org.hsqldb.util.TriggerSample"
 * CREATE TRIGGER del_after  AFTER  DELETE ON trig_test CALL "org.hsqldb.util.TriggerSample"
 * CREATE TRIGGER del_before_row BEFORE DELETE ON trig_test FOR EACH ROW CALL "org.hsqldb.util.TriggerSample"
 * CREATE TRIGGER del_after_row  AFTER  DELETE ON trig_test FOR EACH ROW CALL "org.hsqldb.util.TriggerSample"
 * INSERT INTO trig_test VALUES (1)
 * INSERT INTO trig_test VALUES (2)
 * INSERT INTO trig_test VALUES (3)
 * UPDATE trig_test SET int_field = int_field + 3
 * DELETE FROM trig_test
 */
