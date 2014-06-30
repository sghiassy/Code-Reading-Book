/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/
// Locales.h

#ifndef _LOCALES_H__
#define _LOCALES_H__


namespace VCF {

/**
*These defines are based on the ISO 3166 country codes
*/
#define LOCALE_USA				"USA"
#define LOCALE_DEFAULT			"DEFAULT"

/**
*These defines are the country number codes also from ISO 3166 country codes
*/
#define LOCALE_USA_CODE			840
#define LOCALE_DEFAULT_CODE		-1

class FRAMEWORK_API Locale : public Object {
public:
	Locale( const String& language,
			const String& displayLang,
		    const String& country,
			const int& countryCode ){
		m_country = country;
		m_language = language;
		m_countryCode = countryCode;
		m_displayLang = displayLang;
	};

	virtual ~Locale(){};	

	String getCountry(){
		return m_language;
	};

	String getLanguage() {
		return m_country;
	};

	String getDisplayLang() {
		return m_displayLang;
	};

	int getCountryCode(){
		return m_countryCode;
	};

private:
	String m_country;
	String m_language;
	String m_displayLang;
	int m_countryCode;
};


/**
*Locales is just a list on locales key by the country code
*for  more info see the following links
*<a href="http://userpage.chemie.fu-berlin.de/diverse/doc/ISO_3166.html">ISO 3166 Codes (Countries)</a>
*<a href="http://www.ics.uci.edu/pub/ietf/http/related/iso639.txt">Technical contents of ISO 639:1988 (E/F)(Countries)</a>
*<a href="ftp://dkuug.dk/i18n/iso-639-2.txt">Codes for the representation of names of languages -Part 2: Alpha-3 code</a>
*/
class FRAMEWORK_API Locales : public Object  
{
public:
	static Locales* create();

	Locales();	

	virtual ~Locales();

	static Locale* getLocale( const String& country );

	static Locales* getLocalesInstance() {
		return Locales::localListing;
	}
private:
	std::map<String,Locale*> m_countryMap;

	static Locales* localListing;	
};

};

#endif //_LOCALES_H__
