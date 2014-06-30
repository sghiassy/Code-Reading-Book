//AMTimeline.h

#ifndef DEXTERLIB_AMTIMELINE_H__
#define DEXTERLIB_AMTIMELINE_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_AMTIMELINE_CLASSID 		"78530B75-61F9-11D2-8CAD-00A024580902"
/**
*class AMTimeline
*UUID: 78530B75-61F9-11D2-8CAD-00A024580902
*ProgID: 
*/
class AMTimeline : public VCF::Object, public IAMTimeline, public IPersistStream, public IAMSetErrorLog {
public:
	BEGIN_CLASSINFO( AMTimeline, "DexterLib::AMTimeline", "VCF::Object", DEXTERLIB_AMTIMELINE_CLASSID );
	END_CLASSINFO( AMTimeline );

	AMTimeline();

	virtual ~AMTimeline();

	virtual HRESULT CreateEmptyNode( IAMTimelineObj** ppObj,  );

	virtual HRESULT AddGroup(  );

	virtual HRESULT RemGroupFromList(  );

	virtual HRESULT GetGroup( IAMTimelineObj** ppGroup,  );

	virtual HRESULT GetGroupCount(  );

	virtual HRESULT ClearAllGroups(  );

	virtual HRESULT GetInsertMode(  );

	virtual HRESULT SetInsertMode(  );

	virtual HRESULT EnableTransitions(  );

	virtual HRESULT TransitionsEnabled(  );

	virtual HRESULT EnableEffects(  );

	virtual HRESULT EffectsEnabled(  );

	virtual HRESULT SetInterestRange(  );

	virtual HRESULT GetDuration(  );

	virtual HRESULT GetDuration2(  );

	virtual HRESULT SetDefaultFPS(  );

	virtual HRESULT GetDefaultFPS(  );

	virtual HRESULT IsDirty(  );

	virtual HRESULT GetDirtyRange(  );

	virtual HRESULT GetCountOfType(  );

	virtual HRESULT ValidateSourceNames(  );

	virtual HRESULT SetDefaultTransition(  );

	virtual HRESULT GetDefaultTransition(  );

	virtual HRESULT SetDefaultEffect(  );

	virtual HRESULT GetDefaultEffect(  );

	virtual HRESULT SetDefaultTransitionB(  );

	virtual HRESULT GetDefaultTransitionB( BSTR* pGuid );

	virtual HRESULT SetDefaultEffectB(  );

	virtual HRESULT GetDefaultEffectB( BSTR* pGuid );

	virtual HRESULT IsDirty(  );

	virtual HRESULT Load( IStream* pstm );

	virtual HRESULT Save( IStream* pstm, long fClearDirty );

	virtual HRESULT GetSizeMax( _ULARGE_INTEGER* pcbSize );

	virtual HRESULT getErrorLog( IAMErrorLog** pVal );

	virtual HRESULT setErrorLog( IAMErrorLog* pVal );

};

}  //end of namespace DexterLib

#endif //Adding method 1 of 2 for interface IAMSetErrorLog

