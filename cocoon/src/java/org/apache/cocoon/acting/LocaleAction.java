/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;

import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.Cookie;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Response;
import org.apache.cocoon.environment.Session;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.i18n.I18nUtils;

/**
 * LocaleAction is a class which obtains the request's locale information
 * (language, country, variant) and makes it available to the
 * sitemap/pipeline.
 *
 * Definition in sitemap:
 * <pre>
 * &lt;map:actions&gt;
 *  &lt;map:action name="locale" src="org.apache.cocoon.acting.LocaleAction"/&gt;
 * &lt;/map:actions&gt;
 * </pre>
 *
 * Examples:
 *
 * <pre>
 * &lt;map:match pattern="file"&gt;
 *  &lt;map:act type="locale"&gt;
 *   &lt;map:generate src="file_{lang}{country}{variant}.xml"/&gt;
 *  &lt;/map:act&gt;
 * &lt;/map:match&gt;
 * </pre>
 *
 * or
 *
 * <pre>
 * &lt;map:match pattern="file"&gt;
 *  &lt;map:act type="locale"&gt;
 *   &lt;map:generate src="file.xml?locale={locale}"/&gt;
 *  &lt;/map:act&gt;
 * &lt;/map:match&gt;
 * </pre>
 *
 * <br>
 *
 * The variables <code>lang</code>, <code>country</code>,
 * <code>variant</code>, and <code>locale</code> are all available. Note that
 * <code>country</code> and <code>variant</code> can be empty, however
 * <code>lang</code> and <code>locale</code> will always contain a valid value.
 *
 * <br>
 *
 * The following search criteria are used in order when ascertaining locale
 * values:
 *
 * <ol>
 *   <li>Request CGI parameter <i>locale</i></li>
 *   <li>Session attribute <i>locale</i></li>
 *   <li>First matching Cookie parameter <i>locale</i>
 *       within each cookie sent with the current request</li>
 *   <li>Locale setting of the requesting object</li>
 * </ol>
 *
 * (in the case of language, if the above cases do not yield a valid value
 * the locale value of the server is used)
 *
 * <br>
 *
 * The attribute names can be configured/customized at action definition
 * and/or usage time, using the paramters
 * {language,country,variant,locale}-attribute.
 *
 * eg.
 *
 * <pre>
 * &lt;map:action name="locale" src="org.apache.cocoon.acting.LocaleAction"&gt;
 *  &lt;language-attribute&gt;lg&lt;/language-attribute&gt;
 * &lt;/map:action&gt;
 * </pre>
 *
 * or:
 *
 * <pre>
 * &lt;map:act type="locale"&gt;
 *  &lt;map:parameter name="language-attribute" value="lg"/&gt;
 * &lt;/map:act&gt;
 * </pre>
 *
 * <center>Code originated from org.apache.cocoon.acting.LangSelect</center>
 *
 * @author <a href="mailto:Marcus.Crafter@osa.de">Marcus Crafter</a>
 * @author <a href="mailto:kpiroumian@flagship.ru">Konstantin Piroumian</a>
 * @author <a href="mailto:lassi.immonen@valkeus.com">Lassi Immonen</a>
 */
public class LocaleAction extends ComposerAction implements ThreadSafe, Configurable {

    /**
     * Constant representing the language parameter
     */
    public static final String LANG = "lang";

    /**
     * Constant representing the country parameter
     */
    public static final String COUNTRY = "country";

    /**
     * Constant representing the variant parameter
     */
    public static final String VARIANT = "variant";

    /**
     * Constant representing the locale parameter
     */
    public static final String LOCALE = "locale";

    /**
     * Constant representing the language configuration attribute
     */
    public static final String LANG_ATTR = "language-attribute";

    /**
     * Constant representing the country configuration attribute
     */
    public static final String COUNTRY_ATTR = "country-attribute";

    /**
     * Constant representing the variant configuration attribute
     */
    public static final String VARIANT_ATTR = "variant-attribute";

    /**
     * Constant representing the locale configuration attribute
     */
    public static final String LOCALE_ATTR = "locale-attribute";

    /**
     * Constant representing the request storage configuration attribute
     */
    public static final String STORE_REQUEST = "store-in-request";

    /**
     * Constant representing the session creation configuration attribute
     */
    public static final String CREATE_SESSION = "create-session";

    /**
     * Constant representing the session storage configuration attribute
     */
    public static final String STORE_SESSION = "store-in-session";

    /**
     * Constant representing the cookie storage configuration attribute
     */
    public static final String STORE_COOKIE = "store-in-cookie";

    /**
     * Configure this action.
     *
     * @param conf configuration information (if any)
     */
    public void configure(Configuration conf)
    throws ConfigurationException {
        if (conf != null) {

            Configuration child = conf.getChild(STORE_REQUEST);
            storeInRequest = child.getValueAsBoolean(false);

            debug((storeInRequest ? "will" : "won't") + " set values in request");

            child = conf.getChild(CREATE_SESSION);
            createSession = child.getValueAsBoolean(false);

            debug((createSession ? "will" : "won't") + " create session");

            child = conf.getChild(STORE_SESSION);
            storeInSession = child.getValueAsBoolean(false);

            debug((storeInSession ? "will" : "won't") + " set values in session");

            child = conf.getChild(STORE_COOKIE);
            storeInCookie = child.getValueAsBoolean(false);

            debug((storeInCookie ? "will" : "won't") + " set values in cookies");

            child = conf.getChild(LANG_ATTR);
            langAttr = child.getValue(LANG);

            debug("global language attribute name is " + langAttr);

            child = conf.getChild(COUNTRY_ATTR);
            countryAttr = child.getValue(COUNTRY);

            debug("global country attribute name is " + countryAttr);

            child = conf.getChild(VARIANT_ATTR);
            variantAttr = child.getValue(VARIANT);

            debug("global variant attribute name is " + variantAttr);

            child = conf.getChild(LOCALE_ATTR);
            localeAttr = child.getValue(LOCALE);

            debug("global locale attribute name is " + localeAttr);
        }
    }

