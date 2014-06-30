//COMAdmin.h

#ifndef _COMADMININTERFACES_H__
#define _COMADMININTERFACES_H__

#include "FoundationKit.h"

namespace COMAdmin  {

/**
*Interface ICOMAdminCatalog
*UUID: DD662187-DFC2-11D1-A2CF-00805FC79235
*/
class ICOMAdminCatalog : public Interface { 
public:
	virtual ~ICOMAdminCatalog(){};

	virtual void QueryInterface( GUID* riid, void** ppvObj ) = 0;//[id(1610612736)]

	virtual unsigned long AddRef(  ) = 0;//[id(1610612737)]

	virtual unsigned long Release(  ) = 0;//[id(1610612738)]

	virtual void GetTypeInfoCount( unsigned int* pctinfo ) = 0;//[id(1610678272)]

	virtual void GetTypeInfo( unsigned int itinfo, unsigned long lcid, void** pptinfo ) = 0;//[id(1610678273)]

	virtual void GetIDsOfNames( GUID* riid, char** rgszNames, unsigned int cNames, unsigned long lcid, long* rgdispid ) = 0;//[id(1610678274)]

	virtual void Invoke( long dispidMember, GUID* riid, unsigned long lcid, unsigned short wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, unsigned int* puArgErr ) = 0;//[id(1610678275)]

	virtual IDispatch* GetCollection( BSTR bstrCollName ) = 0;//[id(1)]

	virtual IDispatch* Connect( BSTR bstrConnectString ) = 0;//[id(2)]

	virtual long getMajorVersion(  ) = 0;//[id(3)]

	virtual long getMinorVersion(  ) = 0;//[id(4)]

	virtual IDispatch* GetCollectionByQuery( BSTR bstrCollName, SAFEARRAY(VARIANT)* aQuery ) = 0;//[id(5)]

	virtual void ImportComponent( BSTR bstrApplIdOrName, BSTR bstrCLSIDOrProgId ) = 0;//[id(6)]

	virtual void InstallComponent( BSTR bstrApplIdOrName, BSTR bstrDLL, BSTR bstrTLB, BSTR bstrPSDLL ) = 0;//[id(7)]

	virtual void ShutdownApplication( BSTR bstrApplIdOrName ) = 0;//[id(8)]

	virtual void ExportApplication( BSTR bstrApplIdOrName, BSTR bstrApplicationFile, long lOptions ) = 0;//[id(9)]

	virtual void InstallApplication( BSTR bstrApplicationFile, BSTR bstrDestinationDirectory, long lOptions, BSTR bstrUserId, BSTR bstrPassword, BSTR bstrRSN ) = 0;//[id(10)]

	virtual void StopRouter(  ) = 0;//[id(11)]

	virtual void RefreshRouter(  ) = 0;//[id(12)]

	virtual void StartRouter(  ) = 0;//[id(13)]

	virtual void Reserved1(  ) = 0;//[id(14)]

	virtual void Reserved2(  ) = 0;//[id(15)]

	virtual void InstallMultipleComponents( BSTR bstrApplIdOrName, SAFEARRAY(VARIANT)* varFileNames, SAFEARRAY(VARIANT)* varCLSIDS ) = 0;//[id(16)]

	virtual void GetMultipleComponentsInfo( BSTR bstrApplIdOrName, SAFEARRAY(VARIANT)* varFileNames, SAFEARRAY(VARIANT)* varCLSIDS, SAFEARRAY(VARIANT)* varClassNames, SAFEARRAY(VARIANT)* varFileFlags, SAFEARRAY(VARIANT)* varComponentFlags ) = 0;//[id(17)]

	virtual void RefreshComponents(  ) = 0;//[id(18)]

	virtual void BackupREGDB( BSTR bstrBackupFilePath ) = 0;//[id(19)]

	virtual void RestoreREGDB( BSTR bstrBackupFilePath ) = 0;//[id(20)]

	virtual void QueryApplicationFile( BSTR bstrApplicationFile, BSTR* bstrApplicationName, BSTR* bstrApplicationDescription, boolean* bHasUsers, boolean* bIsProxy, SAFEARRAY(VARIANT)* varFileNames ) = 0;//[id(21)]

	virtual void StartApplication( BSTR bstrApplIdOrName ) = 0;//[id(22)]

	virtual long ServiceCheck( long lService ) = 0;//[id(23)]

	virtual void InstallMultipleEventClasses( BSTR bstrApplIdOrName, SAFEARRAY(VARIANT)* varFileNames, SAFEARRAY(VARIANT)* varCLSIDS ) = 0;//[id(24)]

