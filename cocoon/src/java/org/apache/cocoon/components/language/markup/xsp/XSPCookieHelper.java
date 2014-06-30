/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup.xsp;

import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Cookie;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Response;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Map;

/**
 * This class is a helper class used by Cookie logicsheet
 *
 * This class contains following methods:
 * <pre>
 * public static void addCookie(Map , String , String , String , String ,
 *                              int , String , String , int);
 * public static Cookie[] getCookies(Map);
 * public static void getCookies(Map , ContentHandler)
 *     throws SAXException;
 * public static Cookie getCookie(Map , String ,int )
 *     throws SAXException;
 * public static void getCookie(Map ,String ,int , ContentHandler)
 *     throws SAXException;
 * public static String getComment(Map ,String , int);
 * public static String getDomain(Map , String , int);
 * public static String getMaxAge(Map ,String , int);
 * public static String getName(Map ,String , int);
 * public static String getPath(Map , String , int);
 * public static String getSecure(Map , String , int);
 * public static String getValue(Map , String , int);
 * public static String getVersion(Map , String , int);
 * private static String returnCookieProperty(Map ,String ,int ,String );
 * </pre>
 *
 * @version CVS $Revision: 1.4 $ $Date: 2002/01/26 17:31:44 $
 */
public class XSPCookieHelper extends XSPObjectHelper {
    /**
     * Assign values to the object's namespace uri and prefix
     */
    private static final String URI = Constants.XSP_COOKIE_URI;
    private static final String PREFIX = Constants.XSP_COOKIE_PREFIX;

    /**
     * This method will set a new cookie with values that are passed through parameters
     *
     * @param objectModel
     * @param name name to be set for the cookie
     * @param value value to be set for the cookie
     * @param comment comment to be set for the cookie
     * @param domain domain to be set for the cookie
     * @param maxage maxage to be set for the cookie
     * @param path path to be set for the cookie
     * @param secure secure property to be set for the cookie
     * @param version version to be set for the cookie
     */
    public static void addCookie(Map objectModel, String name, String value,
                                    String comment, String domain, int maxage, String path,
                                    String secure, int version)
    {
        Response response = (Response)objectModel.get(Constants.RESPONSE_OBJECT);
        Cookie cookieToSet = response.createCookie(name,value);

        if ((comment.trim()).length() > 0)
            cookieToSet.setComment(comment);

        if ((domain.trim()).length() > 0)
            cookieToSet.setDomain(domain);

        if (maxage > 0)
            cookieToSet.setMaxAge(maxage);

        if ((path.trim()).length() > 0)
            cookieToSet.setPath("/");

        if (secure == "true")
            cookieToSet.setSecure(true);
        else
            cookieToSet.setSecure(false);

        cookieToSet.setVersion(version);
        response.addCookie(cookieToSet);
    }


    /**
     * This method is used to return all the cookies that present in the passed request object
     *
     * @param objectModel
     * @return an array of Cookie is returned
     */
    public static Cookie[] getCookies(Map objectModel)
    {
        Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
        return request.getCookies();
    }

