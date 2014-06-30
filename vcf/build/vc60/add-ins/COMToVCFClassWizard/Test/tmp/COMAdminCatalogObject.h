//COMAdminCatalogObject.h

#ifndef COMADMIN_COMADMINCATALOGOBJECT_H__
#define COMADMIN_COMADMINCATALOGOBJECT_H__
#include "FoundationKit.h"

namespace COMAdmin  {

#define COMADMIN_COMADMINCATALOGOBJECT_CLASSID 		"F618C515-DFB8-11D1-A2CF-00805FC79235"
/**
*class COMAdminCatalogObject
*UUID: F618C515-DFB8-11D1-A2CF-00805FC79235
*ProgID: 
*/
class COMAdminCatalogObject : public VCF::Object, public ICatalogObject {
public:
	BEGIN_CLASSINFO( COMAdminCatalogObject, "COMAdmin::COMAdminCatalogObject", "VCF::Object", COMADMIN_COMADMINCATALOGOBJECT_CLASSID );
	END_CLASSINFO( COMAdminCatalogObject );

	COMAdminCatalogObject();

	virtual ~COMAdminCatalogObject();

	virtual void QueryInterface( GUID* riid, void** ppvObj );//[id(1610612736)]

	virtual unsigned long AddRef(  );//[id(1610612737)]

	virtual unsigned long Release(  );//[id(1610612738)]

	virtual void GetTypeInfoCount( unsigned int* pctinfo );//[id(1610678272)]

	virtual void GetTypeInfo( unsigned int itinfo, unsigned long lcid, void** pptinfo );//[id(1610678273)]

	virtual void GetIDsOfNames( GUID* riid, char** rgszNames, unsigned int cNames, unsigned long lcid, long* rgdispid );//[id(1610678274)]

	virtual void Invoke( long dispidMember, GUID* riid, unsigned long lcid, unsigned short wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, unsigned int* puArgErr );//[id(1610678275)]

	virtual VARIANT getValue( BSTR bstrPropName );//[id(1)]

	virtual void setValue( BSTR bstrPropName, VARIANT Val );//[id(1)]

	virtual VARIANT getKey(  );//[id(2)]

	virtual VARIANT getName(  );//[id(3)]

	virtual boolean IsPropertyReadOnly( BSTR bstrPropName );//[id(4)]

	virtual boolean getValid(  );//[id(5)]

	virtual boolean IsPropertyWriteOnly( BSTR bstrPropName );//[id(6)]

};

}  //end of namespace COMAdmin

#endif //Adding method 13 of 14 for interface ICatalogObject

