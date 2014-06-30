//MediaLocator.h

#ifndef DEXTERLIB_MEDIALOCATOR_H__
#define DEXTERLIB_MEDIALOCATOR_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_MEDIALOCATOR_CLASSID 		"CC1101F2-79DC-11D2-8CE6-00A0C9441E20"
/**
*class MediaLocator
*UUID: CC1101F2-79DC-11D2-8CE6-00A0C9441E20
*ProgID: 
*/
class MediaLocator : public VCF::Object, public IMediaLocator {
public:
	BEGIN_CLASSINFO( MediaLocator, "DexterLib::MediaLocator", "VCF::Object", DEXTERLIB_MEDIALOCATOR_CLASSID );
	END_CLASSINFO( MediaLocator );

	MediaLocator();

	virtual ~MediaLocator();

	virtual HRESULT FindMediaFile(  );

	virtual HRESULT AddFoundLocation(  );

};

}  //end of namespace DexterLib

#endif //Adding method 1 of 2 for interface IMediaLocator

