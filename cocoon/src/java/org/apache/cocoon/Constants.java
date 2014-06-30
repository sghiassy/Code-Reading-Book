/*
 * The Apache Software License, Version 1.1
 *
 *
 * Copyright (c) 2001 The Apache Software Foundation.  All rights
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
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Axis" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
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
 */

package org.apache.cocoon;

/**
 * The <code>Constants</code> use throughout the core of the Cocoon engine.
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @author <a href="mailto:proyal@managingpartners.com">Peter Royal</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:06 $
 */
public interface Constants {

    /** The name of this project. */
    String NAME          = "Apache Cocoon";

    /** The version of this build. */
    String VERSION       = "2.0.1";

    /** The full name of this project. */
    String COMPLETE_NAME = NAME + " " + VERSION;

    /** The version of the configuration schema */
    String CONF_VERSION  = "2.0";

    /** The year of the build */
    String YEAR          = "1999-2002";

    /** 
     * The request parameter name to reload the configuration.
     *
     * FIXME(GP): Isn't this Servlet specific?
     */
    String RELOAD_PARAM   = "cocoon-reload";

    /**
     * The request parameter name to add a line of the request duration.
     *
     * FIXME(GP): Isn't this Servlet specific?
     */
    String SHOWTIME_PARAM = "cocoon-showtime";

    /**
     * The request parameter name to request a specific view of a resource.
     *
     * FIXME(GP): Isn't this Servlet specific?
     */
    String VIEW_PARAM     = "cocoon-view";

    /**
     * The request parameter name to trigger a specific action.
     *
     * FIXME(GP): Isn't this Servlet specific?
     */
    String ACTION_PARAM   = "cocoon-action";

    /** The name of the property where temporary files will be written to. */
    String TEMPDIR_PROPERTY    = "org.apache.cocoon.properties.tempdir";

    /**
     * The directory to use as context root.
     *
     * FIXME(GP): Isn't this CLI specific?
     */
    String DEFAULT_CONTEXT_DIR = "./webapp";

    /**
     * The diretory to use to use for the generated output.
     *
     * FIXME(GP): Isn't this CLI specific?
     */
    String DEFAULT_DEST_DIR    = "./site";

    /**
     * The diretory to use for generated files.
     *
     * FIXME(GP): Isn't this CLI specific?
     */
    String DEFAULT_WORK_DIR    = "./work";

    /**
     * How a default configuration file is named.
     *
     * FIXME(GP): Isn't this CLI specific?
     */
    String DEFAULT_CONF_FILE   = "cocoon.xconf";

    /** The name of the property holding the class for a XML parser */
    String PARSER_PROPERTY = "org.apache.cocoon.components.parser.Parser";

    /** The name of the class for the default XML parser to use */
    String DEFAULT_PARSER  = "org.apache.cocoon.components.parser.JaxpParser";

    /** The namespace prefix for the XSP core logicsheet. */
    String XSP_PREFIX                = "xsp";

    /** The namespace for the XSP core logicsheet. */
    String XSP_URI                   = "http://apache.org/xsp";

    /** 
     * The namespace prefix for the request logicsheet. 
     *
     * FIXME(GP): Would logicsheets belong to the core?
     */
    String XSP_REQUEST_PREFIX        = "xsp-request";

    /** 
     * The namespace for the request logicsheet. 
     *
     * FIXME(GP): Would logicsheets belong to the core?
     */
    String XSP_REQUEST_URI           = XSP_URI + "/request/2.0";

    /** 
     * The namespace prefix for the response logicsheet. 
     *
     * FIXME(GP): Would logicsheets belong to the core?
     */
    String XSP_RESPONSE_PREFIX       = "xsp-response";

    /** 
     * The namespace for the response logicsheet. 
     *
     * FIXME(GP): Would logicsheets belong to the core?
     */
    String XSP_RESPONSE_URI          = XSP_URI + "/response/2.0";

    /** 
     * The namespace prefix for the cookie logicsheet. 
     *
     * FIXME(GP): Would logicsheets belong to the core?
     */
    String XSP_COOKIE_PREFIX         = "xsp-cookie";

    /** 
     * The namespace for the cookie logicsheet. 
     *
     * FIXME(GP): Would logicsheets belong to the core?
     */
    String XSP_COOKIE_URI            = XSP_URI + "/cookie/2.0";