    /**
     * This method is used to write the values of all the cookies in the resulting XML tree
     * The structure that will be added to the XML tree will be
     * <pre>
     * &lt;cookies&gt;
     *   &lt;cookie&gt;
     *     &lt;name&gt;......&lt;/name&gt;
     *     &lt;value&gt;.....&lt;/value&gt;
     *     &lt;comment&gt;...&lt;/comment&gt;
     *     &lt;domain&gt;....&lt;/domain&gt;
     *     &lt;maxage&gt;....&lt;/maxage&gt;
     *     &lt;path&gt;......&lt;/path&gt;
     *     &lt;secure&gt;....&lt;/secure&gt;
     *     &lt;version&gt;...&lt;/version&gt;
     *   &lt;/cookie&gt;
     *   &lt;cookie&gt;
     *     ...
     *   &lt;/cookie&gt;
     *   ...
     * &lt;/cookies&gt;
     * </pre>
     * If the values of any of these is not present those tags will not be present.
     * @param objectModel
     * @param contentHandler
     * @exception SAXException
     */
    public static void getCookies(Map objectModel, ContentHandler contentHandler)
        throws SAXException
    {
        Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
        Cookie[] cookies = request.getCookies();

        if(cookies != null && cookies.length > 0)
        {
            int count  = 0;

            String tempStr = null;

            Hashtable nodeTable = new Hashtable();
            XSPObjectHelper.start(URI, PREFIX, contentHandler, "cookies");

            for(count=0; count<cookies.length; count++)
            {
                XSPObjectHelper.start(URI, PREFIX, contentHandler, "cookie");

                if ((tempStr = getName(objectModel , null , count)) != null)
                    nodeTable.put("name", tempStr);

                if((tempStr = getValue(objectModel , null , count))!=null)
                    nodeTable.put("value", tempStr);

                if((tempStr = getComment(objectModel , null , count))!=null)
                    nodeTable.put("comment", tempStr);

                if((tempStr = getDomain(objectModel , null , count))!= null)
                    nodeTable.put("domain", tempStr);

                if((tempStr = getMaxAge(objectModel , null , count))!=null)
                    nodeTable.put("maxage", tempStr);

                if((tempStr = getPath(objectModel , null , count)) != null)
                    nodeTable.put("path", tempStr);

                if((tempStr = getSecure(objectModel , null , count)) !=null)
                    nodeTable.put("secure", tempStr);

                if((tempStr = getVersion(objectModel , null , count)) != null)
                    nodeTable.put("version", tempStr);

                Enumeration keys = nodeTable.keys();
                while (keys.hasMoreElements())
                {
                    String nodeName = (String)keys.nextElement();
                    String nodeValue = (String)nodeTable.get(nodeName);
                    XSPObjectHelper.elementData(URI, PREFIX, contentHandler, nodeName, nodeValue);
                }

                XSPObjectHelper.end(URI, PREFIX, contentHandler, "cookie");
            }

            XSPObjectHelper.end(URI, PREFIX, contentHandler, "cookies");
        }
    }

    /**
     * Method used to return a cookie object based on the name or the index that was passed
     *
     * If both name and index of cookie to be extracted is passed in, name will take
     * precedence. Basic thing followed is that, when name is passed, index should be -1 and
     * when index is passed name should null
     * 
     * @param objectModel
     * @param cookieName Name of the cookie which is to be found and returned back
     * @param cookieIndex Index of the cookie which is to be found and returned
     * @return cookie object is returned
     * @exception SAXException
     */
    public static Cookie getCookie(Map objectModel,
                                   String cookieName,
                                   int cookieIndex)
    {
        boolean retrieveByName = false;
        boolean retrieveByIndex = false;
        boolean matchFound = false;

        int count = 0;

        Request request = (Request)objectModel.get(Constants.REQUEST_OBJECT);
        Cookie currentCookie = null;

        if (cookieName != null) {
            retrieveByName = true;
        } else if (cookieIndex >=0) {
            retrieveByIndex =  true;
        }

        Cookie[] cookies = request.getCookies();
        if (cookies != null && retrieveByName) {
            for(count = 0; count < cookies.length; count++) {
                currentCookie = cookies[count];
                if (currentCookie.getName().equals(cookieName)) {
                    matchFound = true;
                    break;
                }
            }
        } else if(cookies != null && retrieveByIndex) {
            if(cookies.length > cookieIndex) {
                currentCookie = cookies[cookieIndex];
                matchFound = true;
            }
        }

        if (matchFound)
            return currentCookie;
        else
            return null;
    }

