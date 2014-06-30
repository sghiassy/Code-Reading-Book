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
import java.util.ListIterator;
import java.util.ArrayList;
import java.util.NoSuchElementException;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.search.Hits;

/**
 * This class should help you to manage paging of hits.
 *
 * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
 * @version    CVS $Id: LuceneCocoonPager.java,v 1.2 2002/01/23 19:06:38 vgritsenko Exp $
 */
public class LuceneCocoonPager implements ListIterator
{

    /**
     *Description of the Field
     *
     * @since
     */
    public final static int COUNT_OF_HITS_PER_PAGE_DEFAULT = 5;
    /**
     *Description of the Field
     *
     * @since
     */
    public final static int HITS_INDEX_START_DEFAULT = 0;

    /**
     * current index of hit to return by next()
     *
     * @since
     */
    int hitsIndex = HITS_INDEX_START_DEFAULT;

    /**
     * maximum count of hits to return by next(), and previous()
     *
     * @since
     */
    int countOfHitsPerPage = COUNT_OF_HITS_PER_PAGE_DEFAULT;

    /**
     * hits to iterate upon
     *
     * @since
     */
    private Hits hits;


    /**
     * @param  hits  Description of Parameter
     * @since
     */
    public LuceneCocoonPager(Hits hits) {
        setHits(hits);
    }


    /**
     *Constructor for the LuceneCocoonPager object
     *
     * @since
     */
    public LuceneCocoonPager() { }


    /**
     *Sets the hits attribute of the LuceneCocoonPager object
     *
     * @param  hits  The new hits value
     * @since
     */
    public void setHits(Hits hits) {
        this.hits = hits;
        this.hitsIndex = HITS_INDEX_START_DEFAULT;
    }


    /**
     * Set count of hits displayed per single page
     *
     * @param  countOfHitsPerPage  The new countOfHitsPerPage value
     * @since
     */
    public void setCountOfHitsPerPage(int countOfHitsPerPage) {
        this.countOfHitsPerPage = countOfHitsPerPage;
    }


    /**
     * Get starting index for retrieving hits
     *
     * @param  start_index  The new startIndex value
     * @since
     */
    public void setStartIndex(int start_index) {
        this.hitsIndex = start_index;
    }


    /**
     * Replaces the last element returned by next or previous with the
     * specified element (optional operation).
     *
     * @param  o  Description of Parameter
     * @since
     */
    public void set(Object o) {
        throw new UnsupportedOperationException();
    }


    /**
     * Get count of hits
     *
     * @return    The count of hits
     * @since
     */
    public int getCountOfHits() {
        return hits.length();
    }

    /**
     * Get count of hits displayed per single page
     *
     * @return    The countOfHitsPerPage value
     * @since
     */
    public int getCountOfHitsPerPage() {
        return this.countOfHitsPerPage;
    }

    /**
     * Caluclate count of pages for displaying all hits
     *
     * @return    The countOfPages value
     * @since
     */
    public int getCountOfPages() {
        int count_of_pages = hits.length() / this.countOfHitsPerPage;
        int remainder = hits.length() % this.countOfHitsPerPage;
        if (remainder != 0) {
            count_of_pages += 1;
        }
        return count_of_pages;
    }


    /**
     * Set starting index for retrieving hits
     *
     * @return    The startIndex value
     * @since
     */
    public int getStartIndex() {
        return this.hitsIndex;
    }


    /**
     * Inserts the specified element into the list (optional operation).
     *
     * @param  o                                  Description of Parameter
     * @exception  UnsupportedOperationException  Description of Exception
     * @since
     */
    public void add(Object o) throws UnsupportedOperationException {
        throw new UnsupportedOperationException();
    }


    /**
     * Returns true if this list iterator has more elements when traversing
     * the list in the forward direction.
     *
     * @return    Description of the Returned Value
     * @since
     */
    public boolean hasNext() {
        boolean has_next = hitsIndex < hits.length();
        return has_next;
    }


