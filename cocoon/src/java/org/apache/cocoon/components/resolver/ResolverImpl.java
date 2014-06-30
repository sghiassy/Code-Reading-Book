/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.resolver;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.parameters.ParameterException;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.Constants;

import org.apache.xml.resolver.helpers.Debug;
import org.apache.xml.resolver.tools.CatalogResolver;

import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import java.io.IOException;


/**
 * A component that uses catalogs for resolving entities.
 * This implementation uses the XML Entity and URI Resolvers from
 * http://www.sun.com/xml/developers/resolver/
 * published by Sun's Norman Walsh. More information on the catalogs can be
 * found at
 * http://xml.apache.org/cocoon/userdocs/concepts/catalog.html
 *
 * The catalog is by default loaded from "resources/entities/catalog".
 * This can be configured by the "catalog" parameter in the cocoon.xconf:
 * &lt;resolver&gt;
 *   &lt;parameter name="catalog" value="mycatalog"/&gt;
 * &lt;/resolver&gt;
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @author <a href="mailto:crossley@indexgeo.com.au">David Crossley</a>
 * @version CVS $Revision: 1.5 $ $Date: 2002/01/31 00:26:08 $
 */
public class ResolverImpl extends AbstractLoggable
        implements Resolver, Contextualizable, Composable, Parameterizable,
                    ThreadSafe, Disposable {

    /** The catalog resolver */
    protected CatalogResolver catalogResolver = new CatalogResolver();

    /** The component manager */
    protected ComponentManager manager = null;

    /** The context */
    protected org.apache.cocoon.environment.Context context;

    /** Contextualize this class */
    public void contextualize(Context context)
            throws ContextException {
        this.context = (org.apache.cocoon.environment.Context)
                        context.get(Constants.CONTEXT_ENVIRONMENT_CONTEXT);
    }


    /**
     * Set the configuration. Load the system catalog and apply any
     * parameters that may have been specified in cocoon.xconf
     * @param conf The configuration information
     * @exception ConfigurationException
     */
    public void parameterize(Parameters params) throws ParameterException {

        /* Over-ride debug level that is set by CatalogManager.properties */
        int debugLevel = Debug.getDebug();
        String verbosity = params.getParameter("verbosity", "");
        if (verbosity != "") {
            if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Setting Catalog resolver "
                + "verbosity level to " + verbosity);
            }
            int verbosityLevel = 0;
            try {
                verbosityLevel = Integer.parseInt(verbosity);
                Debug.setDebug(verbosityLevel);
            } catch (NumberFormatException ce1) {
                this.getLogger().warn("Trouble setting Catalog verbosity",
                                        ce1);
            }
        }

        /* Load the built-in catalog */
        String catalogFile = params.getParameter("catalog",
                                "/resources/entities/catalog");
        try {
            String catalogURL = null;
            catalogURL = this.context.getRealPath(catalogFile);
            if (catalogURL == null) {
                catalogURL = 
                    this.context.getResource(catalogFile).toExternalForm();
            }
            if (this.getLogger().isDebugEnabled()) {
                this.getLogger().debug("System OASIS Catalog URL is "
                    + catalogURL);
            }
            catalogResolver.getCatalog().parseCatalog(catalogURL);
        } catch (Exception e) {
            this.getLogger().warn("Could not get Catalog URL", e);
        }

        /* Load a single additional local catalog */
        String localCatalogFile = params.getParameter("local-catalog", "");
        if (localCatalogFile != "") {
            try {
                if (this.getLogger().isDebugEnabled()) {
                    this.getLogger().debug("Additional Catalog is "
                        + localCatalogFile);
                }
                catalogResolver.getCatalog().parseCatalog(localCatalogFile);
            } catch (Exception e) {
                this.getLogger().warn("Could not get local Catalog file", e);
            }
        }
    }

    /**
     * Set the global component manager.
     * @param manager The global component manager
     */
    public void compose(ComponentManager manager) throws ComponentException {
        if ((this.manager == null) && (manager != null)) {
            this.manager = manager;
        }
    }

    /**
     * Allow the application to resolve external entities.
     *
     * <p>The Parser will call this method before opening any external
     * entity except the top-level document entity (including the
     * external DTD subset, external entities referenced within the
     * DTD, and external entities referenced within the document
     * element): the application may request that the parser resolve
     * the entity itself, that it use an alternative URI, or that it
     * use an entirely different input source.</p>
     *
     * <p>Application writers can use this method to redirect external
     * system identifiers to secure and/or local URIs, to look up
     * public identifiers in a catalogue, or to read an entity from a
     * database or other input source (including, for example, a dialog
     * box).</p>
     *
     * <p>If the system identifier is a URL, the SAX parser must
     * resolve it fully before reporting it to the application.</p>
     *
     * @param publicId The public identifier of the external entity
     *        being referenced, or null if none was supplied.
     * @param systemId The system identifier of the external entity
     *        being referenced.
     * @return An InputSource object describing the new input source,
     *         or null to request that the parser open a regular
     *         URI connection to the system identifier.
     * @exception org.xml.sax.SAXException Any SAX exception, possibly
     *            wrapping another exception.
     * @exception java.io.IOException A Java-specific IO exception,
     *            possibly the result of creating a new InputStream
     *            or Reader for the InputSource.
     * @see org.xml.sax.InputSource
     */
    public InputSource resolveEntity(String publicId, String systemId)
            throws SAXException, IOException {
        InputSource altInputSource = catalogResolver.resolveEntity(publicId,
                                        systemId);
        if (altInputSource != null) {
            if (this.getLogger().isDebugEnabled()) {
                this.getLogger().debug("Resolved catalog entity: "
                    + publicId + " " + altInputSource.getSystemId());
            }
        }
        return altInputSource;
    }

    /**
     * Dispose
     */
    public void dispose() {
    }
}
