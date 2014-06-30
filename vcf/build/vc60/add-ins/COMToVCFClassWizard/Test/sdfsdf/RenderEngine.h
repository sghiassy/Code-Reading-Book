//RenderEngine.h

#ifndef DEXTERLIB_RENDERENGINE_H__
#define DEXTERLIB_RENDERENGINE_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_RENDERENGINE_CLASSID 		"64D8A8E0-80A2-11D2-8CF3-00A0C9441E20"
/**
*class RenderEngine
*UUID: 64D8A8E0-80A2-11D2-8CF3-00A0C9441E20
*ProgID: 
*/
class RenderEngine : public VCF::Object, public IRenderEngine, public IAMSetErrorLog {
public:
	BEGIN_CLASSINFO( RenderEngine, "DexterLib::RenderEngine", "VCF::Object", DEXTERLIB_RENDERENGINE_CLASSID );
	END_CLASSINFO( RenderEngine );

	RenderEngine();

	virtual ~RenderEngine();

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

	virtual HRESULT getErrorLog( IAMErrorLog** pVal );

	virtual HRESULT setErrorLog( IAMErrorLog* pVal );

};

}  //end of namespace DexterLib

#endif //Adding method 1 of 2 for interface IAMSetErrorLog

