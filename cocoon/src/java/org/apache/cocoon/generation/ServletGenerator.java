/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.generation;

import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Context;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Response;
import org.apache.cocoon.environment.SourceResolver;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.util.Map;

/**
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */
public abstract class ServletGenerator extends ComposerGenerator
implements Composable {

    protected Request request=null;
    protected Response response=null;
    protected Context context=null;

    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters par)
        throws ProcessingException, SAXException, IOException {

      super.setup(resolver, objectModel, src, par);
      this.request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
      this.response = (Response) objectModel.get(Constants.RESPONSE_OBJECT);
      this.context = (Context) objectModel.get(Constants.CONTEXT_OBJECT);
    }

    /**
     * Recycle the generator by removing references
     */
    public void recycle() {
        super.recycle();
        this.request = null;
        this.response = null;
        this.context = null;
    }
}
