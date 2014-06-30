/*
 * Tree.java
 *
 * Copyright (c) 2001, The HSQL Development Group
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 *
 * Neither the name of the HSQL Development Group nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This package is based on HypersonicSQL, originally developed by Thomas Mueller.
 *
 */
package org.hsqldb.util;

import java.awt.*;
import java.util.Vector;

/**
 * Class declaration
 *
 *
 * @version 1.0.0.1
 */
public class Tree extends Panel {

    // static
    private static Font	       fFont;
    private static FontMetrics fMetrics;
    private static int	       iRowHeight;
    private static int	       iIndentWidth;
    private int		       iMaxTextLength;

    // drawing
    private Dimension	       dMinimum;
    private Graphics	       gImage;
    private Image	       iImage;

    // height / width
    private int		       iWidth, iHeight;
    private int		       iFirstRow;
    private int		       iTreeWidth, iTreeHeight;
    private int		       iX, iY;

    // data
    private Vector	       vData;
    private int		       iRowCount;

    // scrolling
    private Scrollbar	       sbHoriz, sbVert;
    private int		       iSbWidth, iSbHeight;
    static {
	fFont = new Font("Dialog", Font.PLAIN, 12);
	fMetrics = Toolkit.getDefaultToolkit().getFontMetrics(fFont);
	iRowHeight = getMaxHeight(fMetrics);
	iIndentWidth = 12;
    }

    /**
     * Constructor declaration
     *
     */
    public Tree() {
	super();

	vData = new Vector();

	setLayout(null);

	sbHoriz = new Scrollbar(Scrollbar.HORIZONTAL);

	add(sbHoriz);

	sbVert = new Scrollbar(Scrollbar.VERTICAL);

	add(sbVert);
    }

    /**
     * Method declaration
     *
     *
     * @param d
     */
    public void setMinimumSize(Dimension d) {
	dMinimum = d;
    }

    /**
     * Method declaration
     *
     *
     * @param x
     * @param y
     * @param w
     * @param h
     */
    public void reshape(int x, int y, int w, int h) {
	super.reshape(x, y, w, h);

	iSbHeight = sbHoriz.getPreferredSize().height;
	iSbWidth = sbVert.getPreferredSize().width;
	iHeight = h - iSbHeight;
	iWidth = w - iSbWidth;

	sbHoriz.reshape(0, iHeight, iWidth, iSbHeight);
	sbVert.reshape(iWidth, 0, iSbWidth, iHeight);
	adjustScroll();

	iImage = null;

	repaint();
    }

    /**
     * Method declaration
     *
     */
    public void removeAll() {
	vData = new Vector();
	iRowCount = 0;

	adjustScroll();

	iMaxTextLength = 10;

	repaint();
    }

    /**
     * Method declaration
     *
     *
     * @param key
     * @param value
     * @param state
     * @param color
     */
    public void addRow(String key, String value, String state, int color) {
	String row[] = new String[4];

	if (value == null) {
	    value = "";
	}

	row[0] = key;
	row[1] = value;
	row[2] = state;    // null / "-" / "+"
	row[3] = "" + color;

	vData.addElement(row);

	int len = fMetrics.stringWidth(value);

	if (len > iMaxTextLength) {
	    iMaxTextLength = len;
	}

	iRowCount++;
    }

    /**
     * Method declaration
     *
     *
     * @param key
     * @param value
     */
    public void addRow(String key, String value) {
	addRow(key, value, null, 0);
    }

    /**
     * Method declaration
     *
     */
    public void update() {
	adjustScroll();
	repaint();
    }

    /**
     * Method declaration
     *
     */
    void adjustScroll() {
	iTreeHeight = iRowHeight * (iRowCount + 1);

	// correct would be iMaxTextLength + iMaxIndent*iIndentWidth
	iTreeWidth = iMaxTextLength * 2;

	sbHoriz.setValues(iX, iWidth, 0, iTreeWidth);

	int v = iY / iRowHeight, h = iHeight / iRowHeight;

	sbVert.setValues(v, h, 0, iRowCount + 1);

	iX = sbHoriz.getValue();
	iY = iRowHeight * sbVert.getValue();
    }

    /**
     * Method declaration
     *
     *
     * @param e
     *
     * @return
     */
    public boolean handleEvent(Event e) {
	switch (e.id) {

	case Event.SCROLL_LINE_UP:

	case Event.SCROLL_LINE_DOWN:

	case Event.SCROLL_PAGE_UP:

	case Event.SCROLL_PAGE_DOWN:

	case Event.SCROLL_ABSOLUTE:
	    iX = sbHoriz.getValue();
	    iY = iRowHeight * sbVert.getValue();

	    repaint();

	    return true;
	}

	return super.handleEvent(e);
    }

