/**
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 */
package org.apache.cocoon.components.crawler;


import org.apache.avalon.framework.component.Component;

import java.net.URL;
import java.util.Iterator;

/**
 * The avalon behavioural component interface of crawling.
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Id: CocoonCrawler.java,v 1.2 2002/01/23 19:06:38 vgritsenko Exp $
 */
public interface CocoonCrawler extends Component
{
    /**
     * Role name of this avalon component.
     * Its value is <code>org.apache.cocoon.components.crawler.CocoonCrawler</code>.
     *
     * @since
     */
    String ROLE = "org.apache.cocoon.components.crawler.CocoonCrawler";


    /**
     * start crawling the URL.
     * <p>
     *   Calling this method initiates the crawling.
     * </p>
     *
     * @param  url  Description of Parameter
     * @since
     */
    void crawl(URL url);


    /**
     * Iterate over crawling URLs.
     * <p>
     *   This iterator will returns URL as result of crawling 
     *   the base URL passed via crawling().
     * </p>
     *
     * @return    Iterator iterates over crawling URLs.
     * @since
     */
    Iterator iterator();
}

