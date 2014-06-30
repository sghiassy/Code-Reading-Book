/*
 *  Copyright (C) The Apache Software Foundation. All rights reserved.        *
 *  ------------------------------------------------------------------------- *
 *  This software is published under the terms of the Apache Software License *
 *  version 1.1, a copy of which has been included  with this distribution in *
 *  the LICENSE file.                                                         *
 */
package org.apache.cocoon.components.search;

import java.io.*;

import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;

import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import javax.xml.parsers.*;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.AbstractLoggable;

import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.ProcessingException;

import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.lucene.document.DateField;

import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.xml.sax.Attributes;
import org.xml.sax.ContentHandler;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;

/**
 * A simple class building lucene documents from xml content.
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Revision: 1.3 $ $Date: 2002/01/25 04:34:18 $
 */
public class SimpleLuceneXMLIndexerImpl extends AbstractLoggable
         implements LuceneXMLIndexer, Configurable, Composable, ThreadSafe
{

    /**
     * The component manager instance
     *
     * @since
     */
    protected ComponentManager manager = null;

    /**
     * append this string to the url in order to get the
     * content view of the url
     *
     * @since
     */
    final String CONTENT_QUERY = "cocoon-view=content";

    /**
     * set of allowed content types
     *
     * @since
     */
    final HashSet allowedContentType;


    /**
     * @since
     */
    public SimpleLuceneXMLIndexerImpl() {
        allowedContentType = new HashSet();
        allowedContentType.add("text/xml");
        allowedContentType.add("text/xhtml");
    }


    /**
     * configure
     *
     * @param  conf                        Description of Parameter
     * @exception  ConfigurationException  Description of Exception
     * @since
     */
    public void configure(Configuration conf) throws ConfigurationException { }


    /**
     * Set the current <code>ComponentManager</code> instance used by this
     * <code>Composable</code>.
     *
     * @param  manager                 Description of Parameter
     * @exception  ComponentException  Description of Exception
     * @since
     */
    public void compose(ComponentManager manager) throws ComponentException {
        this.manager = manager;
    }


    /**
     * Build lucenen documents from a URL
     *
     * @param  url                      the content of this url gets indexed.
     * @exception  ProcessingException  Description of Exception
     * @since
     */
    public List build(URL url)
             throws ProcessingException {

        try {
            URL contentURL = new URL(url, url.getPath()
                + ((url.getPath().indexOf("?") == -1) ? "?" : "&")
                + CONTENT_QUERY);
            URLConnection contentURLConnection = contentURL.openConnection();
            String contentType = contentURLConnection.getContentType();
            if (contentType != null &&
                    allowedContentType.contains(contentType)) {

                if (getLogger().isDebugEnabled()) {
                    getLogger().debug("Indexing " + contentURL + " (" + contentType + ")");
                }

                LuceneIndexContentHandler luceneIndexContentHandler = new LuceneIndexContentHandler();
                indexDocument(contentURLConnection, luceneIndexContentHandler);
                //
                // document is parsed
                //
                Iterator it = luceneIndexContentHandler.iterator();
                while (it.hasNext()) {
                    Document d = (Document) it.next();
                    d.add(Field.UnIndexed(URL_FIELD, url.toString()));
                    // store ... false, index ... true, token ... false
                    d.add(new Field(UID_FIELD, uid(contentURLConnection), false, true, false));
                }

                return luceneIndexContentHandler.allDocuments();
            } else {
                if (getLogger().isDebugEnabled()) {
                    getLogger().debug("Ignoring " + contentURL + " (" + contentType + ")");
                }

                return java.util.Collections.EMPTY_LIST;
            }
        } catch (IOException ioe) {
            throw new ProcessingException("Cannot read URL " + url, ioe);
        }
    }


    /**
     * index input stream producing lucene Documents
     *
     * @param  contentURLConnection       the xml content which should get indexed.
     * @param  luceneIndexContentHandler  ContentHandler for generating
     *   a lucene Document from XML content.
     * @exception  ProcessingException    Description of Exception
     * @since
     */
    private void indexDocument(URLConnection contentURLConnection,
            LuceneIndexContentHandler luceneIndexContentHandler)
             throws ProcessingException {

        InputStream is = null;
        InputSource in = null;
        Parser parser = null;

        try {
            is = contentURLConnection.getInputStream();
            in = new InputSource(is);

            // get an XML parser
            parser = (Parser) this.manager.lookup(Parser.ROLE);
            //reader.setErrorHandler(new CocoonErrorHandler());
            parser.setContentHandler(luceneIndexContentHandler);
            parser.parse(in);
            //
            // document is parsed
            //
        } catch (IOException ioe) {
            throw new ProcessingException("Cannot read!", ioe);
        } catch (SAXException saxe) {
            throw new ProcessingException("Cannot parse!", saxe);
        } catch (ComponentException ce) {
            throw new ProcessingException("Cannot lookup xml parser!", ce);
        } finally {
            if (parser != null) {
                this.manager.release(parser);
            }
        }
    }


    /**
     * return a unique uid of a url connection
     *
     * @param  urlConnection  Description of Parameter
     * @return                String unique uid of a urlConnection
     * @since
     */
    private String uid(URLConnection urlConnection) {
        // Append path and date into a string in such a way that lexicographic
        // sorting gives the same results as a walk of the file hierarchy.  Thus
        // null (\u0000) is used both to separate directory components and to
        // separate the path from the date.
        return urlConnection.toString().replace('/', '\u0000') +
                "\u0000" +
                DateField.timeToString(urlConnection.getLastModified());
    }
}

