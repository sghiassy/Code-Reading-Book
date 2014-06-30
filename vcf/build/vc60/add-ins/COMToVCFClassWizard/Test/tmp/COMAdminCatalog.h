//COMAdminCatalog.h

#ifndef COMADMIN_COMADMINCATALOG_H__
#define COMADMIN_COMADMINCATALOG_H__
#include "FoundationKit.h"

namespace COMAdmin  {

#define COMADMIN_COMADMINCATALOG_CLASSID 		"F618C514-DFB8-11D1-A2CF-00805FC79235"
/**
*class COMAdminCatalog
*UUID: F618C514-DFB8-11D1-A2CF-00805FC79235
*ProgID: 
*/
class COMAdminCatalog : public VCF::Object, public ICOMAdminCatalog {
public:
	BEGIN_CLASSINFO( COMAdminCatalog, "COMAdmin::COMAdminCatalog", "VCF::Object", COMADMIN_COMADMINCATALOG_CLASSID );
	END_CLASSINFO( COMAdminCatalog );

	COMAdminCatalog();

	virtual ~COMAdminCatalog();

	virtual void QueryInterface( GUID* riid, void** ppvObj );//[id(1610612736)]

	virtual unsigned long AddRef(  );//[id(1610612737)]

	virtual unsigned long Release(  );//[id(1610612738)]

	virtual void GetTypeInfoCount( unsigned int* pctinfo );//[id(1610678272)]

	virtual void GetTypeInfo( unsigned int itinfo, unsigned long lcid, void** pptinfo );//[id(1610678273)]

	virtual void GetIDsOfNames( GUID* riid, char** rgszNames, unsigned int cNames, unsigned long lcid, long* rgdispid );//[id(1610678274)]

	virtual void Invoke( long dispidMember, GUID* riid, unsigned long lcid, unsigned short wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, unsigned int* puArgErr );//[id(1610678275)]

	virtual IDispatch* GetCollection( BSTR bstrCollName );//[id(1)]

	virtual IDispatch* Connect( BSTR bstrConnectString );//[id(2)]

	virtual long getMajorVersion(  );//[id(3)]

	virtual long getMinorVersion(  );//[id(4)]

	virtual IDispatch* GetCollectionByQuery( BSTR bstrCollName, SAFEARRAY(VARIANT)* aQuery );//[id(5)]

	virtual void ImportComponent( BSTR bstrApplIdOrName, BSTR bstrCLSIDOrProgId );//[id(6)]

	virtual void InstallComponent( BSTR bstrApplIdOrName, BSTR bstrDLL, BSTR bstrTLB, BSTR bstrPSDLL );//[id(7)]

	virtual void ShutdownApplication( BSTR bstrApplIdOrName );//[id(8)]

	virtual void ExportApplication( BSTR bstrApplIdOrName, BSTR bstrApplicationFile, long lOptions );//[id(9)]

	virtual void InstallApplication( BSTR bstrApplicationFile, BSTR bstrDestinationDirectory, long lOptions, BSTR bstrUserId, BSTR bstrPassword, BSTR bstrRSN );//[id(10)]

	virtual void StopRouter(  );//[id(11)]

	virtual void RefreshRouter(  );//[id(12)]

	virtual void StartRouter(  );//[id(13)]

	virtual void Reserved1(  );//[id(14)]

	virtual void Reserved2(  );//[id(15)]

	virtual void InstallMultipleComponents( BSTR bstrApplIdOrName, SAFEARRAY(VARIANT)* varFileNames, SAFEARRAY(VARIANT)* varCLSIDS );//[id(16)]

	virtual void GetMultipleComponentsInfo( BSTR bstrApplIdOrName, SAFEARRAY(VARIANT)* varFileNames, SAFEARRAY(VARIANT)* varCLSIDS, SAFEARRAY(VARIANT)* varClassNames, SAFEARRAY(VARIANT)* varFileFlags, SAFEARRAY(VARIANT)* varComponentFlags );//[id(17)]

	virtual void RefreshComponents(  );//[id(18)]

	virtual void BackupREGDB( BSTR bstrBackupFilePath );//[id(19)]

	virtual void RestoreREGDB( BSTR bstrBackupFilePath );//[id(20)]

	virtual void QueryApplicationFile( BSTR bstrApplicationFile, BSTR* bstrApplicationName, BSTR* bstrApplicationDescription, boolean* bHasUsers, boolean* bIsProxy, SAFEARRAY(VARIANT)* varFileNames );//[id(21)]

	virtual void StartApplication( BSTR bstrApplIdOrName );//[id(22)]

	virtual long ServiceCheck( long lService );//[id(23)]

	virtual void InstallMultipleEventClasses( BSTR bstrApplIdOrName, SAFEARRAY(VARIANT)* varFileNames, SAFEARRAY(VARIANT)* varCLSIDS );//[id(24)]

	virtual void InstallEventClass( BSTR bstrApplIdOrName, BSTR bstrDLL, BSTR bstrTLB, BSTR bstrPSDLL );//[id(25)]

	virtual void GetEventClassesForIID( BSTR bstrIID, SAFEARRAY(VARIANT)* varCLSIDS, SAFEARRAY(VARIANT)* varProgIDs, SAFEARRAY(VARIANT)* varDescriptions );//[id(26)]

};

}  //end of namespace COMAdmin

#endif //Adding method 32 of 33 for interface ICOMAdminCatalog

