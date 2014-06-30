/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.generation;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import org.xml.sax.SAXException;

/**
 * An extension of DirectoryGenerators that adds extra attributes for MP3
 * files.
 * <p>
 * Following extra attributes added to valid MP3 files:
 * <blockquote>
 *   <dl>
 *   <dt> frequency
 *   <dd> the frequency of the MP3 file in KHz (most common: 44.1)
 *   <dt> bitrate
 *   <dd> the bitrate of the MP3 file in Kbit, from 8 to 448.
 *   <dt> mode
 *   <dd> the mode of the MP3 file, one of the following: Stereo, Joint stereo,
 *        Dual channel, Single channel.
 *   <dt> variable-rate (optional)
 *   <dd> value is "yes" if VBR header is detected
 *   <dt> title, artitst, album, year, comment, track, genre (all optional)
 *   <dd> values obtained from MP3 ID3 tag
 *   </dl>
 * </blockquote>
 *
 * @author <a href="mailto:vgritsenko@apache.org">Vadim Gritsenko</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $
 */
public class MP3DirectoryGenerator extends DirectoryGenerator
{
    // MP3 Constants
    private static final int VERSION_MPEG1       = 3;
    private static final int VERSION_MPEG2       = 2;
    private static final int MODE_DUAL_CHANNEL   = 2;
    private static final int MODE_JOINT_STEREO   = 1;
    private static final int MODE_SINGLE_CHANNEL = 3;
    private static final int MODE_STEREO         = 0;
    private static final int BITRATE_441         = 0;
    private static final int BITRATE_48          = 1;
    private static final int BITRATE_32          = 2;
    private static final int VBR_FRAMES_FLAG     = 1;
    private static final int VBR_BYTES_FLAG      = 2;
    private static final int VBR_TOC_FLAG        = 4;
    private static final int VBR_SCALE_FLAG      = 8;

    // Attributes
    protected static String MP3_FREQUENCY_ATTR_NAME = "frequency";
    protected static String MP3_BITRATE_ATTR_NAME   = "bitrate";
    protected static String MP3_MODE_ATTR_NAME      = "mode";
    protected static String MP3_VBR_ATTR_NAME   = "variable-rate";

    protected static String MP3_TITLE_ATTR_NAME   = "title";
    protected static String MP3_ARTITST_ATTR_NAME = "artist";
    protected static String MP3_ALBUM_ATTR_NAME   = "album";
    protected static String MP3_YEAR_ATTR_NAME    = "year";
    protected static String MP3_COMMENT_ATTR_NAME = "comment";
    protected static String MP3_TRACK_ATTR_NAME   = "track";
    protected static String MP3_GENRE_ATTR_NAME   = "genre";

    /**
     * Extends the <code>setNodeAttributes</code> method from the
     * <code>DirectoryGenerator</code> by adding MP3 tag attributes
     * if the path is a MP3 file with valid tag.
     */
    protected void setNodeAttributes(File path) throws SAXException {
        super.setNodeAttributes(path);
        if (path.isDirectory()) {
            return;
        }

        RandomAccessFile in = null;
        try {
            in = new RandomAccessFile(path, "r");
            setID3HeaderAttributes(in);
            setID3TagAttributes(in);
        } catch (IOException e) {
            getLogger().debug("Could not set attributes for " + path, e);
        } finally {
            if(in != null) try{ in.close(); }catch(IOException ignored){}
        }
    }