    /**
     * Method declaration
     *
     *
     * @param g
     */
    public void paint(Graphics g) {
	if (g == null || iWidth <= 0 || iHeight <= 0) {
	    return;
	}

	g.setColor(SystemColor.control);
	g.fillRect(iWidth, iHeight, iSbWidth, iSbHeight);

	if (iImage == null) {
	    iImage = createImage(iWidth, iHeight);
	    gImage = iImage.getGraphics();

	    gImage.setFont(fFont);
	}

	gImage.setColor(Color.white);
	gImage.fillRect(0, 0, iWidth, iHeight);

	int    lasty[] = new int[100];
	String root[] = new String[100];

	root[0] = "";

	int currentindent = 0;
	int y = iRowHeight;

	y -= iY;

	boolean closed = false;

	for (int i = 0; i < iRowCount; i++) {
	    String s[] = (String[]) vData.elementAt(i);
	    String key = s[0];
	    String data = s[1];
	    String folder = s[2];
	    int    ci = currentindent;

	    for (; ci > 0; ci--) {
		if (key.startsWith(root[ci])) {
		    break;
		}
	    }

	    if (root[ci].length() < key.length()) {
		ci++;
	    }

	    if (closed && ci > currentindent) {
		continue;
	    }

	    closed = folder != null && folder.equals("+");
	    root[ci] = key;

	    int x = iIndentWidth * ci - iX;

	    gImage.setColor(Color.lightGray);
	    gImage.drawLine(x, y, x + iIndentWidth, y);
	    gImage.drawLine(x, y, x, lasty[ci]);

	    lasty[ci + 1] = y;

	    int py = y + iRowHeight / 3;
	    int px = x + iIndentWidth * 2;

	    if (folder != null) {
		lasty[ci + 1] += 4;

		int rgb = Integer.parseInt(s[3]);

		gImage.setColor(rgb == 0 ? Color.white : new Color(rgb));
		gImage.fillRect(x + iIndentWidth - 3, y - 3, 7, 7);
		gImage.setColor(Color.black);
		gImage.drawRect(x + iIndentWidth - 4, y - 4, 8, 8);
		gImage.drawLine(x + iIndentWidth - 2, y,
				x + iIndentWidth + 2, y);

		if (folder.equals("+")) {
		    gImage.drawLine(x + iIndentWidth, y - 2,
				    x + iIndentWidth, y + 2);
		}
	    } else {
		px -= iIndentWidth;
	    }

	    gImage.setColor(Color.black);
	    gImage.drawString(data, px, py);

	    currentindent = ci;
	    y += iRowHeight;
	}

	g.drawImage(iImage, 0, 0, this);
    }

    /**
     * Method declaration
     *
     *
     * @param g
     */
    public void update(Graphics g) {
	paint(g);
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    public Dimension preferredSize() {
	return minimumSize();
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    public Dimension getPreferredSize() {
	return minimumSize();
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    public Dimension getMinimumSize() {
	return minimumSize();
    }

    /**
     * Method declaration
     *
     *
     * @return
     */
    public Dimension minimumSize() {
	return dMinimum;
    }

    /**
     * Method declaration
     *
     *
     * @param e
     * @param x
     * @param y
     *
     * @return
     */
    public boolean mouseDown(Event e, int x, int y) {
	if (iRowHeight == 0 || x > iWidth || y > iHeight) {
	    return true;
	}

	y += iRowHeight / 2;

	String root[] = new String[100];

	root[0] = "";

	int     currentindent = 0;
	int     cy = iRowHeight;
	boolean closed = false;
	int     i = 0;

	y += iY;

	for (; i < iRowCount; i++) {
	    String s[] = (String[]) vData.elementAt(i);
	    String key = s[0];
	    String folder = s[2];
	    int    ci = currentindent;

	    for (; ci > 0; ci--) {
		if (key.startsWith(root[ci])) {
		    break;
		}
	    }

	    if (root[ci].length() < key.length()) {
		ci++;
	    }

	    if (closed && ci > currentindent) {
		continue;
	    }

	    if (cy <= y && cy + iRowHeight > y) {
		break;
	    }

	    root[ci] = key;
	    closed = folder != null && folder.equals("+");
	    currentindent = ci;
	    cy += iRowHeight;
	}

	if (i >= 0 && i < iRowCount) {
	    String s[] = (String[]) vData.elementAt(i);
	    String folder = s[2];

	    if (folder != null && folder.equals("+")) {
		folder = "-";
	    } else if (folder != null && folder.equals("-")) {
		folder = "+";
	    }

	    s[2] = folder;

	    vData.setElementAt(s, i);
	    repaint();
	}

	return true;
    }

    /**
     * Method declaration
     *
     *
     * @param f
     *
     * @return
     */
    private static int getMaxHeight(FontMetrics f) {
	return f.getHeight() + 2;
    }

}
