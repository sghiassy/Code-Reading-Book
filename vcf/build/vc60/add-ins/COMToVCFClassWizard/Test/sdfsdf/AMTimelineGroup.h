//AMTimelineGroup.h

#ifndef DEXTERLIB_AMTIMELINEGROUP_H__
#define DEXTERLIB_AMTIMELINEGROUP_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_AMTIMELINEGROUP_CLASSID 		"F6D371E1-B8A6-11D2-8023-00C0DF10D434"
/**
*class AMTimelineGroup
*UUID: F6D371E1-B8A6-11D2-8023-00C0DF10D434
*ProgID: 
*/
class AMTimelineGroup : public VCF::Object, public IAMTimelineGroup, public IAMTimelineComp, public IAMTimelineObj {
public:
	BEGIN_CLASSINFO( AMTimelineGroup, "DexterLib::AMTimelineGroup", "VCF::Object", DEXTERLIB_AMTIMELINEGROUP_CLASSID );
	END_CLASSINFO( AMTimelineGroup );

	AMTimelineGroup();

	virtual ~AMTimelineGroup();

	virtual HRESULT SetTimeline(  );

	virtual HRESULT GetTimeline( IAMTimeline** ppTimeline );

	virtual HRESULT GetPriority(  );

	virtual HRESULT GetMediaType( _AMMediaType* __MIDL_0040 );

	virtual HRESULT SetMediaType( _AMMediaType* __MIDL_0041 );

	virtual HRESULT SetOutputFPS(  );

	virtual HRESULT GetOutputFPS(  );

	virtual HRESULT SetGroupName(  );

	virtual HRESULT GetGroupName( BSTR* pGroupName );

	virtual HRESULT SetPreviewMode(  );

	virtual HRESULT GetPreviewMode(  );

	virtual HRESULT SetMediaTypeForVB( long Val );

	virtual HRESULT GetOutputBuffering( int* pnBuffer );

	virtual HRESULT SetOutputBuffering( int nBuffer );

	virtual HRESULT SetSmartRecompressFormat(  );

	virtual HRESULT GetSmartRecompressFormat(  );

	virtual HRESULT IsSmartRecompressFormatSet(  );

	virtual HRESULT IsRecompressFormatDirty(  );

	virtual HRESULT ClearRecompressFormatDirty(  );

	virtual HRESULT SetRecompFormatFromSource(  );

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

};

}  //end of namespace DexterLib

#endif //Adding method 37 of 38 for interface IAMTimelineObj

