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


/** Each rule in Xml Mapper can invoke certain actions.
    An action implementation will be notified for each matching rule
    on start and end of the tag that matches.

    After all end actions are called, a special cleanup call will allow
    actions to remove temporary data.
*/
public abstract class XmlAction {
    public void start( SaxContext ctx) throws Exception {
    }

    public void end( SaxContext ctx) throws Exception {
    }

    public void cleanup( SaxContext ctx) throws Exception {
    }
}

