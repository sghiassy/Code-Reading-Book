/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

// Cocoon imports

import com.ibm.bsf.BSFException;
import com.ibm.bsf.BSFManager;
import com.ibm.bsf.util.IOUtils;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;

import java.io.FileNotFoundException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/**
 * A simple action that executes any script that can be run by the BSF
 *
 * @author <a href="mailto:jafoster@uwaterloo.ca">Jason Foster</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/25 03:35:52 $
 */

public class ScriptAction
extends ComposerAction
implements ThreadSafe {


    public Map act( Redirector redirector,
                    SourceResolver resolver,
                    Map objectModel,
                    String source,
                    Parameters par )
    throws Exception {
        Source src = null;
        try {
            // Figure out what script to open.  A missing script name is caught
            // by the resolver/SystemId grouping later on and causes an exception
            String scriptName = source;

            // Locate the appropriate file on the filesytem
            src = resolver.resolve(scriptName);
            String systemID = src.getSystemId();

            if (this.getLogger().isDebugEnabled()) {
                getLogger().debug("script source [" + scriptName + "]");
                getLogger().debug("script resolved to [" + systemID + "]");
            }

            // TODO: why doesn't this work?
            // Reader in = src.getCharacterStream();

            Reader in = new InputStreamReader(src.getInputStream());

            // Set up the BSF manager and register relevant helper "beans"

            BSFManager mgr = new BSFManager();
            HashMap actionMap = new HashMap();

            // parameters to act(...)
            mgr.registerBean("resolver", resolver);
            mgr.registerBean("objectModel", objectModel);
            mgr.registerBean("parameters", par);

            // ScriptAction housekeeping
            mgr.registerBean("actionMap", actionMap);

            // helpers
            // TODO: should we check for a null request object here or let the script handle it?

            mgr.registerBean("logger", getLogger());
            mgr.registerBean("request", ( (Request) objectModel.get(Constants.REQUEST_OBJECT) ) );
            mgr.registerBean("scriptaction", this );
            mgr.registerBean("manager", this.manager );

            getLogger().debug("BSFManager execution begining");

            // Execute the script

            mgr.exec(BSFManager.getLangFromFilename(systemID), systemID, 0, 0,
                    IOUtils.getStringFromReader(in));

            getLogger().debug("BSFManager execution complete");

            // Figure out what to return
            // TODO: decide on a more robust communication method

            if ( actionMap.containsKey( "scriptaction-continue" ) )
            {
                return ( Collections.unmodifiableMap(actionMap) );
            }
            else
            {
                return ( null );
            }
        } catch (ProcessingException e) {
            throw e;
        } catch (Exception e) {
            throw new ProcessingException(
                    "Exception in ScriptAction.act()", e);
        } finally {
            if (src != null) src.recycle();
        } // try/catch
    } // public Map act(...)
} // public class ScriptAction
