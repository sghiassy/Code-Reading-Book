/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.generation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.caching.CacheValidity;
import org.apache.cocoon.caching.Cacheable;
import org.apache.cocoon.caching.TimeStampCacheValidity;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.util.HashUtil;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.util.Map;

/**
 *
 *
 * The <code>FileGenerator</code> is a class that reads XML from a source
 * and generates SAX Events.
 * The FileGenerator implements the <code>Cacheable</code> interface.
 *
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */
public class FileGenerator extends ComposerGenerator
implements Cacheable, Recyclable {

    /** The input source */
    protected Source inputSource;

    /**
     * Set the current <code>ComponentManager</code> instance used by this
     * <code>Composable</code>.
     */
    public void compose(ComponentManager manager) throws ComponentException {
        super.compose(manager);
    }

    /**
     * Recycle this component.
     * All instance variables are set to <code>null</code>.
     */
    public void recycle() {
        super.recycle();
        if (this.inputSource != null) {
            this.inputSource.recycle();
            this.inputSource = null;
        }
    }

    /**
     * Setup the file generator.
     * Try to get the last modification date of the source for caching.
     */
    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters par)
        throws ProcessingException, SAXException, IOException {
        super.setup(resolver, objectModel, src, par);
        this.inputSource = resolver.resolve(src);
    }

    /**
     * Generate the unique key.
     * This key must be unique inside the space of this component.
     *
     * @return The generated key hashes the src
     */
    public long generateKey() {
        if (this.inputSource.getLastModified() != 0) {
            return HashUtil.hash(this.inputSource.getSystemId());
        }
        return 0;
    }

    /**
     * Generate the validity object.
     *
     * @return The generated validity object or <code>null</code> if the
     *         component is currently not cacheable.
     */
    public CacheValidity generateValidity() {
        if (this.inputSource.getLastModified() != 0) {
            return new TimeStampCacheValidity(this.inputSource.getLastModified());
        }
        return null;
    }

    /**
     * Generate XML data.
     */
    public void generate()
    throws IOException, SAXException, ProcessingException {
        try {
            getLogger().debug("processing file " + super.source);
            getLogger().debug("file resolved to " + this.inputSource.getSystemId());

            this.inputSource.toSAX(super.xmlConsumer);
        } catch (ProcessingException e) {
            throw e;
        } catch (Exception e) {
            getLogger().error("Could not read resource "
                              + this.inputSource.getSystemId(), e);
            throw new ProcessingException("Could not read resource "
                                          + this.inputSource.getSystemId(), e);
        }
    }
}
