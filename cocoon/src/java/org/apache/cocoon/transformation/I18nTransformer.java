/**
 ****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 ****************************************************************************
 */
package org.apache.cocoon.transformation;

import java.io.IOException;
import java.text.DateFormat;
import java.text.DecimalFormat;
import java.text.MessageFormat;
import java.text.NumberFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Locale;
import java.util.Map;
import java.util.MissingResourceException;
import java.util.Set;
import java.util.StringTokenizer;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import org.apache.avalon.excalibur.pool.Poolable;
import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.configuration.DefaultConfiguration;
import org.apache.avalon.framework.parameters.Parameters;

import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.ResourceNotFoundException;
import org.apache.cocoon.acting.LocaleAction;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.i18n.I18nUtils;
import org.apache.cocoon.i18n.XMLResourceBundle;
import org.apache.cocoon.i18n.XMLResourceBundleFactory;

/**
 * Internationalization transformer is used to transform i18n markup into text
 * based on a particular locale.
 *
 * <h3>i18n transformer</h3>
 * <p>The <strong>i18n transformer</strong> works by obtaining the users locale
 * based on request, session attributes or a cookie data. See
 * {@link org.apache.cocoon.acting.LocaleAction#getLocale(Map) } for details.
 * It then attempts to find a <strong>message catalogue</strong> that satisifies
 * the particular locale, and use it for for text replacement within i18n markup.
 * <p>Catalogues are maintained in separate files, with a naming convention
 * similar to that of ResourceBundle (See java.util.ResourceBundle).
 * ie.
 * <strong>basename</strong>_<strong>locale</strong>, where <i>basename</i>
 * can be any name, and <i>locale</i> can be any locale specified using
 * ISO 639/3166 characters (eg. en_AU, de_AT, es).<br/>
 * <strong>NOTE: </strong>ISO 639 is not a stable standard; some of the language
 * codes it defines (specifically iw, ji, and in) have changed
 * (see java.util.Locale for details).
 *
 * <h3>Catalogues</h3>
 * <p>Catalogues are of the following format:
 * <pre>
 * &lt;?xml version="1.0"?&gt;
 * &lt;!-- message catalogue file for locale ... --&gt;
 * &lt;catalogue xml:lang=&quot;locale&quot;&gt;
 *        &lt;message key="key"&gt;text&lt;/message&gt;
 *        ....
 * &lt;/catalogue&gt;
 * </pre> Where <strong>key</strong> specifies a particular message for that
 * language.
 *
 * <h3>Usage</h3>
 * <p>Files to be translated contain the following markup:
 * <pre>
 * &lt;?xml version="1.0"?&gt;
 * ... some text, translate &lt;i18n:text&gt;key&lt;/i18n:text&gt;
 * </pre>
 * At runtime, the i18n transformer will find a message catalogue for the
 * user's locale, and will appropriately replace the text between the
 * <code>&lt;i18n:text&gt;</code> markup, using the value between the tags as
 * the lookup key.
 * <p>If the i18n transformer cannot find an appropriate message catalogue for
 * the user's given locale, it will recursively try to locate a <i>parent</i>
 * message catalogue, until a valid catalogue can be found.
 * ie:
 * <ul>
 *  <li><strong>catalogue</strong>_<i>language</i>_<i>country</i>_<i>variant</i>.xml
 *  <li><strong>catalogue</strong>_<i>language</i>_<i>country</i>.xml
 *  <li><strong>catalogue</strong>_<i>language</i>.xml
 *  <li><strong>catalogue</strong>.xml
 * </ul>
 * eg: Assuming a basename of <i>messages</i> and a locale of <i>en_AU</i>
 * (no variant), the following search will occur:
 * <ul>
 *  <li><strong>messages</strong>_<i>en</i>_<i>AU</i>.xml
 *  <li><strong>messages</strong>_<i>en</i>.xml
 *  <li><strong>messages</strong>.xml
 * </ul>
 * This allows the developer to write a hierarchy of message catalogues,
 * at each defining messages with increasing depth of variation.
 *
 * <h3>Sitemap configuration</h3>
 * <pre>
 * &lt;map:transformer name="i18n"
 *     src="org.apache.cocoon.transformation.I18nTransformer"&gt;
 *
 *     &lt;catalogue-name&gt;messages&lt;/catalogue-name&gt;
 *     &lt;catalogue-location&gt;translations&lt;/catalogue-location&gt;
 *     &lt;untranslated-text&gt;untranslated&lt;/untranslated-text&gt;
 *     &lt;cache-at-startup&gt;true&lt;/cache-at-startup&gt;
 * &lt;/map:transformer&gt;
 * </pre> where:
 * <ul>
 *  <li><strong>catalogue-name</strong>: base name of the message
 *      catalogue (<i>mandatory</i>).
 *  <li><strong>catalogue-location</strong>: location of the
 *      message catalogues (<i>mandatory</i>).
 *  <li><strong>untranslated-text</strong>: default text used for
 *      untranslated keys (default is 'untranslated-text').
 *  <li><strong>cache-at-startup</strong>: flag whether to cache
 *      messages at startup (false by default).
 * </ul>
 * <p>To use the transformer in a pipeline, simply specify it in a particular
 * transform. eg:
 * <pre>
 * &lt;map:match pattern="file"&gt;
 *     &lt;map:generate src="file.xml"/&gt;
 *     &lt;map:transform type="i18n"/&gt;
 *     &lt;map:serialize/&gt;
 * &lt;/map:match&gt;
 * </pre>
 *
 * <p>Note, <strong>catalogue-name</strong>, <strong>catalogue-location</strong>
 * and <strong>untranslated-text</strong> can all be overridden at the
 * pipeline level by specifying them as parameters to the transform statement.
 *
 * <p>For date, time and number formatting use the following tags:
 * <ul>
 *  <li><strong>&lt;i18n:date/&gt;</strong> gives localized date.</li>
 *  <li><strong>&lt;i18n:date-time/&gt;</strong> gives localized date and time.</li>
 *  <li><strong>&lt;i18n:time/&gt;</strong> gives localized time.</li>
 * </ul>
 * For <code>date</code>, <code>date-time</code> and <code>time</code> the
 * <code>pattern</code> and <code>src-pattern</code> attribute may have also
 * values of: <code>short</code>, <code>medium</code>, <code>long</code> or
 * <code>full</code>. (See java.text.DateFormat for more info on this).
 * <p>For <code>date</code>, <code>date-time</code>, <code>time</code> and
 * <code>number</code> a different <code>locale</code> and
 * <code>source-locale</code> can be specified:
 * <pre>
 * &lt;i18n:date src-pattern="short" src-locale="en_US" locale="de_DE"&gt;
 *      12/24/01
 * &lt;/i18n:date&gt;
 * </pre> will result in 24.12.2001.
 *
 * <p>A given real <code>pattern</code> and <code>src-pattern</code> (not
 * <code>short, medium, long, full</code>) overwrites the
 * <code>locale</code> and <code>src-locale</code>
 *
 * <p>Future work coming:
 * <ul>
 *  <li>Many clean ups ...done on 23-Jan-2002 ;)
 *  <li>Introduce empty translation (XMLResourceBundle)
 *  <li>Remove 'sub-type' attribute (add possible values to 'type')
 *  <li>Introduce new <get-locale /> element
 *  <li>Introduce new 'currency-no-unit' and 'int-currency-no-unit' types
 *  <li>Fix resources disposal
 *  <li>A little clean ups... ;)
 * </ul>
 *
 * @author <a href="mailto:kpiroumian@flagship.ru">Konstantin Piroumian</a>
 * @author <a href="mailto:Marcus.Crafter@managesoft.com">Marcus Crafter</a>
 * @author <a href="mailto:Michael.Enke@wincor-nixdorf.com">Michael Enke</a>
 * @author <a href="mailto:lassi.immonen@valkeus.com">Lassi Immonen</a>
 *
 * @todo Move all formatting/parsing routines to I18nUtils
 */
