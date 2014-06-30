/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.generation;

import org.apache.avalon.framework.component.Component;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.ResourceNotFoundException;
import org.apache.cocoon.components.parser.Parser;
import org.apache.avalon.excalibur.pool.Poolable;
import org.apache.cocoon.environment.http.HttpEnvironment;
import javax.servlet.http.HttpServletRequest;
import org.apache.cocoon.util.PostInputStream;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.io.StringReader;

/**
 *
 * The <code>StreamGenerator</code> is a class that reads XML from a
 * request InputStream and generates SAX Events.
 *
 * For the POST requests with mimetype of
 * application/x-www-form-urlencoded the xml data is expected to be
 * associated with the name specified in the sitemap parameter.  For
 * the POST requests with mimetypes: text/plain, text/xml,
 * application/xml the xml data is in the body of the POST request and
 * its length is specified by the value returned by getContentLength()
 * method.  The StreamGenerator uses helper
 * org.apache.cocoon.util.PostInputStream class for InputStream
 * reading operations.  At the time that Parser is reading the data
 * out of InputStream - Parser has no knowledge about the length of
 * data to be read.  The only way to signal to the Parser that all
 * data was read from the InputStream is to control reading operation-
 * PostInputStream--and to return to the requestor '-1' when the
 * number of bytes read is equal to the getContentLength() value.
 *
 * @author <a href="mailto:Kinga_Dziembowski@hp.com">Kinga Dziembowski</a>
 * @version $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */
public class StreamGenerator extends ComposerGenerator implements Poolable
{
    public static final String CLASS = StreamGenerator.class.getName();

    /** The parameter holding the name associated with the xml data  **/
    public static final String FORM_NAME = "form-name";

    /** The input source */
    private InputSource inputSource;

    /** The system ID of the input source */
    private String systemID;

    /**
     * Recycle this component.
     * All instance variables are set to <code>null</code>.
     */
    public void recycle() {
        super.recycle();
        this.inputSource = null;
        this.systemID = null;
    }

    /**
     * Generate XML data out of request InputStream.
     */
    public void generate() throws IOException, SAXException, ProcessingException
    {
        Parser parser = null;
        String parameter = parameters.getParameter(StreamGenerator.FORM_NAME, null);
        int len = 0;
		String contentType = null;
        try
        {
            HttpServletRequest request = (HttpServletRequest) objectModel.get(HttpEnvironment.HTTP_REQUEST_OBJECT);
            contentType = request.getContentType();
            if (contentType.startsWith("application/x-www-form-urlencoded")) {
                String sXml = request.getParameter(parameter);
                inputSource = new InputSource(new StringReader(sXml));
            } else if (contentType.startsWith("text/plain") ||
                    contentType.startsWith("text/xml") ||
                    contentType.startsWith("application/xml")) {
                len = request.getContentLength();

                if (len > 0) {
                        PostInputStream anStream = new PostInputStream(request.getInputStream(), len);
                        inputSource = new InputSource(anStream);
                } else {
                    throw new IOException("getContentLen() == 0");
                }
            } else {
                throw new IOException("Unexpected getContentType(): " + request.getContentType());
            }

            if (getLogger().isDebugEnabled()) {
                getLogger().debug("processing stream ContentType= " + request.getContentType() + "ContentLen= " + len);
            }
            String charset =  getCharacterEncoding(request, contentType) ;
			if( charset != null)
			{
				this.inputSource.setEncoding(charset);
            }
            parser = (Parser)this.manager.lookup(Parser.ROLE);
            parser.setContentHandler(super.contentHandler);
            parser.setLexicalHandler(super.lexicalHandler);
            parser.parse(this.inputSource);
        } catch (IOException e) {
            getLogger().error("StreamGenerator.generate()", e);
            throw new ResourceNotFoundException("StreamGenerator could not find resource", e);
        } catch (SAXException e) {
            getLogger().error("StreamGenerator.generate()", e);
            throw(e);
        } catch (Exception e) {
            getLogger().error("Could not get parser", e);
            throw new ProcessingException("Exception in StreamGenerator.generate()", e);
        } finally {
            if (parser != null) {
                this.manager.release((Component)parser);
            }
        }
    }

    /**
	* Content type HTTP header can contains character encodinf info
	* for ex. Content-Type: text/xml; charset=UTF-8
	* If the servlet is following spec 2.3 and higher the servlet API can be used to retrieve character encoding part of
	* Content-Type header. Some containers can choose to not unpack charset info - the spec is not strong about it.
	* in any case this method can be used as a latest resource to retrieve the passed charset value.
	* <code>null</code> is returned.
	* It is very common mistake to send : Content-Type: text/xml; charset="UTF-8".
	* Some containers are not filtering this mistake and the processing results in exception..
    * The getCharacterEncoding() compensates for above mistake.
	*
	* @param contentType value associated with Content-Type HTTP header.
	*/
	public String getCharacterEncoding(HttpServletRequest req, String contentType)
	{
		String charencoding = null;
		String charset = "charset=";
		if (contentType == null)
		{
			return (null);
		}
		int idx = contentType.indexOf(charset);
		if (idx == -1)
		{
			return (null);
		}
		try
		{
			charencoding = req.getCharacterEncoding();

			if ( charencoding != null)
			{
				getLogger().debug("charset from container: " + charencoding);
				charencoding = charencoding.trim();
				if ((charencoding.length() > 2) && (charencoding.startsWith("\""))&& (charencoding.endsWith("\"")))
				{
					charencoding = charencoding.substring(1, charencoding.length() - 1);
				}
				getLogger().debug("charset from container clean: " + charencoding);
				return (charencoding);
			}
			else
			{

				return extractCharset( contentType, idx );
			}
		}
		catch(Throwable e)
		{
			// We will be there if the container do not implement getCharacterEncoding() method
			 return extractCharset( contentType, idx );
		}
	}


	protected String extractCharset(String contentType, int idx)
	{
		String charencoding = null;
		String charset = "charset=";

		getLogger().debug("charset from extractCharset");
		charencoding = contentType.substring(idx + charset.length());
		int idxEnd = charencoding.indexOf(";");
		if (idxEnd != -1)
		{
			charencoding = charencoding.substring(0, idxEnd);
		}
		charencoding = charencoding.trim();
		if ((charencoding.length() > 2) && (charencoding.startsWith("\""))&& (charencoding.endsWith("\"")))
		{
			charencoding = charencoding.substring(1, charencoding.length() - 1);
		}
		getLogger().debug("charset from extractCharset: " + charencoding);
		return (charencoding.trim());

	}
}

