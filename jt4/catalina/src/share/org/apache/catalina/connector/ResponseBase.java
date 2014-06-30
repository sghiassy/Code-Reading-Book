/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/connector/ResponseBase.java,v 1.16 2001/08/02 01:43:58 remm Exp $
 * $Revision: 1.16 $
 * $Date: 2001/08/02 01:43:58 $
 *
 * ====================================================================
 *
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowlegement:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowlegement may appear in the software itself,
 *    if and wherever such third-party acknowlegements normally appear.
 *
 * 4. The names "The Jakarta Project", "Tomcat", and "Apache Software
 *    Foundation" must not be used to endorse or promote products derived
 *    from this software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 * [Additional notices, if required by prior licensing conditions]
 *
 */


package org.apache.catalina.connector;


import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.Locale;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.ServletResponse;
import org.apache.catalina.Connector;
import org.apache.catalina.Context;
import org.apache.catalina.Request;
import org.apache.catalina.Response;
import org.apache.catalina.util.CharsetMapper;
import org.apache.catalina.util.RequestUtil;
import org.apache.catalina.util.StringManager;


/**
 * Convenience base implementation of the <b>Response</b> interface, which can
 * be used for the Response implementation required by most Connectors.  Only
 * the connector-specific methods need to be implemented.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.16 $ $Date: 2001/08/02 01:43:58 $
 */