    /**
     * This method is used to find a cookie by it's name or index and place it in
     * the XML resulting tree
     * 
     * The xml structure that will be inserted will be,
     * <pre>
     * &lt;cookie&gt;
     *     &lt;name&gt;......&lt;/name&gt;
     *     &lt;value&gt;.....&lt;/value&gt;
     *     &lt;comment&gt;...&lt;/comment&gt;
     *     &lt;domain&gt;....&lt;/domain&gt;
     *     &lt;maxage&gt;....&lt;/maxage&gt;
     *     &lt;path&gt;......&lt;/path&gt;
     *     &lt;secure&gt;....&lt;/secure&gt;
     *     &lt;version&gt;...&lt;/version&gt;
     * &lt;/cookie&gt;
     * </pre>
     * @param objectModel
     * @param cookieName name of the cookie which is to be found
     * @param cookieIndex index of the cookie which is to be found
     * @param ContentHandler
     * @exception SAXException
     */
    public static void getCookie(Map objectModel,
                                 String cookieName,
                                 int cookieIndex,
                                 ContentHandler contentHandler)
        throws SAXException
    {
        boolean retrieveByName = false;
        boolean retrieveByIndex = false;

        String tempStr = null;
        Hashtable nodeTable = new Hashtable();

        if (cookieName != null) {
            retrieveByName = true;
        } else if (cookieIndex >=0) {
            retrieveByIndex =  true;
        }

        if (retrieveByName || retrieveByIndex)
            tempStr = getName(objectModel , cookieName , cookieIndex);

        if (tempStr !=null) {
            XSPObjectHelper.start(URI, PREFIX, contentHandler, "cookie");

            // name
            nodeTable.put("name", tempStr);

            // value
            if ((tempStr = getValue(objectModel , cookieName , cookieIndex)) != null)
                    nodeTable.put("value", tempStr);

            //comment
            if ((tempStr = getComment(objectModel , cookieName , cookieIndex)) != null)
                nodeTable.put("comment", tempStr);

            //value
            if ((tempStr = getDomain(objectModel , cookieName , cookieIndex)) != null)
                nodeTable.put("domain", tempStr);

            // maxage
            if((tempStr = getMaxAge(objectModel , cookieName , cookieIndex)) != null)
                    nodeTable.put("maxage", tempStr);

            // path
            if((tempStr = getPath(objectModel , cookieName , cookieIndex)) != null)
                nodeTable.put("path", tempStr);

            // secure
            if((tempStr = getSecure(objectModel , cookieName , cookieIndex)) != null)
                nodeTable.put("secure", tempStr);

            // version
            if((tempStr = getVersion(objectModel , cookieName , cookieIndex)) != null)
                nodeTable.put("version", tempStr);

            Enumeration keys = nodeTable.keys();
            while (keys.hasMoreElements())
            {
                String nodeName = (String)keys.nextElement();
                String nodeValue = (String)nodeTable.get(nodeName);
                XSPObjectHelper.elementData(URI, PREFIX, contentHandler, nodeName, nodeValue);
            }

            XSPObjectHelper.end(URI, PREFIX, contentHandler, "cookie");
        }
    }

    /**
     * Method to return the value of comment for a particular cookie based
     * on it's name or index
     * 
     * Rule for passing name and index of the cookie remains same as specified in
     * previous method(s)
     * 
     * @param objectModel
     * @param cookieName name of the cookie whose comment is to be passed
     * @param cookieIndex index of the cookie whose comment is to be passed
     * @return a string is returned containing the comment of the cookie, if not found
     *         then null is returned
     */
    public static String getComment(Map objectModel, String cookieName, int cookieIndex)
    {
        return returnCookieProperty(objectModel , cookieName , cookieIndex , "C");
    }

    /**
     * Method to return the value of domain for a particular cookie based
     * on it's name or index
     * 
     * Rule for passing name and index of the cookie remains same as specified in
     * previous method(s)
     * 
     * @param objectModel
     * @param cookieName name of the cookie whose domain is to be passed
     * @param cookieIndex index of the cookie whose domain is to be passed
     * @return a string is returned containing the domain of the cookie, if not found
     *         then null is returned
     */
    public static String getDomain(Map objectModel, String cookieName , int cookieIndex)
    {
        return returnCookieProperty(objectModel , cookieName , cookieIndex , "D");
    }

    /**
     * Method to return the value of maxage for a particular cookie based
     * on it's name or index
     *
     * Rule for passing name and index of the cookie remains same as specified in
     * previous method(s)
     *
     * @param objectModel
     * @param cookieName name of the cookie whose maxage is to be passed
     * @param cookieIndex index of the cookie whose maxage is to be passed
     * @return a string is returned containing the maxage of the cookie, if not found
     * then null is returned
     */
    public static String getMaxAge(Map objectModel, String cookieName , int cookieIndex)
    {
        return returnCookieProperty(objectModel , cookieName , cookieIndex , "M");
    }

    /**
     * Method to return the value of name for a particular cookie based
     * on it's name or index
     *
     * Rule for passing name and index of the cookie remains same as specified in
     * previous method(s)
     *
     * @param objectModel
     * @param cookieName name of the cookie whose name is to be passed
     * @param cookieIndex index of the cookie whose name is to be passed
     * @return a string is returned containing the name of the cookie, if not found
     * then null is returned
     */
    public static String getName(Map objectModel, String cookieName , int cookieIndex)
    {
        return returnCookieProperty(objectModel , cookieName , cookieIndex , "N");
    }

