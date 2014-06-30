package org.apache.cocoon.i18n;

import java.util.Locale;
import java.util.StringTokenizer;

/**
 * A helper class for i18n formatting and parsing routing.
 * Contains static methods only.
 *
 * @author <a href="mailto:kpiroumian@flagship.ru">Konstantin Piroumian</a>
 * @version 1.0
 */
public class I18nUtils {

    // Locale string delimiter
    private static final String LOCALE_DELIMITER   = "_";

    /**
     * Parses given locale string to Locale object. If the string is null
     * then the given locale is returned.
     *
     * @param localeString a string containing locale in
     * <code>language_country_variant</code> format.
     * @param defaultLocale returned if localeString is <code>null</code>
     */
    public static Locale parseLocale(String localeString, Locale defaultLocale) {
        if (localeString != null) {
            StringTokenizer st = new StringTokenizer(localeString,
                                                     LOCALE_DELIMITER);
            String l = st.hasMoreElements() ? st.nextToken()
                                            : defaultLocale.getLanguage();
            String c = st.hasMoreElements() ? st.nextToken() : "";
            String v = st.hasMoreElements() ? st.nextToken() : "";
            return new Locale(l, c, v);
        }

        return defaultLocale;
    }

    /**
     * Parses given locale string to Locale object. If the string is null
     * then the VM default locale is returned.
     *
     * @param localeString a string containing locale in
     * <code>language_country_variant</code> format.
     *
     * @see #parseLocale(String, Locale)
     * @see java.util.Locale#getDefault()
     */
    public static Locale parseLocale(String localeString) {
        return parseLocale(localeString, Locale.getDefault());
    }

    // Hide constructor to prevent class instance creation
    private I18nUtils() {
    }
}
