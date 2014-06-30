/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup.xsp;
 
import java.io.BufferedInputStream;
import java.sql.ResultSet;
import java.io.InputStream;
import java.sql.Clob;

/**
 * This is a helper class to remove redundant code in
 * esql pages
 *
 * @author <A HREF="mailto:tcurdt@dff.st">Torsten Curdt</A>
 * based on the orginal esql.xsl
 */
 
public class EsqlHelper {

      public final static String getAscii(ResultSet set, String column) {
        InputStream asciiStream = null;
        byte[] buffer = null;
 
        try {
            Clob dbClob = set.getClob(column);
            int length = (int) dbClob.length();
            asciiStream = new BufferedInputStream(dbClob.getAsciiStream());
            buffer = new byte[length];
            asciiStream.read(buffer);
            asciiStream.close();
        } catch (Exception e) {
            throw new RuntimeException("Error getting clob data: " + e.getMessage());
        } finally {
            if (asciiStream != null) try {asciiStream.close();} catch (Exception ase) {
                throw new RuntimeException("Error closing clob stream: " + ase.getMessage());
            }
        }
 
        if (buffer == null) return "";
 
        return new String(buffer);
      }                                                                                                                      

      public final static String getAscii(ResultSet set, int column) {
        InputStream asciiStream = null;
        byte[] buffer = null;
 
        try {
            Clob dbClob = set.getClob(column);
            int length = (int) dbClob.length();
            asciiStream = new BufferedInputStream(dbClob.getAsciiStream());
            buffer = new byte[length];
            asciiStream.read(buffer);
            asciiStream.close();
        } catch (Exception e) {
            throw new RuntimeException("Error getting clob data: " + e.getMessage());
        } finally {
            if (asciiStream != null) try {asciiStream.close();} catch (Exception ase) {
                throw new RuntimeException("Error closing clob stream: " + ase.getMessage());
            }
        }
 
        if (buffer == null) return "";
 
        return new String(buffer);
      }                                                                                                                      

      public final static String getStringFromByteArray(byte[] bytes, String encoding) {
        try {
            return new String(bytes,encoding);
        } catch (java.io.UnsupportedEncodingException uee) {
            throw new RuntimeException("Unsupported Encoding Exception: " + uee.getMessage());
        }
      }

}
