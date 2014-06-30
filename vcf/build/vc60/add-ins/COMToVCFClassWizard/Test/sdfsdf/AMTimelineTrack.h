//AMTimelineTrack.h

#ifndef DEXTERLIB_AMTIMELINETRACK_H__
#define DEXTERLIB_AMTIMELINETRACK_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_AMTIMELINETRACK_CLASSID 		"8F6C3C50-897B-11D2-8CFB-00A0C9441E20"
/**
*class AMTimelineTrack
*UUID: 8F6C3C50-897B-11D2-8CFB-00A0C9441E20
*ProgID: 
*/
class AMTimelineTrack : public VCF::Object, public IAMTimelineTrack, public IAMTimelineObj, public IAMTimelineEffectable, public IAMTimelineTransable, public IAMTimelineVirtualTrack, public IAMTimelineSplittable {
public:
	BEGIN_CLASSINFO( AMTimelineTrack, "DexterLib::AMTimelineTrack", "VCF::Object", DEXTERLIB_AMTIMELINETRACK_CLASSID );
	END_CLASSINFO( AMTimelineTrack );

	AMTimelineTrack();

	virtual ~AMTimelineTrack();

	virtual HRESULT SrcAdd(  );

	virtual HRESULT GetNextSrc( IAMTimelineObj** ppSrc,  );

	virtual HRESULT GetNextSrc2( IAMTimelineObj** ppSrc,  );

	virtual HRESULT MoveEverythingBy(  );

	virtual HRESULT MoveEverythingBy2(  );

	virtual HRESULT GetSourcesCount(  );

	virtual HRESULT AreYouBlank(  );

	virtual HRESULT GetSrcAtTime( IAMTimelineObj** ppSrc,  );

	virtual HRESULT GetSrcAtTime2( IAMTimelineObj** ppSrc,  );

	virtual HRESULT InsertSpace(  );

	virtual HRESULT InsertSpace2(  );

	virtual HRESULT ZeroBetween(  );

	virtual HRESULT ZeroBetween2(  );

	virtual HRESULT GetNextSrcEx( IAMTimelineObj** ppNext );

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

	virtual HRESULT SplitAt(  );

	virtual HRESULT SplitAt2(  );

};

}  //end of namespace DexterLib

#endif //Adding method 1 of 2 for interface IAMTimelineSplittable

