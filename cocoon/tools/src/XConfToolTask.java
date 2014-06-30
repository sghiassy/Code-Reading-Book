/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

import java.io.*;
import java.util.*;
import org.apache.tools.ant.*;
import org.apache.tools.ant.taskdefs.*;
import org.apache.tools.ant.types.*;

/**
 * Add components to the cocoon.xconf
 * This is only a ugly first shot
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/25 02:31:53 $
 */

public final class XConfToolTask extends Task {

    private String configuration;
    private String directory;
    private String extension;

    public void setConfiguration(String configuration) {
        this.configuration = configuration;
    }

    public void setDirectory(String directory) {
        this.directory = directory;
    }

    public void setExtension(String extension) {
        this.extension = extension;
    }

    public void execute() throws BuildException {
        if (this.configuration == null) {
            throw new BuildException("configuration attribute is required", location);
        }

        if (this.extension == null) {
            throw new BuildException("extension attribute is required", location);
        }
        if (this.directory == null) {
            throw new BuildException("directory attribute is required", location);
        }

        try {
            // process recursive
            this.process(new File(this.directory), this.extension, this.configuration);
        } catch (IOException ioe) {
            throw new BuildException("IOException: " + ioe);
        }
    }

    /**
     * Scan recursive
     */
    private void process(final File   directoryFile,
                         final String ext,
                         final String configurationLocation)
    throws IOException, BuildException {

        final File[] files = directoryFile.listFiles();

        for(int i = 0; i < files.length; i++) {

            if (files[i].isDirectory() == true) {
                this.process(files[i], ext, configurationLocation);
            } else {

                if (files[i].getName().endsWith("."+ext) == true) {
                    System.out.println("Reading: " + files[i].getAbsolutePath());
                    final String newComponent = this.load(files[i].getAbsolutePath());

                    this.add(configurationLocation,
                             newComponent);
                }
            }
        }
    }

    /**
     * Add entry to sitemap
     */
    private void add(final String configurationLocation,
                     final String newComponent)
    throws IOException {
        final String data = load( configurationLocation );

        // first search if component already present:
        if ( data.indexOf( newComponent ) == -1 ) {
            int pos = data.indexOf( "<cocoon" );
            if (pos != -1) {
                pos = data.indexOf( ">", pos);

                if (pos != -1) {
                    StringBuffer buffer = new StringBuffer( data.substring( 0, pos+1 ) )
                                .append( "\n\n" )
                                .append( newComponent )
                                .append( data.substring( pos+1 ) );
                    this.save( configurationLocation, buffer.toString() );
                }
            }
        }

    }

    /**
     * Load a file and return the content as a string.
     */
    public String load( String filename )
    throws IOException {
        FileInputStream fis;

        fis = new FileInputStream( filename );
        int available;
        byte[] data = null;
        byte[] tempData;
        byte[] copyData;
        do
        {
            available = 1024;
            tempData = new byte[available];
            available = fis.read( tempData, 0, available );
            if ( available > 0 )
            {
                copyData = new byte[( data == null ? 0 : data.length ) + available];
                if ( data != null )
                {
                    System.arraycopy( data, 0, copyData, 0, data.length );
                }
                System.arraycopy( tempData, 0, copyData, ( data == null ? 0 : data.length ), available );
                data = copyData;
            }
        } while ( available > 0 );
        fis.close();

        return ( data != null ? new String( data ) : "" );
    }

    /**
     * Save the string to a file
     */
    public void save( String filename, String data )
    throws IOException
    {
        FileWriter fw = new FileWriter( filename );
        fw.write( data );
        fw.close();
    }

}
