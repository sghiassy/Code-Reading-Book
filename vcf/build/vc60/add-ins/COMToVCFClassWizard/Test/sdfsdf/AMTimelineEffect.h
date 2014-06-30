//AMTimelineEffect.h

#ifndef DEXTERLIB_AMTIMELINEEFFECT_H__
#define DEXTERLIB_AMTIMELINEEFFECT_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_AMTIMELINEEFFECT_CLASSID 		"74D2EC82-6233-11D2-8CAD-00A024580902"
/**
*class AMTimelineEffect
*UUID: 74D2EC82-6233-11D2-8CAD-00A024580902
*ProgID: 
*/
class AMTimelineEffect : public VCF::Object, public IAMTimelineEffect, public IAMTimelineObj, public IAMTimelineSplittable, public IPropertyBag {
public:
	BEGIN_CLASSINFO( AMTimelineEffect, "DexterLib::AMTimelineEffect", "VCF::Object", DEXTERLIB_AMTIMELINEEFFECT_CLASSID );
	END_CLASSINFO( AMTimelineEffect );

	AMTimelineEffect();

	virtual ~AMTimelineEffect();

	virtual HRESULT EffectGetPriority(  );

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

	virtual HRESULT RemoteRead( String pszPropName, VARIANT* pVar, IErrorLog* pErrorLog, unsigned long varType, IUnknown* pUnkObj );

	virtual HRESULT Write( String pszPropName, VARIANT* pVar );

};

}  //end of namespace DexterLib

#endif //Adding method 1 of 2 for interface IPropertyBag

