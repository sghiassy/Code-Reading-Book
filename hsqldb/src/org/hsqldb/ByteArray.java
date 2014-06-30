/*
 * ByteArray.java
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

import java.io.*;
import java.sql.*;

/**
 * ByteArray class declaration
 * <P> This class allows HSQL to store binary data as an array of bytes.
 * It contains methods to create and access the data, perform comparisons, etc.
 *
 * @version 1.0.0.1
 */
class ByteArray {
    private byte data[];

    /**
     * ByteArray Constructor declaration
     * <P>Converts a string parameter to the array of bytes the ByteArray object
     * will contain.
     *
     * @param s
     */
    ByteArray(String s) {
	data = StringConverter.hexToByte(s);
    }

    /**
     * ByteArray Constructor declaration
     * <P>Creates a ByteArray object from an array of bytes.
     *
     * @param s
     */
    ByteArray(byte []a) {
	data = a;
    }

    /**
     * byteVake method declaration
     * <P>Give access to the object's data
     *
     * @return The array of bytes representing this objects data.
     */
    byte[] byteValue() {
	return data;
    }

    /**
     * compareTo method declaration
     * <P>This method compares the object to another ByteArray object.
     *
     * @param ByteArray object we are comparing against.
     *
     * @return 0 if objects are the same, non-zero otherwise.
     */
    int compareTo(ByteArray o) {
	int len = data.length;
	int lenb = o.data.length;

	for (int i = 0; ; i++) {
	    int a = 0, b = 0;

	    if (i < len) {
		a = ((int) data[i]) & 0xff;
	    } else if (i >= lenb) {
		return 0;
	    }

	    if (i < lenb) {
		b = ((int) o.data[i]) & 0xff;
	    }

	    if (a > b) {
		return 1;
	    }

	    if (b > a) {
		return -1;
	    }
	}
    }

    /**
     * serialize method declaration
     * <P>This method serializes an Object into an array of bytes.
     *
     * @param The Object to serialize
     *
     * @return a static byte array representing the passed Object
     *
     * @throws SQLException
     */
    static byte[] serialize(Object s) throws SQLException {
	ByteArrayOutputStream bo = new ByteArrayOutputStream();

	try {
	    ObjectOutputStream os = new ObjectOutputStream(bo);

	    os.writeObject(s);

	    return bo.toByteArray();
	} catch (Exception e) {
	    throw Trace.error(Trace.SERIALIZATION_FAILURE, e.getMessage());
	}
    }

    /**
     * serializeToString method declaration
     * <P>This method serializes an Object into a String.
     *
     * @param The Object to serialize
     *
     * @return A String representing the passed Object
     *
     * @throws SQLException
     */
    static String serializeToString(Object s) throws SQLException {
	return createString(serialize(s));
    }

    /**
     * deserialize method declaration
     * <P>This method returns the array of bytes stored in the instance of
     * ByteArray class as an Object instance.
     *
     * @return deserialized Object
     *
     * @throws SQLException
     */
    Object deserialize() throws SQLException {
	try {
	    ByteArrayInputStream bi = new ByteArrayInputStream(data);
	    ObjectInputStream    is = new ObjectInputStream(bi);

	    return is.readObject();
	} catch (Exception e) {
	    throw Trace.error(Trace.SERIALIZATION_FAILURE, e.getMessage());
	}
    }

    /**
     * createString method declaration
     * <P>This method creates a String from the passed array of bytes.
     *
     * @param byte array to convert.
     *
     * @return String representation of the byte array.
     */
    static String createString(byte b[]) {
	return StringConverter.byteToHex(b);
    }

    /**
     * toString method declaration
     * <P>This method creates a String from the passed array of bytes stored in
     * this instance of the ByteArray class.
     *
     * @return String representation of the ByteArray.
     */
    public String toString() {
	return createString(data);
    }

    /**
     * hashcode method declaration
     * <P>This method returns the hashcode for the data stored in this instance of
     * the ByteArray class.
     *
     * @return
     */
    public int hashCode() {
	return data.hashCode();
    }

}
