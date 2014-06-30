//MediaDet.h

#ifndef DEXTERLIB_MEDIADET_H__
#define DEXTERLIB_MEDIADET_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_MEDIADET_CLASSID 		"65BD0711-24D2-4FF7-9324-ED2E5D3ABAFA"
/**
*class MediaDet
*UUID: 65BD0711-24D2-4FF7-9324-ED2E5D3ABAFA
*ProgID: 
*/
class MediaDet : public VCF::Object, public IMediaDet {
public:
	BEGIN_CLASSINFO( MediaDet, "DexterLib::MediaDet", "VCF::Object", DEXTERLIB_MEDIADET_CLASSID );
	END_CLASSINFO( MediaDet );

	MediaDet();

	virtual ~MediaDet();

	virtual HRESULT getFilter( IUnknown** pVal );

	virtual HRESULT setFilter( IUnknown* pVal );

	virtual HRESULT getOutputStreams( long* pVal );

	virtual HRESULT getCurrentStream( long* pVal );

	virtual HRESULT setCurrentStream( long pVal );

	virtual HRESULT getStreamType( GUID* pVal );

	virtual HRESULT getStreamTypeB( BSTR* pVal );

	virtual HRESULT getStreamLength( double* pVal );

	virtual HRESULT getFilename( BSTR* pVal );

	virtual HRESULT setFilename( BSTR pVal );

	virtual HRESULT GetBitmapBits(  );

	virtual HRESULT WriteBitmapBits(  );

	virtual HRESULT getStreamMediaType( _AMMediaType* pVal );

	virtual HRESULT GetSampleGrabber( ISampleGrabber** ppVal );

	virtual HRESULT getFrameRate( double* pVal );

	virtual HRESULT EnterBitmapGrabMode(  );

};

}  //end of namespace DexterLib

#endif //Adding method 15 of 16 for interface IMediaDet