    /** 
     * Don't know exactly what this is for. (I can guess it's for the FormValidator)
     *
     * FIXME(GP): Isn't this component specific?
     */
    String XSP_FORMVALIDATOR_PATH    = "org.apache.cocoon.acting.FormValidatorAction.results";

    /**
     * Don't know exactly what this is for. (I can guess it's for the FormValidator)
     *
     * FIXME(GP): Would logicsheets belong to the core?
     */
    String XSP_FORMVALIDATOR_PREFIX  = "xsp-formval";
    /**
     * Don't know exactly what this is for. (I can guess it's for the FormValidator)
     *
     * FIXME(GP): Would logicsheets belong to the core?
     */
    String XSP_FORMVALIDATOR_URI     = Constants.XSP_URI + "/form-validator/2.0";

    /** The URI for xml namespaces */
    String XML_NAMESPACE_URI = "http://www.w3.org/XML/1998/namespace";

    /**
     * Mime-type for the link view
     *
     * FIXME(GP): Isn't this Environment specific?
     */
    String LINK_CONTENT_TYPE     = "application/x-cocoon-links";

    /**
     * Name of the request value for the link view
     *
     * FIXME(GP): Isn't this Environment specific?
     */
    String LINK_VIEW             = "links";

    /** Don't know exactly what this is for (and it is not used in the code base) */
    String LINK_CRAWLING_ROLE    = "static";

    /** 
     * The name of a <code>Request</code> object in the so called objectModel <code>Map</code>. 
     * This constant is also defined in the org.apache.cocoon.environment.ObjectModelHelper class.
     *
     * FIXME(GP): Shouldn't this definition here be removed?
     */
    String REQUEST_OBJECT  = "request";

    /** 
     * The name of a <code>Response</code> object in the so called objectModel <code>Map</code>.
     * This constant is also defined in the org.apache.cocoon.environment.ObjectModelHelper class.
     *
     * FIXME(GP): Shouldn't this definition here be removed?
     */
    String RESPONSE_OBJECT = "response";

    /**
     *The name of a <code>Context</code> object in the so called objectModel <code>Map</code>. 
     * This constant is also defined in the org.apache.cocoon.environment.ObjectModelHelper class.
     *
     * FIXME(GP): Shouldn't this definition here be removed?
     */
    String CONTEXT_OBJECT  = "context";

    /**
     * Describe variable <code>LINK_OBJECT</code> here.
     *
     */
    String LINK_OBJECT     = "link";
    
    /**
     * The name of a <code>NotifyingObject</code> in the so called objectModel <code>Map</code>.
     */   
    String NOTIFYING_OBJECT = "notifying-object";

    /**
     * Describe variable <code>INDEX_URI</code> here.
     *
     * FIXME(GP): It seems to be (CLI) Environment specific!
     */
    String INDEX_URI = "index";

    /** The namespace URI for the Error/Exception XML */
    String ERROR_NAMESPACE_URI = "http://apache.org/cocoon/error/" + CONF_VERSION;

    /** The namespace prefix for the Error/Exception XML */
    String ERROR_NAMESPACE_PREFIX = "error";

    /** Application <code>Context</code> Key for the environmental Context */
    String CONTEXT_ENVIRONMENT_CONTEXT = "environment-context";

    /** Application <code>Context</code> Key for the global classloader */
    String CONTEXT_CLASS_LOADER    = "class-loader";

    /** Application <code>Context</code> Key for the work directory path */
    String CONTEXT_WORK_DIR        = "work-directory";

    /** Application <code>Context</code> Key for the upload directory path */
    String CONTEXT_UPLOAD_DIR      = "upload-directory";

    /** Application <code>Context</code> Key for the cache directory path */
    String CONTEXT_CACHE_DIR       = "cache-directory";

    /** Application <code>Context</code> Key for the current classpath */
    String CONTEXT_CLASSPATH       = "classpath";

    /** 
     * Application <code>Context</code> Key for the URL to the configuration file 
     * (usually named cocoon.xconf) 
     */
    String CONTEXT_CONFIG_URL      = "config-url";

    /** @deprecated this is no longer used */
    String SESSION_STATE_ATTRIBUTE = "org.apache.cocoon.SessionState";

    /**
     * Should descriptors be reloaded?
     *
     * FIXME(GP): Isn't this Action specific only?
     */
    boolean DESCRIPTOR_RELOADABLE_DEFAULT = true;

}


