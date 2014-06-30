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
//ComponentInfo.cpp
#include "ApplicationKit.h"
#include "ComponentInfo.h"

using namespace VCF;


ComponentInfo::ComponentInfo( const String& componentUUID, const String& author, const String& company, 
			  const String& copyright, const String& additionalInfo, Image* componentImage )
{
	m_author = author;
	m_copyright = copyright;
	m_company = company;
	m_additionalInfo = additionalInfo;
	m_componentUUID = componentUUID;
	m_componentImage = componentImage;

	m_reqNamespacesContainer.initContainer( m_requiredNamespaces );
	m_reqHeadersContainer.initContainer( m_requiredHeaders );
}

ComponentInfo::~ComponentInfo()
{
	if ( NULL != m_componentImage ) {
		delete m_componentImage;
	}
}

Enumerator<String>* ComponentInfo::getRequiredHeaders()
{	
	return m_reqHeadersContainer.getEnumerator();
}

Enumerator<String>* ComponentInfo::getRequiredNamespaces()
{
	return m_reqNamespacesContainer.getEnumerator();
}

void ComponentInfo::addHeader( const String& header )
{
	m_requiredHeaders.push_back( header );
}

void ComponentInfo::removeHeader( const String& header )
{
	std::vector<String>::iterator found = std::find( m_requiredHeaders.begin(), m_requiredHeaders.end(), header );
	if ( found != m_requiredHeaders.end() ) {
		m_requiredHeaders.erase( found );
	}
}

void ComponentInfo::removeAllHeaders()
{
	m_requiredHeaders.clear();
}

void ComponentInfo::addNamespace( const String& aNamespace )
{
	m_requiredNamespaces.push_back( aNamespace );
}

void ComponentInfo::removeNamespace( const String& aNamespace )
{
	std::vector<String>::iterator found = std::find( m_requiredNamespaces.begin(), m_requiredNamespaces.end(), aNamespace );
	if ( found != m_requiredNamespaces.end() ) {
		m_requiredNamespaces.erase( found );
	}
}

void ComponentInfo::removeAllNamespaces()
{
	m_requiredNamespaces.clear();
}