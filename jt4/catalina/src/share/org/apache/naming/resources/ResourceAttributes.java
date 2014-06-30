/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/naming/resources/ResourceAttributes.java,v 1.3 2001/09/13 16:31:42 remm Exp $
 * $Revision: 1.3 $
 * $Date: 2001/09/13 16:31:42 $
 *
 * ====================================================================
 *
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowlegement:  
 *       "This product includes software developed by the 
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowlegement may appear in the software itself,
 *    if and wherever such third-party acknowlegements normally appear.
 *
 * 4. The names "The Jakarta Project", "Tomcat", and "Apache Software
 *    Foundation" must not be used to endorse or promote products derived
 *    from this software without prior written permission. For written 
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 * [Additional notices, if required by prior licensing conditions]
 *
 */ 

package org.apache.naming.resources;

import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;
import java.util.Date;
import java.util.StringTokenizer;
import java.util.Locale;
import java.text.SimpleDateFormat;
import java.text.DateFormat;
import java.text.ParseException;
import javax.naming.NamingException;
import javax.naming.NamingEnumeration;
import javax.naming.directory.Attributes;
import javax.naming.directory.BasicAttributes;
import javax.naming.directory.BasicAttribute;
import javax.naming.directory.Attribute;

/**
 * Attributes implementation.
 * 
 * @author <a href="mailto:remm@apache.org">Remy Maucherat</a>
 * @version $Revision: 1.3 $
 */
public class ResourceAttributes extends BasicAttributes {
    
    
    // -------------------------------------------------------------- Constants
    
    
    // Default attribute names
    
    /**
     * Creation date.
     */
    public static final String CREATION_DATE = "creationdate";
    
    
    /**
     * Creation date.
     */
    public static final String ALTERNATE_CREATION_DATE = "last-modified";
    
    
    /**
     * Last modification date.
     */
    public static final String LAST_MODIFIED = "getlastmodified";
    
    
    /**
     * Last modification date.
     */
    public static final String ALTERNATE_LAST_MODIFIED = "last-modified";
    
    
    /**
     * Name.
     */
    public static final String NAME = "displayname";
    
    
    /**
     * Type.
     */
    public static final String TYPE = "resourcetype";
    
    
    /**
     * Type.
     */
    public static final String ALTERNATE_TYPE = "content-type";
    
    
    /**
     * Source.
     */
    public static final String SOURCE = "source";
    
    
    /**
     * MIME type of the content.
     */
    public static final String CONTENT_TYPE = "getcontenttype";
    
    
    /**
     * Content language.
     */
    public static final String CONTENT_LANGUAGE = "getcontentlanguage";
    
    
    /**
     * Content length.
     */
    public static final String CONTENT_LENGTH = "getcontentlength";
    
    
    /**
     * Content length.
     */
    public static final String ALTERNATE_CONTENT_LENGTH = "content-length";
    
    
    /**
     * ETag.
     */
    public static final String ETAG = "getetag";
    
    
    /**
     * Collection type.
     */
    public static final String COLLECTION_TYPE = "<collection/>";
    
    
    /**
     * HTTP date format.
     */
    protected static final SimpleDateFormat format =
        new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss zzz", Locale.US);
    
    
    /**
     * Date formats using for Date parsing.
     */
    protected static final SimpleDateFormat formats[] = {
        new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss zzz", Locale.US),
        new SimpleDateFormat("EEE MMM dd HH:mm:ss zzz yyyy", Locale.US),
        new SimpleDateFormat("EEEEEE, dd-MMM-yy HH:mm:ss zzz", Locale.US),
        new SimpleDateFormat("EEE MMMM d HH:mm:ss yyyy", Locale.US)
    };
    
    
    // ----------------------------------------------------------- Constructors
    
    
    /**
     * Default constructor.
     */
    public ResourceAttributes() {
        super(false);
    }
    
    
    /**
     * Merges with another attribute set.
     */
    public ResourceAttributes(Attributes attributes) {
        super(false);
        // Merging attributes
        try {
            Enumeration enum = attributes.getAll();
            while (enum.hasMoreElements()) {
                Attribute attribute = (Attribute) enum.nextElement();
                // FIXME: Check if it's a protected attribute ?
                put(attribute);
            }
        } catch (Throwable t) {
        }
    }
    
    
    /**
     * Package private constructor (used for caching).
     */
    ResourceAttributes(Hashtable attributes) {
        super(false);
        this.protectedAttributes = attributes;
    }
    
    
    // ----------------------------------------------------- Instance Variables