    /**
     * Method to return the value of path for a particular cookie based
     * on it's name or index
     *
     * Rule for passing name and index of the cookie remains same as specified in
     * previous method(s)
     *
     * @param objectModel
     * @param cookieName name of the cookie whose path is to be passed
     * @param cookieIndex index of the cookie whose path is to be passed
     * @return a string is returned containing the path of the cookie, if not found
     *         then null is returned
     */
    public static String getPath(Map objectModel, String cookieName , int cookieIndex)
    {
        return returnCookieProperty(objectModel , cookieName , cookieIndex , "P");
    }

    /**
     * Method to return the value of secure property for a particular cookie based
     * on it's name or index
     *
     * Rule for passing name and index of the cookie remains same as specified in
     * previous method(s)
     *
     * @param objectModel
     * @param cookieName name of the cookie whose secure property is to be passed
     * @param cookieIndex index of the cookie whose secure property is to be passed
     * @return a string is returned containing the secure property of the cookie, if not found
     *         then null is returned
     */
    public static String getSecure(Map objectModel, String cookieName , int cookieIndex)
    {
        return returnCookieProperty(objectModel , cookieName , cookieIndex , "S");
    }

    /**
     * Method to return the value for a particular cookie based on it's name or index
     *
     * Rule for passing name and index of the cookie remains same as specified in
     * previous method(s)
     *
     * @param objectModel
     * @param cookieName name of the cookie whose value is to be passed
     * @param cookieIndex index of the cookie whose value
     * @return a string is returned containing the value of the cookie, if not found
     *         then null is returned
     */
    public static String getValue(Map objectModel, String cookieName , int cookieIndex)
    {
        return returnCookieProperty(objectModel , cookieName , cookieIndex , "V");
    }

    /**
     * Method to return the version of comment for a particular cookie based
     * on it's name or index
     *
     * Rule for passing name and index of the cookie remains same as specified in
     * previous method(s)
     *
     * @param objectModel
     * @param cookieName name of the cookie whose version is to be passed
     * @param cookieIndex index of the cookie whose version is to be passed
     * @return a string is returned containing the version of the cookie, if not found
     *         then null is returned
     */
    public static String getVersion(Map objectModel, String cookieName , int cookieIndex)
    {
        return returnCookieProperty(objectModel , cookieName , cookieIndex , "Ve");
    }

    /**
     * Method returnCookieProperty will be used to retrieve the property
     * value of cookie. This method will return the value based on the name
     * or the index of the cookie that is passed.
     *
     *     Cookie properties,
     *         comment = "C"
     *         domain  = "D"
     *         maxage  = "M"
     *         name    = "N"
     *         path    = "P"
     *         secure  = "S"
     *         value   = "V"
     *         version = "Ve"
     *
     * @param objectModel
     * @param cookieName Name of the cookie whose property is to be returned.
     *                   This value will be null if cookie index is specified
     * @param cookieIndex Index of the cookie whose property is to be returned.
     *                    This property will be -1 if cookie name is specified.
     *                    If both name and index are specified, name will take
     *                    preference.
     * @param valueOf Specifies the property whose value if to be retrieved.
     *                Properties have been specifed above
     * @return     If the name or index that is passed is improper then a null value
     *             will be returned, otherwise whatever will be the extracted value of
     *             the property will be returned.
     */
    private static String returnCookieProperty(Map objectModel,
                                               String cookieName,
                                               int cookieIndex,
                                               String propertyPrefix)
    {
        Cookie currentCookie = getCookie(objectModel, cookieName, cookieIndex);

        String returnValue = null;
        if (currentCookie != null) {
            if(propertyPrefix.equals("C"))
                returnValue = currentCookie.getComment();
            else if(propertyPrefix.equals("D"))
                returnValue = currentCookie.getDomain();
            else if(propertyPrefix.equals("M"))
                returnValue = Integer.toString(currentCookie.getMaxAge());
            else if(propertyPrefix.equals("N"))
                returnValue = currentCookie.getName();
            else if(propertyPrefix.equals("P"))
                returnValue = currentCookie.getPath();
            else if(propertyPrefix.equals("S"))
                returnValue = returnValue.valueOf(currentCookie.getSecure());
            else if(propertyPrefix.equals("V"))
                returnValue = currentCookie.getValue();
            else if(propertyPrefix.equals("Ve"))
                returnValue = Integer.toString(currentCookie.getVersion());
        }

        return returnValue;
    }
}
