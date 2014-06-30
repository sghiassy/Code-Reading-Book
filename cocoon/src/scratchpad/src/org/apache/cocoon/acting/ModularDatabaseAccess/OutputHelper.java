/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting.ModularDatabaseAccess;

import org.apache.cocoon.environment.Request;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.configuration.Configuration;

/**
 * Communicate results to other components. This could be done via
 * request attributes, session attribute etc. Implementors should obey
 * the transactional nature and e.g. queue values as request
 * attributes and do the real communication e.g. to a bean only when
 * the transaction completes successfully.
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Id: OutputHelper.java,v 1.1 2002/01/03 12:31:36 giacomo Exp $
 * */
public interface OutputHelper extends Component {

    String ROLE = "org.apache.cocoon.acting.ModularDatabaseAccess.OutputHelper";
    
    
    /**
     * communicate an attribute value to further processing logic. 
     * @param modeConf column's mode configuration from resource
     * description. This argument is optional.
     * @param request The request object
     * @param name The attribute's label, consisting of "table.column"
     * or "table.column[index]" in case of multiple attributes of the
     * same spec.
     * @param value The attriute's value.
     * */
    void setAttribute( Configuration modeConf, Request request, String name, Object value );


    /**
     * retrieve an attribute from other processing steps. <em>Do we
     * really need this thingy?</em>
     * @param modeConf column's mode configuration from resource
     * description. This argument is optional.
     * @param request The request object
     * @param name The attribute's label, consisting of "table.column"
     * or "table.column[index]" in case of multiple attributes of the
     * same spec.
     * @return requested object.
     *
     * Really necessary?
     * */
    Object getAttribute( Configuration modeConf, Request request, String name );


    /**
     * If a database transaction needs to rollback, this is called to
     * inform the further processing logic about this fact. All
     * already set attribute values are invalidated. <em>This is difficult
     * because only the request object can be used to synchronize this
     * and build some kind of transaction object. Beaware that sending
     * your data straight to some beans or other entities could result
     * in data corruption!</em>
     * */
    void rollback( Configuration modeConf, Request request, Exception e );


    /**
     * Signal that the database transaction completed
     * successfully. See notes on @link{rollback}.
     * */
    void commit( Configuration modeConf, Request request );


}
