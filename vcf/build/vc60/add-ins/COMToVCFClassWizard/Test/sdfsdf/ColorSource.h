//ColorSource.h

#ifndef DEXTERLIB_COLORSOURCE_H__
#define DEXTERLIB_COLORSOURCE_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_COLORSOURCE_CLASSID 		"0CFDD070-581A-11D2-9EE6-006008039E37"
/**
*class ColorSource
*UUID: 0CFDD070-581A-11D2-9EE6-006008039E37
*ProgID: 
*/
class ColorSource : public VCF::Object, public IBaseFilter {
public:
	BEGIN_CLASSINFO( ColorSource, "DexterLib::ColorSource", "VCF::Object", DEXTERLIB_COLORSOURCE_CLASSID );
	END_CLASSINFO( ColorSource );

	ColorSource();

	virtual ~ColorSource();

	virtual HRESULT EnumPins( IEnumPins** ppEnum );

	virtual HRESULT FindPin( String Id, IPin** ppPin );

	virtual HRESULT QueryFilterInfo( _FilterInfo* pInfo );

	virtual HRESULT JoinFilterGraph( IFilterGraph* pGraph, String pName );

	virtual HRESULT QueryVendorInfo( String* pVendorInfo );

};

}  //end of namespace DexterLib

#endif //Adding method 4 of 5 for interface IBaseFilter

