//SmartRenderEngine.h

#ifndef DEXTERLIB_SMARTRENDERENGINE_H__
#define DEXTERLIB_SMARTRENDERENGINE_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_SMARTRENDERENGINE_CLASSID 		"498B0949-BBE9-4072-98BE-6CCAEB79DC6F"
/**
*class SmartRenderEngine
*UUID: 498B0949-BBE9-4072-98BE-6CCAEB79DC6F
*ProgID: 
*/
class SmartRenderEngine : public VCF::Object, public IRenderEngine, public ISmartRenderEngine, public IAMSetErrorLog {
public:
	BEGIN_CLASSINFO( SmartRenderEngine, "DexterLib::SmartRenderEngine", "VCF::Object", DEXTERLIB_SMARTRENDERENGINE_CLASSID );
	END_CLASSINFO( SmartRenderEngine );

	SmartRenderEngine();

	virtual ~SmartRenderEngine();

	virtual HRESULT SetTimelineObject(  );

	virtual HRESULT GetTimelineObject( IAMTimeline** ppTimeline );

	virtual HRESULT GetFilterGraph( IGraphBuilder** ppFG );

	virtual HRESULT SetFilterGraph(  );

	virtual HRESULT SetInterestRange(  );

	virtual HRESULT SetInterestRange2(  );

	virtual HRESULT SetRenderRange(  );

	virtual HRESULT SetRenderRange2(  );

	virtual HRESULT GetGroupOutputPin( IPin** ppRenderPin );

	virtual HRESULT ScrapIt(  );

	virtual HRESULT RenderOutputPins(  );

	virtual HRESULT GetVendorString( BSTR* pVendorID );

	virtual HRESULT ConnectFrontEnd(  );

	virtual HRESULT SetSourceConnectCallback(  );

	virtual HRESULT SetDynamicReconnectLevel(  );

	virtual HRESULT DoSmartRecompression(  );

	virtual HRESULT UseInSmartRecompressionGraph(  );

	virtual HRESULT SetSourceNameValidation(  );

	virtual HRESULT Commit(  );

	virtual HRESULT Decommit(  );

	virtual HRESULT GetCaps(  );

	virtual HRESULT SetGroupCompressor(  );

	virtual HRESULT GetGroupCompressor(  );

	virtual HRESULT SetFindCompressorCB(  );

	virtual HRESULT getErrorLog( IAMErrorLog** pVal );

	virtual HRESULT setErrorLog( IAMErrorLog* pVal );

};

}  //end of namespace DexterLib

#endif //Adding method 1 of 2 for interface IAMSetErrorLog

