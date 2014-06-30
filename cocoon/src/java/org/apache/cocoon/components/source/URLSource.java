/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.components.source;

import org.apache.avalon.framework.component.ComponentManager;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.ResourceNotFoundException;
import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.environment.ModifiableSource;
import org.apache.cocoon.xml.XMLConsumer;
import org.xml.sax.ContentHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.ext.LexicalHandler;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLConnection;

/**
 * Description of a source which is described by an URL.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/22 00:17:12 $
 */

public class URLSource implements ModifiableSource {

    /** Identifier for file urls */
    private final String FILE = "file:";

    /** The last modification date or 0 */
    private long lastModificationDate;

    /** The content length */
    private long contentLength;

    /** The system id */
    private String systemId;

    /** The URL of the source */
    private URL url;

    /** The connection for a real URL */
    private URLConnection connection;

    /** Is this a file or a "real" URL */
    private boolean isFile;

    /** Are we initialized? */
    private boolean gotInfos;

    /** The ComponentManager needed for streaming */
    private ComponentManager manager;

    /**
     * Construct a new object
     */
    public URLSource(URL url, ComponentManager manager)
    throws IOException {
        this.manager = manager;
        this.systemId = url.toExternalForm();
        this.isFile = systemId.startsWith(FILE);
        this.url = url;
        this.gotInfos = false;
    }

    /**
     * Get the last modification date and content length of the source.
     * Any exceptions are ignored.
     */
    private void getInfos() {
        if (!this.gotInfos) {
            if (this.isFile) {
                File file = new File(systemId.substring(FILE.length()));
                this.lastModificationDate = file.lastModified();
                this.contentLength = file.length();
            } else {
                try {
                    if (this.connection == null) {
                        this.connection = this.url.openConnection();
                        String userInfo = this.getUserInfo();
                        if (this.url.getProtocol().startsWith("http") && userInfo != null) {
                            this.connection.setRequestProperty("Authorization","Basic "+this.encodeBASE64(userInfo));
                        }
                    }
                    this.lastModificationDate = this.connection.getLastModified();
                    this.contentLength = this.connection.getContentLength();
                } catch (IOException ignore) {
                    this.lastModificationDate = 0;
                    this.contentLength = -1;
                }
            }
            this.gotInfos = true;
        }
    }

    /**
     * Get the last modification date of the source or 0 if it
     * is not possible to determine the date.
     */
    public long getLastModified() {
        this.getInfos();
        return this.lastModificationDate;
    }

    /**
     * Get the content length of the source or -1 if it
     * is not possible to determine the length.
     */
    public long getContentLength() {
        this.getInfos();
        return this.contentLength;
    }

    /**
     * Return an <code>InputStream</code> object to read from the source.
     *
     * @throws ResourceNotFoundException if file not found or
     *         HTTP location does not exist.
     * @throws IOException if I/O error occured.
     */
    public InputStream getInputStream()
    throws IOException, ProcessingException {
        this.getInfos();
        try{
            InputStream input = null;
            if (this.isFile) {
                input = new FileInputStream(this.systemId.substring(FILE.length()));
            } else {
                if (this.connection == null) {
                    this.connection = this.url.openConnection();
                    /* The following requires a jdk 1.3 */
                    String userInfo = this.getUserInfo();
                    if (this.url.getProtocol().startsWith("http") && userInfo != null) {
                        this.connection.setRequestProperty("Authorization","Basic "+encodeBASE64(userInfo));
                    }
                }

                input = this.connection.getInputStream();
                this.connection = null; // make sure a new connection is created next time
            }
            return input;
        }catch(FileNotFoundException e){
            throw new ResourceNotFoundException("Resource not found "
                                                + this.systemId);
        }
    }

    private static boolean checkedURLClass = false;
    private static boolean urlSupportsGetUserInfo = false;
    private static Method  urlGetUserInfo = null;
    private static Object[] emptyParams = new Object[0];

    /**
     * Check if the <code>URL</code> class supports the getUserInfo()
     * method which is introduced in jdk 1.3
     */
    private String getUserInfo() {
        if (URLSource.checkedURLClass) {
            if (URLSource.urlSupportsGetUserInfo) {
                try {
                    return (String) URLSource.urlGetUserInfo.invoke(this.url, URLSource.emptyParams);
                } catch (Exception e){
                    // ignore this anyway
                }
            }
            return null;
        } else {
            // test if the url class supports the getUserInfo method
            try {
                URLSource.urlGetUserInfo = URL.class.getMethod("getUserInfo", null);
                String ui = (String)URLSource.urlGetUserInfo.invoke(this.url, URLSource.emptyParams);
                URLSource.checkedURLClass = true;
                URLSource.urlSupportsGetUserInfo = true;
                return ui;
            } catch (Exception e){
            }
            URLSource.checkedURLClass = true;
            URLSource.urlSupportsGetUserInfo = false;
            URLSource.urlGetUserInfo = null;
            return null;
        }
    }

