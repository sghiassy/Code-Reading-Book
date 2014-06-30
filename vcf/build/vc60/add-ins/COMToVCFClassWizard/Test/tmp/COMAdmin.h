//COMAdmin.h

#ifndef _COMADMIN_H__
#define _COMADMIN_H__

#include "FoundationKit.h"

namespace COMAdmin  {

//Typelibrary typedefs

	enum COMAdminApplicationInstallOptions {
		 COMAdminInstallNoUsers = 0,
		 COMAdminInstallUsers = 1,
		 COMAdminInstallForceOverwriteOfFiles = 2
	};
	enum COMAdminApplicationExportOptions {
		 COMAdminExportNoUsers = 0,
		 COMAdminExportUsers = 1,
		 COMAdminExportApplicationProxy = 2,
		 COMAdminExportForceOverwriteOfFiles = 4
	};
	enum COMAdminThreadingModels {
		 COMAdminThreadingModelApartment = 0,
		 COMAdminThreadingModelFree = 1,
		 COMAdminThreadingModelMain = 2,
		 COMAdminThreadingModelBoth = 3,
		 COMAdminThreadingModelNeutral = 4,
		 COMAdminThreadingModelNotSpecified = 5
	};
	enum COMAdminTransactionOptions {
		 COMAdminTransactionIgnored = 0,
		 COMAdminTransactionNone = 1,
		 COMAdminTransactionSupported = 2,
		 COMAdminTransactionRequired = 3,
		 COMAdminTransactionRequiresNew = 4
	};
	enum COMAdminSynchronizationOptions {
		 COMAdminSynchronizationIgnored = 0,
		 COMAdminSynchronizationNone = 1,
		 COMAdminSynchronizationSupported = 2,
		 COMAdminSynchronizationRequired = 3,
		 COMAdminSynchronizationRequiresNew = 4
	};
	enum COMAdminActivationOptions {
		 COMAdminActivationInproc = 0,
		 COMAdminActivationLocal = 1
	};
	enum COMAdminAccessChecksLevelOptions {
		 COMAdminAccessChecksApplicationLevel = 0,
		 COMAdminAccessChecksApplicationComponentLevel = 1
	};
	enum COMAdminAuthenticationLevelOptions {
		 COMAdminAuthenticationDefault = 0,
		 COMAdminAuthenticationNone = 1,
		 COMAdminAuthenticationConnect = 2,
		 COMAdminAuthenticationCall = 3,
		 COMAdminAuthenticationPacket = 4,
		 COMAdminAuthenticationIntegrity = 5,
		 COMAdminAuthenticationPrivacy = 6
	};
	enum COMAdminImpersonationLevelOptions {
		 COMAdminImpersonationAnonymous = 1,
		 COMAdminImpersonationIdentify = 2,
		 COMAdminImpersonationImpersonate = 3,
		 COMAdminImpersonationDelegate = 4
	};
	enum COMAdminAuthenticationCapabilitiesOptions {
		 COMAdminAuthenticationCapabilitiesNone = 0,
		 COMAdminAuthenticationCapabilitiesStaticCloaking = 32,
		 COMAdminAuthenticationCapabilitiesDynamicCloaking = 64,
		 COMAdminAuthenticationCapabilitiesSecureReference = 2
	};
	enum COMAdminOS {
		 COMAdminOSWindows3_1 = 1,
		 COMAdminOSWindows9x = 2,
		 COMAdminOSWindowsNT = 3,
		 COMAdminOSWindowsNTEnterprise = 4
	};
	enum COMAdminServiceOptions {
		 COMAdminServiceLoadBalanceRouter = 1
	};
	enum COMAdminServiceStatusOptions {
		 COMAdminServiceStopped = 0,
		 COMAdminServiceStartPending = 1,
		 COMAdminServiceStopPending = 2,
		 COMAdminServiceRunning = 3,
		 COMAdminServiceContinuePending = 4,
		 COMAdminServicePausePending = 5,
		 COMAdminServicePaused = 6,
		 COMAdminServiceUnknownState = 7
	};
	enum COMAdminFileFlags {
		 COMAdminFileFlagLoadable = 1,
		 COMAdminFileFlagCOM = 2,
		 COMAdminFileFlagContainsPS = 4,
		 COMAdminFileFlagContainsComp = 8,
		 COMAdminFileFlagContainsTLB = 16,
		 COMAdminFileFlagSelfReg = 32,
		 COMAdminFileFlagSelfUnReg = 64,
		 COMAdminFileFlagUnloadableDLL = 128,
		 COMAdminFileFlagDoesNotExist = 256,
		 COMAdminFileFlagAlreadyInstalled = 512,
		 COMAdminFileFlagBadTLB = 1024,
		 COMAdminFileFlagGetClassObjFailed = 2048,
		 COMAdminFileFlagClassNotAvailable = 4096,
		 COMAdminFileFlagRegistrar = 8192,
		 COMAdminFileFlagNoRegistrar = 16384,
		 COMAdminFileFlagDLLRegsvrFailed = 32768,
		 COMAdminFileFlagRegTLBFailed = 65536,
		 COMAdminFileFlagRegistrarFailed = 131072,
		 COMAdminFileFlagError = 262144
	};
	enum COMAdminComponentFlags {
		 COMAdminCompFlagTypeInfoFound = 1,
		 COMAdminCompFlagCOMPlusPropertiesFound = 2,
		 COMAdminCompFlagProxyFound = 4,
		 COMAdminCompFlagInterfacesFound = 8,
		 COMAdminCompFlagAlreadyInstalled = 16,
		 COMAdminCompFlagNotInApplication = 32
	};
	enum COMAdminErrorCodes {
		 COMAdminErrObjectErrors = -2146368511,
		 COMAdminErrObjectInvalid = -2146368510,
		 COMAdminErrKeyMissing = -2146368509,
		 COMAdminErrAlreadyInstalled = -2146368508,
		 COMAdminErrAppFileWriteFail = -2146368505,
		 COMAdminErrAppFileReadFail = -2146368504,
		 COMAdminErrAppFileVersion = -2146368503,
		 COMAdminErrBadPath = -2146368502,
		 COMAdminErrApplicationExists = -2146368501,
		 COMAdminErrRoleExists = -2146368500,
		 COMAdminErrCantCopyFile = -2146368499,
		 COMAdminErrNoUser = -2146368497,
		 COMAdminErrInvalidUserids = -2146368496,
		 COMAdminErrNoRegistryCLSID = -2146368495,
		 COMAdminErrBadRegistryProgID = -2146368494,
		 COMAdminErrAuthenticationLevel = -2146368493,
		 COMAdminErrUserPasswdNotValid = -2146368492,
		 COMAdminErrCLSIDOrIIDMismatch = -2146368488,
		 COMAdminErrRemoteInterface = -2146368487,
		 COMAdminErrDllRegisterServer = -2146368486,
		 COMAdminErrNoServerShare = -2146368485,
		 COMAdminErrDllLoadFailed = -2146368483,
		 COMAdminErrBadRegistryLibID = -2146368482,
		 COMAdminErrAppDirNotFound = -2146368481,
		 COMAdminErrRegistrarFailed = -2146368477,
		 COMAdminErrCompFileDoesNotExist = -2146368476,
		 COMAdminErrCompFileLoadDLLFail = -2146368475,
		 COMAdminErrCompFileGetClassObj = -2146368474,
		 COMAdminErrCompFileClassNotAvail = -2146368473,
		 COMAdminErrCompFileBadTLB = -2146368472,
		 COMAdminErrCompFileNotInstallable = -2146368471,
		 COMAdminErrNotChangeable = -2146368470,
		 COMAdminErrNotDeletable = -2146368469,
		 COMAdminErrSession = -2146368468,
		 COMAdminErrCompMoveLocked = -2146368467,
		 COMAdminErrCompMoveBadDest = -2146368466,
		 COMAdminErrRegisterTLB = -2146368464,
		 COMAdminErrSystemApp = -2146368461,
		 COMAdminErrCompFileNoRegistrar = -2146368460,
		 COMAdminErrCoReqCompInstalled = -2146368459,
		 COMAdminErrServiceNotInstalled = -2146368458,
		 COMAdminErrPropertySaveFailed = -2146368457,
		 COMAdminErrObjectExists = -2146368456,
		 COMAdminErrRegFileCorrupt = -2146368453,
		 COMAdminErrPropertyOverflow = -2146368452,
		 COMAdminErrNotInRegistry = -2146368450,
		 COMAdminErrObjectNotPoolable = -2146368449,
		 COMAdminErrApplidMatchesClsid = -2146368442,
		 COMAdminErrRoleDoesNotExist = -2146368441,
		 COMAdminErrStartAppNeedsComponents = -2146368440,
		 COMAdminErrRequiresDifferentPlatform = -2146368439,
		 COMAdminErrQueuingServiceNotAvailable = -2146367998,
		 COMAdminErrObjectParentMissing = -2146367480,
		 COMAdminErrObjectDoesNotExist = -2146367479,
		 COMAdminErrCanNotExportAppProxy = -2146368438,
		 COMAdminErrCanNotStartApp = -2146368437,
		 COMAdminErrCanNotExportSystemApp = -2146368436,
		 COMAdminErrCanNotSubscribeToComponent = -2146368435
	};

//End of Typelibrary typedefs

}  //end of namespace COMAdmin

#endif //_COMADMIN_H__

