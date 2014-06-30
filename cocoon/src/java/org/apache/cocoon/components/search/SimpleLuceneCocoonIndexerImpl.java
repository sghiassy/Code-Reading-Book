/*
 *  Copyright (C) The Apache Software Foundation. All rights reserved.        *
 *  ------------------------------------------------------------------------- *
 *  This software is published under the terms of the Apache Software License *
 *  version 1.1, a copy of which has been included  with this distribution in *
 *  the LICENSE file.                                                         *
 */
package org.apache.cocoon.components.search;

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

import org.apache.cocoon.components.crawler.CocoonCrawler;
import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.document.DateField;

import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.*;
import org.apache.lucene.store.Directory;

/**
 * A lucene indexer.
 *
 * <p>
 *  XML documents are indexed using lucene.
 *  Links to XML documents are supplied by
 *  a crawler, requesting links of documents by specifying a cocoon-view, and
 *  HTTP protocol.
 * </p>
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Revision: 1.3 $ $Date: 2002/01/25 04:34:18 $
 */
public class SimpleLuceneCocoonIndexerImpl extends AbstractLoggable
         implements LuceneCocoonIndexer, Configurable, Composable, Disposable
{

    /**
     *Description of the Field
     *
     * @since
     */
    public final static String ANALYZER_CLASSNAME_CONFIG = "analyzer-classname";
    /**
     *Description of the Field
     *
     * @since
     */
    public final static String ANALYZER_CLASSNAME_DEFAULT = "org.apache.lucene.analysis.standard.StandardAnalyzer";

    /**
     *Description of the Field
     *
     * @since
     */
    public final static String DIRECTORY_CONFIG = "directory";
    /**
     *Description of the Field
     *
     * @since
     */
    public final static String DIRECTORY_DEFAULT = null;

    /**
     *Description of the Field
     *
     * @since
     */
    public final static String MERGE_FACTOR_CONFIG = "merge-factor";
    /**
     *Description of the Field
     *
     * @since
     */
    public final static int MERGE_FACTOR_DEFAULT = 20;

    /**
     * The component manager instance
     *
     * @since
     */
    protected ComponentManager manager = null;

    Analyzer analyzer;
    private String analyzerClassnameDefault = ANALYZER_CLASSNAME_DEFAULT;
    private String directoryDefault = DIRECTORY_DEFAULT;
    private int mergeFactor = MERGE_FACTOR_DEFAULT;


    /**
     *Sets the analyzer attribute of the SimpleLuceneCocoonIndexerImpl object
     *
     * @param  analyzer  The new analyzer value
     * @since
     */
    public void setAnalyzer(Analyzer analyzer) {
        this.analyzer = analyzer;
    }


    /**
     *Description of the Method
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
            }
        }
        child = conf.getChild(MERGE_FACTOR_CONFIG, false);
        if (child != null) {
            int int_value = conf.getValueAsInteger(MERGE_FACTOR_DEFAULT);
            mergeFactor = int_value;
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
    public void dispose() { }


    /**
     * index content of base_url, index content of links from base_url.
     *
     * @param  index                    the lucene store to write the index to
     * @param  create                   iff true create, or overwrite existing index, else
     *   update existing index.
     * @param  base_url                 index content of base_url, and crawl through all its
     *   links recursivly.
     * @exception  ProcessingException  Description of Exception
     * @since
     */
    public void index(Directory index, boolean create, URL base_url)
             throws ProcessingException {

        IndexWriter writer = null;
        LuceneXMLIndexer lxi = null;
        CocoonCrawler cocoonCrawler = null;

        try {
            lxi = (LuceneXMLIndexer) manager.lookup(LuceneXMLIndexer.ROLE);

            writer = new IndexWriter(index, analyzer, create);
            writer.mergeFactor = this.mergeFactor;

            cocoonCrawler = (CocoonCrawler) manager.lookup(CocoonCrawler.ROLE);
            cocoonCrawler.crawl(base_url);

            Iterator cocoonCrawlerIterator = cocoonCrawler.iterator();
            while (cocoonCrawlerIterator.hasNext()) {
                URL crawl_url = (URL) cocoonCrawlerIterator.next();

                if (!crawl_url.getHost().equals(base_url.getHost()) ||
                        crawl_url.getPort() != base_url.getPort()) {

                    // skip urls using different host, or port than host,
                    // or port of base url
                    if (getLogger().isDebugEnabled()) {
                        getLogger().debug("Skipping crawling URL " + crawl_url.toString() +
                            " as base_url is " + base_url.toString());
                    }
                    continue;
                }

                // build lucene documents from the content of the crawl_url
                Iterator i = lxi.build(crawl_url).iterator();

                // add all built lucene documents
                while (i.hasNext()) {
                    writer.addDocument((Document) i.next());
                }
            }
            // optimize it
            writer.optimize();
        } catch (IOException ioe) {
            throw new ProcessingException("IOException in index()", ioe);
        } catch (ComponentException ce) {
            throw new ProcessingException("ComponentException in index()", ce);
        } finally {
            if (writer != null) {
                try {
                    writer.close();
                } catch (IOException ioe) {
                }
                writer = null;
            }

            if (lxi != null) {
                manager.release(lxi);
                lxi = null;
            }
            if (cocoonCrawler != null) {
                manager.release(cocoonCrawler);
                cocoonCrawler = null;
            }
        }
    }


    /**
     *Description of the Class
     *
     * @author     huberb1
     * @version
     */
    class DocumentDeletableIterator
    {
        private IndexReader reader;
        // existing index
        private TermEnum uidIter;

        // document id iterator


        /**
         *Constructor for the DocumentDeletableIterator object
         *
         * @param  directory        Description of Parameter
         * @exception  IOException  Description of Exception
         * @since
         */
        public DocumentDeletableIterator(Directory directory) throws IOException {
            reader = IndexReader.open(directory);
            // open existing index
            uidIter = reader.terms(new Term("uid", ""));
            // init uid iterator
        }


        /**
         *Description of the Method
         *
         * @exception  IOException  Description of Exception
         * @since
         */
        public void deleteAllStaleDocuments() throws IOException {
            while (uidIter.term() != null && uidIter.term().field() == "uid") {
                reader.delete(uidIter.term());
                uidIter.next();
            }
        }


        /**
         *Description of the Method
         *
         * @param  uid              Description of Parameter
         * @exception  IOException  Description of Exception
         * @since
         */
        public void deleteModifiedDocuments(String uid) throws IOException {
            while (documentHasBeenModified(uidIter.term(), uid)) {
                reader.delete(uidIter.term());
                uidIter.next();
            }
            if (documentHasNotBeenModified(uidIter.term(), uid)) {
                uidIter.next();
            }
        }


        /**
         *Description of the Method
         *
         * @exception  Throwable  Description of Exception
         * @since
         */
        protected void finalize() throws Throwable {
            super.finalize();
            if (uidIter != null) {
                uidIter.close();
                // close uid iterator
                uidIter = null;
            }
            if (reader != null) {
                reader.close();
                // close existing index
                reader = null;
            }
        }


        /**
         *Description of the Method
         *
         * @param  term  Description of Parameter
         * @return       Description of the Returned Value
         * @since
         */
        boolean documentIsDeletable(Term term) {
            return term != null && term.field() == "uid";
        }


        /**
         *Description of the Method
         *
         * @param  term  Description of Parameter
         * @param  uid   Description of Parameter
         * @return       Description of the Returned Value
         * @since
         */
        boolean documentHasBeenModified(Term term, String uid) {
            return documentIsDeletable(term) &&
                    term.text().compareTo(uid) < 0;
        }


        /**
         *Description of the Method
         *
         * @param  term  Description of Parameter
         * @param  uid   Description of Parameter
         * @return       Description of the Returned Value
         * @since
         */
        boolean documentHasNotBeenModified(Term term, String uid) {
            return documentIsDeletable(term) &&
                    term.text().compareTo(uid) == 0;
        }
    }
}

