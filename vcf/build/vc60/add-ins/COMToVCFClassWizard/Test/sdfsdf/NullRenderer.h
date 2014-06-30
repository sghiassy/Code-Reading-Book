//NullRenderer.h

#ifndef DEXTERLIB_NULLRENDERER_H__
#define DEXTERLIB_NULLRENDERER_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_NULLRENDERER_CLASSID 		"C1F400A4-3F08-11D3-9F0B-006008039E37"
/**
*class NullRenderer
*UUID: C1F400A4-3F08-11D3-9F0B-006008039E37
*ProgID: 
*/
class NullRenderer : public VCF::Object, public IBaseFilter {
public:
	BEGIN_CLASSINFO( NullRenderer, "DexterLib::NullRenderer", "VCF::Object", DEXTERLIB_NULLRENDERER_CLASSID );
	END_CLASSINFO( NullRenderer );

	NullRenderer();

	virtual ~NullRenderer();

	virtual HRESULT EnumPins( IEnumPins** ppEnum );

	virtual HRESULT FindPin( String Id, IPin** ppPin );

	virtual HRESULT QueryFilterInfo( _FilterInfo* pInfo );

	virtual HRESULT JoinFilterGraph( IFilterGraph* pGraph, String pName );

	virtual HRESULT QueryVendorInfo( String* pVendorInfo );

};

}  //end of namespace DexterLib

#endif //Adding method 4 of 5 for interface IBaseFilter

