/**
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 */
package org.apache.cocoon.components.search;

import java.net.URL;
import java.util.Iterator;
import java.util.List;

import org.apache.avalon.framework.component.Component;

import org.apache.cocoon.ProcessingException;
import org.apache.lucene.document.Document;

/**
 * The avalon behavioural component interface of generating
 * lucene documents from an xml content.
 *
 * <p>
 *  The well-known fields of a lucene documents are defined as
 *  <code>*_FIELD</code> constants.
 * </p>
 * <p>
 *  You may access generated lucene documents via
 *  <code>allDocuments()</code>, or <code>iterator()</code>.
 * </p>
 * <p>
 *  You trigger the generating of lucene documents via
 *  <code>build()</code>.
 * </p>
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Id: LuceneXMLIndexer.java,v 1.3 2002/01/25 04:34:18 vgritsenko Exp $
 */
public interface LuceneXMLIndexer extends Component
{

    /**
     * The ROLE name of this avalon component.
     * <p>
     *   Its value if the FQN of this interface, 
     *   ie. <code>org.apache.cocoon.components.search.LuceneXMLIndexer</code>. 
     * </p>
     *
     * @since
     */
    String ROLE = "org.apache.cocoon.components.search.LuceneXMLIndexer";

    /**
     * A Lucene document field name, containing xml content text of all xml elements.
     * <p>
     *   A concrete implementation of this interface SHOULD
     *   provides a field named body.
     * </p>
     * <p>
     *   A concrete implementation MAY provide additional lucene
     *   document fields.
     * </p>
     *
     * @since
     */
    String BODY_FIELD = "body";

    /**
     * A Lucene document field name, containg the URI/URL of the indexed
     * document.
     * <p>
     *   A concrete implementation of this interface SHOULD
     *   provide a field named url.
     * </p>
     *
     * @since
     */
    String URL_FIELD = "url";

    /**
     * A Lucene document field name, containg the a unique key of the indexed
     * document.
     * <p>
     *  This document field is used internally to track document
     *  changes, and updates.
     * </p>
     * <p>
     *   A concrete implementation of this interface SHOULD
     *   provide a field named uid.
     * </p>
     *
     * @since
     */
    String UID_FIELD = "uid";

    /**
     * Build lucene documents from a URL.
     * <p>
     *   This method will read the content of the URL, and generates
     *   one or more lucene documents. The generated lucence documents
     *   can be fetched using methods allDocuments(), and iterator().
     * </p>
     *
     * @param  url                      the content of this url gets indexed.
     * @exception  ProcessingException  Description of Exception
     * @since
     */
    List build(URL url) throws ProcessingException;
}
