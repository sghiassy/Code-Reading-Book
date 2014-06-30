/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.xml;



/**
 * This interfaces identifies classes that produce XML data, sending SAX
 * events to the configured <code>XMLConsumer</code>.
 * <br>
 * It's beyond the scope of this interface to specify a way in which the XML
 * data production is started.
 *
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:22 $
 */
public interface XMLProducer {

    /**
     * Set the <code>XMLConsumer</code> that will receive XML data.
     */
    void setConsumer(XMLConsumer consumer);


}
