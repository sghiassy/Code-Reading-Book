/*
 *  Copyright (C) The Apache Software Foundation. All rights reserved.        *
 *  ------------------------------------------------------------------------- *
 *  This software is published under the terms of the Apache Software License *
 *  version 1.1, a copy of which has been included  with this distribution in *
 *  the LICENSE file.                                                         *
 */
package org.apache.cocoon.components.search;
import java.io.File;

import java.io.IOException;
import java.net.URL;
import java.util.Iterator;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.AbstractLoggable;

import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.util.ClassUtils;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.document.DateField;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.*;
import org.apache.lucene.queryParser.*;
import org.apache.lucene.search.*;
import org.apache.lucene.store.Directory;
import org.apache.lucene.store.FSDirectory;

/**
 * This class provides searching via lucene.
 *
 * <p>
 *   In order to do searching you need a lucene Directory where the lucene generated
 *   index resides. 
 *   Moreover you must know the lucene Analyzer which has been used for
 *   indexing, and which will be used for searching.
 * </p>
 * <p>
 *   Knowing this you can may start searching having a query which is parsable
 *   by an QueryParser, and having the name of the default field to use in
 *   searching.
 * </p>
 * <p>
 *   This class returns an Hit object as its search result.
 * </p>
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Revision: 1.2 $ $Date: 2002/01/23 19:06:38 $
 */
