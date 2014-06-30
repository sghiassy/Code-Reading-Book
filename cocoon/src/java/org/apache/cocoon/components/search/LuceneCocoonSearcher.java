/*
 *  Copyright (C) The Apache Software Foundation. All rights reserved.        *
 *  ------------------------------------------------------------------------- *
 *  This software is published under the terms of the Apache Software License *
 *  version 1.1, a copy of which has been included  with this distribution in *
 *  the LICENSE file.                                                         *
 */
package org.apache.cocoon.components.search;

import org.apache.avalon.framework.component.Component;
import org.apache.cocoon.ProcessingException;
import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.search.Hits;
import org.apache.lucene.store.Directory;

/**
 * The avalon behavioural component interface of a searcher.
 * <p>
 *   This component defines an interface for searching.
 *   The idea is to abstract the process of searching having a query string,
 *   and an index, and generating hits which matches the query string in the index.
 * </p>
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Id: LuceneCocoonSearcher.java,v 1.1 2002/01/03 12:31:13 giacomo Exp $
 */
public interface LuceneCocoonSearcher extends Component
{
    /**
     * The ROLE name of this avalon component.
     * <p>
     *   Its value if the FQN of this interface, 
     *   ie. <code>org.apache.cocoon.components.search.LuceneCocoonSearcher</code>. 
     * </p>
     *
     * @since
     */
    String ROLE = "org.apache.cocoon.components.search.LuceneCocoonSearcher";


    /**
     * Sets the analyzer attribute of the LuceneCocoonSearcher object
     * <p>
     *   The analyzer determines the tokenization of the query,
     *   and strategy of matching.
     * </p>
     * <p>
     *   The analyzer class defined here should be equivalent to the analyzer
     *   class used when creating the index used for searching.
     * </p>
     *
     * @param  analyzer  The new analyzer value
     * @since
     */
    void setAnalyzer(Analyzer analyzer);


    /**
     * Sets the directory attribute of the LuceneCocoonSearcher object
     * <p>
     *   The directory specifies the directory used for looking up the 
     *   index. It defines the physical place of the index
     * </p>
     *
     * @param  directory  The new directory value
     * @since
     */
    void setDirectory(Directory directory);


    /**
     * Search a query-string, returning zero, or more hits.
     * <p>
     * </p>
     *
     * @param  query_string             A query string parsable by a query parser.
     * @param  default_field            The default field of the query string.
     * @return                          Hits zero or more hits matching the query string
     * @exception  ProcessingException  throwing due to processing errors while
     *   looking up the index directory, parsing the query string, generating the hits.
     * @since
     */
    Hits search(String query_string, String default_field) throws ProcessingException;
}