public abstract class ResponseBase
    implements Response, ServletResponse {


    // ----------------------------------------------------- Instance Variables


    /**
     * The buffer through which all of our output bytes are passed.
     */
    protected byte[] buffer = new byte[1024];


    /**
     * The number of data bytes currently in the buffer.
     */
    protected int bufferCount = 0;


    /**
     * Has this response been committed yet?
     */
    protected boolean committed = false;


    /**
     * The Connector through which this Response is returned.
     */
    protected Connector connector = null;


    /**
     * The actual number of bytes written to this Response.
     */
    protected int contentCount = 0;


    /**
     * The content length associated with this Response.
     */
    protected int contentLength = -1;


    /**
     * The content type associated with this Response.
     */
    protected String contentType = null;


    /**
     * The Context within which this Response is being produced.
     */
    protected Context context = null;


    /**
     * The character encoding associated with this Response.
     */
    protected String encoding = null;


    /**
     * The facade associated with this response.
     */
    protected ResponseFacade facade = new ResponseFacade(this);


    /**
     * Are we currently processing inside a RequestDispatcher.include()?
     */
    protected boolean included = false;


    /**
     * Descriptive information about this Response implementation.
     */
    protected static final String info =
        "org.apache.catalina.connector.ResponseBase/1.0";


    /**
     * The Locale associated with this Response.
     */
    protected Locale locale = Locale.getDefault();


    /**
     * The output stream associated with this Response.
     */
    protected OutputStream output = null;


    /**
     * The Request with which this Response is associated.
     */
    protected Request request = null;


    /**
     * The string manager for this package.
     */
    protected static StringManager sm =
        StringManager.getManager(Constants.Package);


    /**
     * The ServletOutputStream that has been returned by
     * <code>getOutputStream()</code>, if any.
     */
    protected ServletOutputStream stream = null;


    /**
     * The PrintWriter that has been returned by
     * <code>getWriter()</code>, if any.
     */
    protected PrintWriter writer = null;


    /**
     * Error flag. True if the response is an error report.
     */
    protected boolean error = false;


    // ------------------------------------------------------------- Properties


    /**
     * Return the Connector through which this Response will be transmitted.
     */
    public Connector getConnector() {

        return (this.connector);

    }


    /**
     * Set the Connector through which this Response will be transmitted.
     *
     * @param connector The new connector
     */
    public void setConnector(Connector connector) {

        this.connector = connector;

    }


    /**
     * Return the number of bytes actually written to the output stream.
     */
    public int getContentCount() {

        return (this.contentCount);

    }


    /**
     * Return the Context with which this Response is associated.
     */
    public Context getContext() {

        return (this.context);

    }


    /**
     * Set the Context with which this Response is associated.  This should
     * be called as soon as the appropriate Context is identified.
     *
     * @param context The associated Context
     */
    public void setContext(Context context) {

        this.context = context;

    }


    /**
     * Return the "processing inside an include" flag.
     */
    public boolean getIncluded() {

        return (this.included);

    }


    /**
     * Set the "processing inside an include" flag.
     *
     * @param included <code>true</code> if we are currently inside a
     *  RequestDispatcher.include(), else <code>false</code>
     */
    public void setIncluded(boolean included) {

        this.included = included;

    }


    /**
     * Return descriptive information about this Response implementation and
     * the corresponding version number, in the format
     * <code>&lt;description&gt;/&lt;version&gt;</code>.
     */
    public String getInfo() {

        return (this.info);

    }


    /**
     * Return the Request with which this Response is associated.
     */
    public Request getRequest() {

        return (this.request);

    }


    /**
     * Set the Request with which this Response is associated.
     *
     * @param request The new associated request
     */
    public void setRequest(Request request) {

        this.request = request;

    }


    /**
     * Return the <code>ServletResponse</code> for which this object
     * is the facade.
     */
    public ServletResponse getResponse() {

        return (facade);

    }


    /**
     * Return the output stream associated with this Response.
     */
    public OutputStream getStream() {

        return (this.output);

    }


    /**
     * Set the output stream associated with this Response.
     *
     * @param stream The new output stream
     */
    public void setStream(OutputStream stream) {

        this.output = stream;

    }


    /**
     * Set the error flag.
     */
    public void setError() {

        this.error = true;

    }


    /**
     * Error flag accessor.
     */
    public boolean isError() {

        return (this.error);

    }


    // --------------------------------------------------------- Public Methods


    /**
     * Create and return a ServletOutputStream to write the content
     * associated with this Response.
     *
     * @exception IOException if an input/output error occurs
     */
    public ServletOutputStream createOutputStream() throws IOException {

        return (new ResponseStream(this));

    }


    /**
     * Perform whatever actions are required to flush and close the output
     * stream or writer, in a single operation.
     *
     * @exception IOException if an input/output error occurs
     */
    public void finishResponse() throws IOException {

        // If no stream has been requested yet, get one so we can
        // flush the necessary headers
        if (this.stream == null) {
            ServletOutputStream sos = getOutputStream();
            sos.flush();
            sos.close();
            return;
        }

        // If our stream is closed, no action is necessary
        if ( ((ResponseStream) stream).closed() )
            return;

        // Flush and close the appropriate output mechanism
        if (writer != null) {
            writer.flush();
            writer.close();
        } else {
            stream.flush();
            stream.close();
        }

        // The underlying output stream (perhaps from a socket)
        // is not our responsibility

    }


    /**
     * Return the content length that was set or calculated for this Response.
     */
    public int getContentLength() {

        return (this.contentLength);

    }


    /**
     * Return the content type that was set or calculated for this response,
     * or <code>null</code> if no content type was set.
     */
    public String getContentType() {

        return (this.contentType);

    }


    /**
     * Return a PrintWriter that can be used to render error messages,
     * regardless of whether a stream or writer has already been acquired.
     */
    public PrintWriter getReporter() {

        if (isError()) {

            try {
                flushBuffer();
            } catch (IOException e) {
                ;
            }
            try {
                if (this.stream == null)
                    this.stream = createOutputStream();
            } catch (IOException e) {
                return null;
            }
            return (new PrintWriter(this.stream));

        } else {

            if (this.stream != null) {
                return null;
            } else {
                try {
                    return (new PrintWriter(getOutputStream()));
                } catch (IOException e) {
                    return null;
                }
            }

        }

    }


    /**
     * Release all object references, and initialize instance variables, in
     * preparation for reuse of this object.
     */
    public void recycle() {

        // buffer is NOT reset when recycling
        bufferCount = 0;
        committed = false;
        // connector is NOT reset when recycling
        contentCount = 0;
        contentLength = -1;
        contentType = null;
        context = null;
        encoding = null;
        included = false;
        locale = Locale.getDefault();
        output = null;
        request = null;
        stream = null;
        writer = null;
        error = false;

    }


    // -------------------------------------------------------- Package Methods


    /**
     * Write the specified byte to our output stream, flushing if necessary.
     *
     * @param b The byte to be written
     *
     * @exception IOException if an input/output error occurs
     */
    public void write(int b) throws IOException {

        if (bufferCount >= buffer.length)
            flushBuffer();
        buffer[bufferCount++] = (byte) b;
        contentCount++;

    }


    /**
     * Write <code>b.length</code> bytes from the specified byte array
     * to our output stream.  Flush the output stream as necessary.
     *
     * @param b The byte array to be written
     *
     * @exception IOException if an input/output error occurs
     */
    public void write(byte b[]) throws IOException {

        write(b, 0, b.length);

    }


    /**
     * Write <code>len</code> bytes from the specified byte array, starting
     * at the specified offset, to our output stream.  Flush the output
     * stream as necessary.
     *
     * @param b The byte array containing the bytes to be written
     * @param off Zero-relative starting offset of the bytes to be written
     * @param len The number of bytes to be written
     *
     * @exception IOException if an input/output error occurs
     */
    public void write(byte b[], int off, int len) throws IOException {

        // If the whole thing fits in the buffer, just put it there
        if (len == 0)
            return;
        if (len <= (buffer.length - bufferCount)) {
            System.arraycopy(b, off, buffer, bufferCount, len);
            bufferCount += len;
            contentCount += len;
            return;
        }

        // Flush the buffer and start writing full-buffer-size chunks
        flushBuffer();
        int iterations = len / buffer.length;
        int leftoverStart = iterations * buffer.length;
        int leftoverLen = len - leftoverStart;
        for (int i = 0; i < iterations; i++)
            write(b, off + (i * buffer.length), buffer.length);

        // Write the remainder (guaranteed to fit in the buffer)
        if (leftoverLen > 0)
            write(b, off + leftoverStart, leftoverLen);

    }


    // ------------------------------------------------ ServletResponse Methods


    /**
     * Flush the buffer and commit this response.
     *
     * @exception IOException if an input/output error occurs
     */
    public void flushBuffer() throws IOException {

        committed = true;
        if (bufferCount > 0) {
            try {
                output.write(buffer, 0, bufferCount);
            } finally {
                bufferCount = 0;
            }
        }

    }


    /**
     * Return the actual buffer size used for this Response.
     */
    public int getBufferSize() {

        return (buffer.length);

    }


    /**
     * Return the character encoding used for this Response.
     */
    public String getCharacterEncoding() {

        if (encoding == null)
            return ("ISO-8859-1");
        else
            return (encoding);

    }


    /**
     * Return the servlet output stream associated with this Response.
     *
     * @exception IllegalStateException if <code>getWriter</code> has
     *  already been called for this response
     * @exception IOException if an input/output error occurs
     */
    public ServletOutputStream getOutputStream() throws IOException {

        if (writer != null)
            throw new IllegalStateException
                (sm.getString("responseBase.getOutputStream.ise"));

        if (stream == null)
            stream = createOutputStream();
        ((ResponseStream) stream).setCommit(true);
        return (stream);

    }


    /**
     * Return the Locale assigned to this response.
     */
    public Locale getLocale() {

        return (locale);

    }


    /**
     * Return the writer associated with this Response.
     *
     * @exception IllegalStateException if <code>getOutputStream</code> has
     *  already been called for this response
     * @exception IOException if an input/output error occurs
     */
    public PrintWriter getWriter() throws IOException {

        if (writer != null)
            return (writer);

        if (stream != null)
            throw new IllegalStateException
                (sm.getString("responseBase.getWriter.ise"));

        stream = createOutputStream();
        ((ResponseStream) stream).setCommit(false);
        OutputStreamWriter osr =
          new OutputStreamWriter(stream, getCharacterEncoding());
        writer = new ResponseWriter(osr, (ResponseStream) stream);
        return (writer);

    }


    /**
     * Has the output of this response already been committed?
     */
    public boolean isCommitted() {

        return (committed);

    }


    /**
     * Clear any content written to the buffer.
     *
     * @exception IllegalStateException if this response has already
     *  been committed
     */
    public void reset() {

        if (committed)
            throw new IllegalStateException
                (sm.getString("responseBase.reset.ise"));

        if (included)
            return;     // Ignore any call from an included servlet

        if (stream != null)
            ((ResponseStream) stream).reset();
        bufferCount = 0;
        contentLength = -1;
        contentType = null;

    }


    /**
     * Reset the data buffer but not any status or header information.
     *
     * @exception IllegalStateException if the response has already
     *  been committed
     */
    public void resetBuffer() {

        if (committed)
            throw new IllegalStateException
                (sm.getString("responseBase.resetBuffer.ise"));

        bufferCount = 0;

    }


    /**
     * Set the buffer size to be used for this Response.
     *
     * @param size The new buffer size
     *
     * @exception IllegalStateException if this method is called after
     *  output has been committed for this response
     */
    public void setBufferSize(int size) {

        if (committed || (bufferCount > 0))
            throw new IllegalStateException
                (sm.getString("responseBase.setBufferSize.ise"));

        if (buffer.length >= size)
            return;
        buffer = new byte[size];

    }


    /**
     * Set the content length (in bytes) for this Response.
     *
     * @param length The new content length
     */
    public void setContentLength(int length) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        this.contentLength = length;

    }


    /**
     * Set the content type for this Response.
     *
     * @param type The new content type
     */
    public void setContentType(String type) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        this.contentType = type;
        if (type.indexOf(';') >= 0) {
            encoding = RequestUtil.parseCharacterEncoding(type);
            if (encoding == null)
                encoding = "ISO-8859-1";
        }

    }


    /**
     * Set the Locale that is appropriate for this response, including
     * setting the appropriate character encoding.
     *
     * @param locale The new locale
     */
    public void setLocale(Locale locale) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        this.locale = locale;
        if ((this.encoding == null) && (this.context != null)) {
            CharsetMapper mapper = context.getCharsetMapper();
            this.encoding = mapper.getCharset(locale);
        }

    }


}
