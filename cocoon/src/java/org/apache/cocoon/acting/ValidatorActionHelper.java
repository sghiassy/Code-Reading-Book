package org.apache.cocoon.acting;



/**
 * Helper class to pass a the result of a validation back along with
 * the validated object itself.
 *
 * @author Christian Haul &lt;haul@informatik.tu-darmstadt.de&gt;
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:07 $
 */

public class ValidatorActionHelper
{
    protected ValidatorActionResult result = ValidatorActionResult.OK;
    protected Object object = null;

    /**
     * Create a ValidatorActionHelper object that contains just the
     * object. Defaults to <code>OK</code> as validation result.
     *
     * @param validatedObject object that has been validated
     */

    public ValidatorActionHelper ( Object validatedObject ) {
	this.object = validatedObject;
	this.result = ValidatorActionResult.OK;
    }

    /**
     * Create a ValidatorActionHelper object that contains just the
     * object. Defaults to <code>OK</code> as validation result.
     *
     * @param validatedObject object that has been validated
     * @param validationResult result of the validation
     */
    public ValidatorActionHelper ( Object validatedObject, ValidatorActionResult validationResult ) {
	this.object = validatedObject;
	this.result = validationResult;
    }

    /**
     * Tests if the validation result is <code>OK</code>
     *
     */
    public boolean isOK() {
	return (result.equals(ValidatorActionResult.OK));
    }

    /**
     * Tests if the validation result is <code>ISNULL</code>,
     * e.g. when the value is null but is not supposed to be null.
     *  
     */
    public boolean isNull() {
	return (result.equals(ValidatorActionResult.ISNULL));
    }

    /**
     * Tests if the validation result is <code>TOOLARGE</code>,
     * e.g. in case of a double or long the value is too large or in
     * case of a string it is too long.
     *
     */
    public boolean isTooLarge() {
	return (result.equals(ValidatorActionResult.TOOLARGE));
    }

    /**
     * Tests if the validation result is <code>TOOSMALL</code>,
     * e.g. in case of a double or long the value is too small or in
     * case of a string it is too short.
     *
     */
    public boolean isTooSmall() {
	return (result.equals(ValidatorActionResult.TOOSMALL));
    }

    /**
     * Tests if the validation result is <code>NOMATCH</code>, can
     * only occur when 
     *
     */
    public boolean doesNotMatch() {
	return (result.equals(ValidatorActionResult.NOMATCH));
    }

    /**
     * Returns the tested object.
     *
     */
    public Object getObject() {
	return object;
    }

    /**
     * Returns the result.
     *
     */
    public ValidatorActionResult getResult() {
	return result;
    }

}
