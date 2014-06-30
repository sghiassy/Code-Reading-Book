//AMTimelineObj.h

#ifndef DEXTERLIB_AMTIMELINEOBJ_H__
#define DEXTERLIB_AMTIMELINEOBJ_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_AMTIMELINEOBJ_CLASSID 		"78530B78-61F9-11D2-8CAD-00A024580902"
/**
*class AMTimelineObj
*UUID: 78530B78-61F9-11D2-8CAD-00A024580902
*ProgID: 
*/
class AMTimelineObj : public VCF::Object, public IAMTimelineObj {
public:
	BEGIN_CLASSINFO( AMTimelineObj, "DexterLib::AMTimelineObj", "VCF::Object", DEXTERLIB_AMTIMELINEOBJ_CLASSID );
	END_CLASSINFO( AMTimelineObj );

	AMTimelineObj();

	virtual ~AMTimelineObj();

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

