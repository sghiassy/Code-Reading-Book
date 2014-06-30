/*
 *  Copyright (C) The Apache Software Foundation. All rights reserved.        *
 *  ------------------------------------------------------------------------- *
 *  This software is published under the terms of the Apache Software License *
 *  version 1.1, a copy of which has been included  with this distribution in *
 *  the LICENSE file.                                                         *
 */
package org.apache.cocoon.components.search;

import java.net.URL;

import org.apache.avalon.framework.component.Component;
import org.apache.cocoon.ProcessingException;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.store.Directory;

/**
 * The avalon behavioural component interface of an indexer.
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Id: LuceneCocoonIndexer.java,v 1.2 2002/01/23 19:06:38 vgritsenko Exp $
 */
public interface LuceneCocoonIndexer extends Component
{
    /**
     *Description of the Field
     *
     * @since
     */
    String ROLE = "org.apache.cocoon.components.search.LuceneCocoonIndexer";


    /**
     *Sets the analyzer attribute of the LuceneCocoonIndexer object
     *
     * @param  analyzer  The new analyzer value
     * @since
     */
    void setAnalyzer(Analyzer analyzer);


    /**
     *Description of the Method
     *
     * @param  index                    Description of Parameter
     * @param  create                   Description of Parameter
     * @param  base_url                 Description of Parameter
     * @exception  ProcessingException  Description of Exception
     * @since
     */
    void index(Directory index, boolean create, URL base_url)
             throws ProcessingException;
}