    /**
     * Protected attributes.
     */
    protected Hashtable protectedAttributes = new Hashtable();


    // ------------------------------------------------------------- Properties


    // --------------------------------------------------------- Public Methods


    /**
     * Creation date accessor.
     * 
     * @return Creation date
     */
    public Date getCreationDate() {
        Attribute creationDate = getProtectedAttribute(CREATION_DATE);
        if (creationDate == null)
            creationDate = getProtectedAttribute(ALTERNATE_CREATION_DATE);
        if (creationDate == null)
            return null;
        try {
            if (creationDate.get() instanceof Date) {
                return (Date) creationDate.get();
            } else {
                String creationDateValue = creationDate.get().toString();
                Date result = null;
                // Parsing the HTTP Date
                for (int i = 0; (result == null) && 
                         (i < formats.length); i++) {
                    try {
                        result = formats[i].parse(creationDateValue);
                    } catch (ParseException e) {
                        ;
                    }
                }
                return result;
            }
        } catch (NamingException e) {
            // No value for the attribute (shouldn't happen)
            return null;
        }
    }
    
    
    /**
     * Last modified date accessor.
     * 
     * @return Last modification date
     */
    public Date getLastModified() {
        Attribute lastModified = getProtectedAttribute(LAST_MODIFIED);
        if (lastModified == null)
            lastModified = getProtectedAttribute(ALTERNATE_LAST_MODIFIED);
        if (lastModified == null)
            return null;
        try {
            if (lastModified.get() instanceof Date) {
                return (Date) lastModified.get();
            } else {
                String lastModifiedValue = lastModified.get().toString();
                Date result = null;
                // Parsing the HTTP Date
                for (int i = 0; (result == null) && 
                         (i < formats.length); i++) {
                    try {
                        result = formats[i].parse(lastModifiedValue);
                    } catch (ParseException e) {
                        ;
                    }
                }
                return result;
            }
        } catch (NamingException e) {
            // No value for the attribute (shouldn't happen)
            return null;
        }
    }
    
    
    /**
     * Content length accessor.
     * 
     * @return String
     */
    public long getContentLength() {
        Attribute contentLength = getProtectedAttribute(CONTENT_LENGTH);
        if (contentLength == null)
            contentLength = getProtectedAttribute(ALTERNATE_CONTENT_LENGTH);
        if (contentLength == null)
            return -1L;
        try {
            if (contentLength.get() instanceof Long) {
                return ((Long) contentLength.get()).longValue();
            }
            if (contentLength.get() instanceof String) {
                return (new Long((String) contentLength.get())).longValue();
            }
        } catch (NamingException e) {
        }
        return -1L;
        
    }
    
    
    /**
     * Is collection.
     */
    public boolean isCollection() {
        return (getResourceType().equals(COLLECTION_TYPE));
    }
    
    
    /**
     * Name accessor.
     * 
     * @return String name 
     */
    public String getName() {
        Attribute name = getProtectedAttribute(NAME);
        try {
            if (name == null) {
                return "";
            } else {
                return (String) name.get();
            }
        } catch (NamingException e) {
            return "";
        }
    }


    /**
     * Resource type accessor.
     * 
     * @return String resource type
     */
    public String getResourceType() {
        Attribute resourceType = getProtectedAttribute(TYPE);
        if (resourceType == null)
            resourceType = getProtectedAttribute(ALTERNATE_TYPE);
        if (resourceType == null)
            return "";
        try {
            return (String) resourceType.get();
        } catch (NamingException e) {
            return "";
        }
    }


