/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.generation;

import org.apache.log.Hierarchy;
import org.xml.sax.SAXException;

import java.io.*;

/**
 * An extension of DirectoryGenerators that adds extra attributes for image
 * files.
 *
 * @author <a href="mailto:balld@webslingerZ.com">Donald A. Ball Jr.</a>
 * @version $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */
public class ImageDirectoryGenerator extends DirectoryGenerator {

    protected static String IMAGE_WIDTH_ATTR_NAME = "width";
    protected static String IMAGE_HEIGHT_ATTR_NAME = "height";

    /**
     * Extends the <code>setNodeAttributes</code> method from the
     * <code>DirectoryGenerator</code> by adding width and height attributes
     * if the path is a GIF or a JPEG file.
     */
    protected void setNodeAttributes(File path) throws SAXException {
        super.setNodeAttributes(path);
        if (path.isDirectory()) {
            return;
        }
        try {
            int dim[] = getSize(path);
            getLogger().debug("getSize(path) = " + dim);
            attributes.addAttribute("",IMAGE_WIDTH_ATTR_NAME,IMAGE_WIDTH_ATTR_NAME,"CDATA",String.valueOf(dim[0]));
            attributes.addAttribute("",IMAGE_HEIGHT_ATTR_NAME,IMAGE_HEIGHT_ATTR_NAME,"CDATA",String.valueOf(dim[1]));
        } catch (RuntimeException e) {
            getLogger().debug("ImageDirectoryGenerator.setNodeAttributes", e);
        } catch (Exception e) {
            getLogger().error("ImageDirectoryGenerator.setNodeAttributes", e);
            throw new SAXException(e);
        }
    }

    // returns width as first element, height as second
    public static int[] getSize(File file) throws FileNotFoundException, IOException {
        String type = getFileType(file);
        try {
            if(type.equals("gif")) return getGifSize(file);
            else return getJpegSize(file);
        } catch(Exception e) {
            Hierarchy.getDefaultHierarchy().getLoggerFor("cocoon").debug("File is not a valid GIF or Jpeg", e);
            throw new RuntimeException("File is not a valid GIF or Jpeg");
        }

    }

    // returns width as first element, height as second
    public static int[] getJpegSize(File file) throws FileNotFoundException, IOException {
        BufferedInputStream in = null;
        try {
            in = new BufferedInputStream(new FileInputStream(file));
            // check for "magic" header
            byte[] buf = new byte[2];
            int count = in.read(buf, 0, 2);
            if(count < 2) throw new RuntimeException("Not a valid Jpeg file!");
            if((buf[0]) != (byte)0xFF
            || (buf[1]) != (byte)0xD8 )
            throw new RuntimeException("Not a valid Jpeg file!");

            int width = 0;
            int height = 0;

            boolean done = false;
            int ch = 0;

            try {
                while(ch != 0xDA && !done) {
                    /* Find next marker (JPEG markers begin with 0xFF) */
                    while (ch != 0xFF) { ch = in.read(); }
                    /* JPEG markers can be padded with unlimited 0xFF's */
                    while (ch == 0xFF) { ch = in.read(); }
                    /* Now, ch contains the value of the marker. */
                    if(ch >= 0xC0 && ch <= 0xC3) {
                        // skip 3 bytes
                        in.read();
                        in.read();
                        in.read();
                        height = 256 * in.read();
                        height += in.read();
                        width = 256 * in.read();
                        width += in.read();
                        done = true;
                    } else {
                        /* We MUST skip variables, since FF's within variable names are NOT valid JPEG markers */
                        int length = 256 * in.read();
                        length += in.read();
                        if(length < 2) throw new RuntimeException("Erroneous JPEG marker length");
                        for(int foo = 0; foo<length-2; foo++)
                            in.read();
                    }
                }
            } catch(Exception e) {
                Hierarchy.getDefaultHierarchy().getLoggerFor("cocoon").debug("Not a valid Jpeg file!", e);
                throw new RuntimeException("Not a valid Jpeg file!");
            }

            int[] dim = { width, height };
            return dim;

        } finally {
            if(in != null) try { in.close(); } catch(Exception e) {Hierarchy.getDefaultHierarchy().getLoggerFor("cocoon").debug("Close stream", e);}
        }
    }

    // returns width as first element, height as second
    public static int[] getGifSize(File file) throws FileNotFoundException, IOException {
        BufferedInputStream in = null;
        try {
            in = new BufferedInputStream(new FileInputStream(file));
            byte[] buf = new byte[10];
            int count = in.read(buf, 0, 10);
            if(count < 10) throw new RuntimeException("Not a valid GIF file!");
            if((buf[0]) != (byte)'G'
            || (buf[1]) != (byte)'I'
            || (buf[2]) != (byte)'F' )
            throw new RuntimeException("Not a valid GIF file!");

            int w1 = ((int)buf[6] & 0xff) | (buf[6] & 0x80);
            int w2 = ((int)buf[7] & 0xff) | (buf[7] & 0x80);
            int h1 = ((int)buf[8] & 0xff) | (buf[8] & 0x80);
            int h2 = ((int)buf[9] & 0xff) | (buf[9] & 0x80);

            int width = w1 + (w2 << 8);
            int height = h1 + (h2 << 8);

            int[] dim = { width, height };
            return dim;

        } finally {
            if(in != null) try { in.close(); } catch(Exception e) {Hierarchy.getDefaultHierarchy().getLoggerFor("cocoon").debug("Close stream", e);}
        }
    }

    // returns "gif", "jpeg" or NULL
    public static String getFileType(File file) throws FileNotFoundException, IOException {
        BufferedInputStream in = null;
        try {
            in = new BufferedInputStream(new FileInputStream(file));
            byte[] buf = new byte[3];
            int count = in.read(buf, 0, 3);
            if(count < 3) return null;
            if((buf[0]) == (byte)'G'
            && (buf[1]) == (byte)'I'
            && (buf[2]) == (byte)'F' )
            return "gif";

            if((buf[0]) == (byte)0xFF
            && (buf[1]) == (byte)0xD8 )
            return "jpeg";

            return null;
        } finally {
            if(in != null) try { in.close(); } catch(Exception e) {Hierarchy.getDefaultHierarchy().getLoggerFor("cocoon").debug("Close stream", e);}
        }
    }
}
