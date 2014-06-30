package org.apache.catalina.util.xml;

import java.beans.*;
import java.io.*;
import java.io.IOException;
import java.lang.reflect.*;
import java.util.*;
import java.util.StringTokenizer;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.*;
import org.xml.sax.helpers.*;
import org.w3c.dom.*;

/** micro-XPath match - match a path
 */
class PathMatch implements XmlMatch {
    String names[]=new String[10];
    int pos=0;

    public PathMatch( String tagName ) {
        StringTokenizer st=new StringTokenizer( tagName, "/" );
        while( st.hasMoreTokens() ) {
            names[pos]=st.nextToken();
            pos++;
        }
    }

    public boolean match( SaxContext ctx ) {
        int depth=ctx.getTagCount();

        for( int j=pos-1; j>=0; j--) {
            if( depth<1) {
                //              System.out.println("Pattern too long ");
                return false;
            }
            String tag=ctx.getTag(depth-1);
            if( ! names[j].equals( tag ) ) {
                //              System.out.println("XXX" + names[j] + " " + tag);
                return false;
            }
            depth--;
        }


        return true;
    }

    public String toString() {
        return "Tag("+names+")";
    }

}