public class SimpleLuceneCocoonSearcherImpl extends AbstractLoggable
         implements LuceneCocoonSearcher, Configurable, Composable, Disposable, Recyclable
{

    /**
     * Configuration element name of lucene's Analyzer class.
     * <p>
     *   Its value is
     *   <code>analyzer-classname</code>.
     * </p>
     *
     * @since
     */
    protected final static String ANALYZER_CLASSNAME_CONFIG = "analyzer-classname";
    /**
     * Configuration element default value of lucene's Analyzer class.
     * <p>
     *   Its value is,
     *   <code>org.apache.lucene.analysis.standard.StandardAnalyzer</code>.
     * </p>
     *
     * @since
     */
    protected final static String ANALYZER_CLASSNAME_DEFAULT = "org.apache.lucene.analysis.standard.StandardAnalyzer";

    /**
     * Configuration element name of default search field.
     * <p>
     *   Its value is
     *   <code>default-seach-field</code>.
     * </p>
     *
     * @since
     */
    protected final static String DEFAULT_SEARCH_FIELD_CONFIG = "default-search-field";
    /**
     * Configuration element default value of lucene's default search field.
     * <p>
     *   Its value is <code>body</code>.
     * </p>
     *
     * @since
     */
    protected final static String DEFAULT_SEARCH_FIELD_DEFAULT = "body";

    /**
     * Configuration element name of default-query.
     * <p>
     *   Its value is
     *   <code>default-query</code>.
     * </p>
     *
     * @since
     */
    protected final static String DEFAULT_QUERY_CONFIG = "default-query";
    /**
     * Configuration element default value of default-query.
     * <p>
     *   Its value is <code>null</code>.
     * </p>
     *
     * @since
     */
    protected final static String DEFAULT_QUERY_DEFAULT = null;

    /**
     * Configuration element name of query parser class name.
     * <p>
     *   Its value is
     *   <code>queryparser-classname</code>.
     * </p>
     *
     * @since
     */
    protected final static String QUERYPARSER_CLASSNAME_CONFIG = "queryparser-classname";
    /**
     * Configuration element default value of queryparser-classname.
     * <p>
     *   Its value is 
     *   <code>org.apache.lucene.queryParser.QueryParser</code>.
     * </p>
     *
     * @since
     */
    protected final static String QUERYPARSER_CLASSNAME_DEFAULT = "org.apache.lucene.queryParser.QueryParser";

    /**
     * Configuration element name of lucene's default filesystem default
     * directory.
     * <p>
     *   Its value is <code>directory</code>.
     * </p>
     *
     * @since
     */
    protected final static String DIRECTORY_CONFIG = "directory";
    /**
     * Configuration element default value of filesystem default directory.
     * <p>
     *   Its value is <code>null</code>.
     * </p>
     *
     * @since
     */
    protected final static String DIRECTORY_DEFAULT = null;

    /**
     * The component manager instance
     *
     * @since
     */
    protected ComponentManager manager = null;
    
    private String analyzerClassnameDefault = ANALYZER_CLASSNAME_DEFAULT;
    private String defaultSearchFieldDefault = DEFAULT_SEARCH_FIELD_DEFAULT;
    private String defaultQueryDefault = DEFAULT_QUERY_DEFAULT;
    private String queryparserClassnameDefault = QUERYPARSER_CLASSNAME_DEFAULT;
    private String directoryDefault = DIRECTORY_DEFAULT;

    /**
     * The lucene analyzer used for searching
     */
    private Analyzer analyzer;
    /**
     * The lucene directory used for searching
     */
    private Directory directory;
    /**
     * The lucene index searcher used for searching
     */
    private IndexSearcher indexSearcher;

    /**
     * A lucene index reader cache to maximize sharing of
     * lucene index readers
     */
    private IndexReaderCache indexReaderCache;

    /**
     * set an analyzer, overriding the analyzerClassnameDefault.
     *
     * @param  analyzer  The new analyzer value
     * @since
     */
    public void setAnalyzer(Analyzer analyzer) {
        this.analyzer = analyzer;
    }


    /**
     *Sets the directory attribute of the SimpleLuceneCocoonSearcherImpl object
     *
     * @param  directory  The new directory value
     * @since
     */
    public void setDirectory(Directory directory) {
        this.directory = directory;
        if (indexReaderCache != null) {
            indexReaderCache.close();
            indexReaderCache = null;
        }
    }


    /**
     * Get an IndexReader.
     * <p>
     *   As an IndexReader might be cached, it is check if the indexReader is
     *   still valid.
     * </p>
     *
     * @return                  IndexReader an up to date indexReader
     * @exception  IOException  Description of Exception
     * @since
     */
    public IndexReader getReader() throws IOException {
        if (indexReaderCache == null) {
            indexReaderCache = new IndexReaderCache();
        }
        return indexReaderCache.getIndexReader(directory);
    }


    /**
     * configure
     *
     * @param  conf                        Description of Parameter
     * @exception  ConfigurationException  Description of Exception
     * @since
     */
    public void configure(Configuration conf) throws ConfigurationException {
        Configuration child;
        String value;

        child = conf.getChild(ANALYZER_CLASSNAME_CONFIG, false);
        if (child != null) {
            value = conf.getValue(ANALYZER_CLASSNAME_DEFAULT);
            if (value != null) {
                analyzerClassnameDefault = value;
                try {
                    analyzer = (Analyzer) ClassUtils.newInstance(analyzerClassnameDefault);
                } catch (Exception e) {
                    throw new ConfigurationException("Cannot create analyzer of class " +
                            analyzerClassnameDefault, e);
                }
            }
        }

        child = conf.getChild(DEFAULT_SEARCH_FIELD_CONFIG, false);
        if (child != null) {
            value = conf.getValue(DEFAULT_SEARCH_FIELD_DEFAULT);
            if (value != null) {
                defaultSearchFieldDefault = value;
            }
        }

        child = conf.getChild(DEFAULT_QUERY_CONFIG, false);
        if (child != null) {
            value = conf.getValue(DEFAULT_QUERY_DEFAULT);
            if (value != null) {
                defaultQueryDefault = value;
            }
        }

        child = conf.getChild(QUERYPARSER_CLASSNAME_CONFIG, false);
        if (child != null) {
            value = conf.getValue(QUERYPARSER_CLASSNAME_DEFAULT);
            if (value != null) {
                queryparserClassnameDefault = value;
            }
        }

        child = conf.getChild(DIRECTORY_CONFIG, false);
        if (child != null) {
            value = conf.getValue(DIRECTORY_DEFAULT);
            if (value != null) {
                directoryDefault = value;
                try {
                    setDirectory(FSDirectory.getDirectory(new File(directoryDefault), false));
                } catch (IOException ioe) {
                    throw new ConfigurationException("Cannot set index directory " + directoryDefault, ioe);
                }
            }
        }
    }


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
     *Description of the Method
     *
     * @since
     */
    public void dispose() {
        releaseIndexSearcher();
        releaseIndexReaderCache();
    }


    /**
     *Description of the Method
     *
     * @since
     */
    public void recycle() {
        releaseIndexSearcher();
        releaseIndexReaderCache();
    }


    /**
     *Description of the Method
     *
     * @param  query_string             Description of Parameter
     * @param  default_field            Description of Parameter
     * @return                          Description of the Returned Value
     * @exception  ProcessingException  Description of Exception
     * @since
     */
    public Hits search(String query_string, String default_field) throws ProcessingException {
        Hits hits = null;

        if (query_string == null) {
            query_string = defaultQueryDefault;
        }
        if (default_field == null) {
            default_field = defaultSearchFieldDefault;
        }

        try {
            Query query = QueryParser.parse(query_string, default_field, analyzer);

            // release index searcher for each new search
            releaseIndexSearcher();

            IndexSearcher indexSearcher = new IndexSearcher(getReader());
            hits = indexSearcher.search(query);
            // do not close indexSearcher now, as using hits needs an
            // opened indexSearcher indexSearcher.close();
        } catch (ParseException pe) {
            throw new ProcessingException("Cannot parse query " + query_string, pe);
        } catch (IOException ioe) {
            throw new ProcessingException("Cannot access hits", ioe);
        }
        return hits;
    }


    /**
     *Description of the Method
     *
     * @since
     */
    private void releaseIndexSearcher() {
        if (indexSearcher != null) {
            try {
                indexSearcher.close();
            } catch (IOException ioe) {
                // ignore it
            }
            indexSearcher = null;
        }
    }


    /**
     *Description of the Method
     *
     * @since
     */
    private void releaseIndexReaderCache() {
        if (indexReaderCache != null) {
            indexReaderCache = null;
        }
    }


    /**
     * This class should help to minimise usage of IndexReaders.
     *
     * @author     huberb1
     * @version
     */
    static class IndexReaderCache
    {
        private IndexReader indexReader;
        private long lastModified;


        /**
         * Create an IndexReaderCache.
         *
         * @since
         */
        IndexReaderCache() { }


        /**
         * return cached IndexReader object.
         *
         * @param  directory  lucene index directory
         * @return            The indexReader value
         * @since
         */
        public IndexReader getIndexReader(Directory directory) throws IOException {
            if (indexReader == null) {
                createIndexReader(directory);
            } else {
                if (!indexReaderIsValid(directory)) {
                    createIndexReader(directory);
                }
            }
            return indexReader;
        }


        /**
         * Close an opened lucene IndexReader
         *
         * @since
         */
        public void close() {
            if (indexReader != null) {
                try {
                    indexReader.close();
                } catch (IOException ioe) {
                    // ignore it
                }
                indexReader = null;
            }
        }


        /**
         * Check if cached IndexReader is up to date.
         *
         * @param  directory        lucene index directory
         * @return                  boolean  return true if there is a cached IndexReader object,
         *   and its lastModified date is greater equal than the lastModified date
         *   of its lucene Directory.
         * @exception  IOException  Description of Exception
         * @since
         */
        public boolean indexReaderIsValid(Directory directory) throws IOException {
            return indexReader != null &&
                    indexReader.lastModified(directory) == lastModified;
        }


        /**
         *  Release all resources, most notably the lucene IndexReader.
         *
         * @exception  Throwable  Description of Exception
         * @since
         */
        protected void finalize() throws Throwable {
            close();
        }


        /**
         * Create unconditionally a lucene IndexReader.
         *
         * @param  directory        lucene index directory
         * @exception  IOException  Description of Exception
         * @since
         */
        private void createIndexReader(Directory directory) throws IOException {
            close();
            indexReader = IndexReader.open(directory);
            lastModified = indexReader.lastModified(directory);
        }
    }
}