    /**
     * Action which obtains the current environments locale information, and
     * places it in the objectModel (and optionally in a session/cookie).
     */
    public Map act(
        Redirector redirector,
        SourceResolver resolver,
        Map objectModel,
        String source,
        Parameters par
    ) throws Exception {

        // obtain locale information (note, Locale.get* do not return null)
        String lc = getLocaleAttribute(objectModel);
        Locale locale = I18nUtils.parseLocale(lc);
        String l = locale.getLanguage();
        String c = locale.getCountry();
        String v = locale.getVariant();

        debug("obtained locale information, locale = " + lc);
        debug("language = " + l + ", country = " + c + ", variant = " + v);

        if (getLogger().isDebugEnabled()) {
            checkParams(par);
        }

        if (storeInRequest) {
            Request request =
               (Request) objectModel.get(Constants.REQUEST_OBJECT);

            request.setAttribute(localeAttr, lc);
            debug("updated request");
        }

        // store in session if so configured
        if (storeInSession) {
            Request request =
               (Request) objectModel.get(Constants.REQUEST_OBJECT);
            Session session = request.getSession(createSession);

            if (session != null) {
                session.setAttribute(localeAttr, lc);
                debug("updated session");
            }
        }

        // store in a cookie if so configured
        if (storeInCookie) {
            Response response =
               (Response) objectModel.get(Constants.RESPONSE_OBJECT);

            response.addCookie(response.createCookie(localeAttr, lc));
            debug("created cookies");
        }

        // set up a map for XPath Substituion
        Map map = new HashMap();
        map.put(langAttr, l);
        map.put(countryAttr, c);
        map.put(variantAttr, v);
        map.put(localeAttr, lc);

        debug("returning map for XPath substitutions");

        return map;
    }

    /**
     * Helper method to access Locale sub component values.
     *
     * @param objectModel requesting object's environment
     * @return locale value
     * @throws Exception should some error occur
     */
    public static String getLocaleAttribute(Map objectModel) throws Exception {

        String ret_val;

        // 1. Request CGI parameter 'locale'
        Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);

        if ((ret_val = request.getParameter(LOCALE)) != null)
            return ret_val;

        // 2. Session attribute 'locale'
        Session session = request.getSession(false);
        if (session != null &&
                ((ret_val = (String) session.getAttribute(LOCALE)) != null)
           )
            return ret_val;

        // 3. First matching cookie parameter 'locale' within each cookie sent
        Cookie[] cookies = request.getCookies();

        if (cookies != null) {
            for (int i = 0; i < cookies.length; ++i) {
                Cookie cookie = cookies[i];
                if (cookie.getName().equals(LOCALE))
                    return cookie.getValue();
            }
        }

        // 4. Locale setting of the requesting object/server
        return request.getLocale().toString();
    }

    /**
     * Method to check <map:act type="locale"/> invocations for local
     * customisation.
     *
     * eg.
     *
     * <pre>
     * &lt;map:act type="locale"&gt;
     *     &lt;map:parameter name="language-attribute" value="lg"/&gt;
     * &lt;/map:act&gt;
     * </pre>
     */
    private void checkParams(Parameters par) {

        langAttr = par.getParameter(LANG_ATTR, langAttr);
        countryAttr = par.getParameter(COUNTRY_ATTR, countryAttr);
        variantAttr = par.getParameter(VARIANT_ATTR, variantAttr);
        localeAttr = par.getParameter(LOCALE_ATTR, localeAttr);

        storeInRequest = par.getParameterAsBoolean(STORE_REQUEST, storeInRequest);
        storeInSession = par.getParameterAsBoolean(STORE_SESSION, storeInSession);
        createSession = par.getParameterAsBoolean(CREATE_SESSION, createSession);
        storeInCookie = par.getParameterAsBoolean(STORE_COOKIE, storeInCookie);

        debug("checking for local overrides, " +
            LANG_ATTR + " = " + langAttr + ", " +
            COUNTRY_ATTR + " = " + countryAttr + ", " +
            VARIANT_ATTR + " = " + variantAttr + ", " +
            LOCALE_ATTR + " = " + localeAttr + ", " +
            STORE_REQUEST + " = " + storeInRequest + ", " +
            STORE_SESSION + " = " + storeInSession + ", " +
            CREATE_SESSION + " = " + createSession + ", " +
            STORE_COOKIE + " = " + storeInCookie
        );
    }

    /**
     * Helper debug method, prefixes all debug messages with the classes name
     *
     * @param msg debugging message
     */
    private void debug(String msg) {
        getLogger().debug(getClass().getName() + ": " + msg);
    }

    // Store the lang in request. Default is not to do this.
    private boolean storeInRequest = false;

    // Store the lang in session, if available. Default is not to do this.
    private boolean storeInSession = false;

    // Should we create a session if needed. Default is not to do this.
    private boolean createSession = false;

    // Should we add a cookie with the lang. Default is not to do this.
    private boolean storeInCookie = false;

    // Configuration attributes.
    private String langAttr;
    private String countryAttr;
    private String variantAttr;
    private String localeAttr;
}
