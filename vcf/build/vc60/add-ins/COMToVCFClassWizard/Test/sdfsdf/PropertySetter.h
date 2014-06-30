//PropertySetter.h

#ifndef DEXTERLIB_PROPERTYSETTER_H__
#define DEXTERLIB_PROPERTYSETTER_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_PROPERTYSETTER_CLASSID 		"ADF95821-DED7-11D2-ACBE-0080C75E246E"
/**
*class PropertySetter
*UUID: ADF95821-DED7-11D2-ACBE-0080C75E246E
*ProgID: 
*/
class PropertySetter : public VCF::Object, public IPropertySetter, public IAMSetErrorLog {
public:
	BEGIN_CLASSINFO( PropertySetter, "DexterLib::PropertySetter", "VCF::Object", DEXTERLIB_PROPERTYSETTER_CLASSID );
	END_CLASSINFO( PropertySetter );

	PropertySetter();

	virtual ~PropertySetter();

	virtual HRESULT LoadXML( IUnknown* pxml );

	virtual HRESULT PrintXML( unsigned char* pszXML, int cbXML, int* pcbPrinted, int indent );

	virtual HRESULT CloneProps( IPropertySetter** ppSetter, int64 rtStart, int64 rtStop );

	virtual HRESULT AddProp( DEXTER_PARAM Param, DEXTER_VALUE* paValue );

	virtual HRESULT GetProps( long* pcParams, DEXTER_PARAM** paParam, DEXTER_VALUE** paValue );

	virtual HRESULT FreeProps( long cParams, DEXTER_PARAM* paParam, DEXTER_VALUE* paValue );

	virtual HRESULT ClearProps(  );

	virtual HRESULT SaveToBlob( long* pcSize, unsigned char** ppb );

	virtual HRESULT LoadFromBlob( long cSize, unsigned char* pb );

	virtual HRESULT SetProps( IUnknown* pTarget, int64 rtNow );

	virtual HRESULT getErrorLog( IAMErrorLog** pVal );

	virtual HRESULT setErrorLog( IAMErrorLog* pVal );

};

}  //end of namespace DexterLib

#endif //Adding method 1 of 2 for interface IAMSetErrorLog

