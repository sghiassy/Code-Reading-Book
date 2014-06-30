//Xml2Dex.h

#ifndef DEXTERLIB_XML2DEX_H__
#define DEXTERLIB_XML2DEX_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_XML2DEX_CLASSID 		"18C628EE-962A-11D2-8D08-00A0C9441E20"
/**
*class Xml2Dex
*UUID: 18C628EE-962A-11D2-8D08-00A0C9441E20
*ProgID: 
*/
class Xml2Dex : public VCF::Object, public IXml2Dex {
public:
	BEGIN_CLASSINFO( Xml2Dex, "DexterLib::Xml2Dex", "VCF::Object", DEXTERLIB_XML2DEX_CLASSID );
	END_CLASSINFO( Xml2Dex );

	Xml2Dex();

	virtual ~Xml2Dex();

	virtual HRESULT CreateGraphFromFile( IUnknown** ppGraph,  );

	virtual HRESULT WriteGrfFile(  );

	virtual HRESULT WriteXMLFile(  );

	virtual HRESULT ReadXMLFile(  );

	virtual HRESULT Delete(  );

	virtual HRESULT WriteXMLPart(  );

	virtual HRESULT PasteXMLFile(  );

	virtual HRESULT CopyXML(  );

	virtual HRESULT PasteXML(  );

	virtual HRESULT Reset(  );

	virtual HRESULT ReadXML(  );

	virtual HRESULT WriteXML(  );

};

}  //end of namespace DexterLib

#endif //Adding method 11 of 12 for interface IXml2Dex

