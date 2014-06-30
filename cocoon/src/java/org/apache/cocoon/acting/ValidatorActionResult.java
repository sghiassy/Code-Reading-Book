/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;
import org.apache.cocoon.util.EnumerationFactory;

/**
 * A number of constants to represent the possible outcomes of a
 * validation.
 *
 * @author Christian Haul &lt;haul@informatik.tu-darmstadt.de&gt;
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $ 
 */

public class ValidatorActionResult extends EnumerationFactory {

	/**
	 * no error occurred, parameter successfully checked.
	 */
    public static final ValidatorActionResult
	OK         = new ValidatorActionResult ("OK");          // 0
    
	/**
	 * some error occurred, this is a result that is never set but
	 * serves as a comparison target. 
	 */
    public static final ValidatorActionResult
	ERROR      = new ValidatorActionResult ("ERROR");       // 1

	/**
	 * the parameter is null but isn't allowed to.
	 */
    public static final ValidatorActionResult
	ISNULL     = new ValidatorActionResult ("ISNULL");      // 2

	/**
	 * either value or length in case of a string is less than the
	 * specified minimum.
	 */
    public static final ValidatorActionResult
	TOOSMALL   = new ValidatorActionResult ("TOOSMALL");    // 3

	/**
	 * either value or length in case of a string is greater than
	 * the specified maximum.
	 */
    public static final ValidatorActionResult
	TOOLARGE   = new ValidatorActionResult ("TOOLARGE");    // 4

	/**
	 * a string parameter's value is not matched by the specified
	 * regular expression.
	 */
    public static final ValidatorActionResult
	NOMATCH    = new ValidatorActionResult ("NOMATCH");     // 5

	/**
	 * this is returned when the result of a validation is
	 * requested but no such result is found in the request
	 * attribute.
	 */
    public static final ValidatorActionResult
	NOTPRESENT = new ValidatorActionResult ("NOTPRESENT");  // 6

    /**
     * Make constructor private to inhibit creation outside.
     */
    private ValidatorActionResult (String image) {
	super (image);
    }
    private ValidatorActionResult () {
	super ();
    }
}
