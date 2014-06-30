/*
 * Copyright (C) The Apache Software Foundation. All rights reserved.
 *
 * This software is published under the terms of the Apache Software License
 * version 1.1, a copy of which has been included with this distribution in
 * the LICENSE file.
 */
//package org.apache.avalon.excalibur.i18n;
package org.apache.cocoon.i18n;

/** JDK classes **/
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.logger.Loggable;
import org.apache.cocoon.components.xpath.XPathProcessor;
import org.apache.log.Logger;
import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import java.io.IOException;
import java.util.*;

/**
 * @author <a href="mailto:mengelhart@earthtrip.com">Mike Engelhart</a>
 * @author <a href="mailto:neeme@one.lv">Neeme Praks</a>
 * @author <a href="mailto:oleg@one.lv">Oleg Podolsky</a>
 * @version $Id: XMLResourceBundle.java,v 1.3 2002/01/22 00:17:13 vgritsenko Exp $
 */
public class XMLResourceBundle
    extends ResourceBundle
    implements Loggable, Component, Composable, Disposable
{
    /** DOM factory */
    protected static DocumentBuilderFactory docfactory =
    	DocumentBuilderFactory.newInstance();

    /** Cache for storing string values for existing XPaths */
    private Hashtable cache = new Hashtable();

    /** Cache for storing non-existing XPaths */
    private Map cacheNotFound = new HashMap();

    /** Bundle name */
    private String name = "";

    /** DOM-tree containing the bundle content */
    private Document doc;

    /** Locale of the bundle */
    private Locale locale;

    /** Parent of the current bundle */
    protected XMLResourceBundle parent = null;

    /** Logger */
    protected Logger logger;

    /** Component Manager */
    protected ComponentManager manager = null;

    /** XPath Processor */
    private XPathProcessor processor = null;

    public void compose(ComponentManager manager) {
        this.manager = manager;
        try {
            this.processor = (XPathProcessor)this.manager.lookup(XPathProcessor.ROLE);
        } catch (Exception e) {
            logger.error("cannot obtain XPathProcessor", e);
        }
    }

    public void dispose()
    {
        this.manager.release((Component)this.processor);
        this.processor = null;
    }

    /**
     * Initalize the bundle
     *
     * @param name              name of the bundle
     * @param fileName          name of the XML source file
     * @param locale            locale
     * @param parent            parent bundle of this bundle
     * @param cacheAtStartup    cache all the keys when constructing?
     * @exception IOException   if an IO error occurs while reading the file
     * @exception ParserConfigurationException if no parser is configured
     * @exception SAXException  if an error occurs while parsing the file
     */
    public void init(String name, String fileName, Locale locale, XMLResourceBundle parent, boolean cacheAtStartup)
        throws IOException, ParserConfigurationException, SAXException
    {
        if (logger.isInfoEnabled()) logger.info("Constructing XMLResourceBundle: " + name + ", locale: " + locale);
        this.name = name;
        this.doc = loadResourceBundle(fileName);
        this.locale = locale;
        this.parent = parent;
        if (cacheAtStartup)
            cacheAll(doc.getDocumentElement(), "");
    }

    /**
     * Load the DOM tree, based on the file name.
     *
     * @param fileName          name of the XML source file
     * @return the DOM tree
     * @exception IOException   if an IO error occurs while reading the file
     * @exception ParserConfigurationException if no parser is configured
     * @exception SAXException  if an error occurs while parsing the file
     */
    protected static Document loadResourceBundle(String fileName)
        throws IOException, ParserConfigurationException, SAXException
    {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = factory.newDocumentBuilder();
        return builder.parse(fileName);
    }

    /**
     * Gets the name of the bundle.
     *
     * @return the name
     */
    public String getName()
    {
        return this.name;
    }

    /**
     * Gets the source DOM tree of the bundle.
     *
     * @return the DOM tree
     */
    public Document getDocument()
    {
        return this.doc;
    }

    /**
     * Gets the locale of the bundle.
     *
     * @return the locale
     */
    public Locale getLocale()
    {
        return locale;
    }

    /**
     * Does the &quot;key-cache&quot; contain the value with such key?
     *
     * @param   key     the key to the value to be returned
     * @return  true if contains, false otherwise
     */
    private boolean cacheContains(String key)
    {
        if (logger.isDebugEnabled()) logger.debug(name + ": cache contains: " + key);
        return cache.containsKey(key);
    }

    /**
     * Does the &quot;key-not-found-cache&quot; contain such key?
     *
     * @param   key     the key to the value to be returned
     * @return  true if contains, false otherwise
     */
    private boolean cacheNotFoundContains(String key)
    {
        if (logger.isDebugEnabled()) logger.debug(name + ": cache_not_found contains: " + key);
        return cacheNotFound.containsKey(key);
    }

    /**
     * Cache the key and value in &quot;key-cache&quot;.
     *
     * @param   key     the key
     * @param   value   the value
     */
    private void cacheKey(String key, String value)
    {
        if (logger.isDebugEnabled()) logger.debug(name + ": caching: " + key + " = " + value);
        cache.put(key, value);
    }

    /**
     * Cache the key in &quot;key-not-found-cache&quot;.
     *
     * @param   key     the key
     */
    private void cacheNotFoundKey(String key)
    {
        if (logger.isDebugEnabled()) logger.debug(name + ": caching not_found: " + key);
        cacheNotFound.put(key, "");
    }

    /**
     * Gets the value by the key from the &quot;key-cache&quot;.
     *
     * @param   key     the key
     * @return          the value
     */
    private String getFromCache(String key)
    {
        if (logger.isDebugEnabled()) logger.debug(name + ": returning from cache: " + key);
        return (String) cache.get(key);
    }

    /**
     * Steps through the bundle tree and stores all text element values
     * in bundle's cache. Also stores attributes for all element nodes.
     *
     * @param   parent          parent node, must be an element
     * @param   pathToParent    XPath to the parent node
     */
    private void cacheAll(Node parent, String pathToParent)
    {
        NodeList children = parent.getChildNodes();
        int childnum = children.getLength();

        for(int i = 0; i < childnum; i++)
        {
            Node child = children.item(i);

            if(child.getNodeType() == Node.ELEMENT_NODE)
            {
                StringBuffer pathToChild = new StringBuffer(pathToParent).append('/').append(child.getNodeName());

                NamedNodeMap attrs = child.getAttributes();
                if(attrs != null)
                {
                    Node temp = null;
                    String pathToAttr = null;
                    int attrnum = attrs.getLength();
                    for(int j = 0; j < attrnum; j++)
                    {
                        temp = attrs.item(j);
                        if (!temp.getNodeName().equalsIgnoreCase("xml:lang"))
                            pathToChild.append("[@").append(temp.getNodeName())
                                    .append("='").append(temp.getNodeValue()).append("']");
                    }
                }

                String childValue = getTextValue(child);
                if(childValue != null)
                    cacheKey(pathToChild.toString(), childValue);
                else
                    cacheAll(child, pathToChild.toString());
            }
        }
    }

    /**
     * Get value by key and substitute variables.
     *
     * @param key           key
     * @param dictionary    map with variable values
     * @return              value with variable values substituted
     * @exception MissingResourceException if resource was not found
     */
    public String getString(String key, Map dictionary)
        throws MissingResourceException
    {
        String value = _getString(key);
        if (value == null)
            throw new MissingResourceException(
                    "Unable to locate resource: " + key,
                    XMLResourceBundle.class.getName(),
                    key);
        else
            return substitute(value, dictionary);
    }

    /**
     * Get value by key.
     *
     * @param   key     the key
     * @return          the value
     */
    private String _getString(String key)
    {
        if (key == null) return null;
        String value = getFromCache(key);

        if (value == null && !cacheNotFoundContains(key))
        {
	    if (doc != null)
                value = _getString(this.doc.getDocumentElement(), key);

            if (value == null)
            {
                if (this.parent != null)
                    value = this.parent._getString(key);
            }

            if (value != null)
                cacheKey(key, value);
            else
                cacheNotFoundKey(key);
        }
        return value;
    }

    /**
     * Get value by key from a concrete node.
     *
     * @param   node    the node
     * @param   key     the key
     * @return          the value
     */
    private String _getString(Node node, String key)
    {
        try {
            return getTextValue(_getNode(node, key));
        } catch (Exception e) {
            logger.error(name + ": error while locating resource: " + key, e);
        }
        return null;
    }

    /**
     * Get the text value of the node.
     *
     * @param   node    the node
     * @return          the value
     */
    private static String getTextValue(Node element)
    {
        if (element == null) return null;
        NodeList list = element.getChildNodes();
        int listsize = list.getLength();

        Node item = null;
        String itemValue = null;

        for(int i = 0; i < listsize; i++)
        {
            item = list.item(i);
	    if(item.getNodeType() != Node.TEXT_NODE)
	        return null;

            itemValue = item.getNodeValue();
	    if(itemValue == null)
	        return null;

            itemValue = itemValue.trim();
	    if(itemValue.length() == 0)
	        return null;

            return itemValue;
        }
        return null;
    }

    /**
     * Get the node with the supplied XPath key.
     *
     * @param   key     the key
     * @return          the node
     */
    private Node _getNode(String key)
    {
        return _getNode(this.doc.getDocumentElement(), key);
    }

    /**
     * Get the node with the supplied XPath key, starting from concrete
     * root node.
     *
     * @param   rootNode    the root node
     * @param   key         the key
     * @return              the node
     */
    private Node _getNode(Node rootNode, String key)
    {
        try {
            return this.processor.selectSingleNode(rootNode, key);
        } catch (Exception e) {
            logger.error("Error while locating resource with key: " + key, e);
        }
        return null;
    }

    /**
     * Substitute the &quot;variables&quot; in the string with the values
     * provided in the map.
     *
     * @param value         value where to search for variables
     * @param dictionary    map with variable values
     * @return              value with variable values substituted
     */
    public String substitute(String value, Map dictionary)
    {
        if (value == null || dictionary == null) return value;

        StringBuffer result = new StringBuffer(value.length());
        int startPos = 0;
        int endPos = 0;
        int lastPos = value.length();
        Object varValue = "";
        String varKey = "", oldKey = "";
        while (endPos < lastPos)
        {
            startPos = endPos;
            endPos = value.indexOf('{', startPos);
            if (endPos == -1)
                endPos = lastPos;
            result.append(value.substring(startPos, endPos));
            if (endPos < lastPos)
                endPos++;
            if (endPos < lastPos)
            {
                startPos = endPos;
                endPos = value.indexOf('}', startPos);
                if (endPos == -1)
                    endPos = lastPos;
                oldKey = varKey;
                varKey = value.substring(startPos, endPos);
                if (!oldKey.equals(varKey))
                    varValue = dictionary.get(varKey);
                if (varValue != null)
                {
                    if (logger.isDebugEnabled()) logger.debug("Substituting var: " + varKey + " --> " + varValue);
                    result.append(varValue);
                }
                else
                {
                    if (logger.isWarnEnabled()) logger.warn(name + ": var not found: " + varKey);
                    result.append('{').append(varKey).append('}');
                }
                if (endPos < lastPos)
                    endPos++;
            }
        }
        return result.toString();
    }

    /**
     * Set the logger.
     *
     * @param logger the logger
     */
    public void setLogger( final Logger logger )
    {
        this.logger = logger;
    }

    /**
     * Return an Object by key.
     * Implementation of the ResourceBundle abstract method.
     *
     * @param key the key
     * @return the object
     */
    protected Object handleGetObject(String key)
        throws MissingResourceException
    {
        return (Object) getString(key, null);
    }

    /**
     * Return an enumeration of the keys.
     * Implementation of the ResourceBundle abstract method.
     *
     * @return the enumeration of keys
     */
    public Enumeration getKeys()
    {
        return cache.keys();
    }
}
