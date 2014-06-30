//COMAdminCatalogCollection.h

#ifndef COMADMIN_COMADMINCATALOGCOLLECTION_H__
#define COMADMIN_COMADMINCATALOGCOLLECTION_H__
#include "FoundationKit.h"

namespace COMAdmin  {

#define COMADMIN_COMADMINCATALOGCOLLECTION_CLASSID 		"F618C516-DFB8-11D1-A2CF-00805FC79235"
/**
*class COMAdminCatalogCollection
*UUID: F618C516-DFB8-11D1-A2CF-00805FC79235
*ProgID: 
*/
class COMAdminCatalogCollection : public VCF::Object, public ICatalogCollection {
public:
	BEGIN_CLASSINFO( COMAdminCatalogCollection, "COMAdmin::COMAdminCatalogCollection", "VCF::Object", COMADMIN_COMADMINCATALOGCOLLECTION_CLASSID );
	END_CLASSINFO( COMAdminCatalogCollection );

	COMAdminCatalogCollection();

	virtual ~COMAdminCatalogCollection();

	virtual void QueryInterface( GUID* riid, void** ppvObj );//[id(1610612736)]

	virtual unsigned long AddRef(  );//[id(1610612737)]

	virtual unsigned long Release(  );//[id(1610612738)]

	virtual void GetTypeInfoCount( unsigned int* pctinfo );//[id(1610678272)]

	virtual void GetTypeInfo( unsigned int itinfo, unsigned long lcid, void** pptinfo );//[id(1610678273)]

	virtual void GetIDsOfNames( GUID* riid, char** rgszNames, unsigned int cNames, unsigned long lcid, long* rgdispid );//[id(1610678274)]

	virtual void Invoke( long dispidMember, GUID* riid, unsigned long lcid, unsigned short wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, unsigned int* puArgErr );//[id(1610678275)]

	virtual IUnknown* get_NewEnum(  );//[id(-4)]

	virtual IDispatch* getItem( long lIndex );//[id(1)]

	virtual long getCount(  );//[id(1610743810)]

	virtual void Remove( long lIndex );//[id(1610743811)]

	virtual IDispatch* Add(  );//[id(1610743812)]

	virtual void Populate(  );//[id(2)]

	virtual long SaveChanges(  );//[id(3)]

	virtual IDispatch* GetCollection( BSTR bstrCollName, VARIANT varObjectKey );//[id(4)]

	virtual VARIANT getName(  );//[id(6)]

	virtual boolean getAddEnabled(  );//[id(7)]

	virtual boolean getRemoveEnabled(  );//[id(8)]

	virtual IDispatch* GetUtilInterface(  );//[id(9)]

	virtual long getDataStoreMajorVersion(  );//[id(10)]

	virtual long getDataStoreMinorVersion(  );//[id(11)]

	virtual void PopulateByKey( SAFEARRAY(VARIANT) aKeys );//[id(12)]

	virtual void PopulateByQuery( BSTR bstrQueryString, long lQueryType );//[id(13)]

};

}  //end of namespace COMAdmin

#endif //Adding method 22 of 23 for interface ICatalogCollection

