//AMTimelineTrans.h

#ifndef DEXTERLIB_AMTIMELINETRANS_H__
#define DEXTERLIB_AMTIMELINETRANS_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_AMTIMELINETRANS_CLASSID 		"74D2EC81-6233-11D2-8CAD-00A024580902"
/**
*class AMTimelineTrans
*UUID: 74D2EC81-6233-11D2-8CAD-00A024580902
*ProgID: 
*/
class AMTimelineTrans : public VCF::Object, public IAMTimelineTrans, public IAMTimelineObj, public IAMTimelineSplittable {
public:
	BEGIN_CLASSINFO( AMTimelineTrans, "DexterLib::AMTimelineTrans", "VCF::Object", DEXTERLIB_AMTIMELINETRANS_CLASSID );
	END_CLASSINFO( AMTimelineTrans );

	AMTimelineTrans();

	virtual ~AMTimelineTrans();

	virtual HRESULT GetCutPoint(  );

	virtual HRESULT GetCutPoint2(  );

	virtual HRESULT SetCutPoint(  );

	virtual HRESULT SetCutPoint2(  );

	virtual HRESULT GetSwapInputs(  );

	virtual HRESULT SetSwapInputs(  );

	virtual HRESULT GetCutsOnly(  );

	virtual HRESULT SetCutsOnly(  );

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

	virtual HRESULT SplitAt(  );

	virtual HRESULT SplitAt2(  );

};

}  //end of namespace DexterLib

#endif //Adding method 1 of 2 for interface IAMTimelineSplittable

