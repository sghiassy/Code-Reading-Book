/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.*;

import java.util.*;

/**
 * LangSelect Action returns two character language code to sitemap.
 *
 * Definition in sitemap:
 * <pre>
 * &lt;map:actions&gt;
 *                &lt;map:action name="lang_select" src="org.apache.cocoon.acting.LangSelect"/&gt;
 * </pre>
 *
 * And you use it in following way:
 *
 * <pre>
 * &lt;map:match pattern="file"&gt;
 * &lt;map:act type="lang_select"&gt;
 *                &lt;map:generate src="file_{lang}.xml"/&gt;
 * &lt;/map:act&gt;
 * </pre>
 *
 * {lang} is substituted with language code.
 * eg. if user selects url ../file?lang=en
 * then Sitemap engine generates file_en.xml source.
 *
 *
 * Creation date: (3.11.2000 14:32:19)
 * Modification date: (29.05.2001 0:30:01)
 * @author: <a href="mailto:kpiroumian@flagship.ru">Konstantin Piroumian</a>
 * @author: <a href="mailto:lassi.immonen@valkeus.com">Lassi Immonen</a>
 */
public class LangSelect extends java.lang.Object implements Action, Configurable, ThreadSafe {

    /**
     * The default language: en (English).
     */
    private final static String DEFAULT_LANG = "en";

    /**
     * The name that will be used as a request param or a cookie name.
     */
    private final static String PARAM_LANG = "lang";

    /**
     * Should we store the lang in request.
     * Default is off.
     */
     private boolean storeInRequest = false;

    /**
     * Should we store the lang in session, if one is available.
     * Default is off.
     */
     private boolean storeInSession = false;

    /**
     * Should we create a session to store the lang if it's not created yet.
     * Not used if use-session is false.
     * Default is off.
     */
    private boolean createSession = false;

    /**
     * Should we add a cookie with the lang.
     */
    private boolean storeInCookie = false;

    /**
     * Configure this action.
     */
    public void configure(Configuration conf)
    throws ConfigurationException {
        if (conf != null) {

            Configuration child = conf.getChild("store-in-request");
            this.storeInRequest = child.getValueAsBoolean(false);

            child = conf.getChild("create-session");
            this.createSession = child.getValueAsBoolean(false);
//            getLogger().debug("Create session is " + this.createSession + " for " + this);

            child = conf.getChild("store-in-session");
            this.storeInSession = child.getValueAsBoolean(false);
//            getLogger().debug("Store in session is " + this.storeInSession + " for " + this);

            child = conf.getChild("store-in-cookie");
            this.storeInCookie = child.getValueAsBoolean(false);
//            getLogger().debug("Use cookie is " + this.storeInCookie + " for " + this);
        }
    }

    /**
     * Selects language if it is not set already in objectModel
     * Puts lang parameter to session and to objectModel
     * if it is not already there.
     */
    public Map act(Redirector redirector, SourceResolver resolver, Map objectModel, String source,
            Parameters par) throws Exception {

        String lang;

        if (objectModel.containsKey(PARAM_LANG)) {
            lang = (String)(objectModel.get(PARAM_LANG));
        } else {
            lang = getLang(objectModel, par);
            objectModel.put(PARAM_LANG, lang);
        }

        // Creating session to store the language code
        Request request =
                (Request)(objectModel.get(Constants.REQUEST_OBJECT));
        request.getSession(createSession);

        if (storeInSession || storeInCookie) {
                storeLang(objectModel, lang);
        }

        Map m = new HashMap(1);
        m.put(PARAM_LANG, lang);
        return m;
    }

    /**
     * Returns two character language code by checking environment in following order
     * <ol>
     *   <li>Http request has parameter lang</li>
     *   <li>Http session has parameter lang</li>
     *   <li>Cookies has parameter lang</li>
     *   <li>User locales has matching language we are providing</li>
     *   <li>Otherwise we return default_lang from sitemap or if that is not found then 'en'</li>
     * </ol>
     * @return java.lang.String
     * @param objectModel java.util.Map
     * @param par org.apache.avalon.framework.parameter.Parameters
     */
    public static String getLang(Map objectModel, Parameters par) {

        List langs_avail = new ArrayList();
        List langs_user = new ArrayList();

        Iterator params = par.getParameterNames();
        while (params.hasNext()) {
            String paramname = (String)(params.next());
            if (paramname.startsWith("available_lang")) {
                langs_avail.add(par.getParameter(paramname, null));
            }
        }
        String def_lang = par.getParameter("default_lang", LangSelect.DEFAULT_LANG);

        Request request =
                (Request)(objectModel.get(Constants.REQUEST_OBJECT));

        String lang = null;

        lang = request.getParameter(PARAM_LANG);

        if (lang == null) {
            Session session = request.getSession(false);
            if (session != null) {
                Object session_lang = session.getAttribute(PARAM_LANG);
                if (session_lang != null) {
                    lang = session_lang.toString();
                }
            }

        }

        if (lang == null) {
            Cookie[] cookies = request.getCookies();
            if (cookies != null) {
                for ( int i = 0; i < cookies.length; i++) {
                    Cookie cookie = cookies[i];
                    if (cookie.getName().equals(PARAM_LANG)) {
                        lang = cookie.getValue();
                    }
                }
            }
        }

        if (lang == null) {

            Enumeration locales = request.getLocales();
            while (locales.hasMoreElements()) {
                Locale locale = (Locale)(locales.nextElement());
                langs_user.add(locale.getLanguage());
            }

            boolean match = false;
            int i = 0;

            for ( ; i < langs_user.size() && !match; i++) {
                for ( int j = 0; j < langs_avail.size(); j++) {
                    if (((String)(langs_user.get(i))).equals(
                            (String)(langs_avail.get(j)))) {
                        match = true;
                        break;
                    }
                }
            }
            if (match) {
                lang = (String)(langs_user.get(i - 1));
            } else {
                lang = def_lang;
            }
        }
        return lang;
    }

    /**
     * Stores language code in the session or a cookie
     * @param objectModel java.util.Map
     * @param lang String The language code to store
     */
    public void storeLang(Map objectModel, String lang) throws Exception {

        if (lang == null) {
            lang = DEFAULT_LANG;
        }

         Request request =
             (Request)(objectModel.get(Constants.REQUEST_OBJECT));

        if (storeInRequest) {
            request.setAttribute(PARAM_LANG, lang);
        }

        if (storeInSession) {
            // We do not create session here to keep this method static.
            // Session must be created outside if needed.
            Session session = request.getSession(false);
            // Try to store in a session.
            if (session != null) {
                synchronized (session) {
                    session.setAttribute(PARAM_LANG, lang);
                }
            }
            else {
                throw new Exception("LangSelect: session is not available.");
            }
        }

        if (storeInCookie) {
            Response response =
                (Response)(objectModel.get(Constants.RESPONSE_OBJECT));

            Cookie langCookie = response.createCookie(PARAM_LANG, lang);
            response.addCookie(langCookie);
        }
    }
}