	virtual void InstallEventClass( BSTR bstrApplIdOrName, BSTR bstrDLL, BSTR bstrTLB, BSTR bstrPSDLL ) = 0;//[id(25)]

	virtual void GetEventClassesForIID( BSTR bstrIID, SAFEARRAY(VARIANT)* varCLSIDS, SAFEARRAY(VARIANT)* varProgIDs, SAFEARRAY(VARIANT)* varDescriptions ) = 0;//[id(26)]

};


/**
*Interface ICatalogCollection
*UUID: 6EB22872-8A19-11D0-81B6-00A0C9231C29
*/
class ICatalogCollection : public Interface { 
public:
	virtual ~ICatalogCollection(){};

	virtual void QueryInterface( GUID* riid, void** ppvObj ) = 0;//[id(1610612736)]

	virtual unsigned long AddRef(  ) = 0;//[id(1610612737)]

	virtual unsigned long Release(  ) = 0;//[id(1610612738)]

	virtual void GetTypeInfoCount( unsigned int* pctinfo ) = 0;//[id(1610678272)]

	virtual void GetTypeInfo( unsigned int itinfo, unsigned long lcid, void** pptinfo ) = 0;//[id(1610678273)]

	virtual void GetIDsOfNames( GUID* riid, char** rgszNames, unsigned int cNames, unsigned long lcid, long* rgdispid ) = 0;//[id(1610678274)]

	virtual void Invoke( long dispidMember, GUID* riid, unsigned long lcid, unsigned short wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, unsigned int* puArgErr ) = 0;//[id(1610678275)]

	virtual IUnknown* get_NewEnum(  ) = 0;//[id(-4)]

	virtual IDispatch* getItem( long lIndex ) = 0;//[id(1)]

	virtual long getCount(  ) = 0;//[id(1610743810)]

	virtual void Remove( long lIndex ) = 0;//[id(1610743811)]

	virtual IDispatch* Add(  ) = 0;//[id(1610743812)]

	virtual void Populate(  ) = 0;//[id(2)]

	virtual long SaveChanges(  ) = 0;//[id(3)]

	virtual IDispatch* GetCollection( BSTR bstrCollName, VARIANT varObjectKey ) = 0;//[id(4)]

	virtual VARIANT getName(  ) = 0;//[id(6)]

	virtual boolean getAddEnabled(  ) = 0;//[id(7)]

	virtual boolean getRemoveEnabled(  ) = 0;//[id(8)]

	virtual IDispatch* GetUtilInterface(  ) = 0;//[id(9)]

	virtual long getDataStoreMajorVersion(  ) = 0;//[id(10)]

	virtual long getDataStoreMinorVersion(  ) = 0;//[id(11)]

	virtual void PopulateByKey( SAFEARRAY(VARIANT) aKeys ) = 0;//[id(12)]

	virtual void PopulateByQuery( BSTR bstrQueryString, long lQueryType ) = 0;//[id(13)]

};


/**
*Interface ICatalogObject
*UUID: 6EB22871-8A19-11D0-81B6-00A0C9231C29
*/
class ICatalogObject : public Interface { 
public:
	virtual ~ICatalogObject(){};

	virtual void QueryInterface( GUID* riid, void** ppvObj ) = 0;//[id(1610612736)]

	virtual unsigned long AddRef(  ) = 0;//[id(1610612737)]

	virtual unsigned long Release(  ) = 0;//[id(1610612738)]

	virtual void GetTypeInfoCount( unsigned int* pctinfo ) = 0;//[id(1610678272)]

	virtual void GetTypeInfo( unsigned int itinfo, unsigned long lcid, void** pptinfo ) = 0;//[id(1610678273)]

	virtual void GetIDsOfNames( GUID* riid, char** rgszNames, unsigned int cNames, unsigned long lcid, long* rgdispid ) = 0;//[id(1610678274)]

	virtual void Invoke( long dispidMember, GUID* riid, unsigned long lcid, unsigned short wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, unsigned int* puArgErr ) = 0;//[id(1610678275)]

	virtual VARIANT getValue( BSTR bstrPropName ) = 0;//[id(1)]

	virtual void setValue( BSTR bstrPropName, VARIANT Val ) = 0;//[id(1)]

	virtual VARIANT getKey(  ) = 0;//[id(2)]

	virtual VARIANT getName(  ) = 0;//[id(3)]

	virtual boolean IsPropertyReadOnly( BSTR bstrPropName ) = 0;//[id(4)]

	virtual boolean getValid(  ) = 0;//[id(5)]

	virtual boolean IsPropertyWriteOnly( BSTR bstrPropName ) = 0;//[id(6)]

};


}  //end of namespace COMAdmin

#endif //_COMADMININTERFACES_H__

