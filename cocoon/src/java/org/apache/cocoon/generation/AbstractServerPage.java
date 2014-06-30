/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.generation;

import org.apache.cocoon.caching.CacheValidity;
import org.apache.cocoon.caching.Cacheable;
import org.apache.cocoon.caching.NOPCacheValidity;
import org.apache.cocoon.components.language.generator.CompiledComponent;
import org.apache.cocoon.environment.Request;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.File;

/**
 * Base implementation of <code>ServerPagesGenerator</code>. This class
 * declares variables that must be explicitly initialized by code generators.
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/08 10:47:45 $
 */
public abstract class AbstractServerPage
  extends ServletGenerator implements CompiledComponent, Cacheable
{
  /**
   * Code generators should produce a static
   * block that initializes the generator's
   * creation date and file dependency list.
   * Example:
   *
   *  static {
   *    dateCreated = 958058788948L;
   *    dependencies = new File[] {
   *      new File("source.xml"),
   *    };
   *  }
   *
   */

  /** The creation date */
  protected static long dateCreated = -1L;
  /** The dependency file list */
  protected static File[] dependencies = null;

  /**
   * Determines whether this generator's source files have changed
   *
   * @return Whether any of the files this generator depends on has changed
   * since it was created
   */
  public final boolean modifiedSince(long date) {
    if (dateCreated < date) {
      return true;
    }

    for (int i = 0; i < dependencies.length; i++) {
      if (dateCreated < dependencies[i].lastModified()) {
        return true;
      }
    }

    return false;
  }

  /**
   * Determines whether generated content has changed since
   * last invocation. Users may override this method to take
   * advantage of SAX event cacheing
   *
   * @param request The request whose data must be inspected to assert whether
   * dynamically generated content has changed
   * @return Whether content has changes for this request's data
   */
  public boolean hasContentChanged(Request request) {
    return true;
  }

  /**
   * Generates the unique key.
   * This key must be unique inside the space of this component.
   * Users may override this method to take
   * advantage of SAX event cacheing
   *
   * @return A long representing the cache key (defaults to not cachable)
   */
  public long generateKey() {
    return 0;
  }

  /**
   * Generate the validity object.
   *
   * @return The generated validity object, <code>NOPCacheValidity</code>
   *         is the default if hasContentChange() gives false otherwise
   *         <code>null</code> will be returned.
   */
  public CacheValidity generateValidity() {
    if (hasContentChanged(request))
      return null;
    else
      return NOPCacheValidity.CACHE_VALIDITY;
  }

  // FIXME: Add more methods!
  /* SAX Utility Methods */
  /**
   * Add an attribute
   *
   * @param attr The attribute list to add to
   * @param name The attribute name
   * @param value The attribute value
   */
  protected void attribute(AttributesImpl attr, String name, String value) {
    attr.addAttribute("", name, name, "CDATA", value);
  }

  /**
   * Start an element
   *
   * @param name The element name
   * @param attr The element attributes
   */
  protected void start(String name, AttributesImpl attr) throws SAXException {
    this.contentHandler.startElement("", name, name, attr);
    attr.clear();
  }

  /**
   * End an element
   *
   * @param name The element name
   */
  protected void end(String name) throws SAXException {
    this.contentHandler.endElement("", name, name);
  }

  /**
   * Add character data
   *
   * @param data The character data
   */
  protected void characters(String data) throws SAXException {
    this.contentHandler.characters(data.toCharArray(), 0, data.length());
  }

  /**
   * Add a comment
   *
   * @param comment The comment data
   */
  protected void comment(String data) throws SAXException {
    this.lexicalHandler.comment(data.toCharArray(), 0, data.length());
  }
}
