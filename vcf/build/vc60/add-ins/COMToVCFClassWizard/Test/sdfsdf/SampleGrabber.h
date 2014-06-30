//SampleGrabber.h

#ifndef DEXTERLIB_SAMPLEGRABBER_H__
#define DEXTERLIB_SAMPLEGRABBER_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_SAMPLEGRABBER_CLASSID 		"C1F400A0-3F08-11D3-9F0B-006008039E37"
/**
*class SampleGrabber
*UUID: C1F400A0-3F08-11D3-9F0B-006008039E37
*ProgID: 
*/
class SampleGrabber : public VCF::Object, public ISampleGrabber {
public:
	BEGIN_CLASSINFO( SampleGrabber, "DexterLib::SampleGrabber", "VCF::Object", DEXTERLIB_SAMPLEGRABBER_CLASSID );
	END_CLASSINFO( SampleGrabber );

	SampleGrabber();

	virtual ~SampleGrabber();

	virtual HRESULT SetOneShot(  );

	virtual HRESULT SetMediaType(  );

	virtual HRESULT GetConnectedMediaType(  );

	virtual HRESULT SetBufferSamples(  );

	virtual HRESULT GetCurrentBuffer( long* pBufferSize, long* pBuffer );

	virtual HRESULT GetCurrentSample( IMediaSample** ppSample );

	virtual HRESULT SetCallback(  );

};

}  //end of namespace DexterLib

#endif //Adding method 6 of 7 for interface ISampleGrabber

