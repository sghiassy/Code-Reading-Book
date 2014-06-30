/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.selection;

import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import java.util.Map;

/**
 * A <code>Selector</code> that matches a string in the parameters object passed to it.
 *
 * <pre>
 *  &lt;map:selector name="parameter" factory="org.apache.cocoon.selection.ParameterSelector"/&gt;
 *
 *   &lt;map:select type="parameter"&gt;
 *      &lt;parameter name="parameter-selector-test" value="{$mySitemapParameter}"/&gt;
 *      &lt;map:when test="myParameterValue"&gt;
 *         &lt;!-- executes iff {$mySitemapParameter} == "myParameterValue" --&gt;
 *         &lt;map:transform src="stylesheets/page/uk.xsl"/&gt;
 *      &lt;/map:when&gt;
 *      &lt;map:otherwise&gt;
 *         &lt;map:transform src="stylesheets/page/us.xsl"/&gt;
 *      &lt;/map:otherwise&gt;
 *   &lt;/map:select&gt;
 * </pre>
 *
 * The purpose of this selector is to allow an action to set parameters
 * and to be able to select between different pipeline configurations
 * depending on those parameters.
 *
 * @author <a href="mailto:leo.sutic@inspireinfrastructure.com">Leo Sutic</a>
 * @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:19 $
 */
public class ParameterSelector implements ThreadSafe, Selector {

    public boolean select(String expression, Map objectModel, Parameters parameters) {
        String compareToString = parameters.getParameter("parameter-selector-test", null);
        return compareToString != null && compareToString.equals(expression);
    }
}