    /**
     * Returns true if this list iterator has more elements when traversing
     * the list in the reverse direction.
     *
     * @return    Description of the Returned Value
     * @since
     */
    public boolean hasPrevious() {
        boolean has_previous = hitsIndex > countOfHitsPerPage;
        return has_previous;
    }


    /**
     * Returns the next element in the list.
     *
     * @return    Description of the Returned Value
     * @since
     */
    public Object next() {
        ArrayList hitsPerPageList = new ArrayList();
        int endIndex = Math.min(hits.length(), hitsIndex + countOfHitsPerPage);
        if (hitsIndex < endIndex) {
            while (hitsIndex < endIndex) {
                try {
                    HitWrapper hit_wrapper = new HitWrapper(
                            hits.score(hitsIndex),
                            hits.doc(hitsIndex));
                    hitsPerPageList.add(hit_wrapper);
                } catch (IOException ioe) {
                    throw new NoSuchElementException("no more hits: " + ioe.getMessage());
                }
                hitsIndex++;
            }
        } else {
            throw new NoSuchElementException();
        }
        return hitsPerPageList;
    }


    /**
     * Returns the index of the element that would be returned by a
     * subsequent call to next.
     *
     * @return    Description of the Returned Value
     * @since
     */
    public int nextIndex() {
        int next_index = Math.min(hitsIndex, hits.length());
        return next_index;
    }


    /**
     * Returns the previous element in the list.
     *
     * @return    Description of the Returned Value
     * @since
     */
    public Object previous() {
        ArrayList hitsPerPageList = new ArrayList();

        int startIndex = Math.max(0, hitsIndex - 2 * countOfHitsPerPage);
        int endIndex = Math.min(hits.length() - 1, hitsIndex - countOfHitsPerPage);

        if (startIndex < endIndex) {
            while (startIndex < endIndex) {
                try {
                    HitWrapper hit_wrapper = new HitWrapper(
                            hits.score(startIndex),
                            hits.doc(startIndex));
                    hitsPerPageList.add(hit_wrapper);
                } catch (IOException ioe) {
                    throw new NoSuchElementException("no more hits: " + ioe.getMessage());
                }
                startIndex++;
            }
            hitsIndex = endIndex;
        } else {
            throw new NoSuchElementException();
        }
        return hitsPerPageList;
    }


    /**
     * Returns the index of the element that would be returned by a
     * subsequent call to previous.
     *
     * @return    Description of the Returned Value
     * @since
     */
    public int previousIndex() {
        int previous_index = Math.max(0, hitsIndex - 2 * countOfHitsPerPage);
        return previous_index;
    }


    /**
     * Removes from the list the last element that was returned by next or
     * previous (optional operation).
     *
     * @since
     */
    public void remove() {
        throw new UnsupportedOperationException();
    }


    /**
     * A helper class encapsulating found document, and its score
     *
     * @author     <a href="mailto:berni_huber@a1.net">Bernhard Huber</a>
     * @version    CVS $Id: LuceneCocoonPager.java,v 1.2 2002/01/23 19:06:38 vgritsenko Exp $
     */
    public static class HitWrapper
    {
        float score;
        Document document;


        /**
         * Constructor for the HitWrapper object
         *
         * @param  score     Description of Parameter
         * @param  document  Description of Parameter
         * @since
         */
        public HitWrapper(float score, Document document) {
            this.document = document;
            this.score = score;
        }


        /**
         * Gets the document attribute of the HitWrapper object
         *
         * @return    The document value
         * @since
         */
        public Document getDocument() {
            return document;
        }


        /**
         * Gets the score attribute of the HitWrapper object
         *
         * @return    The score value
         * @since
         */
        public float getScore() {
            return score;
        }


        /**
         * Gets the field attribute of the HitWrapper object
         *
         * @param  field  Description of Parameter
         * @return        The field value
         * @since
         */
        public String getField(String field) {
            return document.get(field);
        }
    }

}

