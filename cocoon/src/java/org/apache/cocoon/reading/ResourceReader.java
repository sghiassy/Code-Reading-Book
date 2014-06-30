/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.reading;

import org.apache.avalon.excalibur.pool.Poolable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.caching.CacheValidity;
import org.apache.cocoon.caching.Cacheable;
import org.apache.cocoon.caching.TimeStampCacheValidity;
import org.apache.cocoon.environment.Context;
import org.apache.cocoon.environment.Response;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.util.HashUtil;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.io.InputStream;
import java.util.Date;
import java.util.Map;

/**
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/22 09:01:12 $
 *
 * The <code>ResourceReader</code> component is used to serve binary data
 * in a sitemap pipeline. It makes use of HTTP Headers to determine if
 * the requested resource should be written to the <code>OutputStream</code>
 * or if it can signal that it hasn't changed.
 *
 * Parameters:
 *   <dl>
 *     <dt>&lt;expires&gt;</dt>
 *       <dd>This parameter is optional. When specified it determines how long
 *           in miliseconds the resources can be cached by any proxy or browser
 *           between Cocoon2 and the requesting visitor.
 *       </dd>
 *   </dl>
 */
public class ResourceReader
  extends AbstractReader
  implements Cacheable {

    /** The  source */
    private Source      inputSource;


    /**
     * Setup the reader.
     * The resource is opened to get an <code>InputStream</code>,
     * the length and the last modification date
     */
    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters par)
        throws ProcessingException, SAXException, IOException {
        super.setup(resolver, objectModel, src, par);
        this.inputSource = this.resolver.resolve(super.source);
    }

    public void recycle() {
        super.recycle();
        if (this.inputSource != null) {
            this.inputSource.recycle();
            this.inputSource = null;
        }
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
     * @return the time the read source was last modified or 0 if it is not
     *         possible to detect
     */
    public long getLastModified() {
        return this.inputSource.getLastModified();
    }

    /**
     * Generates the requested resource.
     */
    public void generate()
    throws IOException, ProcessingException {
        Response response = (Response) objectModel.get(Constants.RESPONSE_OBJECT);

        try {
            long expires = parameters.getParameterAsInteger("expires", -1);

            if (expires > 0) {
                response.setDateHeader("Expires", System.currentTimeMillis() + expires);
            }

            response.setHeader("Accept-Ranges", "bytes");

            byte[] buffer = new byte[8192];
            int length = -1;

            InputStream inputStream = this.inputSource.getInputStream();
            while ((length = inputStream.read(buffer)) > -1) {
                out.write(buffer, 0, length);
            }
            inputStream.close();
            inputStream = null;
            out.flush();
        } catch (IOException ioe) {
            getLogger().debug("Received an IOException, assuming client severed connection on purpose");
        }
    }

    /**
     * Returns the mime-type of the resource in process.
     */
    public String getMimeType () {
        Context ctx = (Context) objectModel.get(Constants.CONTEXT_OBJECT);

        if (ctx != null) {
           return ctx.getMimeType(this.source);
        } else {
           return null;
        }
    }

}
