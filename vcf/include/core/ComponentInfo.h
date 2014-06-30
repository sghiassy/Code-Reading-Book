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
//ComponentInfo.h

#ifndef _COMPONENTINFO_H__
#define _COMPONENTINFO_H__



namespace VCF {

class Image;

class APPKIT_API ComponentInfo : public Object {
public :
	ComponentInfo( const String& componentUUID, const String& author="", const String& company="", 
					const String& copyright="", const String& additionalInfo="", Image* componentImage=NULL );

	virtual ~ComponentInfo();
	
	String getAuthor() {
		return m_author;
	}

	String getCopyright() {
		return m_copyright;
	}

	String getCompany() {
		return m_company;
	}

	String getAdditionalInfo() {
		return m_additionalInfo;
	}

	String getComponentUUID() {
		return m_componentUUID;
	}

	Image* getComponentImage() {
		return m_componentImage;
	}
	
	Enumerator<String>* getRequiredHeaders();

	Enumerator<String>* getRequiredNamespaces();

	void addHeader( const String& header );

	void removeHeader( const String& header );

	void removeAllHeaders();

	void addNamespace( const String& aNamespace );

	void removeNamespace( const String& aNamespace );

	void removeAllNamespaces();
protected:
	String m_author;
	String m_copyright;
	String m_company;
	String m_additionalInfo;
	String m_componentUUID;
	Image* m_componentImage;

	std::vector<String> m_requiredHeaders;
	EnumeratorContainer<std::vector<String>,String> m_reqHeadersContainer;

	std::vector<String> m_requiredNamespaces;
	EnumeratorContainer<std::vector<String>,String> m_reqNamespacesContainer;
};


};

#endif //_COMPONENTINFO_H__