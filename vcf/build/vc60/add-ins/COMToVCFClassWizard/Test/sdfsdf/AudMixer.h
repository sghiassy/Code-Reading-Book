//AudMixer.h

#ifndef DEXTERLIB_AUDMIXER_H__
#define DEXTERLIB_AUDMIXER_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_AUDMIXER_CLASSID 		"036A9790-C153-11D2-9EF7-006008039E37"
/**
*class AudMixer
*UUID: 036A9790-C153-11D2-9EF7-006008039E37
*ProgID: 
*/
class AudMixer : public VCF::Object, public IBaseFilter {
public:
	BEGIN_CLASSINFO( AudMixer, "DexterLib::AudMixer", "VCF::Object", DEXTERLIB_AUDMIXER_CLASSID );
	END_CLASSINFO( AudMixer );

	AudMixer();

	virtual ~AudMixer();

	virtual HRESULT EnumPins( IEnumPins** ppEnum );

	virtual HRESULT FindPin( String Id, IPin** ppPin );

	virtual HRESULT QueryFilterInfo( _FilterInfo* pInfo );

	virtual HRESULT JoinFilterGraph( IFilterGraph* pGraph, String pName );

	virtual HRESULT QueryVendorInfo( String* pVendorInfo );

};

}  //end of namespace DexterLib

#endif //Adding method 4 of 5 for interface IBaseFilter