    /**
     * Read ID3 Tag
     */
    private void setID3TagAttributes(RandomAccessFile in) throws IOException
    {
        String s;

        // TAG takes 128 bytes
        if (in.length() < 128) return;
        in.seek(in.length() - 128);
        byte [] buf = new byte[128];
        // Read TAG
        if (in.read(buf,0, 128) != 128) return;
        // Check TAG presence
        if(buf[0] != 'T' || buf[1] != 'A' || buf[2] != 'G') return;
         
        s = new String(buf, 3, 30).trim();
        if(s.length() > 0)
            attributes.addAttribute("", MP3_TITLE_ATTR_NAME, MP3_TITLE_ATTR_NAME, "CDATA", s);
        s = new String(buf, 33,30).trim();
        if(s.length() > 0)
            attributes.addAttribute("", MP3_ARTITST_ATTR_NAME, MP3_ARTITST_ATTR_NAME, "CDATA", s);
        s = new String(buf, 63,30).trim();
        if(s.length() > 0)
            attributes.addAttribute("", MP3_ALBUM_ATTR_NAME, MP3_ALBUM_ATTR_NAME, "CDATA", s);
        s = new String(buf, 93, 4).trim();
        if(s.length() > 0)
            attributes.addAttribute("", MP3_YEAR_ATTR_NAME, MP3_YEAR_ATTR_NAME, "CDATA", s);
        s = new String(buf, 97,29).trim();
        if(s.length() > 0)
            attributes.addAttribute("", MP3_COMMENT_ATTR_NAME, MP3_COMMENT_ATTR_NAME, "CDATA", s);
        if(buf[126] > 0)
            attributes.addAttribute("", MP3_TRACK_ATTR_NAME, MP3_TRACK_ATTR_NAME, "CDATA",
                Byte.toString(buf[126]));
        if(buf[127] > 0)
            attributes.addAttribute("", MP3_GENRE_ATTR_NAME, MP3_GENRE_ATTR_NAME, "CDATA",
                Byte.toString(buf[127]));
    }

    private void setID3HeaderAttributes(RandomAccessFile in) throws IOException
    {
        byte[] buffer = new byte[4];

        // http://floach.pimpin.net/grd/mp3info/frmheader/index.html
        if (in.read(buffer, 0, 3) != 3) {
            return;
        }
        int header = ((buffer[0] << 16) & 0x00FF0000) | ((buffer[1] << 8) & 0x0000FF00) | ((buffer[2] << 0) & 0x000000FF);
        do {
            header <<= 8;
            if (in.read(buffer, 3, 1) != 1) {
                return;
            }
            header |= (buffer[3] & 0x000000FF);
        } while (!isSyncMark(header));

        int version = (header >>> 19) & 3;
        int layer = 4 - (header >>> 17) & 3;
        int protection = (header >>> 16) & 1;
        int bitrate = (header >>> 12) & 0xF;
        int frequency = (header >>> 10) & 3;
        // Value 3 is reserved
        if (frequency == 3) return;
        int padding = (header >>> 9) & 1;
        int mode = ((header >>> 6) & 3);

        attributes.addAttribute("", MP3_FREQUENCY_ATTR_NAME, MP3_FREQUENCY_ATTR_NAME, "CDATA",
            frequencyString(version, frequency));
        attributes.addAttribute("", MP3_MODE_ATTR_NAME, MP3_MODE_ATTR_NAME, "CDATA",
            mode(mode));

        int frames = getVBRHeaderFrames(in, version, mode);
        if (frames != -1) {
            // get average frame size by deviding fileSize by the number of frames
            float medFrameSize = (float)in.length() / frames;
            // This does not work properly: (version == VERSION_MPEG1? 12000.0:144000.0)
            bitrate = (int)(medFrameSize * frequency(version, frequency) / 144000.0);
            attributes.addAttribute("", MP3_BITRATE_ATTR_NAME, MP3_BITRATE_ATTR_NAME, "CDATA",
                Integer.toString(bitrate));
        } else {
            attributes.addAttribute("", MP3_BITRATE_ATTR_NAME, MP3_BITRATE_ATTR_NAME, "CDATA",
                bitrate(version, layer, bitrate));
        }
    }

    private static boolean isSyncMark(int header)
    {
        boolean sync = ((header & 0xFFF00000) == 0xFFF00000);
        // filter out invalid sample rate
        if (sync) sync = ((header >>> 10) & 3) != 3;
        // filter out invalid layer
        if (sync) sync = ((header >>> 17) & 3) != 0;
        // filter out invalid version 
        if (sync) sync = ((header >>> 19) & 3) != 1;
        return sync;
    }
    
