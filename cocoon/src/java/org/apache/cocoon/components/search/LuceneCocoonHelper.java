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

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.store.FSDirectory;
import org.apache.lucene.store.Directory;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.IndexWriter;

/**
 * This class encapsulates some helper methods.
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Id: LuceneCocoonHelper.java,v 1.2 2002/01/23 19:06:38 vgritsenko Exp $
 */
public class LuceneCocoonHelper
{
    /**
     *Gets the directory attribute of the LuceneCocoonHelper class
     *
     * @param  directory        Description of Parameter
     * @param  create           Description of Parameter
     * @return                  The directory value
     * @exception  IOException  Description of Exception
     * @since
     */
    public static Directory getDirectory(File directory, boolean create) throws IOException {
        FSDirectory fsDirectory = FSDirectory.getDirectory(directory, create);
        return fsDirectory;
    }

    /**
     *Gets the analyzer attribute of the LuceneCocoonHelper class
     *
     * @param  analyzer_class_name  Description of Parameter
     * @return                      The analyzer value
     * @since
     */
    public static Analyzer getAnalyzer(String analyzer_class_name) {
        Analyzer analyzer = null;
        try {
            Class analyzer_class = Class.forName(analyzer_class_name);
            analyzer = (Analyzer) analyzer_class.newInstance();
        } catch (Exception e) {
        }
        return analyzer;
    }

    /**
     *Gets the indexReader attribute of the LuceneCocoonHelper class
     *
     * @param  directory        Description of Parameter
     * @return                  The indexReader value
     * @exception  IOException  Description of Exception
     * @since
     */
    public static IndexReader getIndexReader(Directory directory) throws IOException {
        IndexReader reader = IndexReader.open(directory);
        return reader;
    }

    /**
     *Gets the indexWriter attribute of the LuceneCocoonHelper class
     *
     * @param  index            Description of Parameter
     * @param  analyzer         Description of Parameter
     * @param  create           Description of Parameter
     * @return                  The indexWriter value
     * @exception  IOException  Description of Exception
     * @since
     */
    public static IndexWriter getIndexWriter(Directory index, Analyzer analyzer, boolean create) throws IOException {
        IndexWriter writer = new IndexWriter(index, analyzer, create);
        return writer;
    }
}

