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
// Locales.cpp
#include "FoundationKit.h"


using namespace VCF;

Locales* Locales::create()
{
	if ( NULL == Locales::localListing ) {
		Locales::localListing = new Locales();
		Locales::localListing->init();
	}

	return Locales::localListing;
}

Locales::Locales()
{	
	m_countryMap[LOCALE_USA] = 
		new Locale( "en", "English", LOCALE_USA, LOCALE_USA_CODE );
	m_countryMap[LOCALE_DEFAULT] = 
		new Locale( "default", "default", LOCALE_DEFAULT, LOCALE_DEFAULT_CODE );
}

Locales::~Locales()
{
	std::map<String,Locale*>::iterator it = m_countryMap.begin();
	while ( it != m_countryMap.end() ){
		delete it->second;
		it ++;
	}
	m_countryMap.clear();
}	

Locale* Locales::getLocale( const String& country )
{
	std::map<String,Locale*>::iterator found = 
		Locales::localListing->m_countryMap.find( country );

	Locale* result = NULL;
	if ( found != Locales::localListing->m_countryMap.end() ){
		result = found->second;
	}

	return result;
}