    private int getVBRHeaderFrames(RandomAccessFile in, int version, int mode) throws IOException
    {
        byte[] buffer = new byte[12];

        // Try to detect VBR header
        int skip;
        if (version == VERSION_MPEG1) {
            if (mode == MODE_SINGLE_CHANNEL) skip = 17;
            else skip = 32;
        } else { // mpeg version 2 or 2.5
            if (mode == MODE_SINGLE_CHANNEL) skip = 9;
            else skip = 17;
        }
        while (skip > 0) {
            if (in.read() == -1) return -1;
            skip --;
        }

        if (in.read(buffer, 0, 12) != 12) {
            return -1;
        }
        if (buffer[0] != 'X' || buffer[1] != 'i' || buffer[2] != 'n' || buffer[3] != 'g'){
            return -1;
        }

        attributes.addAttribute("", MP3_VBR_ATTR_NAME, MP3_VBR_ATTR_NAME, "CDATA",
            "yes");

        int flags =
            ((buffer[4] & 0xFF) << 24) |
            ((buffer[5] & 0xFF) << 16) |
            ((buffer[6] & 0xFF) <<  8) |
             (buffer[7] & 0xFF);

        if ((flags & VBR_FRAMES_FLAG) == VBR_FRAMES_FLAG){
            int frames = 
                ((buffer[ 8] & 0xFF) << 24) |
                ((buffer[ 9] & 0xFF) << 16) |
                ((buffer[10] & 0xFF) <<  8) |
                 (buffer[11] & 0xFF);
            return frames;
        } else {
            return -1;
        }
    }
    
    // version - layer - bitrate index
    private static final String bitrates[][][] =
    {
      {
        // MPEG2 - layer 1
        {"free format", "32", "48", "56", "64", "80", "96", "112", "128", "144", "160", "176", "192", "224", "256", "forbidden"},
        // MPEG2 - layer 2
        {"free format", "8", "16", "24", "32", "40", "48", "56", "64", "80", "96", "112", "128", "144", "160", "forbidden"},
        // MPEG2 - layer 3
        {"free format", "8", "16", "24", "32", "40", "48", "56", "64", "80", "96", "112", "128", "144", "160", "forbidden"}
      },
      {
        // MPEG1 - layer 1
        {"free format", "32", "64", "96", "128", "160", "192", "224", "256", "288", "320", "352", "384", "416", "448", "forbidden"},
        // MPEG1 - layer 2
        {"free format", "32", "48", "56", "64", "80", "96", "112", "128", "160", "192", "224", "256", "320", "384", "forbidden"},
        // MPEG1 - layer 3
        {"free format", "32", "40", "48", "56", "64", "80" , "96", "112", "128", "160", "192", "224", "256", "320", "forbidden"}
      }
    };
    
    private static String bitrate(int version, int layer, int bitrate_index)
    {
        return bitrates[version & 1][layer - 1][bitrate_index];
    }
    
    private static String mode(int mode)
    {
        switch(mode)
        { 
        case MODE_STEREO:
            return "Stereo";
        case MODE_JOINT_STEREO:
            return "Joint stereo";
        case MODE_DUAL_CHANNEL:
            return "Dual channel";
        case MODE_SINGLE_CHANNEL:
            return "Single channel";
        }
        return null;
    }

    private static final int frequencies[][] =
    {
        {32000, 16000,  8000}, //MPEG 2.5
        {    0,     0,     0}, //reserved
        {22050, 24000, 16000}, //MPEG 2
        {44100, 48000, 32000}  //MPEG 1
    };

    private static int frequency(int version, int frequency)
    {
        return frequencies[version][frequency];
    }

    private static String frequencyString(int version, int frequency)
    {
        return String.valueOf((float)frequency(version, frequency)/1000);
    }
}