    /**
     * Return the unique identifer for this source
     */
    public String getSystemId() {
        return this.systemId;
    }

    /**
     * Refresh this object and update the last modified date
     * and content length.
     */
    public void refresh() {
        // reset connection
        this.connection = null;
        this.gotInfos = false;
    }

    /**
     * Return a new <code>InputSource</code> object
     *
     * @throws ResourceNotFoundException if file not found or
     *         HTTP location does not exist.
     * @throws IOException if I/O error occured.
     */
    public InputSource getInputSource()
    throws IOException, ProcessingException {
        InputSource newObject = new InputSource(this.getInputStream());
        newObject.setSystemId(this.systemId);
        return newObject;
    }

     public static final char [ ] alphabet = {
       'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', // 0 to 7
       'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', // 8 to 15
       'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', // 16 to 23
       'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', // 24 to 31
       'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', // 32 to 39
       'o', 'p', 'q', 'r', 's', 't', 'u', 'v', // 40 to 47
       'w', 'x', 'y', 'z', '0', '1', '2', '3', // 48 to 55
       '4', '5', '6', '7', '8', '9', '+', '/' }; // 56 to 63

     /**
      * BASE 64 encoding.
      * See also RFC 1421
      * @since 1.2
      */
     public static String encodeBASE64 ( String s ) {
         return encodeBASE64 ( s.getBytes ( ) );
     }

     /**
      * BASE 64 encoding.
      * See also RFC 1421
      * @since 1.2
      */
     public static String encodeBASE64 ( byte [ ] octetString ) {
         int bits24;
         int bits6;

         char [ ] out
         = new char [ ( ( octetString.length - 1 ) / 3 + 1 ) * 4 ];

         int outIndex = 0;
         int i = 0;

         while ( ( i + 3 ) <= octetString.length ) {
             // store the octets
             bits24 = ( octetString [ i++ ] & 0xFF ) << 16;
             bits24 |= ( octetString [ i++ ] & 0xFF ) << 8;
             bits24 |= ( octetString [ i++ ] & 0xFF ) << 0;

             bits6 = ( bits24 & 0x00FC0000 ) >> 18;
             out [ outIndex++ ] = alphabet [ bits6 ];
             bits6 = ( bits24 & 0x0003F000 ) >> 12;
             out [ outIndex++ ] = alphabet [ bits6 ];
             bits6 = ( bits24 & 0x00000FC0 ) >> 6;
             out [ outIndex++ ] = alphabet [ bits6 ];
             bits6 = ( bits24 & 0x0000003F );
             out [ outIndex++ ] = alphabet [ bits6 ];
         }

         if ( octetString.length - i == 2 ) {
             // store the octets
             bits24 = ( octetString [ i ] & 0xFF ) << 16;
             bits24 |= ( octetString [ i + 1 ] & 0xFF ) << 8;

             bits6 = ( bits24 & 0x00FC0000 ) >> 18;
             out [ outIndex++ ] = alphabet [ bits6 ];
             bits6 = ( bits24 & 0x0003F000 ) >> 12;
             out [ outIndex++ ] = alphabet [ bits6 ];
             bits6 = ( bits24 & 0x00000FC0 ) >> 6;
             out [ outIndex++ ] = alphabet [ bits6 ];

             // padding
             out [ outIndex++ ] = '=';
         } else if ( octetString.length - i == 1 ) {
             // store the octets
             bits24 = ( octetString [ i ] & 0xFF ) << 16;

             bits6 = ( bits24 & 0x00FC0000 ) >> 18;
             out [ outIndex++ ] = alphabet [ bits6 ];
             bits6 = ( bits24 & 0x0003F000 ) >> 12;
             out [ outIndex++ ] = alphabet [ bits6 ];

             // padding
             out [ outIndex++ ] = '=';
             out [ outIndex++ ] = '=';
         }

         return new String ( out );
     }

    /**
     * Stream content to a content handler or to an XMLConsumer.
     *
     * @throws ResourceNotFoundException if file not found or
     *         HTTP location does not exist.
     * @throws SAXException if failed to parse source document.
     */
    public void toSAX(ContentHandler handler)
        throws SAXException, ProcessingException
    {
        Parser parser = null;
        try {
            parser = (Parser)this.manager.lookup(Parser.ROLE);

            if (handler instanceof XMLConsumer) {
                parser.setConsumer((XMLConsumer)handler);
            } else {
                parser.setContentHandler(handler);
                if (handler instanceof LexicalHandler) {
                    parser.setLexicalHandler((LexicalHandler)handler);
                }
            }
            parser.parse(this.getInputSource());
        } catch (ProcessingException e){
            // Preserve original exception
            throw e;
        } catch (SAXException e) {
            // Preserve original exception
            throw e;
        } catch (Exception e){
            throw new ProcessingException("Exception during processing of "
                                          + this.systemId, e);
        } finally {
            if (parser != null) this.manager.release(parser);
        }
    }

    public void recycle()
    {
    }
}
