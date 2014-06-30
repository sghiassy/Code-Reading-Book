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
 * Add components to the sitemap
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/25 02:31:53 $
 */

public final class SitemapToolTask extends Task {

    private String sitemap;
    private String directory;
    private String extension;

    public void setSitemap(String sitemap) {
        this.sitemap = sitemap;
    }

    public void setDirectory(String directory) {
        this.directory = directory;
    }

    public void setExtension(String extension) {
        this.extension = extension;
    }

    public void execute() throws BuildException {
        if (this.sitemap == null) {
            throw new BuildException("sitemap attribute is required", location);
        }

        if (this.extension == null) {
            throw new BuildException("extension attribute is required", location);
        }
        if (this.directory == null) {
            throw new BuildException("directory attribute is required", location);
        }

        try {
            // process recursive
            this.process(new File(this.directory), this.extension, this.sitemap);
        } catch (IOException ioe) {
            throw new BuildException("IOException: " + ioe);
        }
    }

    /**
     * Scan recursive
     */
    private void process(final File   directoryFile,
                         final String ext,
                         final String sitemapLocation)
    throws IOException, BuildException {

        final File[] files = directoryFile.listFiles();

        for(int i = 0; i < files.length; i++) {

            if (files[i].isDirectory() == true) {
                this.process(files[i], ext, sitemapLocation);
            } else {

                if (files[i].getName().endsWith("."+ext) == true) {
                    System.out.println("Reading: " + files[i].getAbsolutePath());
                    final String data = this.load(files[i].getAbsolutePath());

                    // separate the data by lines
                    final StringTokenizer st = new StringTokenizer(data);
                    while (st.hasMoreElements() == true) {

                        // now get the properties of a line. These are separated by a "|"
                        final String line = (String)st.nextElement();
                        final StringTokenizer prop = new StringTokenizer(line, "|");
                        String category = null;
                        String componentName = null;
                        String className = null;
                        String configuration = null;
                        String label = null;
                        String mimeType = null;
                        while (prop.hasMoreElements() == true) {
                            final String property = (String)prop.nextElement();
                            final int pos = property.indexOf(":");
                            final String propName = property.substring(0, pos);
                            final String propVal  = property.substring(pos+1);
                            if     (propName.equals("category")) category = propVal;
                            else if (propName.equals("componentName")) componentName = propVal;
                            else if (propName.equals("componentClass")) className = propVal;
                            else if (propName.equals("configuration")) configuration = propVal;
                            else if (propName.equals("label")) label = propVal;
                            else if (propName.equals("mimeType")) mimeType = propVal;
                            else throw new BuildException("Unknown property " + propName + " in file " + files[i].getAbsolutePath());
                        }

                        // Test for required values
                        if (category == null) {
                            throw new BuildException("category property is required in file " + files[i].getAbsolutePath(), location);
                        }
                        if (componentName == null) {
                            throw new BuildException("componentName property is required in file " + files[i].getAbsolutePath(), location);
                        }
                        if (className == null) {
                            throw new BuildException("componentClass property is required in file " + files[i].getAbsolutePath(), location);
                        }

                        this.add(sitemapLocation,
                                 category,
                                 componentName,
                                 className,
                                 configuration,
                                 label,
                                 mimeType);
                    }
                }
            }
        }
    }

    /**
     * Add entry to sitemap
     */
    private void add(final String sitemapLocation,
                     final String category,
                     final String componentName,
                     final String className,
                     final String configuration,
                     final String label,
                     final String mimeType)
    throws IOException {
        final String data = load( sitemapLocation );

        final String searchString = new StringBuffer( "</map:" )
                                        .append( category )
                                        .append( ">" ).toString();
        final int pos = data.indexOf( searchString );
        int categoryStartPos = data.indexOf(new StringBuffer( "<map:" )
                                            .append( category )
                                            .append( ">" ).toString() );

        if ( categoryStartPos == -1 )
            categoryStartPos = data.indexOf(new StringBuffer( "<map:" )
                                            .append( category )
                                            .append( " " ).toString() );

        if ( categoryStartPos != -1 && categoryStartPos < pos && pos != -1 ) {
            // the category exists, now search if a component
            // with the name already exists
            int componentPos = data.substring( categoryStartPos, pos ).indexOf(
                    new StringBuffer( "name=\"" )
                              .append( componentName )
                              .append( "\"" ).toString() );
            if ( componentPos == -1 ) {
                StringBuffer buffer = new StringBuffer( data.substring( 0, pos ) )
                            .append( "<map:" )
                            .append( category.substring( 0, category.length() - 1 ) )
                            .append( " name=\"" )
                            .append( componentName )
                            .append( "\" src=\"" ).append( className ).append( "\"" )
                            .append( " logger=\"sitemap.")
                            .append( category.substring( 0, category.length() - 1 ) )
                            .append( '.' )
                            .append( componentName )
                            .append( '\"');

                if ( null != mimeType && mimeType.length() > 0) {
                     buffer.append( " mime-type=\"" ).append( mimeType ).append( "\"" );
                }
                if ( null != label && label.length() > 0) {
                     buffer.append( " label=\"" ).append( label ).append( "\"" );
                }
                if ( null != configuration ) {
                    buffer.append( ">\n" ).append( configuration ).append( "\n" )
                        .append( "</map:" ).append( category.substring( 0, category.length() - 1 ) ).append( ">\n" );
                } else {
                    buffer.append( "/>\n" );
                }
                buffer.append( data.substring( pos ) ).toString();
                this.save( sitemapLocation, buffer.toString() );
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
