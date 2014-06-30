/* ========================================================================= *
 *                                                                           *
 *                 The Apache Software License,  Version 1.1                 *
 *                                                                           *
 *         Copyright (c) 1999, 2000  The Apache Software Foundation.         *
 *                           All rights reserved.                            *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * Redistribution and use in source and binary forms,  with or without modi- *
 * fication, are permitted provided that the following conditions are met:   *
 *                                                                           *
 * 1. Redistributions of source code  must retain the above copyright notice *
 *    notice, this list of conditions and the following disclaimer.          *
 *                                                                           *
 * 2. Redistributions  in binary  form  must  reproduce the  above copyright *
 *    notice,  this list of conditions  and the following  disclaimer in the *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. The end-user documentation  included with the redistribution,  if any, *
 *    must include the following acknowlegement:                             *
 *                                                                           *
 *       "This product includes  software developed  by the Apache  Software *
 *        Foundation <http://www.apache.org/>."                              *
 *                                                                           *
 *    Alternately, this acknowlegement may appear in the software itself, if *
 *    and wherever such third-party acknowlegements normally appear.         *
 *                                                                           *
 * 4. The names  "The  Jakarta  Project",  "Tomcat",  and  "Apache  Software *
 *    Foundation"  must not be used  to endorse or promote  products derived *
 *    from this  software without  prior  written  permission.  For  written *
 *    permission, please contact <apache@apache.org>.                        *
 *                                                                           *
 * 5. Products derived from this software may not be called "Apache" nor may *
 *    "Apache" appear in their names without prior written permission of the *
 *    Apache Software Foundation.                                            *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES *
 * INCLUDING, BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY *
 * AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.  IN NO EVENT SHALL *
 * THE APACHE  SOFTWARE  FOUNDATION OR  ITS CONTRIBUTORS  BE LIABLE  FOR ANY *
 * DIRECT,  INDIRECT,   INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL *
 * DAMAGES (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS *
 * OR SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) *
 * HOWEVER CAUSED AND  ON ANY  THEORY  OF  LIABILITY,  WHETHER IN  CONTRACT, *
 * STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN *
 * ANY  WAY  OUT OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF  ADVISED  OF THE *
 * POSSIBILITY OF SUCH DAMAGE.                                               *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * This software  consists of voluntary  contributions made  by many indivi- *
 * duals on behalf of the  Apache Software Foundation.  For more information *
 * on the Apache Software Foundation, please see <http://www.apache.org/>.   *
 *                                                                           *
 * ========================================================================= */

package org.apache.tester;

import java.beans.PropertyEditorSupport;
import java.text.ParsePosition;
import java.text.SimpleDateFormat;
import java.sql.Date;

/**
 * PropertyEditor implementation for a java.sql.Date property.
 *
 * @author Craig R. McClanahan
 * @revision $Date: 2001/07/25 04:27:11 $ $Revision: 1.1 $
 */
public class DatePropertyEditor extends PropertyEditorSupport {


    // ----------------------------------------------------- Instance Variables


    /**
     * The date format to which dates converted by this property editor
     * must conform.
     */
    private SimpleDateFormat format = new SimpleDateFormat("MM/dd/yyyy");


    // --------------------------------------------------------- Public Methods


    /**
     * Convert our Date object into a String that conforms to our
     * specified <code>format</code>, and return it.  If this is not
     * possible, return <code>null</code>.
     */
    public String getAsText() {

        try {
            Date date = (Date) getValue();
            return (format.format(date));
        } catch (ClassCastException e) {
            return (null);
        } catch (IllegalArgumentException e) {
            return (null);
        }

    }


    /**
     * Convert the specified String value into a Date, if it conforms to
     * our specified <code>format</code> , else throw IllegalArgumentException.
     *
     * @param value String value to be converted
     *
     * @exception IllegalArgumentException if a conversion error occurs
     */
    public void setAsText(String value) throws IllegalArgumentException {

        // Validate the format of the input string
        if (value == null)
            throw new IllegalArgumentException
                ("Cannot convert null String to a Date");
        if (value.length() != 10)
            throw new IllegalArgumentException
                ("String '" + value + "' has invalid length " +
                 value.length());
        for (int i = 0; i < 10; i++) {
            char ch = value.charAt(i);
            if ((i == 2) || (i == 5)) {
                if (ch != '/')
                    throw new IllegalArgumentException
                        ("String '" + value + "' missing slash at index " +
                         i);
            } else {
                if (!Character.isDigit(ch))
                    throw new IllegalArgumentException
                        ("String '" + value + "' missing digit at index " +
                         i);
            }
        }

        // Convert the incoming value to a java.sql.Date
        java.util.Date temp = format.parse(value, new ParsePosition(0)); 
        java.sql.Date date = new java.sql.Date(temp.getTime());
        setValue(date);
    }

        
}
