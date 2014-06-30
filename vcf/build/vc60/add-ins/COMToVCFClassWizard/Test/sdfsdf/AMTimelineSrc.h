//AMTimelineSrc.h

#ifndef DEXTERLIB_AMTIMELINESRC_H__
#define DEXTERLIB_AMTIMELINESRC_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_AMTIMELINESRC_CLASSID 		"78530B7A-61F9-11D2-8CAD-00A024580902"
/**
*class AMTimelineSrc
*UUID: 78530B7A-61F9-11D2-8CAD-00A024580902
*ProgID: 
*/
class AMTimelineSrc : public VCF::Object, public IAMTimelineSrc, public IAMTimelineObj, public IAMTimelineEffectable, public IAMTimelineSplittable {
public:
	BEGIN_CLASSINFO( AMTimelineSrc, "DexterLib::AMTimelineSrc", "VCF::Object", DEXTERLIB_AMTIMELINESRC_CLASSID );
	END_CLASSINFO( AMTimelineSrc );

	AMTimelineSrc();

	virtual ~AMTimelineSrc();

	virtual HRESULT GetMediaTimes(  );

	virtual HRESULT GetMediaTimes2(  );

	virtual HRESULT ModifyStopTime(  );

	virtual HRESULT ModifyStopTime2(  );

	virtual HRESULT FixMediaTimes(  );

	virtual HRESULT FixMediaTimes2(  );

	virtual HRESULT SetMediaTimes(  );

	virtual HRESULT SetMediaTimes2(  );

	virtual HRESULT SetMediaLength(  );

	virtual HRESULT SetMediaLength2(  );

	virtual HRESULT GetMediaLength(  );

	virtual HRESULT GetMediaLength2(  );

	virtual HRESULT GetMediaName( BSTR* pVal );

	virtual HRESULT SetMediaName(  );

	virtual HRESULT SpliceWithNext(  );

	virtual HRESULT GetStreamNumber(  );

	virtual HRESULT SetStreamNumber(  );

	virtual HRESULT IsNormalRate(  );

	virtual HRESULT GetDefaultFPS(  );

	virtual HRESULT SetDefaultFPS(  );

	virtual HRESULT GetStretchMode(  );

	virtual HRESULT SetStretchMode(  );

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

	virtual HRESULT SplitAt(  );

	virtual HRESULT SplitAt2(  );

};

}  //end of namespace DexterLib

#endif //Adding method 1 of 2 for interface IAMTimelineSplittable