public class I18nTransformer extends AbstractTransformer
    implements Composable, Poolable, Configurable, Recyclable, Disposable {

    /**
     * The namespace for i18n is "http://apache.org/cocoon/i18n/2.0".
     */
    public static final String I18N_NAMESPACE_URI =
        "http://apache.org/cocoon/i18n/2.0";

    //
    // i18n elements
    //

    /**
     * i18n:text element is used to translate simple text, e.g.:<br/>
     * <pre>
     *  &lt;i18n:text&gt;This is a multilanguage string&lt;/i18n:text&gt;
     * </pre>
     */
    public static final String I18N_TEXT_ELEMENT            = "text";

    /**
     * i18n:translate element is used to translate text with parameter
     * substitution, e.g.:<br/>
     * <pre>
     * &lt;i18n:translate&gt;
     *     This is a multilanguage string with {0} param
     *     &lt;i18n:param&gt;1&lt;/i18n:param&gt;
     * &lt;/i18n:translate&gt;
     * </pre>
     *
     * @see #I18N_TEXT_ELEMENT
     * @see #I18N_PARAM_ELEMENT
     */
    public static final String I18N_TRANSLATE_ELEMENT       = "translate";

    /**
     * i18n:param is used with i18n:translate to provide substitution params.
     * The param can have i18n:text as its value to provide multilungual value.
     * Parameters can have additional attributes to be used for formatting:
     * <ul>
     *      <li><code>type</code> - can be <code>date, date-time, time or
     *      number</code>. Used to format params before substitution.
     *      </li>
     *      <li><code>sub-type</code> - can be <code>currency, percent</code>
     *      and used with <code>number</code> type to format a given number
     *      value as a currency or percent.
     *      </li>
     *      <li><code>value</code> - the value of the param. If no value is
     *      specified then the text inside of the param element will be used.
     *      </li>
     *      <li><code>locale</code> - used only with <code>number, date, time,
     *      date-time</code> types and used to override the current locale to
     *      format the given value.
     *      </li>
     *      <li><code>src-locale</code> - used with <code>number, date, time,
     *      date-time</code> types and specify the locale that should be used to
     *      parse the given value.
     *      </li>
     *      <li><code>pattern</code> - used with <code>number, date, time,
     *      date-time</code> types and specify the pattern that should be used
     *      to format the given value.
     *      </li>
     *      <li><code>src-pattern</code> - used with <code>number, date, time,
     *      date-time</code> types and specify the pattern that should be used
     *      to parse the given value.
     *      </li>
     * </ul>
     * @see #I18N_TRANSLATE_ELEMENT
     * @see #I18N_DATE_ELEMENT
     * @see #I18N_TIME_ELEMENT
     * @see #I18N_DATE_TIME_ELEMENT
     * @see #I18N_NUMBER_ELEMENT
     */
    public static final String I18N_PARAM_ELEMENT           = "param";

    /**
     * i18n:date is used to provide a localized date string. Allowed attributes
     * are: <code>pattern, src-pattern, locale, src-locale</code>
     * Usage examples:
     * <pre>
     *  &lt;i18n:date src-pattern="short" src-locale="en_US" locale="de_DE"&gt;
     *      12/24/01
     *  &lt;/i18n:date&gt;
     *
     * &lt;i18n:date pattern="dd/MM/yyyy" /&gt;
     * </pre>
     *
     * If no value is specified then the current date will be used. E.g.:
     * <pre>
     * &lt;i18n:date /&gt;
     * </pre> displays the current date formatted with default pattern for
     * the current locale.
     *
     * @see #I18N_PARAM_ELEMENT
     * @see #I18N_DATE_TIME_ELEMENT
     * @see #I18N_TIME_ELEMENT
     * @see #I18N_NUMBER_ELEMENT
     */
    public static final String I18N_DATE_ELEMENT            = "date";

    /**
     * i18n:date-time is used to provide a localized date and time string.
     * Allowed attributes are: <code>pattern, src-pattern, locale,
     * src-locale</code>
     * Usage examples:
     * <pre>
     *  &lt;i18n:date-time src-pattern="short" src-locale="en_US" locale="de_DE"
     *  &gt;
     *      12/24/01 1:00 AM
     *  &lt;/i18n:date&gt;
     *
     *  &lt;i18n:date-time pattern="dd/MM/yyyy hh:mm" /&gt;
     * </pre>
     *
     * If no value is specified then the current date and time will be used.
     * E.g.:
     * <pre>
     * &lt;i18n:date-time /&gt;
     * </pre> displays the current date formatted with default pattern for
     * the current locale.
     *
     * @see #I18N_PARAM_ELEMENT
     * @see #I18N_DATE_ELEMENT
     * @see #I18N_TIME_ELEMENT
     * @see #I18N_NUMBER_ELEMENT
     */
    public static final String I18N_DATE_TIME_ELEMENT       = "date-time";

    /**
     * i18n:time is used to provide a localized time string. Allowed attributes
     * are: <code>pattern, src-pattern, locale, src-locale</code>
     * Usage examples:
     * <pre>
     *  &lt;i18n:time src-pattern="short" src-locale="en_US" locale="de_DE"&gt;
     *      1:00 AM
     *  &lt;/i18n:time&gt;
     *
     * &lt;i18n:time pattern="hh:mm:ss" /&gt;
     * </pre>
     *
     * If no value is specified then the current time will be used. E.g.:
     * <pre>
     * &lt;i18n:time /&gt;
     * </pre> displays the current time formatted with default pattern for
     * the current locale.
     *
     * @see #I18N_PARAM_ELEMENT
     * @see #I18N_DATE_TIME_ELEMENT
     * @see #I18N_DATE_ELEMENT
     * @see #I18N_NUMBER_ELEMENT
     */
    public static final String I18N_TIME_ELEMENT            = "time";

    /**
     * i18n:number is used to provide a localized number string. Allowed
     * attributes are: <code>pattern, src-pattern, locale, src-locale, sub-type
     * </code>
     * Usage examples:
     * <pre>
     *  &lt;i18n:number src-pattern="short" src-locale="en_US" locale="de_DE"&gt;
     *      1000.0
     *  &lt;/i18n:number&gt;
     *
     * &lt;i18n:number sub-type="currency" /&gt;
     * </pre>
     *
     * If no value is specifies then 0 will be used.
     *
     * @see #I18N_PARAM_ELEMENT
     * @see #I18N_DATE_TIME_ELEMENT
     * @see #I18N_TIME_ELEMENT
     * @see #I18N_DATE_ELEMENT
     */
    public static final String I18N_NUMBER_ELEMENT      = "number";

    /** @todo Implement currency element */
    public static final String I18N_CURRENCY_ELEMENT    = "currency";

    /** @todo Implement percent element */
    public static final String I18N_PERCENT_ELEMENT     = "percent";

    /** @todo Implemement integer currency element */
    public static final String I18N_INT_CURRENCY_ELEMENT
        = "int-currency";

    /** @todo Implement currency without unit element */
    public static final String I18N_CURRENCY_NO_UNIT_ELEMENT =
        "currency-no-unit";

    /** @todo Implement integer currency without unit element */
    public static final String I18N_INT_CURRENCY_NO_UNIT_ELEMENT =
        "int-currency-no-unit";

    // i18n general attributes

    /**
     * This attribute is used with i18n:text element to indicate the key of
     * the according message. The character data of the element will be used
     * if no message is found by this key. E.g.:
     * <pre>
     * &lt;i18n:text i18n:key="a_key"&gt;article_text1&lt;/i18n:text&gt;
     * </pre>
     */
    public static final String I18N_KEY_ATTRIBUTE           = "key";

    /**
     * This attribute is used with <strong>any</strong> element (even not i18n)
     * to translate attribute values. Should contain whitespace separated
     * attribute names that should be translated. E.g.
     * <pre>
     * &lt;para title="first" name="article" i18n:attr="title name" /&gt;
     * </pre>
     */
    public static final String I18N_ATTR_ATTRIBUTE          = "attr";

    // i18n number and date formatting attributes

    /**
     * This attribute is used with date and number formatting elements to
     * indicate the pattern that should be used to parse the element value.
     *
     * @see #I18N_PARAM_ELEMENT
     * @see #I18N_DATE_TIME_ELEMENT
     * @see #I18N_DATE_ELEMENT
     * @see #I18N_TIME_ELEMENT
     * @see #I18N_NUMBER_ELEMENT
     */
    public static final String I18N_SRC_PATTERN_ATTRIBUTE   = "src-pattern";

    /**
     * This attribute is used with date and number formatting elements to
     * indicate the pattern that should be used to format the element value.
     *
     * @see #I18N_PARAM_ELEMENT
     * @see #I18N_DATE_TIME_ELEMENT
     * @see #I18N_DATE_ELEMENT
     * @see #I18N_TIME_ELEMENT
     * @see #I18N_NUMBER_ELEMENT
     */
    public static final String I18N_PATTERN_ATTRIBUTE       = "pattern";

    /**
     * This attribute is used with date and number formatting elements to
     * indicate the locale that should be used to format the element value.
     *
     * @see #I18N_PARAM_ELEMENT
     * @see #I18N_DATE_TIME_ELEMENT
     * @see #I18N_DATE_ELEMENT
     * @see #I18N_TIME_ELEMENT
     * @see #I18N_NUMBER_ELEMENT
     */
    public static final String I18N_LOCALE_ATTRIBUTE        = "locale";

    /**
     * This attribute is used with date and number formatting elements to
     * indicate the locale that should be used to parse the element value.
     *
     * @see #I18N_PARAM_ELEMENT
     * @see #I18N_DATE_TIME_ELEMENT
     * @see #I18N_DATE_ELEMENT
     * @see #I18N_TIME_ELEMENT
     * @see #I18N_NUMBER_ELEMENT
     */
    public static final String I18N_SRC_LOCALE_ATTRIBUTE    = "src-locale";

    /**
     * This attribute is used with date and number formatting elements to
     * indicate the value that should be parsed and formatted. If value
     * attribute is not used then the character data of the element will be used.
     *
     * @see #I18N_PARAM_ELEMENT
     * @see #I18N_DATE_TIME_ELEMENT
     * @see #I18N_DATE_ELEMENT
     * @see #I18N_TIME_ELEMENT
     * @see #I18N_NUMBER_ELEMENT
     */
    public static final String I18N_VALUE_ATTRIBUTE         = "value";

    /**
     * This attribute is used with <code>i18:param</code> to
     * indicate the parameter type: <code>date, time, date-time</code> or
     * <code>number, currency, percent, int-currency, currency-no-unit,
     * int-currency-no-unit</code>.
     */
    public static final String I18N_TYPE_ATTRIBUTE          = "type";

    /**
     * This attribute is used with <code>i18:number</code> to
     * indicate a sub-type: <code>currency</code>, <code>int-currency</code>
     * or <code>percent</code>.
     */
    public static final String I18N_SUB_TYPE_ATTRIBUTE      = "sub-type";

    // Ñonfiguration parameters

    /**
     * This configuration parameter specifies the message catalog name.
     */
    public static final String I18N_CATALOGUE_NAME      = "catalogue-name";

    /**
     * This configuration parameter specifies the message catalog location
     * relative to the current sitemap.
     */
    public static final String I18N_CATALOGUE_LOCATION  = "catalogue-location";

    /**
     * This configuration parameter specifies the message that should be
     * displayed in case of a not translated text (message not found).
     */
    public static final String I18N_UNTRANSLATED        = "untranslated-text";

    /**
     * This configuration parameter specifies if the message catalog should be
     * cached at startup.
     */
    public static final String I18N_CACHE_STARTUP       = "cache-at-startup";

    /**
     * This constant specifies the XPath prefix that will be used
     * to retrieve a value from a message catalogue. The resulting XPath is
     * looks like this:
     * <code>/catalogue/message[@key='key_value']</code>
     *<br/>
     * FIXME (KP): We need a more generic way of key interpretation: to use
     * XPath expression as keys, or keys with non-XML dictionaries.
     */
    public static final String I18N_CATALOGUE_PREFIX    = "/catalogue/message";

    // Used to check that the catalogue location is a directory.
    // FIXME (KP): Why should it be a file? It can be any resource!
    private static final String FILE                        = "file:";

    // States of the transformer
    private static final int STATE_OUTSIDE                  = 0;
    private static final int STATE_INSIDE_TEXT              = 1;
    private static final int STATE_INSIDE_PARAM             = 2;
    private static final int STATE_INSIDE_TRANSLATE         = 3;
    private static final int STATE_INSIDE_TRANSLATE_TEXT    = 4;
    private static final int STATE_TRANSLATE_KEY            = 5;
    private static final int STATE_TRANSLATE_TEXT_KEY       = 6;
    private static final int STATE_INSIDE_DATE              = 7;
    private static final int STATE_INSIDE_DATE_TIME         = 8;
    private static final int STATE_INSIDE_TIME              = 9;
    private static final int STATE_INSIDE_NUMBER            = 10;

    // All date-time related parameter types and element names
    private static final Set dateTypes;

    // All number related parameter types and element names
    private static final Set numberTypes;

    // Date pattern types map: short, medium, long, full
    private static final Map datePatterns;

    static {
        // initialize date types set
        HashSet set = new HashSet();
        set.add(I18N_DATE_ELEMENT);
        set.add(I18N_TIME_ELEMENT);
        set.add(I18N_DATE_TIME_ELEMENT);
        dateTypes = Collections.unmodifiableSet(set);

        // initialize number types set
        set = new HashSet();
        set.add(I18N_NUMBER_ELEMENT);
        set.add(I18N_PERCENT_ELEMENT);
        set.add(I18N_CURRENCY_ELEMENT);
        set.add(I18N_INT_CURRENCY_ELEMENT);
        set.add(I18N_CURRENCY_NO_UNIT_ELEMENT);
        set.add(I18N_INT_CURRENCY_NO_UNIT_ELEMENT);
        numberTypes = Collections.unmodifiableSet(set);

        // Initialize date patterns map
        Map map = new HashMap();
        map.put("SHORT", new Integer(DateFormat.SHORT));
        map.put("MEDIUM", new Integer(DateFormat.MEDIUM));
        map.put("LONG", new Integer(DateFormat.LONG));
        map.put("FULL", new Integer(DateFormat.FULL));
        datePatterns = Collections.unmodifiableMap(map);
    }

    // Component manager for this component
    private ComponentManager manager;

    // Current state of the transformer. The value is STATE_OUTSIDE by default.
    private int current_state;

    // Previous state. Used in text translation inside params and translate
    // elements.
    private int prev_state;

    // The i18n:key attribute is stored for the current element.
    // If no translation found for the key then the character data of element is
    // used as default value.
    private String current_key;

    // Translated text inside the i18n:text element.
    private String translated_text;

    // Translated text, ready for param substitution.
    private String substitute_text;

    // Current parameter value (translated or not)
    private String param_value;

    // i18n:params are stored for index substitutions.
    private ArrayList indexedParams;

    // Message formatter for param substitution.
    private MessageFormat formatter;

    // Current locale
    private Locale locale;

    // Date and number elements and params formatting attributes with values.
    private HashMap formattingParams;

    // Dictionary data
    private XMLResourceBundle dictionary;

    // FIXME: Shouldn't factory be a global component?
    // Now every I18nTransformer have own instance of factory
    // every of which in turn have own file cache.
    // FIXME (KP): Can we use static factory variable?
    private XMLResourceBundleFactory factory = new XMLResourceBundleFactory();

    //
    // i18n configuration variables
    //

    // Catalogue name value
    private String catalogueName;

    // Catalogue location value
    private String catalogueLocation;

    // Untranslated message value
    private String untranslated;

    // Cache at startup setting value
    private boolean cacheAtStartup;

    // Helper variable used to hold the old untranslated value
    private String globalUntranslated;

    /**
     * Returns the current locale setting of this transformer instance.
     * @return current Locale object
     */
    public Locale getLocale() {
        return this.locale;
    }

    /**
     * Configure this transformer.
     */
    public void configure(Configuration conf) throws ConfigurationException {
        if (conf != null) {
            // read in the config options from the transformer definition

            // obtain the base name of the message catalogue
            Configuration child = conf.getChild(I18N_CATALOGUE_NAME);
            catalogueName = child.getValue(null);
            debug("Default catalogue name is " + catalogueName);

            // obtain the directory location of message catalogues
            child = conf.getChild(I18N_CATALOGUE_LOCATION);
            catalogueLocation = child.getValue(null);
            debug("Default catalogue location is " + catalogueLocation);

            // check our mandatory parameters
            if (catalogueName == null || catalogueLocation == null)
                throw new ConfigurationException(
                    "I18nTransformer requires the name and location of " +
                    "the message catalogues"
                );

            // obtain default text to use for untranslated messages
            child = conf.getChild(I18N_UNTRANSLATED);
            untranslated = child.getValue(I18N_UNTRANSLATED);
            debug("Default untranslated text is '" + untranslated + "'");

            // obtain config option, whether to cache messages at startup time
            child = conf.getChild(I18N_CACHE_STARTUP);
            cacheAtStartup = child.getValueAsBoolean(false);
            debug((cacheAtStartup ? "will" : "won't") +
                   " cache messages during startup, by default"
            );

            // activate resource bundle logging
            factory.setLogger(getLogger());
        }
    }

    /**
     *  Uses <code>org.apache.cocoon.acting.LocaleAction.getLocale()</code>
     *  to get language user has selected.
     *  FIXME (KP): Why should I18nTransformer depend on an action?
     */
    public void setup(SourceResolver resolver, Map objectModel, String source,
                      Parameters parameters)
        throws ProcessingException, SAXException, IOException {

        try {
            // check parameters to see if anything has been locally overloaded
            String localCatLocation = null;
            String localCatName = null;
            String localUntranslated = null;

            if (parameters != null) {
                localCatLocation =
                    parameters.getParameter(I18N_CATALOGUE_LOCATION, null);
                localCatName =
                    parameters.getParameter(I18N_CATALOGUE_NAME, null);
                localUntranslated =
                    parameters.getParameter(I18N_UNTRANSLATED, null);
            }

            // if untranslated-text has been overridden, save the original
            // value so it can be restored when this object is recycled.
            // FIXME (KP): What about other overriden params?
            if (localUntranslated != null) {
                globalUntranslated = untranslated;
                untranslated = localUntranslated;
            }

            // configure the factory
            _setup(resolver, localCatLocation == null ? catalogueLocation
                                                      : localCatLocation);

            // Get current locale
            String lc = LocaleAction.getLocaleAttribute(objectModel);
            Locale locale = I18nUtils.parseLocale(lc);
            debug("using locale " + locale.toString());

            // setup everything for the current locale
            dictionary = (XMLResourceBundle) factory.select(
                localCatName == null ? catalogueName : localCatName,
                locale
            );
            debug( "selected dictionary " + dictionary );

            // Initialize instance state variables

            this.locale             = locale;
            this.current_state      = STATE_OUTSIDE;
            this.prev_state         = STATE_OUTSIDE;
            this.current_key        = null;
            this.translated_text    = null;
            this.substitute_text    = null;
            this.param_value        = null;
            this.indexedParams      = new ArrayList();
            this.formattingParams   = null;

            // Create and initialize a formatter
            this.formatter = new MessageFormat("");
            this.formatter.setLocale(locale);

        } catch (Exception e) {
            debug("exception generated, leaving unconfigured");
            throw new ProcessingException(e.getMessage(), e);
        }
    }

    /**
     * Internal setup of XML resource bundle and factory.
     *
     * REVISIT: when we can get the resolver anywhere, we can pass the
     * configuration object directly to XMLResourceBundle.
     */
    private void _setup(SourceResolver resolver, String location)
        throws Exception {

        // configure the factory to log correctly and cache catalogues
        DefaultConfiguration configuration =
            new DefaultConfiguration("name", "location");
        DefaultConfiguration cacheConf =
            new DefaultConfiguration(
                XMLResourceBundleFactory.ConfigurationKeys.CACHE_AT_STARTUP,
                "location"
            );
        cacheConf.setValue(new Boolean(cacheAtStartup).toString());
        configuration.addChild(cacheConf);

        // set the root location for message catalogues
        DefaultConfiguration dirConf =
            new DefaultConfiguration(
                XMLResourceBundleFactory.ConfigurationKeys.ROOT_DIRECTORY,
                "location"
            );

        debug("catalog location:" + location);
        Source source = resolver.resolve(location);
        try {
            String systemId = source.getSystemId();
            if (!systemId.startsWith(FILE)) {
                throw new ResourceNotFoundException(
                    systemId + " does not denote a directory"
                );
            }
            debug("catalog directory:" + systemId);
            dirConf.setValue(systemId);
            configuration.addChild(dirConf);
        } finally {
            source.recycle();
        }

        // Pass created configuration object to the factory
        factory.configure(configuration);
        debug("configured");
    }

    public void compose(ComponentManager manager) {
        this.manager = manager;
        factory.compose(manager);
    }

    //
    // Standart SAX event handlers
    //

    public void startElement(String uri, String name, String raw,
                             Attributes attr) throws SAXException {

        if (I18N_NAMESPACE_URI.equals(uri)) {
            debug("Starting i18n element: " + name);
            startI18NElement(name, attr);
            return;
        }

        super.startElement(uri, name, raw, translateAttributes(name, attr));
    }


    public void endElement(String uri, String name, String raw)
        throws SAXException {

        if (I18N_NAMESPACE_URI.equals(uri)) {
            endI18NElement(name);
            return;
        }

        super.endElement(uri, name, raw);
    }

    public void characters(char[] ch, int start, int len) throws SAXException {

        if (current_state != STATE_OUTSIDE) {
            i18nCharacters(ch, start, len);
            return;
        }

        super.characters(ch, start, len);

    }

    //
    // i18n specific event handlers
    //

    private void startI18NElement(String name, Attributes attr)
        throws SAXException {

        debug("Start i18n element: " + name);

        if (I18N_TEXT_ELEMENT.equals(name)) {
            if (current_state != STATE_OUTSIDE
                && current_state != STATE_INSIDE_PARAM
                && current_state != STATE_INSIDE_TRANSLATE) {

                throw new SAXException(
                    this.getClass().getName()
                    + ": nested i18n:text elements are not allowed."
                    + " Current state: " + current_state
                );
            }

            prev_state = current_state;
            current_state = STATE_INSIDE_TEXT;
            current_key = attr.getValue(I18N_NAMESPACE_URI, I18N_KEY_ATTRIBUTE);
        } else if (I18N_TRANSLATE_ELEMENT.equals(name)) {
            if (current_state != STATE_OUTSIDE) {
                throw new SAXException(
                    this.getClass().getName()
                    + ": i18n:translate element must be used "
                    + "outside of other i18n elements. Current state: "
                    + current_state
                );
            }

            current_state = STATE_INSIDE_TRANSLATE;
        } else if (I18N_PARAM_ELEMENT.equals(name)) {
            if (current_state != STATE_INSIDE_TRANSLATE) {
                throw new SAXException(
                    this.getClass().getName()
                    + ": i18n:param element can be used only inside "
                    + "i18n:translate element. Current state: "
                    + current_state
                );
            }

            setFormattingParams(attr);
            current_state = STATE_INSIDE_PARAM;
        } else if (I18N_DATE_ELEMENT.equals(name)) {
            if (current_state != STATE_OUTSIDE) {
                throw new SAXException(
                    this.getClass().getName()
                    + ": i18n:date elements are not allowed "
                    + "inside of other i18n elements."
                );
            }

            setFormattingParams(attr);
            current_state = STATE_INSIDE_DATE;
        } else if (I18N_DATE_TIME_ELEMENT.equals(name)) {
            if (current_state != STATE_OUTSIDE) {
                throw new SAXException(
                    this.getClass().getName()
                    + ": i18n:date-time elements are not allowed "
                    + "inside of other i18n elements."
                );
            }

            setFormattingParams(attr);
            current_state = STATE_INSIDE_DATE_TIME;
        } else if (I18N_TIME_ELEMENT.equals(name)) {
            if (current_state != STATE_OUTSIDE) {
                throw new SAXException(
                    this.getClass().getName()
                    + ": i18n:date elements are not allowed "
                    + "inside of other i18n elements."
                );
            }

            setFormattingParams(attr);
            current_state = STATE_INSIDE_TIME;
        } else if (I18N_NUMBER_ELEMENT.equals(name)) {
            if (current_state != STATE_OUTSIDE) {
                throw new SAXException(
                    this.getClass().getName()
                    + ": i18n:number elements are not allowed "
                    + "inside of other i18n elements."
                );
            }

            setFormattingParams(attr);
            current_state = STATE_INSIDE_NUMBER;
        }
    }

    // Get all possible i18n formatting attribute values and store in a Map
    private void setFormattingParams(Attributes attr) throws SAXException {
        formattingParams = new HashMap(3);  // average number of attributes is 3

        String attr_value = attr.getValue(I18N_SRC_PATTERN_ATTRIBUTE);
        if (attr_value != null) {
            formattingParams.put(I18N_SRC_PATTERN_ATTRIBUTE, attr_value);
        }

        attr_value = attr.getValue(I18N_PATTERN_ATTRIBUTE);
        if (attr_value != null) {
            formattingParams.put(I18N_PATTERN_ATTRIBUTE, attr_value);
        }

        attr_value = attr.getValue(I18N_VALUE_ATTRIBUTE);
        if (attr_value != null) {
            formattingParams.put(I18N_VALUE_ATTRIBUTE, attr_value);
        }

        attr_value = attr.getValue(I18N_LOCALE_ATTRIBUTE);
        if (attr_value != null) {
            formattingParams.put(I18N_LOCALE_ATTRIBUTE, attr_value);
        }

        attr_value = attr.getValue(I18N_SRC_LOCALE_ATTRIBUTE);
        if (attr_value != null) {
            formattingParams.put(I18N_SRC_LOCALE_ATTRIBUTE, attr_value);
        }

        attr_value = attr.getValue(I18N_TYPE_ATTRIBUTE);
        if (attr_value != null) {
            formattingParams.put(I18N_TYPE_ATTRIBUTE, attr_value);
        }

        attr_value = attr.getValue(I18N_SUB_TYPE_ATTRIBUTE);
        if (attr_value != null) {
            formattingParams.put(I18N_SUB_TYPE_ATTRIBUTE, attr_value);
        }

    }

    private void endI18NElement(String name) throws SAXException {
        debug("End i18n element: " + name);
        switch (current_state) {
        case STATE_INSIDE_TEXT:
            endTextElement();
            break;

        case STATE_INSIDE_TRANSLATE:
            endTranslateElement();
            break;

        case STATE_INSIDE_PARAM:
            endParamElement();
            break;

        case STATE_INSIDE_DATE:
        case STATE_INSIDE_DATE_TIME:
        case STATE_INSIDE_TIME:
            endDate_TimeElement();
            break;

        case STATE_INSIDE_NUMBER:
            endNumberElement();
            break;
        }
    }

    private void i18nCharacters(char[] ch, int start, int len)
        throws SAXException {

        String textValue = new String(ch, start, len).trim();
        if (textValue == null || textValue.length() == 0) {
            return;
        }

        debug( "i18n message text = '" + textValue + "'" );

        switch (current_state) {
        case STATE_INSIDE_TEXT:
            if (current_key != null) { // if i18n:key attribute were used
                translated_text = getString(current_key, untranslated);

                // If no translation found and untranslated param is null
                if (translated_text == null) {
                    translated_text = textValue;  // use the key
                }

                // reset the key holding variable
                current_key = null;
            } else {    // use text value as dictionary key
                translated_text = getString(textValue, untranslated);
            }
            break;

        case STATE_INSIDE_TRANSLATE:
            // Store text for param substitution (do not translate)
            if (substitute_text == null) {
                substitute_text = textValue;
            }
            break;

        case STATE_INSIDE_PARAM:
            // Store translation for param substitution
            if (param_value == null) {
                param_value = textValue;
            }
            break;

        case STATE_INSIDE_DATE:
        case STATE_INSIDE_DATE_TIME:
        case STATE_INSIDE_TIME:
        case STATE_INSIDE_NUMBER:
            if (formattingParams.get(I18N_VALUE_ATTRIBUTE) == null) {
                formattingParams.put(I18N_VALUE_ATTRIBUTE, textValue);
            } else {
                ; // ignore the text inside of date element
            }
            break;

        default:
            throw new IllegalStateException(
                this.getClass().getName()
                + " developer's fault");
        }
    }

    // Translate all attributes that are listed in i18n:attr attribute
    private Attributes translateAttributes(String name, Attributes attr)
        throws SAXException {

        if (attr == null) {
            return attr;
        }

        AttributesImpl temp_attr = new AttributesImpl(attr);

        // Translate all attributes from i18n:attr="name1 name2 ..."
        // using their values as keys

        int i18n_attr_index =
            temp_attr.getIndex(I18N_NAMESPACE_URI,I18N_ATTR_ATTRIBUTE);

        if (i18n_attr_index != -1) {
            StringTokenizer st =
                new StringTokenizer(temp_attr.getValue(i18n_attr_index));

            // remove the i18n:attr attribute - we don't need it
            temp_attr.removeAttribute(i18n_attr_index);

            // iterate through listed attributes and translate them
            while (st.hasMoreElements()) {
                String attr_name = st.nextToken();
                int attr_index = temp_attr.getIndex(attr_name);
                if (attr_index != -1) {
                    String text2translate = temp_attr.getValue(attr_index);
                    String result = getString(text2translate, untranslated);

                    // set the translated value
                    if (result != null) {
                        temp_attr.setValue(attr_index, result);
                    } else {
                        getLogger().warn("translation not found for attribute "
                                         + attr_name + " in element: " + name);
                    }
                } else {
                    getLogger().warn("i18n attribute '" + attr_name
                                      + "' not found in element: " + name);
                }
            }

            return temp_attr;
        }

        // nothing to translate, just return
        return attr;
    }

    private void endTextElement() throws SAXException {
        debug("End text element, translated_text: " + translated_text);
        switch (prev_state) {
        case STATE_OUTSIDE:
            // simply translate text (key translation already performed)
            if (translated_text != null) {
                super.contentHandler.characters(translated_text.toCharArray(),
                                                0, translated_text.length());
            } else {
                // else - translation not found
                debug( "--- Translation not found! ---" );
            }
            break;

        case STATE_INSIDE_TRANSLATE:
            substitute_text = translated_text;
            break;

        case STATE_INSIDE_PARAM:
            param_value = translated_text;
            break;
        }

        translated_text = null;
        current_state = prev_state;
        prev_state = STATE_OUTSIDE;
    }

    // Process substitution parameter
    private void endParamElement() throws SAXException {
        debug("Substitution param: " + param_value);

        String paramType = (String)formattingParams.get(I18N_TYPE_ATTRIBUTE);
        if (paramType != null) {
            // We have a typed parameter

            debug("Param type: " + paramType);
            if (formattingParams.get(I18N_VALUE_ATTRIBUTE) == null &&
                param_value != null) {

                debug("Put param value: " + param_value);
                formattingParams.put(I18N_VALUE_ATTRIBUTE, param_value);
            }

            // Check if we have a date or a number parameter
            if (dateTypes.contains(paramType)) {
                debug("Formatting date_time param: " + formattingParams);
                param_value = formatDate_Time(formattingParams);
            } else if (numberTypes.contains(paramType)) {
                debug("Formatting number param: " + formattingParams);
                param_value = formatNumber(formattingParams);
            }
        }

        debug("Added substitution param: " + param_value);
        indexedParams.add(param_value);
        param_value = null;
        current_state = STATE_INSIDE_TRANSLATE;
    }

    private void endTranslateElement() throws SAXException {
        if (substitute_text == null) {
            return;
        }

        String result;
        if (indexedParams.size() > 0 && substitute_text.length() > 0) {
            debug("Text for susbtitution: " + substitute_text);
            result = formatter.format(substitute_text, indexedParams.toArray());
            debug("Result of susbtitution: " + result);
        } else {
            result = substitute_text;
        }

        super.contentHandler.characters(result.toCharArray(), 0, result.length());
        indexedParams.clear();
        substitute_text = null;
        current_state = STATE_OUTSIDE;
    }

    private void endDate_TimeElement() throws SAXException {
        String result = formatDate_Time(formattingParams);
        super.contentHandler.characters(result.toCharArray(), 0, result.length());
        current_state = STATE_OUTSIDE;
    }

    // Helper method: creates Locale object from a string value in a map
    private Locale getLocale(Map params, String attribute) {
        // the specific locale value
        String lc = (String)params.get(attribute);
        return I18nUtils.parseLocale(lc, this.locale);
    }

    private String formatDate_Time(Map params) throws SAXException {
        // Check that we have not null params
        if (params == null) {
            throw new IllegalArgumentException("Nothing to format");
        }

        // Formatters
        SimpleDateFormat to_fmt = null;
        SimpleDateFormat from_fmt = null;

        // Date formatting styles
        int srcStyle = DateFormat.DEFAULT;
        int style = DateFormat.DEFAULT;

        // Date formatting patterns
        boolean realPattern = false;
        boolean realSrcPattern = false;

        // From locale
        Locale srcLoc = getLocale(params, I18N_SRC_LOCALE_ATTRIBUTE);
        // To locale
        Locale loc = getLocale(params, I18N_LOCALE_ATTRIBUTE);

        // From pattern
        String srcPattern = (String)params.get(I18N_SRC_PATTERN_ATTRIBUTE);
        // To pattern
        String pattern = (String)params.get(I18N_PATTERN_ATTRIBUTE);
        // The date value
        String value = (String)params.get(I18N_VALUE_ATTRIBUTE);

        // A src-pattern attribute is present
        if (srcPattern != null) {
            // Check if we have a real pattern
            Integer patternValue = (Integer)datePatterns.get(srcPattern.toUpperCase());
            if (patternValue != null) {
                srcStyle = patternValue.intValue();
            } else {
                realSrcPattern = true;
            }
        }

        // A pattern attribute is present
        if (pattern != null) {
            Integer patternValue = (Integer)datePatterns.get(pattern.toUpperCase());
            if (patternValue != null) {
                style = patternValue.intValue();
            } else {
                realPattern = true;
            }
        }

        // If we are inside of a typed param
        String paramType = (String)formattingParams.get(I18N_TYPE_ATTRIBUTE);

        // Initializing date formatters
        if (current_state == STATE_INSIDE_DATE ||
            I18N_DATE_ELEMENT.equals(paramType)) {

            to_fmt = (SimpleDateFormat)DateFormat.getDateInstance(style, loc);
            from_fmt = (SimpleDateFormat)DateFormat.getDateInstance(
                srcStyle,
                srcLoc
            );
        } else if (current_state == STATE_INSIDE_DATE_TIME ||
                   I18N_DATE_TIME_ELEMENT.equals(paramType)) {
            to_fmt = (SimpleDateFormat)DateFormat.getDateTimeInstance(
                style,
                style,
                loc
            );
            from_fmt = (SimpleDateFormat)DateFormat.getDateTimeInstance(
                srcStyle,
                srcStyle,
                srcLoc
            );
        } else {
            // STATE_INSIDE_TIME or param type='time'
            to_fmt = (SimpleDateFormat)DateFormat.getTimeInstance(style, loc);
            from_fmt = (SimpleDateFormat)DateFormat.getTimeInstance(
                srcStyle,
                srcLoc
            );
        }

        // parsed date object
        Date dateValue = null;

        // pattern overwrites locale format
        if (realSrcPattern) {
            from_fmt.applyPattern(srcPattern);
        }

        if (realPattern) {
            to_fmt.applyPattern(pattern);
        }

        // get current date and time by default
        if (value == null) {
            dateValue = new Date();
        } else {
            try {
                dateValue = from_fmt.parse(value);
            } catch (ParseException pe) {
                throw new SAXException(
                    this.getClass().getName()
                    + "i18n:date - parsing error.", pe
                );
            }
        }

        // we have all necessary data here: do formatting.
        debug("### Formatting date: " + dateValue + " with localized pattern "
            + to_fmt.toLocalizedPattern() + " for locale: " + locale);
        return to_fmt.format(dateValue);
    }

    private void endNumberElement() throws SAXException {
        String result = formatNumber(formattingParams);
        super.contentHandler.characters(result.toCharArray(), 0, result.length());
        current_state = STATE_OUTSIDE;
    }

    private String formatNumber(Map params) throws SAXException {
        if (params == null) {
            throw new SAXException(
                this.getClass().getName()
                + ": i18n:number - error in element attributes."
            );
        }

        // from pattern
        String srcPattern = (String)params.get(I18N_SRC_PATTERN_ATTRIBUTE);
        // to pattern
        String pattern = (String)params.get(I18N_PATTERN_ATTRIBUTE);
        // the number value
        String value = (String)params.get(I18N_VALUE_ATTRIBUTE);
        // sub-type
        String subType = (String)params.get(I18N_SUB_TYPE_ATTRIBUTE);

        // parsed number
        Number numberValue = null;

        // locale, may be switched locale
        Locale loc = getLocale(params, I18N_LOCALE_ATTRIBUTE);
        Locale srcLoc = getLocale(params, I18N_SRC_LOCALE_ATTRIBUTE);

        // src format
        DecimalFormat from_fmt = (DecimalFormat)NumberFormat.getInstance(srcLoc);
        int int_currency = 0;

        // src-pattern overwrites locale format
        if (srcPattern != null) {
            from_fmt.applyPattern(srcPattern);
        }

        // to format
        DecimalFormat to_fmt = null;
        char dec = from_fmt.getDecimalFormatSymbols().getDecimalSeparator();
        int decAt = 0;
        boolean appendDec = false;
        if (subType == null) {
            to_fmt = (DecimalFormat)NumberFormat.getInstance(loc);
            to_fmt.setMaximumFractionDigits(309);
            for (int i = value.length() - 1;
                 i >= 0 && value.charAt(i) != dec; i--, decAt++) {
                ;
            }

            if (decAt < value.length())to_fmt.setMinimumFractionDigits(decAt);
            decAt = 0;
            for (int i = 0; i < value.length() && value.charAt(i) != dec; i++) {
                if (Character.isDigit(value.charAt(i))) {
                    decAt++;
                }
            }

            to_fmt.setMinimumIntegerDigits(decAt);
            if (value.charAt(value.length() - 1) == dec) {
                appendDec = true;
            }
        } else if (subType.equals("currency")) {
            to_fmt = (DecimalFormat)NumberFormat.getCurrencyInstance(loc);
        } else if (subType.equals("int-currency")) {
            to_fmt = (DecimalFormat)NumberFormat.getCurrencyInstance(loc);
            int_currency = 1;
            for (int i = 0; i < to_fmt.getMaximumFractionDigits(); i++) {
                int_currency *= 10;
            }
        } else if (subType.equals("percent")) {
            to_fmt = (DecimalFormat)NumberFormat.getPercentInstance(loc);
        }

        // pattern overwrites locale format
        if (pattern != null) {
            to_fmt.applyPattern(pattern);
        }

        if (value == null) {
            numberValue = new Long(0);
        } else {
            try {
                numberValue = from_fmt.parse(value);
                if (int_currency > 0) {
                    numberValue = new Double(numberValue.doubleValue() /
                                             int_currency);
                } else {
                    // what?
                }
            } catch (ParseException pe) {
                throw new SAXException(
                    this.getClass().getName()
                    + "i18n:number - parsing error.", pe
                );
            }
        }

        // we have all necessary data here: do formatting.
        String result = to_fmt.format(numberValue);
        if (appendDec) result = result + dec;
        debug("i18n:number result: " + result);
        return result;
    }

    // Helper method to retrieve a message from the dictionary
    // Returnes null if no message is found
    private String getString(String key) {
        return getString(key, null);
    }

    // Helper method to retrieve a message from the dictionary.
    // A default value is returned if message is not found
    private String getString(String key, String defaultValue) {
        try {
            String value = dictionary.getString(
                I18N_CATALOGUE_PREFIX + "[@key='" + key + "']"
            );

            return value != null ? value : defaultValue;
        } catch (MissingResourceException e)  {
            return defaultValue;
        }
    }

    private void setLocale(Locale locale) {
        this.locale = locale;
    }

    // Helper method to debug messages
    private void debug(String msg) {
        getLogger().debug("I18nTransformer: " + msg);
    }

    public void recycle() {
        // restore untranslated-text if necessary
        if (globalUntranslated != null &&
             !untranslated.equals(globalUntranslated)) {

            untranslated = globalUntranslated;
        }

        factory.release(dictionary);
        dictionary = null;
    }

    public void dispose() {
        factory.dispose();
        factory = null;
    }

/*
    //
    static public void main(String[] args) {

        Locale locale = null;
        Locale[] locales = Locale.getAvailableLocales();

        for (int i = 0; i < locales.length; i++) {
            locale = locales[i];n
            SimpleDateFormat fmt =
                (SimpleDateFormat)DateFormat.getDateTimeInstance(
                    DateFormat.DEFAULT,
                    DateFormat.DEFAULT,
                    locale
                );

        String localized = fmt.format(new Date());
        NumberFormat n_fmt = NumberFormat.getCurrencyInstance(locale);
        String money = n_fmt.format(1210.5);

        System.out.println(
            "Locale ["
             + locale.getLanguage() + ", "
             + locale.getCountry() + ", "
             + locale.getVariant() + "] : "
             + locale.getDisplayName()
             + " \t Date: " + localized
             + " \t Money: " + money);
        }
    }
*/
}