    /**
     * Get attribute.
     */
    public Attribute get(String attrID) {
        // Try in the protected list first
        Attribute result = getProtectedAttribute(attrID);
        if (result != null)
            return result;
        else
            return super.get(attrID);
    }
    
    
    /**
     * Get all attributes.
     */
    public NamingEnumeration getAll() {
        Enumeration enum = super.getAll();
        Vector attributes = new Vector();
        while (enum.hasMoreElements()) {
            Object obj = enum.nextElement();
            attributes.addElement(obj);
        }
        enum = protectedAttributes.elements();
        while (enum.hasMoreElements()) {
            Object obj = enum.nextElement();
            attributes.addElement(obj);
        }
        return new RecyclableNamingEnumeration(attributes);
    }
    
    
    /**
     * Get all attribute IDs.
     */
    public NamingEnumeration getIDs() {
        Enumeration enum = super.getIDs();
        Vector attributeIDs = new Vector();
        while (enum.hasMoreElements()) {
            Object obj = enum.nextElement();
            attributeIDs.addElement(obj);
        }
        enum = protectedAttributes.elements();
        while (enum.hasMoreElements()) {
            Object obj = ((Attribute) enum.nextElement()).getID();
            attributeIDs.addElement(obj);
        }
        return new RecyclableNamingEnumeration(attributeIDs);
    }
    
    
    /**
     * Retrieves the number of attributes in the attribute set.
     */
    public int size() {
        return (protectedAttributes.size() + super.size());
    }
    
    
    /**
     * Generates the string representation of this attribute set.
     * The string consists of each attribute identifier and the contents
     * of each attribute. The contents of this string is useful
     * for debugging and is not meant to be interpreted programmatically.
     * 
     * @return A non-null string listing the contents of this attribute set.
     */
    public String toString() {
        String result = protectedAttributes.toString();
        if (super.size() > 0)
            result += super.toString();
        return result;
    }
    
    
    // -------------------------------------------------------- Package Methods


    /**
     * Creation date mutator.
     * 
     * @param creationDate New creation date
     */
    public void setCreationDate(Date creationDate) {
        setProtectedAttribute(CREATION_DATE, creationDate);
    }
    
    
    /**
     * Last modified mutator.
     * 
     * @param lastModified New last modified date
     */
    public void setLastModified(Date lastModified) {
        setProtectedAttribute(LAST_MODIFIED, lastModified);
    }
    
    
    /**
     * Content length mutator.
     * 
     * @param contentLength New content length
     */
    public void setContentLength(long contentLength) {
        setProtectedAttribute(CONTENT_LENGTH, new Long(contentLength));
    }
    
    
    /**
     * Name mutator.
     * 
     * @param name New name
     */
    public void setName(String name) {
        setProtectedAttribute(NAME, name);
    }
    
    
    /**
     * Type mutator.
     * 
     * @param resourceType New resource type
     */
    public void setResourceType(String resourceType) {
        setProtectedAttribute(TYPE, resourceType);
    }
    
    
    /**
     * Get all the attributes in a Hashtable. Will mutate the 
     * protectedAttributes hashtable.
     */
    Hashtable getAttributes() {
        Enumeration enum = super.getAll();
        while (enum.hasMoreElements()) {
            Attribute attribute = (Attribute) enum.nextElement();
            String id = attribute.getID();
            if (!protectedAttributes.containsKey(id)) {
                protectedAttributes.put(id, attribute);
            }
        }
        return protectedAttributes;
    }
    
    
    // ------------------------------------------------------ Protected Methods


    /**
     * Protected attribute mutator.
     * 
     * @param name Attribute name
     * @param value Attribute value
     */
    protected void setProtectedAttribute(String name, Object value) {
        Attribute attribute = new BasicAttribute(name, value);
        protectedAttributes.put(name, attribute);
    }
    
    
    /**
     * Protected attribute accessor.
     * 
     * @param name Attribute name
     */
    protected Attribute getProtectedAttribute(String name) {
        Attribute result = null;
        if (name.startsWith(Constants.DEFAULT_NAMESPACE)) {
            result = (Attribute) protectedAttributes.get
                (name.substring(Constants.DEFAULT_NAMESPACE.length()));
        } else {
            result = (Attribute) protectedAttributes.get(name);
        }
        if (result != null)
            return result;
        return super.get(name);
    }
    
    
    /**
     * Initialize the set of protected attributes.
     */
    protected void intializeAttributes() {
        setCreationDate(new Date());
        setProtectedAttribute(NAME, "");
        // By default, a resource is a collection
        setProtectedAttribute(TYPE, "<collection/>");
        setProtectedAttribute(SOURCE, "");
        setContentLength(-1);
        setLastModified(new Date());
    }


}
