//AMTimelineComp.h

#ifndef DEXTERLIB_AMTIMELINECOMP_H__
#define DEXTERLIB_AMTIMELINECOMP_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_AMTIMELINECOMP_CLASSID 		"74D2EC80-6233-11D2-8CAD-00A024580902"
/**
*class AMTimelineComp
*UUID: 74D2EC80-6233-11D2-8CAD-00A024580902
*ProgID: 
*/
class AMTimelineComp : public VCF::Object, public IAMTimelineComp, public IAMTimelineObj, public IAMTimelineEffectable, public IAMTimelineTransable, public IAMTimelineVirtualTrack {
public:
	BEGIN_CLASSINFO( AMTimelineComp, "DexterLib::AMTimelineComp", "VCF::Object", DEXTERLIB_AMTIMELINECOMP_CLASSID );
	END_CLASSINFO( AMTimelineComp );

	AMTimelineComp();

	virtual ~AMTimelineComp();

	virtual HRESULT VTrackInsBefore(  );

	virtual HRESULT VTrackSwapPriorities(  );

	virtual HRESULT VTrackGetCount(  );

	virtual HRESULT GetVTrack( IAMTimelineObj** ppVirtualTrack,  );

	virtual HRESULT GetCountOfType(  );

	virtual HRESULT GetRecursiveLayerOfType( IAMTimelineObj** ppVirtualTrack,  );

	virtual HRESULT GetRecursiveLayerOfTypeI( IAMTimelineObj** ppVirtualTrack, long* pWhichLayer,  );

	virtual HRESULT GetNextVTrack( IAMTimelineObj** ppNextVirtualTrack );

	virtual HRESULT GetStartStop(  );

	virtual HRESULT GetStartStop2(  );

	virtual HRESULT FixTimes(  );

	virtual HRESULT FixTimes2(  );

	virtual HRESULT SetStartStop(  );

	virtual HRESULT SetStartStop2(  );

	virtual HRESULT GetPropertySetter( IPropertySetter** pVal );

	virtual HRESULT SetPropertySetter(  );

	virtual HRESULT GetSubObject( IUnknown** pVal );

	virtual HRESULT SetSubObject(  );

	virtual HRESULT SetSubObjectGUID(  );

	virtual HRESULT SetSubObjectGUIDB(  );

	virtual HRESULT GetSubObjectGUID(  );

	virtual HRESULT GetSubObjectGUIDB( BSTR* pVal );

	virtual HRESULT GetSubObjectLoaded(  );

	virtual HRESULT GetTimelineType(  );

	virtual HRESULT SetTimelineType(  );

	virtual HRESULT GetUserID(  );

	virtual HRESULT SetUserID(  );

	virtual HRESULT GetGenID(  );

	virtual HRESULT GetUserName( BSTR* pVal );

	virtual HRESULT SetUserName(  );

	virtual HRESULT GetUserData(  );

	virtual HRESULT SetUserData(  );

	virtual HRESULT GetMuted(  );

	virtual HRESULT SetMuted(  );

	virtual HRESULT GetLocked(  );

	virtual HRESULT SetLocked(  );

	virtual HRESULT GetDirtyRange(  );

	virtual HRESULT GetDirtyRange2(  );

	virtual HRESULT SetDirtyRange(  );

	virtual HRESULT SetDirtyRange2(  );

	virtual HRESULT ClearDirty(  );

	virtual HRESULT Remove(  );

	virtual HRESULT RemoveAll(  );

	virtual HRESULT GetTimelineNoRef(  );

	virtual HRESULT GetGroupIBelongTo( IAMTimelineGroup** ppGroup );

	virtual HRESULT GetEmbedDepth(  );

	virtual HRESULT EffectInsBefore(  );

	virtual HRESULT EffectSwapPriorities(  );

	virtual HRESULT EffectGetCount(  );

	virtual HRESULT GetEffect( IAMTimelineObj** ppFx,  );

	virtual HRESULT TransAdd(  );

	virtual HRESULT TransGetCount(  );

	virtual HRESULT GetNextTrans( IAMTimelineObj** ppTrans,  );

	virtual HRESULT GetNextTrans2( IAMTimelineObj** ppTrans,  );

	virtual HRESULT GetTransAtTime( IAMTimelineObj** ppObj,  );

	virtual HRESULT GetTransAtTime2( IAMTimelineObj** ppObj,  );

	virtual HRESULT TrackGetPriority(  );

	virtual HRESULT SetTrackDirty(  );

};

}  //end of namespace DexterLib

#endif //Adding method 1 of 2 for interface IAMTimelineVirtualTrack

