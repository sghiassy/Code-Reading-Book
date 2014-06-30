/**
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 */
package org.apache.cocoon.components.crawler;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.avalon.excalibur.pool.Recyclable;

import org.apache.cocoon.Constants;
import org.apache.cocoon.util.Tokenizer;

import org.apache.log.Logger;

import org.apache.regexp.RE;
import org.apache.regexp.RESyntaxException;

import java.io.InputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.net.URL;
import java.net.URLConnection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;

/**
 * A simple cocoon crawler.
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Id: SimpleCocoonCrawlerImpl.java,v 1.2 2002/01/23 19:06:38 vgritsenko Exp $
 */
public class SimpleCocoonCrawlerImpl extends AbstractLoggable
         implements CocoonCrawler, Configurable, Disposable, Recyclable
{

    /**
     * Config element name specifying expected link content-typ.
     * <p>
     *   Its value is <code>link-content-type</code>.
     * </p>
     *
     * @since
     */
    public final static String LINK_CONTENT_TYPE_CONFIG = "link-content-type";

    /**
     * Default value of <code>link-content-type</code> configuration value.
     * <p>
     *   Its value is <code>application/x-cocoon-links</code>.
     * </p>
     *
     * @since
     */
    public final String LINK_CONTENT_TYPE_DEFAULT = "application/x-cocoon-links";
    
    /**
     * Config element name specifying query-string appendend for requesting links
     * of an URL.
     * <p>
     *  Its value is <code>link-view-query</code>.
     * </p>
     *
     * @since
     */
    public final static String LINK_VIEW_QUERY_CONFIG = "link-view-query";
    /**
     * Default value of <code>link-view-query</code> configuration value.
     * <p>
     *   Its value is <code>?cocoon-view=links</code>.
     * </p>
     *
     * @since
     */
    public final static String LINK_VIEW_QUERY_DEFAULT = "cocoon-view=links";

    /**
     * Config element name specifying excluding regular expression pattern.
     * <p>
     *  Its value is <code>exclude</code>.
     * </p>
     *
     * @since
     */
    public final static String EXCLUDE_CONFIG = "exclude";

    /**
     * Config element name specifying including regular expression pattern.
     * <p>
     *  Its value is <code>include</code>.
     * </p>
     *
     * @since
     */
    public final static String INCLUDE_CONFIG = "include";

    /**
     * Config element name specifying http header value for user-Agent.
     * <p>
     *  Its value is <code>user-agent</code>.
     * </p>
     *
     * @since
     */
    public final static String USER_AGENT_CONFIG = "user-agent";
    /**
     * Default value of <code>user-agent</code> configuration value.
     * <p>
     *   Its value is @see org.apache.cocoon.Constants#COMPLETE_NAME.
     * </p>
     *
     * @since
     */
    public final static String USER_AGENT_DEFAULT = Constants.COMPLETE_NAME;

    /**
     * Config element name specifying http header value for accept.
     * <p>
     *  Its value is <code>accept</code>.
     * </p>
     *
     * @since
     */
    public final static String ACCEPT_CONFIG = "accept";
    /**
     * Default value of <code>accept</code> configuration value.
     * <p>
     *   Its value is <code>* / *</code>
     * </p>
     *
     * @since
     */
    public final static String ACCEPT_DEFAULT = "*/*";

    private String linkViewQuery = LINK_VIEW_QUERY_DEFAULT;
    private String linkContentType = LINK_CONTENT_TYPE_DEFAULT;
    private HashSet excludeCrawlingURL;
    private HashSet includeCrawlingURL;
    private String userAgent = USER_AGENT_DEFAULT;
    private String accept = ACCEPT_DEFAULT;

    private HashSet crawled;
    private HashSet urlsToProcess;


    /**
     * Constructor for the SimpleCocoonCrawlerImpl object
     *
     * @since
     */
    public SimpleCocoonCrawlerImpl() {
        // by default include everything
        includeCrawlingURL = null;
        // by default exclude common image patterns
        excludeCrawlingURL = null;
    }


    /**
     * Configure the crawler component.
     * <p>
     *  Configure can specify which URI to include, and which URI to exclude
     *  from crawling. You specify the patterns as regular expressions.
     * </p>
     * <p>
     *  Morover you can configure
     *  the required content-type of crawling request, and the
     *  query-string appended to each crawling request.
     * </p>
     * <pre><tt>
     * &lt;include&gt;.*\.html?&lt;/exclude&gt; or &lt;exclude&gt;.*\.html?, .*\.xsp&lt;/exclude&gt;
     * &lt;exclude&gt;.*\.gif&lt;/exclude&gt; or &lt;exclude&gt;.*\.gif, .*\.jpe?g&lt;/exclude&gt;
     * &lt;link-content-type&gt; application/x-cocoon-links &lt;/link-content-type&gt;
     * &lt;link-view-query&gt; ?cocoon-view=links &lt;/link-view-query&gt;
     * </tt></pre>
     *
     * @param  configuration               XML configuration of this avalon component.
     * @exception  ConfigurationException  is throwing if configuration is invalid.
     * @since
     */
    public void configure(Configuration configuration)
             throws ConfigurationException {

        Configuration[] children;
        children = configuration.getChildren(INCLUDE_CONFIG);
        if (children != null && children.length > 0) {
            includeCrawlingURL = new HashSet();
            for (int i = 0; i < children.length; i++) {
                String pattern = children[i].getValue();
                try {
                    Tokenizer t = new Tokenizer(pattern, ", ");
                    while (t.hasMoreTokens()) {
                        String tokenized_pattern = t.nextToken();
                        this.includeCrawlingURL.add(new RE(tokenized_pattern));
                    }
                } catch (RESyntaxException rese) {
                    getLogger().error("Cannot create includeing regular-expression for " + 
                      pattern, rese);
                }
            }
        }

        children = configuration.getChildren(EXCLUDE_CONFIG);
        if (children != null && children.length > 0) {
            excludeCrawlingURL = new HashSet();
            for (int i = 0; i < children.length; i++) {
                String pattern = children[i].getValue();
                try {
                    Tokenizer t = new Tokenizer(pattern, ", ");
                    while (t.hasMoreTokens()) {
                        String tokenized_pattern = t.nextToken();
                        this.excludeCrawlingURL.add(new RE(tokenized_pattern));
                    }
                } catch (RESyntaxException rese) {
                    getLogger().error("Cannot create excluding regular-expression for " + 
                      pattern, rese);
                }
            }
        } else {
            excludeCrawlingURL = new HashSet();
            setDefaultExcludeFromCrawling();
        }

        Configuration child;
        String value;
        child = configuration.getChild(LINK_CONTENT_TYPE_CONFIG, false);
        if (child != null) {
            value = child.getValue();
            if (value != null && value.length() > 0) {
                this.linkContentType = value.trim();
            }
        }
        child = configuration.getChild(LINK_VIEW_QUERY_CONFIG, false);
        if (child != null) {
            value = child.getValue();
            if (value != null && value.length() > 0) {
                this.linkViewQuery = value.trim();
            }
        }

        child = configuration.getChild(USER_AGENT_CONFIG, false);
        if (child != null) {
            value = child.getValue();
            if (value != null && value.length() > 0) {
                this.userAgent = value;
            }
        }

        child = configuration.getChild(ACCEPT_CONFIG, false);
        if (child != null) {
            value = child.getValue();
            if (value != null && value.length() > 0) {
                this.accept = value;
            }
        }
    }


    /**
     * dispose at end of life cycle, releasing all resources.
     *
     * @since
     */
    public void dispose() {
        crawled = null;
        urlsToProcess = null;
        excludeCrawlingURL = null;
        includeCrawlingURL = null;
    }


    /**
     * recylcle this object, relasing resources
     *
     * @since
     */
    public void recycle() {
        crawled = null;
        urlsToProcess = null;
    }


    /**
     * Start crawling a URL.
     *
     * <p>
     *   Use this method to start crawling.
     *   Get the this url, and all its children  by using <code>iterator()</code>.
     *   The Iterator object will return URL objects.
     * </p>
     * <p>
     *  You may use the crawl(), and iterator() methods the following way:
     * </p>
     * <pre><tt>
     *   SimpleCocoonCrawlerImpl scci = ....;
     *   scci.crawl( "http://foo/bar" );
     *   Iterator i = scci.iterator();
     *   while (i.hasNext()) {
     *     URL url = (URL)i.next();
     *     ...
     *   }
     * </tt></pre>
     * <p>
     *   The i.next() method returns a URL, and calculates the links of the
     *   URL before return it.
     * </p>
     *
     * @param  url  Crawl this URL, getting all links from this URL.
     * @since
     */
    public void crawl(URL url) {
        crawled = new HashSet();
        urlsToProcess = new HashSet();

        if (getLogger().isDebugEnabled()) {
            getLogger().debug("crawl URL " + url);
        }

        urlsToProcess.add(url);
    }


    /**
     * Return iterator, iterating over all links of the currently crawled URL.
     * <p>
     *   The Iterator object will return URL objects at its <code>next()</code>
     *   method.
     * </p>
     *
     * @return    Iterator iterator of all links from the crawl URL.
     * @since
     */
    public Iterator iterator() {
        return new CocoonCrawlerIterator(this);
    }


    /**
     * Default exclude patterns.
     * <p>
     *   By default URLs matching following patterns are excluded:
     * </p>
     * <ul>
     *   <li>.*\\.gif(\\?.*)?$ - exclude gif images</li>
     *   <li>.*\\.png(\\?.*)?$ - exclude png images</li>
     *   <li>.*\\.jpe?g(\\?.*)?$ - exclude jpeg images</li>
     *   <li>.*\\.js(\\?.*)?$ - exclude javascript </li>
     *   <li>.*\\.css(\\?.*)?$ - exclude cascaded stylesheets</li>
     * </ul>
     *
     * @since
     */
    private void setDefaultExcludeFromCrawling() {
        String[] EXCLUDE_FROM_CRAWLING_DEFAULT = {
                ".*\\.gif(\\?.*)?$",
                ".*\\.png(\\?.*)?$",
                ".*\\.jpe?g(\\?.*)?$",
                ".*\\.js(\\?.*)?$",
                ".*\\.css(\\?.*)?$"
                };

        for (int i = 0; i < EXCLUDE_FROM_CRAWLING_DEFAULT.length; i++) {
            String pattern = EXCLUDE_FROM_CRAWLING_DEFAULT[i];
            try {
                excludeCrawlingURL.add(new RE(pattern));
            } catch (RESyntaxException rese) {
                getLogger().error("Cannot create excluding regular-expression for " +
                        pattern, rese);
            }
        }
    }


    /**
     * Compute list of links from the url.
     * <p>
     *   Check for include, exclude pattern, content-type, and if url
     *   has been craweled already.
     * </p>
     *
     * @param  url  Crawl this URL
     * @return      List of URLs, which are links from url, asserting the conditions.
     * @since
     */
    private List getLinks(URL url) {
        ArrayList url_links = null;

        if (getLogger().isDebugEnabled()) {
            getLogger().debug("getLinks URL " + url);
        }

        if (!isIncludedURL(url.toString())) {
            return null;
        }
        // don't try to get links for url which is excluded
        if (isExcludedURL(url.toString())) {
            return null;
        }

        // don't try to get links for url which has been crawled already
        if (crawled.contains(url.toString())) {
            return null;
        }

        // mark it as crawled
        crawled.add(url.toString());

        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Get links of URL: " + url.toString());
        }

        // get links of url
        try {
            URL links_url = new URL(url, url.getPath()
                + ((url.getPath().indexOf("?") == -1) ? "?" : "&") 
                + linkViewQuery);
            URLConnection links_url_connection = links_url.openConnection();
            InputStream is = links_url_connection.getInputStream();
            BufferedReader br = new BufferedReader(new InputStreamReader(is));

            String content_type = links_url_connection.getContentType();
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("Content-type: " + content_type);
            }

            if (content_type.equals(linkContentType)) {
                url_links = new ArrayList();

                // content is supposed to be a list of links,
                // relative to current URL
                String line;
                while ((line = br.readLine()) != null) {
                    URL new_url = new URL(url, line);
                    boolean add_url = true;
                    // don't add new_url twice
                    if (add_url) {
                        add_url &= !url_links.contains(new_url);
                    }

                    // don't add new_url if it has been crawled already
                    if (add_url) {
                        add_url &= !crawled.contains(new_url.toString());
                    }

                    // don't add if is not matched by existing include definition
                    if (add_url) {
                        add_url &= isIncludedURL(new_url.toString());
                    }

                    // don't add if is matched by existing exclude definition
                    if (add_url) {
                        add_url &= !isExcludedURL(new_url.toString());
                    }
                    if (add_url) {
                        if (getLogger().isDebugEnabled()) {
                            getLogger().debug("Add URL: " + new_url.toString());
                        }
                        url_links.add(new_url);
                    }
                }
                // now we have a list of URL which should be examined
            }
        } catch (IOException ioe) {
            getLogger().warn("Problems get links of " + url, ioe);
        }
        return url_links;
    }


    /**
     * check if URL is a candidate for indexing
     *
     * @param  url  Description of Parameter
     * @return      The excludedURL value
     * @since
     */
    private boolean isExcludedURL(String url) {
        // by default include URL for crawling
        if (excludeCrawlingURL == null) {
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("exclude no URL " + url);
            }
            return false;
        }

        final String s = url.toString();
        Iterator i = excludeCrawlingURL.iterator();
        while (i.hasNext()) {
            RE pattern = (RE) i.next();
            if (pattern.match(s)) {
                if (getLogger().isDebugEnabled()) {
                    getLogger().debug("exclude URL " + url);
                }
                return true;
            }
        }
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("exclude not URL " + url);
        }
        return false;
    }


    /**
     * check if URL is a candidate for indexing
     *
     * @param  url  Description of Parameter
     * @return      The includedURL value
     * @since
     */
    private boolean isIncludedURL(String url) {
        // by default include URL for crawling
        if (includeCrawlingURL == null) {
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("include all URL " + url);
            }
            return true;
        }

        final String s = url.toString();
        Iterator i = includeCrawlingURL.iterator();
        while (i.hasNext()) {
            RE pattern = (RE) i.next();
            if (pattern.match(s)) {
                if (getLogger().isDebugEnabled()) {
                    getLogger().debug("include URL " + url);
                }
                return true;
            }
        }
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("include not URL " + url);
        }
        return false;
    }


    /**
     * Helper class implementing an Iterator
     * <p>
     *   This Iterator implementation calculates the links of an URL
     *   before returning in the next() method.
     * </p>
     *
     * @author     <a href="mailto:berni_huber@a1.net>Bernhard Huber</a>
     * @version    $Id: SimpleCocoonCrawlerImpl.java,v 1.2 2002/01/23 19:06:38 vgritsenko Exp $
     */
    public static class CocoonCrawlerIterator implements Iterator
    {
        private SimpleCocoonCrawlerImpl cocoonCrawler;


        /**
         *Constructor for the CocoonCrawlerIterator object
         *
         * @param  cocoonCrawler  Description of Parameter
         * @since
         */
        CocoonCrawlerIterator(SimpleCocoonCrawlerImpl cocoonCrawler) {
            this.cocoonCrawler = cocoonCrawler;
        }


        /**
         * check if crawling is finished.
         *
         * @return    Description of the Returned Value
         * @since
         */
        public boolean hasNext() {
            return cocoonCrawler.urlsToProcess.size() > 0;
        }


        /**
         * return the next URL
         *
         * @return    Description of the Returned Value
         * @since
         */
        public Object next() {
            URL url = null;
            Iterator i = cocoonCrawler.urlsToProcess.iterator();
            if (i.hasNext()) {
                // fetch a URL
                url = (URL) i.next();

                // remove it from the to-do list
                cocoonCrawler.urlsToProcess.remove(url);

                // calc all links from this url
                List url_links = cocoonCrawler.getLinks(url);
                if (url_links != null) {
                    // add links of this url to the to-do list
                    cocoonCrawler.urlsToProcess.addAll(url_links);
                }
            }
            // finally return this url
            return url;
        }


        /**
         * remove is not implemented
         *
         * @since
         */
        public void remove() {
            throw new UnsupportedOperationException("remove is not implemented");
        }
    }
}

