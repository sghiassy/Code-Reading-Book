package org.apache.catalina.util.xml;

import org.apache.catalina.util.*;
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

// XXX this interface is not final, but a prototype.

/** SAX Context - used to match and perform actions
 *    provide access to the current stack and XML elements.
 *
 * @author costin@dnt.ro
 */
public interface SaxContext  {

    // -------------------- Access to parsing context

    /** Depth of the tag stack.
     */
    public int getTagCount();

    /** Access attributes of a particular tag
     */
    public AttributeList getAttributeList( int pos );

    /** Access a particular tag
     */
    public String getTag( int pos );

    /** Body of the last tag
     */
    public String getBody();

    // -------------------- Object stack

    /**
       The root object is either set by caller before starting the parse
       or can be created using the first tag. It is used to set object in
       the result graph by navigation ( using root and a path). Please
       use the stack, it's much faster and better.
    */
    public Object getRoot();

    /** We maintain a stack to keep java objects that are generated
        as result of parsing. You can either use the stack ( which is
        very powerfull construct !), or use the root object
        and navigation in the result tree.
    */
    public Stack getObjectStack();

    // -------------------- Utilities

    public int getDebug();

    public void log( String s );

    public String getPublicId();

}

