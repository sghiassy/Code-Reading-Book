// TypelibDump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <comdef.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include "TypelibDump.h"
#include "TypeLibraryConverterDlg.h"


// Stolen from OLEMISC.CPP in the MFC 3.0 source.  Function names
// changed from Afx* to _*.
//
#define _countof(array) (sizeof(array)/sizeof(array[0]))

LPCTSTR _GetScodeString(SCODE sc)
{
	struct SCODE_ENTRY
	{
		SCODE sc;
		LPCTSTR lpszName;
	};
	#define MAKE_SCODE_ENTRY(sc)    { sc, _T(#sc) }
	static const SCODE_ENTRY scNameTable[] =
	{
		MAKE_SCODE_ENTRY(S_OK),
		MAKE_SCODE_ENTRY(S_FALSE),

		MAKE_SCODE_ENTRY(CACHE_S_FORMATETC_NOTSUPPORTED),
		MAKE_SCODE_ENTRY(CACHE_S_SAMECACHE),
		MAKE_SCODE_ENTRY(CACHE_S_SOMECACHES_NOTUPDATED),
		MAKE_SCODE_ENTRY(CONVERT10_S_NO_PRESENTATION),
		MAKE_SCODE_ENTRY(DATA_S_SAMEFORMATETC),
		MAKE_SCODE_ENTRY(DRAGDROP_S_CANCEL),
		MAKE_SCODE_ENTRY(DRAGDROP_S_DROP),
		MAKE_SCODE_ENTRY(DRAGDROP_S_USEDEFAULTCURSORS),
		MAKE_SCODE_ENTRY(INPLACE_S_TRUNCATED),
		MAKE_SCODE_ENTRY(MK_S_HIM),
		MAKE_SCODE_ENTRY(MK_S_ME),
		MAKE_SCODE_ENTRY(MK_S_MONIKERALREADYREGISTERED),
		MAKE_SCODE_ENTRY(MK_S_REDUCED_TO_SELF),
		MAKE_SCODE_ENTRY(MK_S_US),
		MAKE_SCODE_ENTRY(OLE_S_MAC_CLIPFORMAT),
		MAKE_SCODE_ENTRY(OLE_S_STATIC),
		MAKE_SCODE_ENTRY(OLE_S_USEREG),
		MAKE_SCODE_ENTRY(OLEOBJ_S_CANNOT_DOVERB_NOW),
		MAKE_SCODE_ENTRY(OLEOBJ_S_INVALIDHWND),
		MAKE_SCODE_ENTRY(OLEOBJ_S_INVALIDVERB),
		MAKE_SCODE_ENTRY(OLEOBJ_S_LAST),
		MAKE_SCODE_ENTRY(STG_S_CONVERTED),
		MAKE_SCODE_ENTRY(VIEW_S_ALREADY_FROZEN),

		MAKE_SCODE_ENTRY(E_UNEXPECTED),
		MAKE_SCODE_ENTRY(E_NOTIMPL),
		MAKE_SCODE_ENTRY(E_OUTOFMEMORY),
		MAKE_SCODE_ENTRY(E_INVALIDARG),
		MAKE_SCODE_ENTRY(E_NOINTERFACE),
		MAKE_SCODE_ENTRY(E_POINTER),
		MAKE_SCODE_ENTRY(E_HANDLE),
		MAKE_SCODE_ENTRY(E_ABORT),
		MAKE_SCODE_ENTRY(E_FAIL),
		MAKE_SCODE_ENTRY(E_ACCESSDENIED),

		MAKE_SCODE_ENTRY(CACHE_E_NOCACHE_UPDATED),
		MAKE_SCODE_ENTRY(CLASS_E_CLASSNOTAVAILABLE),
		MAKE_SCODE_ENTRY(CLASS_E_NOAGGREGATION),
		MAKE_SCODE_ENTRY(CLIPBRD_E_BAD_DATA),
		MAKE_SCODE_ENTRY(CLIPBRD_E_CANT_CLOSE),
		MAKE_SCODE_ENTRY(CLIPBRD_E_CANT_EMPTY),
		MAKE_SCODE_ENTRY(CLIPBRD_E_CANT_OPEN),
		MAKE_SCODE_ENTRY(CLIPBRD_E_CANT_SET),
		MAKE_SCODE_ENTRY(CO_E_ALREADYINITIALIZED),
		MAKE_SCODE_ENTRY(CO_E_APPDIDNTREG),
		MAKE_SCODE_ENTRY(CO_E_APPNOTFOUND),
		MAKE_SCODE_ENTRY(CO_E_APPSINGLEUSE),
		MAKE_SCODE_ENTRY(CO_E_BAD_PATH),
		MAKE_SCODE_ENTRY(CO_E_CANTDETERMINECLASS),
		MAKE_SCODE_ENTRY(CO_E_CLASS_CREATE_FAILED),
		MAKE_SCODE_ENTRY(CO_E_CLASSSTRING),
		MAKE_SCODE_ENTRY(CO_E_DLLNOTFOUND),
		MAKE_SCODE_ENTRY(CO_E_ERRORINAPP),
		MAKE_SCODE_ENTRY(CO_E_ERRORINDLL),
		MAKE_SCODE_ENTRY(CO_E_IIDSTRING),
		MAKE_SCODE_ENTRY(CO_E_NOTINITIALIZED),
		MAKE_SCODE_ENTRY(CO_E_OBJISREG),
		MAKE_SCODE_ENTRY(CO_E_OBJNOTCONNECTED),
		MAKE_SCODE_ENTRY(CO_E_OBJNOTREG),
		MAKE_SCODE_ENTRY(CO_E_OBJSRV_RPC_FAILURE),
		MAKE_SCODE_ENTRY(CO_E_SCM_ERROR),
		MAKE_SCODE_ENTRY(CO_E_SCM_RPC_FAILURE),
		MAKE_SCODE_ENTRY(CO_E_SERVER_EXEC_FAILURE),
		MAKE_SCODE_ENTRY(CO_E_SERVER_STOPPING),
		MAKE_SCODE_ENTRY(CO_E_WRONGOSFORAPP),
		MAKE_SCODE_ENTRY(CONVERT10_E_OLESTREAM_BITMAP_TO_DIB),
		MAKE_SCODE_ENTRY(CONVERT10_E_OLESTREAM_FMT),
		MAKE_SCODE_ENTRY(CONVERT10_E_OLESTREAM_GET),
		MAKE_SCODE_ENTRY(CONVERT10_E_OLESTREAM_PUT),
		MAKE_SCODE_ENTRY(CONVERT10_E_STG_DIB_TO_BITMAP),
		MAKE_SCODE_ENTRY(CONVERT10_E_STG_FMT),
		MAKE_SCODE_ENTRY(CONVERT10_E_STG_NO_STD_STREAM),
		MAKE_SCODE_ENTRY(DISP_E_ARRAYISLOCKED),
		MAKE_SCODE_ENTRY(DISP_E_BADCALLEE),
		MAKE_SCODE_ENTRY(DISP_E_BADINDEX),
		MAKE_SCODE_ENTRY(DISP_E_BADPARAMCOUNT),
		MAKE_SCODE_ENTRY(DISP_E_BADVARTYPE),
		MAKE_SCODE_ENTRY(DISP_E_EXCEPTION),
		MAKE_SCODE_ENTRY(DISP_E_MEMBERNOTFOUND),
		MAKE_SCODE_ENTRY(DISP_E_NONAMEDARGS),
		MAKE_SCODE_ENTRY(DISP_E_NOTACOLLECTION),
		MAKE_SCODE_ENTRY(DISP_E_OVERFLOW),
		MAKE_SCODE_ENTRY(DISP_E_PARAMNOTFOUND),
		MAKE_SCODE_ENTRY(DISP_E_PARAMNOTOPTIONAL),
		MAKE_SCODE_ENTRY(DISP_E_TYPEMISMATCH),
		MAKE_SCODE_ENTRY(DISP_E_UNKNOWNINTERFACE),
		MAKE_SCODE_ENTRY(DISP_E_UNKNOWNLCID),
		MAKE_SCODE_ENTRY(DISP_E_UNKNOWNNAME),
		MAKE_SCODE_ENTRY(DRAGDROP_E_ALREADYREGISTERED),
		MAKE_SCODE_ENTRY(DRAGDROP_E_INVALIDHWND),
		MAKE_SCODE_ENTRY(DRAGDROP_E_NOTREGISTERED),
		MAKE_SCODE_ENTRY(DV_E_CLIPFORMAT),
		MAKE_SCODE_ENTRY(DV_E_DVASPECT),
		MAKE_SCODE_ENTRY(DV_E_DVTARGETDEVICE),
		MAKE_SCODE_ENTRY(DV_E_DVTARGETDEVICE_SIZE),
		MAKE_SCODE_ENTRY(DV_E_FORMATETC),
		MAKE_SCODE_ENTRY(DV_E_LINDEX),
		MAKE_SCODE_ENTRY(DV_E_NOIVIEWOBJECT),
		MAKE_SCODE_ENTRY(DV_E_STATDATA),
		MAKE_SCODE_ENTRY(DV_E_STGMEDIUM),
		MAKE_SCODE_ENTRY(DV_E_TYMED),
		MAKE_SCODE_ENTRY(INPLACE_E_NOTOOLSPACE),
		MAKE_SCODE_ENTRY(INPLACE_E_NOTUNDOABLE),
		MAKE_SCODE_ENTRY(MEM_E_INVALID_LINK),
		MAKE_SCODE_ENTRY(MEM_E_INVALID_ROOT),
		MAKE_SCODE_ENTRY(MEM_E_INVALID_SIZE),
		MAKE_SCODE_ENTRY(MK_E_CANTOPENFILE),
		MAKE_SCODE_ENTRY(MK_E_CONNECTMANUALLY),
		MAKE_SCODE_ENTRY(MK_E_ENUMERATION_FAILED),
		MAKE_SCODE_ENTRY(MK_E_EXCEEDEDDEADLINE),
		MAKE_SCODE_ENTRY(MK_E_INTERMEDIATEINTERFACENOTSUPPORTED),
		MAKE_SCODE_ENTRY(MK_E_INVALIDEXTENSION),
		MAKE_SCODE_ENTRY(MK_E_MUSTBOTHERUSER),
		MAKE_SCODE_ENTRY(MK_E_NEEDGENERIC),
		MAKE_SCODE_ENTRY(MK_E_NO_NORMALIZED),
		MAKE_SCODE_ENTRY(MK_E_NOINVERSE),
		MAKE_SCODE_ENTRY(MK_E_NOOBJECT),
		MAKE_SCODE_ENTRY(MK_E_NOPREFIX),
		MAKE_SCODE_ENTRY(MK_E_NOSTORAGE),
		MAKE_SCODE_ENTRY(MK_E_NOTBINDABLE),
		MAKE_SCODE_ENTRY(MK_E_NOTBOUND),
		MAKE_SCODE_ENTRY(MK_E_SYNTAX),
		MAKE_SCODE_ENTRY(MK_E_UNAVAILABLE),
		MAKE_SCODE_ENTRY(OLE_E_ADVF),
		MAKE_SCODE_ENTRY(OLE_E_ADVISENOTSUPPORTED),
		MAKE_SCODE_ENTRY(OLE_E_BLANK),
		MAKE_SCODE_ENTRY(OLE_E_CANT_BINDTOSOURCE),
		MAKE_SCODE_ENTRY(OLE_E_CANT_GETMONIKER),
		MAKE_SCODE_ENTRY(OLE_E_CANTCONVERT),
		MAKE_SCODE_ENTRY(OLE_E_CLASSDIFF),
		MAKE_SCODE_ENTRY(OLE_E_ENUM_NOMORE),
		MAKE_SCODE_ENTRY(OLE_E_INVALIDHWND),
		MAKE_SCODE_ENTRY(OLE_E_INVALIDRECT),
		MAKE_SCODE_ENTRY(OLE_E_NOCACHE),
		MAKE_SCODE_ENTRY(OLE_E_NOCONNECTION),
		MAKE_SCODE_ENTRY(OLE_E_NOSTORAGE),
		MAKE_SCODE_ENTRY(OLE_E_NOT_INPLACEACTIVE),
		MAKE_SCODE_ENTRY(OLE_E_NOTRUNNING),
		MAKE_SCODE_ENTRY(OLE_E_OLEVERB),
		MAKE_SCODE_ENTRY(OLE_E_PROMPTSAVECANCELLED),
		MAKE_SCODE_ENTRY(OLE_E_STATIC),
		MAKE_SCODE_ENTRY(OLE_E_WRONGCOMPOBJ),
		MAKE_SCODE_ENTRY(OLEOBJ_E_INVALIDVERB),
		MAKE_SCODE_ENTRY(OLEOBJ_E_NOVERBS),
		MAKE_SCODE_ENTRY(REGDB_E_CLASSNOTREG),
		MAKE_SCODE_ENTRY(REGDB_E_IIDNOTREG),
		MAKE_SCODE_ENTRY(REGDB_E_INVALIDVALUE),
		MAKE_SCODE_ENTRY(REGDB_E_KEYMISSING),
		MAKE_SCODE_ENTRY(REGDB_E_READREGDB),
		MAKE_SCODE_ENTRY(REGDB_E_WRITEREGDB),
		MAKE_SCODE_ENTRY(RPC_E_ATTEMPTED_MULTITHREAD),
		MAKE_SCODE_ENTRY(RPC_E_CALL_CANCELED),
		MAKE_SCODE_ENTRY(RPC_E_CALL_REJECTED),
		MAKE_SCODE_ENTRY(RPC_E_CANTCALLOUT_AGAIN),
		MAKE_SCODE_ENTRY(RPC_E_CANTCALLOUT_INASYNCCALL),
		MAKE_SCODE_ENTRY(RPC_E_CANTCALLOUT_INEXTERNALCALL),
		MAKE_SCODE_ENTRY(RPC_E_CANTCALLOUT_ININPUTSYNCCALL),
		MAKE_SCODE_ENTRY(RPC_E_CANTPOST_INSENDCALL),
		MAKE_SCODE_ENTRY(RPC_E_CANTTRANSMIT_CALL),
		MAKE_SCODE_ENTRY(RPC_E_CHANGED_MODE),
		MAKE_SCODE_ENTRY(RPC_E_CLIENT_CANTMARSHAL_DATA),
		MAKE_SCODE_ENTRY(RPC_E_CLIENT_CANTUNMARSHAL_DATA),
		MAKE_SCODE_ENTRY(RPC_E_CLIENT_DIED),
		MAKE_SCODE_ENTRY(RPC_E_CONNECTION_TERMINATED),
		MAKE_SCODE_ENTRY(RPC_E_DISCONNECTED),
		MAKE_SCODE_ENTRY(RPC_E_FAULT),
		MAKE_SCODE_ENTRY(RPC_E_INVALID_CALLDATA),
		MAKE_SCODE_ENTRY(RPC_E_INVALID_DATA),
		MAKE_SCODE_ENTRY(RPC_E_INVALID_DATAPACKET),
		MAKE_SCODE_ENTRY(RPC_E_INVALID_PARAMETER),
		MAKE_SCODE_ENTRY(RPC_E_INVALIDMETHOD),
		MAKE_SCODE_ENTRY(RPC_E_NOT_REGISTERED),
		MAKE_SCODE_ENTRY(RPC_E_OUT_OF_RESOURCES),
		MAKE_SCODE_ENTRY(RPC_E_RETRY),
		MAKE_SCODE_ENTRY(RPC_E_SERVER_CANTMARSHAL_DATA),
		MAKE_SCODE_ENTRY(RPC_E_SERVER_CANTUNMARSHAL_DATA),
		MAKE_SCODE_ENTRY(RPC_E_SERVER_DIED),
		MAKE_SCODE_ENTRY(RPC_E_SERVER_DIED_DNE),
		MAKE_SCODE_ENTRY(RPC_E_SERVERCALL_REJECTED),
		MAKE_SCODE_ENTRY(RPC_E_SERVERCALL_RETRYLATER),
		MAKE_SCODE_ENTRY(RPC_E_SERVERFAULT),
		MAKE_SCODE_ENTRY(RPC_E_SYS_CALL_FAILED),
		MAKE_SCODE_ENTRY(RPC_E_THREAD_NOT_INIT),
		MAKE_SCODE_ENTRY(RPC_E_UNEXPECTED),
		MAKE_SCODE_ENTRY(RPC_E_WRONG_THREAD),
		MAKE_SCODE_ENTRY(STG_E_ABNORMALAPIEXIT),
		MAKE_SCODE_ENTRY(STG_E_ACCESSDENIED),
		MAKE_SCODE_ENTRY(STG_E_CANTSAVE),
		MAKE_SCODE_ENTRY(STG_E_DISKISWRITEPROTECTED),
		MAKE_SCODE_ENTRY(STG_E_EXTANTMARSHALLINGS),
		MAKE_SCODE_ENTRY(STG_E_FILEALREADYEXISTS),
		MAKE_SCODE_ENTRY(STG_E_FILENOTFOUND),
		MAKE_SCODE_ENTRY(STG_E_INSUFFICIENTMEMORY),
		MAKE_SCODE_ENTRY(STG_E_INUSE),
		MAKE_SCODE_ENTRY(STG_E_INVALIDFLAG),
		MAKE_SCODE_ENTRY(STG_E_INVALIDFUNCTION),
		MAKE_SCODE_ENTRY(STG_E_INVALIDHANDLE),
		MAKE_SCODE_ENTRY(STG_E_INVALIDHEADER),
		MAKE_SCODE_ENTRY(STG_E_INVALIDNAME),
		MAKE_SCODE_ENTRY(STG_E_INVALIDPARAMETER),
		MAKE_SCODE_ENTRY(STG_E_INVALIDPOINTER),
		MAKE_SCODE_ENTRY(STG_E_LOCKVIOLATION),
		MAKE_SCODE_ENTRY(STG_E_MEDIUMFULL),
		MAKE_SCODE_ENTRY(STG_E_NOMOREFILES),
		MAKE_SCODE_ENTRY(STG_E_NOTCURRENT),
		MAKE_SCODE_ENTRY(STG_E_NOTFILEBASEDSTORAGE),
		MAKE_SCODE_ENTRY(STG_E_OLDDLL),
		MAKE_SCODE_ENTRY(STG_E_OLDFORMAT),
		MAKE_SCODE_ENTRY(STG_E_PATHNOTFOUND),
		MAKE_SCODE_ENTRY(STG_E_READFAULT),
		MAKE_SCODE_ENTRY(STG_E_REVERTED),
		MAKE_SCODE_ENTRY(STG_E_SEEKERROR),
		MAKE_SCODE_ENTRY(STG_E_SHAREREQUIRED),
		MAKE_SCODE_ENTRY(STG_E_SHAREVIOLATION),
		MAKE_SCODE_ENTRY(STG_E_TOOMANYOPENFILES),
		MAKE_SCODE_ENTRY(STG_E_UNIMPLEMENTEDFUNCTION),
		MAKE_SCODE_ENTRY(STG_E_UNKNOWN),
		MAKE_SCODE_ENTRY(STG_E_WRITEFAULT),
		MAKE_SCODE_ENTRY(TYPE_E_AMBIGUOUSNAME),
		MAKE_SCODE_ENTRY(TYPE_E_BADMODULEKIND),
		MAKE_SCODE_ENTRY(TYPE_E_BUFFERTOOSMALL),
		MAKE_SCODE_ENTRY(TYPE_E_CANTCREATETMPFILE),
		MAKE_SCODE_ENTRY(TYPE_E_CANTLOADLIBRARY),
		MAKE_SCODE_ENTRY(TYPE_E_CIRCULARTYPE),
		MAKE_SCODE_ENTRY(TYPE_E_DLLFUNCTIONNOTFOUND),
		MAKE_SCODE_ENTRY(TYPE_E_DUPLICATEID),
		MAKE_SCODE_ENTRY(TYPE_E_ELEMENTNOTFOUND),
		MAKE_SCODE_ENTRY(TYPE_E_INCONSISTENTPROPFUNCS),
		MAKE_SCODE_ENTRY(TYPE_E_INVALIDSTATE),
		MAKE_SCODE_ENTRY(TYPE_E_INVDATAREAD),
		MAKE_SCODE_ENTRY(TYPE_E_IOERROR),
		MAKE_SCODE_ENTRY(TYPE_E_LIBNOTREGISTERED),
		MAKE_SCODE_ENTRY(TYPE_E_NAMECONFLICT),
		MAKE_SCODE_ENTRY(TYPE_E_OUTOFBOUNDS),
		MAKE_SCODE_ENTRY(TYPE_E_QUALIFIEDNAMEDISALLOWED),
		MAKE_SCODE_ENTRY(TYPE_E_REGISTRYACCESS),
		MAKE_SCODE_ENTRY(TYPE_E_SIZETOOBIG),
		MAKE_SCODE_ENTRY(TYPE_E_TYPEMISMATCH),
		MAKE_SCODE_ENTRY(TYPE_E_UNDEFINEDTYPE),
		MAKE_SCODE_ENTRY(TYPE_E_UNKNOWNLCID),
		MAKE_SCODE_ENTRY(TYPE_E_UNSUPFORMAT),
		MAKE_SCODE_ENTRY(TYPE_E_WRONGTYPEKIND),
		MAKE_SCODE_ENTRY(VIEW_E_DRAW),
	};
	#undef MAKE_SCODE_ENTRY

	// look for it in the table
	for (int i = 0; i < _countof(scNameTable); i++)
	{
		if (sc == scNameTable[i].sc)
			return scNameTable[i].lpszName;
	}

	static TCHAR szError[32] ;
	wsprintf( szError, _T("%#08.8x"), sc ) ;

	return szError ;    // not found
}

static TCHAR* g_rgszVT[] =
{
	_T("void"),             //VT_EMPTY           = 0,   /* [V]   [P]  nothing                     */
	_T("null"),             //VT_NULL            = 1,   /* [V]        SQL style Null              */
	_T("short"),            //VT_I2              = 2,   /* [V][T][P]  2 byte signed int           */
	_T("long"),             //VT_I4              = 3,   /* [V][T][P]  4 byte signed int           */
	_T("float"),           //VT_R4              = 4,   /* [V][T][P]  4 byte real                 */
	_T("double"),           //VT_R8              = 5,   /* [V][T][P]  8 byte real                 */
	_T("CURRENCY"),         //VT_CY              = 6,   /* [V][T][P]  currency                    */
	_T("DATE"),             //VT_DATE            = 7,   /* [V][T][P]  date                        */
	_T("BSTR"),             //VT_BSTR            = 8,   /* [V][T][P]  binary string               */
	_T("IDispatch*"),       //VT_DISPATCH        = 9,   /* [V][T]     IDispatch FAR*              */
	_T("SCODE"),            //VT_ERROR           = 10,  /* [V][T]     SCODE                       */
	_T("bool"),          //VT_BOOL            = 11,  /* [V][T][P]  True=-1, False=0            */
	_T("VARIANT"),          //VT_VARIANT         = 12,  /* [V][T][P]  VARIANT FAR*                */
	_T("IUnknown*"),        //VT_UNKNOWN         = 13,  /* [V][T]     IUnknown FAR*               */
	_T("wchar_t"),          //VT_WBSTR           = 14,  /* [V][T]     wide binary string          */
	_T(""),                 //                   = 15,
	_T("char"),             //VT_I1              = 16,  /*    [T]     signed char                 */
	_T("unsigned char"),             //VT_UI1             = 17,  /*    [T]     unsigned char               */
	_T("unsigned short"),           //VT_UI2             = 18,  /*    [T]     unsigned short              */
	_T("unsigned long"),            //VT_UI4             = 19,  /*    [T]     unsigned short              */
	_T("int64"),            //VT_I8              = 20,  /*    [T][P]  signed 64-bit int           */
	_T("uint64"),           //VT_UI8             = 21,  /*    [T]     unsigned 64-bit int         */
	_T("int"),              //VT_INT             = 22,  /*    [T]     signed machine int          */
	_T("unsigned int"),             //VT_UINT            = 23,  /*    [T]     unsigned machine int        */
	_T("void"),             //VT_VOID            = 24,  /*    [T]     C style void                */
	_T("HRESULT"),          //VT_HRESULT         = 25,  /*    [T]                                 */
	_T("PTR"),              //VT_PTR             = 26,  /*    [T]     pointer type                */
	_T("SAFEARRAY"),        //VT_SAFEARRAY       = 27,  /*    [T]     (use VT_ARRAY in VARIANT)   */
	_T("CARRAY"),           //VT_CARRAY          = 28,  /*    [T]     C style array               */
	_T("USERDEFINED"),      //VT_USERDEFINED     = 29,  /*    [T]     user defined type         */
	_T("String"),            //VT_LPSTR           = 30,  /*    [T][P]  null terminated string      */
	_T("String"),           //VT_LPWSTR          = 31,  /*    [T][P]  wide null terminated string */
	_T(""),                 //                   = 32,
	_T(""),                 //                   = 33,
	_T(""),                 //                   = 34,
	_T(""),                 //                   = 35,
	_T(""),                 //                   = 36,
	_T(""),                 //                   = 37,
	_T(""),                 //                   = 38,
	_T(""),                 //                   = 39,
	_T(""),                 //                   = 40,
	_T(""),                 //                   = 41,
	_T(""),                 //                   = 42,
	_T(""),                 //                   = 43,
	_T(""),                 //                   = 44,
	_T(""),                 //                   = 45,
	_T(""),                 //                   = 46,
	_T(""),                 //                   = 47,
	_T(""),                 //                   = 48,
	_T(""),                 //                   = 49,
	_T(""),                 //                   = 50,
	_T(""),                 //                   = 51,
	_T(""),                 //                   = 52,
	_T(""),                 //                   = 53,
	_T(""),                 //                   = 54,
	_T(""),                 //                   = 55,
	_T(""),                 //                   = 56,
	_T(""),                 //                   = 57,
	_T(""),                 //                   = 58,
	_T(""),                 //                   = 59,
	_T(""),                 //                   = 60,
	_T(""),                 //                   = 61,
	_T(""),                 //                   = 62,
	_T(""),                 //                   = 63,
	_T("FILETIME"),         //VT_FILETIME        = 64,  /*       [P]  FILETIME                    */
	_T("BLOB"),             //VT_BLOB            = 65,  /*       [P]  Length prefixed bytes       */
	_T("STREAM"),           //VT_STREAM          = 66,  /*       [P]  Name of the stream follows  */
	_T("STORAGE"),          //VT_STORAGE         = 67,  /*       [P]  Name of the storage follows */
	_T("STREAMED_OBJECT"),  //VT_STREAMED_OBJECT = 68,  /*       [P]  Stream contains an object   */
	_T("STORED_OBJECT"),    //VT_STORED_OBJECT   = 69,  /*       [P]  Storage contains an object  */
	_T("BLOB_OBJECT"),      //VT_BLOB_OBJECT     = 70,  /*       [P]  Blob contains an object     */
	_T("CF"),               //VT_CF              = 71,  /*       [P]  Clipboard format            */
	_T("CLSID")            //VT_CLSID           = 72   /*       [P]  A Class ID                  */
};



int StringFromGUID2T(REFGUID rguid, LPTSTR lpszGUID, int cbMax )
{
	
	//OLECHAR* lpszOle = (OLECHAR*)_alloca(cbMax*sizeof(OLECHAR));
	_bstr_t lpszOle;

	int nCount = ::StringFromGUID2(rguid,lpszOle, cbMax*sizeof(OLECHAR));
	if (nCount == 0)
	{
		lpszGUID[0] = '\0';
		return 0; // buffer too small for the returned string
	}
	LPTSTR lpszRes = (LPTSTR)lpszOle;
	lstrcpy(lpszGUID, lpszRes);
	return 0;
}

BSTR VTtoString( VARTYPE vt )
{
	_bstr_t str("") ;
	vt &= ~0xF000 ;
	if (vt <= VT_CLSID) {
		str = g_rgszVT[vt]  ;
		CString tmp = (TCHAR*)str;
		bool isPointer = false;
		if ( tmp.GetAt(tmp.GetLength()-1) == '*' ) {
			isPointer = true;
			tmp.Delete(tmp.GetLength()-1,1); 
		}
		std::map<CString,CString>::iterator found = TypeLibraryConverterDlg::g_TypeConversionMap.find( tmp );
		if ( found != TypeLibraryConverterDlg::g_TypeConversionMap.end() ) {
			str = found->second;
			if ( true == isPointer ) {
				str += "*";
			}
		}
	}
	else
	   str = "BAD VARTYPE" ;

	return str.copy() ;
}

BSTR TYPEDESCtoString( ITypeInfo* pti, TYPEDESC* ptdesc )
{
	_bstr_t str("") ;

	if (ptdesc->vt == VT_PTR)
	{
		// ptdesc->lptdesc points to a TYPEDESC that specifies the thing pointed to

		str = TYPEDESCtoString( pti, ptdesc->lptdesc ) ;
		str += "*" ;
		return str.copy();
	}

	if ((ptdesc->vt & 0x0FFF) == VT_CARRAY)
	{
		// ptdesc->lpadesc points to an ARRAYDESC
		str = TYPEDESCtoString( pti, &ptdesc->lpadesc->tdescElem ) ;

		// Allocate cDims * lstrlen("[123456]")
		
		TCHAR strTemp[256];
		memset(strTemp,0,256);
		for (USHORT n = 0 ; n < ptdesc->lpadesc->cDims ; n++)
		{
			sprintf( strTemp, _T("[%d]"), ptdesc->lpadesc->rgbounds[n].cElements ) ;
			str += strTemp ;
		}
		return str.copy() ;
	}

	if ((ptdesc->vt & 0x0FFF) == VT_SAFEARRAY)
	{
		str = _bstr_t("SAFEARRAY(") + TYPEDESCtoString( pti, ptdesc->lptdesc ) + _bstr_t(")") ;
		return str.copy() ;
	}

	if (ptdesc->vt == VT_USERDEFINED)
	{
		// Use ptdesc->hreftype and pti->GetRefTypeInfo
		//
		assert(pti) ;
		ITypeInfo* ptiRefType = NULL ;
		HRESULT hr = pti->GetRefTypeInfo( ptdesc->hreftype, &ptiRefType ) ;
		if (SUCCEEDED(hr))
		{
			BSTR            bstrName = NULL ;
			BSTR            bstrDoc = NULL ;
			BSTR            bstrHelp = NULL ;
			DWORD           dwHelpID ;
			hr = ptiRefType->GetDocumentation( MEMBERID_NIL, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp ) ;
			if (FAILED(hr))
			{
				#ifdef _DEBUG
				//ErrorMessage(_T("ITypeInfo::GetDocumentation() failed"), hr ) ;
				#endif
				return _bstr_t(_T("ITypeInfo::GetDocumentation() failed in TYPEDESCToString")).copy() ;
			}

			str = bstrName ;

			SysFreeString(bstrName) ;
			SysFreeString( bstrDoc ) ;
			SysFreeString( bstrHelp ) ;

			ptiRefType->Release() ;
		}
		else
		{
			#ifdef _DEBUG
			//ErrorMessage( _T("GetRefTypeInfo failed in TYPEDESCToString"), hr ) ;
			#endif
			return _bstr_t(_T("<GetRefTypeInfo failed>")).copy() ;
		}

		return str.copy();
	}
	return VTtoString( ptdesc->vt );
}


typedef enum {
		typeUnknown         ,
		typeUnknown2        ,
		typeTypeLib         ,
		typeTypeLib2        ,
		typeTypeInfo        ,
		typeTypeInfo2       ,
		typeMethods         ,
		typeMethods2        ,
		typeProperties      ,
		typeProperties2     ,
		typeConstants       ,
		typeConstants2      ,
		typeImplTypes       ,
		typeImplTypes2      ,
		typeMethod          ,
		typeProperty        ,
		typeConstant        ,
		typeEnum            ,       // TKIND_ENUM
		typeRecord          ,       // TKIND_RECORD
		typeModule          ,       // TKIND_MODULE
		typeInterface       ,       // TKIND_INTERFACE
		typeDispinterface   ,       // TKIND_DISPATCH
		typeCoClass         ,       // TKIND_COCLASS
		typeAlias           ,       // TKIND_ALIAS
		typeUnion           ,       // TKIND_UNION
		typeEnums           ,
		typeRecords         ,
		typeModules         ,
		typeInterfaces      ,
		typeDispinterfaces  ,
		typeCoClasses       ,
		typeAliases         ,
		typeUnions          ,
	} ITEM_TYPE ;



#define MAX_NAMES   64     // max parameters to a function

//_COM_SMARTPTR_TYPEDEF(IMyInterface, __uuidof(IMyInterface));

BSTR GenerateStructEnumUnion(  ITypeInfo* pEnumTypeInfo, long indent )
{
	_bstr_t typeDefs;
	
	HRESULT hr = E_FAIL;

	_bstr_t indentStr = "";
	for ( int i=0;i<indent;i++){
		indentStr += "\t";
	}	
	
	TYPEATTR*   pEnumAttr = NULL ;
	hr = pEnumTypeInfo->GetTypeAttr( &pEnumAttr);
	
	switch ( pEnumAttr->typekind ) {
		case TKIND_ENUM:case TKIND_RECORD: case TKIND_UNION:{
			BSTR            bstrName = NULL ;
			BSTR            bstrDoc = NULL ;
			BSTR            bstrHelp = NULL ;
			DWORD           dwHelpID ;
			hr = pEnumTypeInfo->GetDocumentation( MEMBERID_NIL, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp );
			
			switch ( pEnumAttr->typekind ) {
			case TKIND_RECORD: {
				typeDefs += indentStr;
				typeDefs += "struct ";
							   }
				break;
				
			case TKIND_UNION: {
				typeDefs += indentStr;
				typeDefs += "union ";
							  }
				break;
				
			case TKIND_ENUM: {
				typeDefs += indentStr;
				typeDefs += "enum ";
							 }
				break;
			}
			typeDefs += bstrName;
			
			SysFreeString(bstrName) ;
			SysFreeString( bstrDoc ) ;
			SysFreeString( bstrHelp ) ;
			
			typeDefs += " {\n";
			if ( (pEnumAttr->typekind == TKIND_RECORD) || (pEnumAttr->typekind == TKIND_UNION) ) {
				for (UINT ei = 0 ; ei < pEnumAttr->cVars ; ei++) {
					VARDESC*    pvardesc = NULL ;
					pEnumTypeInfo->GetVarDesc( ei, &pvardesc );
					pEnumTypeInfo->GetDocumentation( pvardesc->memid, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp );
					BSTR            rgbstrNames[1] ;
					UINT enum_cNames = 0;
					pEnumTypeInfo->GetNames( pvardesc->memid, rgbstrNames, 1, (UINT FAR*)&enum_cNames );
					_bstr_t str = TYPEDESCtoString( pEnumTypeInfo, &pvardesc->elemdescVar.tdesc );
					
					SysFreeString(bstrName) ;
					SysFreeString( bstrDoc ) ;
					SysFreeString( bstrHelp ) ;
					
					typeDefs += indentStr;
					typeDefs += "\t";
					typeDefs += str;
					typeDefs += " ";
					typeDefs += rgbstrNames[0];
					typeDefs += ";\n";
					
					SysFreeString(rgbstrNames[0]) ;
				}
			}
			else if ((pEnumAttr->typekind == TKIND_ENUM)) {
				for (UINT ei = 0 ; ei < pEnumAttr->cVars ; ei++) {
					VARDESC*    pvardesc = NULL ;
					hr = pEnumTypeInfo->GetVarDesc( ei, &pvardesc );
					_bstr_t str = TYPEDESCtoString( pEnumTypeInfo, &pvardesc->elemdescVar.tdesc );
					VARIANT varValue ;
					VariantInit( &varValue ) ;
					hr = VariantChangeType( &varValue, pvardesc->lpvarValue, 0, VT_BSTR );
					if ( FAILED(hr) ) {
						if (pvardesc->lpvarValue->vt == VT_ERROR || pvardesc->lpvarValue->vt == VT_HRESULT)
						{
							varValue.vt = VT_BSTR ;
							varValue.bstrVal = _bstr_t(_GetScodeString(pvardesc->lpvarValue->scode)).copy() ;
							hr = S_OK ;
						}
					}
					BSTR            rgbstrNames[1] ;
					UINT enum_cNames = 0;
					pEnumTypeInfo->GetNames( pvardesc->memid, rgbstrNames, 1, (UINT FAR*)&enum_cNames );
					typeDefs += indentStr;
					typeDefs += "\t ";
					//enumStr += str;
					//enumStr += " ";
					typeDefs += rgbstrNames[0];
					typeDefs += " = ";
					if (pvardesc->lpvarValue->vt == VT_BSTR) {
						typeDefs += "\"";
						typeDefs += varValue.bstrVal;
						typeDefs += "\"";	
					}
					else {
						typeDefs += varValue.bstrVal;
					}
					if ( ei == pEnumAttr->cVars-1 ) {
						typeDefs += "\n";
					}
					else {
						typeDefs += ",\n";
					}
					SysFreeString(rgbstrNames[0]);
				}
			}
			typeDefs += indentStr;
			typeDefs += "};\n";
			
			//printf( "typedef found: \n%s\n", (char*)enumStr );
		}
		break;
	}
	pEnumTypeInfo->ReleaseTypeAttr( pEnumAttr );
			
	return typeDefs.copy();
}

BSTR GenerateClassTypeDefs( ITypeLib* pTypeLib, long indent )
{
	_bstr_t typeDefs;
	
	HRESULT hr = E_FAIL;

	_bstr_t indentStr = "";
	for ( int i=0;i<indent;i++){
		indentStr += "\t";
	}
	UINT tic = pTypeLib->GetTypeInfoCount();
	for (int ti=0;ti<tic;ti++) {
		ITypeInfo* pEnumTypeInfo = NULL;
		hr = pTypeLib->GetTypeInfo( ti, &pEnumTypeInfo );
		if ( SUCCEEDED(hr) ) {
			TYPEATTR*   pEnumAttr = NULL ;
			hr = pEnumTypeInfo->GetTypeAttr( &pEnumAttr);
			
			switch ( pEnumAttr->typekind ) {
				case TKIND_ENUM:case TKIND_RECORD: case TKIND_UNION:{
					
					typeDefs += GenerateStructEnumUnion( pEnumTypeInfo, indent );

				}
				break;

				case TKIND_ALIAS:{
					BSTR            bstrName = NULL ;
					BSTR            bstrDoc = NULL ;
					BSTR            bstrHelp = NULL ;
					DWORD           dwHelpID ;
					hr = pEnumTypeInfo->GetDocumentation( MEMBERID_NIL, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp );

					typeDefs += "typedef ";
					typeDefs += TYPEDESCtoString( pEnumTypeInfo, &pEnumAttr->tdescAlias );
					typeDefs += " ";
					typeDefs += bstrName;
					typeDefs += ";\n\n";
					//bstrName;

					SysFreeString(bstrName) ;
					SysFreeString( bstrDoc ) ;
					SysFreeString( bstrHelp ) ;
				}
				break;
			}
			pEnumTypeInfo->ReleaseTypeAttr( pEnumAttr );
			pEnumTypeInfo->Release();
		}
	}
			
	return typeDefs.copy();
}

BSTR GenerateClassMethod( ITypeInfo* pMethodTypeInfo, long methodID )
{
	_bstr_t method;
	
	FUNCDESC*   pfuncdesc = NULL ;
	BSTR            rgbstrNames[MAX_NAMES] ;
	BSTR            bstrName = NULL ;
	BSTR            bstrDoc = NULL ;
	BSTR            bstrHelp = NULL ;
	DWORD           dwHelpID ;
	for (UINT ui = 0 ; ui < MAX_NAMES ; ui++)
		rgbstrNames[ui] = NULL ;

	HRESULT hr = pMethodTypeInfo->GetFuncDesc( methodID, &pfuncdesc );

	hr = pMethodTypeInfo->GetDocumentation( pfuncdesc->memid, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp );
	UINT cNames = 0 ;
	hr = pMethodTypeInfo->GetNames( pfuncdesc->memid, rgbstrNames, MAX_NAMES, &cNames );
	
	//if ((int)cNames < pfuncdesc->cParams + 1)
	//{
	//	rgbstrNames[cNames] = ::SysAllocString(OLESTR("rhs")) ;
	//	cNames++ ;
	//}
	//assert((int)cNames == pfuncdesc->cParams+1) ;
	
	
	method += TYPEDESCtoString( pMethodTypeInfo, &pfuncdesc->elemdescFunc.tdesc );
	method += " ";
	
	if (pfuncdesc->invkind & INVOKE_PROPERTYGET) {
		cNames --;
		method += "get_";
	}
	else if ((pfuncdesc->invkind & INVOKE_PROPERTYPUT) || (pfuncdesc->invkind & INVOKE_PROPERTYPUTREF)) {
		method += "set_";
	}
	method += rgbstrNames[0];
	method += "( ";
	
	for ( UINT p=0;p<pfuncdesc->cParams;p++) {
		bool paramIsPointer = false;
		if ( pfuncdesc->lprgelemdescParam ) {
			if ( pfuncdesc->lprgelemdescParam[p].idldesc.wIDLFlags ) {
				if (pfuncdesc->lprgelemdescParam[p].idldesc.wIDLFlags & IDLFLAG_FOUT) {
					paramIsPointer = true;
				}
				if (pfuncdesc->lprgelemdescParam[p].idldesc.wIDLFlags & IDLFLAG_FRETVAL) {
					paramIsPointer = true;
				}
				
				if ((pfuncdesc->lprgelemdescParam[p].tdesc.vt & 0x0FFF) == VT_CARRAY) {
					_bstr_t val = TYPEDESCtoString( pMethodTypeInfo, &pfuncdesc->lprgelemdescParam[p].tdesc.lpadesc->tdescElem );
				}
				else {
					_bstr_t type = TYPEDESCtoString( pMethodTypeInfo, &pfuncdesc->lprgelemdescParam[p].tdesc );
					_bstr_t paramName = rgbstrNames[p+1];									
					
					method += type;
					method += " ";
					if ( paramName.length() == 0 ) {
						paramName = "Val";
					}
					method += paramName;
					
					if ( p!=(pfuncdesc->cParams-1) ) {
						method += ", ";
					}
				}
			}
		}
	}
	method += " ); //";
	TCHAR methIDStr[256];
	memset( methIDStr, 0, 256 );
	sprintf( methIDStr, "(id[%d])", pfuncdesc->memid );
	method += methIDStr;
	
	SysFreeString(bstrName) ;
	SysFreeString( bstrDoc ) ;
	SysFreeString( bstrHelp ) ;

	for (ui = 0 ; ui < MAX_NAMES ; ui++) {
		if (rgbstrNames[ui]) {
			SysFreeString(rgbstrNames[ui]) ;
		}
	}
	pMethodTypeInfo->ReleaseFuncDesc( pfuncdesc ) ;
	
	return method.copy();
}


BSTR GenerateClassHeaderFromCOMCLSID( const CLSID& clsid )
{
	_bstr_t body;

	_bstr_t className;
	_bstr_t intefaceName;

	body += "class ";
	body += className;
	body += "  {  \n";
	body += "public:\n";
	body += "\t";
	body += className;
	body += " ();\n\n";
	body += "\tvirtual ~";
	body += className;
	body += " ();\n\n";
	body += "\n\t//auto generated methods for interface ";
	body += intefaceName;
	body += "\n\n";
	
	HRESULT hr = E_FAIL;

	IUnknown* pIUnknown = NULL;
	hr = CoCreateInstance( clsid, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, 
		                          (void**) &pIUnknown );

	IDispatch* pIDisp = NULL;
	hr = pIUnknown->QueryInterface( IID_IDispatch, (void**)&pIDisp );
	if ( SUCCEEDED(hr) ) {
		ITypeInfo* pTypeInfo = NULL;
		ITypeLib* pTypeLib = NULL;	
		UINT hasInfo = 0;
		hr = pIDisp->GetTypeInfoCount( &hasInfo );
		if ( hasInfo ) {
			hr = pIDisp->GetTypeInfo( 0, 0, &pTypeInfo );
			if ( SUCCEEDED(hr) ) {
				UINT index = 0;
				hr = pTypeInfo->GetContainingTypeLib( &pTypeLib, &index );
				pTypeInfo->Release();
				pTypeInfo = NULL;

				if ( SUCCEEDED(hr) ) {
					
					hr = pTypeLib->GetTypeInfoOfGuid( clsid, &pTypeInfo );	

					_bstr_t typeDefs = GenerateClassTypeDefs( pTypeLib, 1 );

					body += typeDefs;
					body += "\n\n";

					TYPEATTR*   pattr = NULL ;
					hr = pTypeInfo->GetTypeAttr( &pattr);
					BSTR            bstrName = NULL ;
					BSTR            bstrDoc = NULL ;
					BSTR            bstrHelp = NULL ;
					DWORD           dwHelpID ;
					ITypeInfo*      ptiImpl = NULL ;
					TYPEATTR*       pattrImpl = NULL ;
					pTypeInfo->GetDocumentation( MEMBERID_NIL, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp );
					for (UINT n = 0 ; n < pattr->cImplTypes ; n++)	{
						int impltype = 0;
						HREFTYPE href = NULL ;
						hr = pTypeInfo->GetImplTypeFlags( n, &impltype );
						hr = pTypeInfo->GetRefTypeOfImplType(n, &href);
						hr = pTypeInfo->GetRefTypeInfo( href, &ptiImpl );
						hr = ptiImpl->GetDocumentation( MEMBERID_NIL, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp );
						_bstr_t interfaceName = bstrName;

						//printf( "Interface \"%s\"\n", (char*)interfaceName);

						hr = ptiImpl->GetTypeAttr( &pattrImpl);
						switch ( pattrImpl->typekind ) {
							case TKIND_ENUM:
							case TKIND_RECORD:
							case TKIND_UNION:
							case TKIND_ALIAS: {
								
							}
							break;

							case TKIND_INTERFACE: {
								OLECHAR guidStr[256];
								StringFromGUID2( pattrImpl->guid, guidStr, 256 );
								body += "\t// generated methods for interface ";
								body += interfaceName;
								body += " (guid: ";
								body += guidStr;
								body += ")\n";
								for (UINT i = 0 ; i < pattrImpl->cFuncs ; i++)	{
									
									body += "\t";
									body += GenerateClassMethod( ptiImpl, i );
									body += "\n\n";
								}
							}
							break;

							case TKIND_DISPATCH: {
								OLECHAR guidStr[256];
								StringFromGUID2( pattrImpl->guid, guidStr, 256 );
								body += "\t// generated methods for dispatch interface ";
								body += interfaceName;
								body += " (guid: ";
								body += guidStr;
								body += ")\n";
								for (UINT i = 0 ; i < pattrImpl->cFuncs ; i++)	{
									
									body += "\t";
									body += GenerateClassMethod( ptiImpl, i );
									body += "\n\n";
								}
							}
							break;
						}
						ptiImpl->Release();
					}
					pTypeInfo->ReleaseTypeAttr( pattr );
					pTypeInfo->Release();
				}
			}
		}
		pIDisp->Release();
	}

	pIUnknown->Release();

	body += "};\n";

	return body.copy();
}

/*
int main(int argc, char* argv[])
{

	IStream*    pstm = NULL ;
	//pstm = CreateMemoryStream() ;
	//_com_ptr_t obj;

	OleInitialize( 0 );

	CLSID clsid;
	_bstr_t progID( "OWC.Pivottable.9" );//"DHTMLEdit.DHTMLEdit" );

	HRESULT hr = CLSIDFromProgID( (LPOLESTR)progID, &clsid );
	
	if ( SUCCEEDED(hr) ) {
		_bstr_t classBody = GenerateClassHeaderFromCOMCLSID( clsid );	
		
		
		printf( "%s", (char*)classBody );
	}

	OleUninitialize();
	
	return 0;
}
*/

CoClassHolder::~CoClassHolder()
{	
	m_implementedInterfaces.clear();
}

InterfaceHolder::~InterfaceHolder()
{
	std::vector<MethodHolder*>::iterator it = m_methods.begin();
	while ( it != m_methods.end() ) 	{
		MethodHolder* mh = *it;
		delete mh;
		mh = NULL;
		it ++;
	}
	m_methods.clear();
}

TypeLibHolder::~TypeLibHolder()
{
	std::vector<CoClassHolder*>::iterator it = m_coClasses.begin();
	while ( it != m_coClasses.end() ) 	{
		CoClassHolder* ch = *it;
		delete ch;
		ch = NULL;
		it ++;
	}
	m_coClasses.clear();

	std::map<_bstr_t,InterfaceHolder*>::iterator it2 = m_interfaces.begin();
	while ( it2 != m_interfaces.end() ) 	{
		InterfaceHolder* ih = it2->second;
		delete ih;
		ih = NULL;
		it2 ++;
	}
	m_interfaces.clear();
}

HRESULT GenerateTypeLibHolder( ITypeLib* pTypeLib, TypeLibHolder& typeLibHolder )
{
	HRESULT hr = E_FAIL;

	typeLibHolder.m_typeDefs = GenerateClassTypeDefs( pTypeLib, 1 );
	BSTR            bstrName = NULL ;
	BSTR            bstrDoc = NULL ;
	BSTR            bstrHelp = NULL ;
	DWORD           dwHelpID ;

	hr = pTypeLib->GetDocumentation( MEMBERID_NIL, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp ) ;
	
	typeLibHolder.m_typeLibName = bstrName;

	SysFreeString(bstrName) ;
	SysFreeString( bstrDoc ) ;
	SysFreeString( bstrHelp ) ;

	_bstr_t indentStr = "";
	int indent = 0;
	for ( int i=0;i<indent;i++){
		indentStr += "\t";
	}
	UINT tic = pTypeLib->GetTypeInfoCount();
	for (int ti=0;ti<tic;ti++) {
		ITypeInfo* pTypeInfo = NULL;
		hr = pTypeLib->GetTypeInfo( ti, &pTypeInfo );
		if ( SUCCEEDED(hr) ) {
			TYPEATTR*   pTypeAttr = NULL ;
			hr = pTypeInfo->GetTypeAttr( &pTypeAttr);
			
			switch ( pTypeAttr->typekind ) {
				case TKIND_COCLASS: {								
					hr = pTypeInfo->GetDocumentation( MEMBERID_NIL, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp );
					_bstr_t coClassName = bstrName;
					SysFreeString(bstrName) ;
					SysFreeString( bstrDoc ) ;
					SysFreeString( bstrHelp ) ;

					OLECHAR guidStr[256];
					StringFromGUID2( pTypeAttr->guid, guidStr, 256 );

					CoClassHolder* newCoClass = new CoClassHolder();
					newCoClass->m_className = coClassName;
					newCoClass->m_classID = guidStr;
					for (UINT n = 0 ; n < pTypeAttr->cImplTypes ; n++)	{
						ITypeInfo*      ptiImpl = NULL ;
						int impltype = 0;
						TYPEATTR*       pattrImpl = NULL ;
						HREFTYPE href = NULL ;
						hr = pTypeInfo->GetImplTypeFlags( n, &impltype );
						hr = pTypeInfo->GetRefTypeOfImplType(n, &href);
						hr = pTypeInfo->GetRefTypeInfo( href, &ptiImpl );
						hr = ptiImpl->GetTypeAttr( &pattrImpl);
						hr = ptiImpl->GetDocumentation( MEMBERID_NIL, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp );
						switch ( pattrImpl->typekind ) {
							case TKIND_INTERFACE: case TKIND_DISPATCH: {
								_bstr_t tmp = bstrName;

								SysFreeString(bstrName) ;
								SysFreeString( bstrDoc ) ;
								SysFreeString( bstrHelp ) ;

								std::map<_bstr_t,InterfaceHolder*>::iterator found = 
									typeLibHolder.m_interfaces.find( tmp );
								InterfaceHolder* newInterface = NULL;
								if ( found != typeLibHolder.m_interfaces.end() ) {
									newInterface = found->second;

									typeLibHolder.m_interfaces[tmp] = newInterface;
								}
								else {
									newInterface = new InterfaceHolder();
					
									GenerateInterface( ptiImpl, pattrImpl, newInterface );
								}
								
								newCoClass->m_implementedInterfaces.push_back( newInterface );								
							}
							break;
						}
					}
					typeLibHolder.m_coClasses.push_back( newCoClass );
				}
				break;

				case TKIND_INTERFACE: case TKIND_DISPATCH: {					
					InterfaceHolder* newInterface = new InterfaceHolder();
					
					hr = GenerateInterface( pTypeInfo, pTypeAttr, newInterface );
					_bstr_t tmp = newInterface->m_interfaceName;
					typeLibHolder.m_interfaces[tmp] = newInterface;
				}
				break;
			}
		}
	}
	return hr;
}

HRESULT GenerateMethod( ITypeInfo* pMethodTypeInfo, long methodID, MethodHolder* pMethod )
{
	HRESULT hr = E_FAIL;

	_bstr_t method;
	
	FUNCDESC*   pfuncdesc = NULL ;
	BSTR            rgbstrNames[MAX_NAMES] ;
	BSTR            bstrName = NULL ;
	BSTR            bstrDoc = NULL ;
	BSTR            bstrHelp = NULL ;
	DWORD           dwHelpID ;
	for (UINT ui = 0 ; ui < MAX_NAMES ; ui++)
		rgbstrNames[ui] = NULL ;

	hr = pMethodTypeInfo->GetFuncDesc( methodID, &pfuncdesc );

	hr = pMethodTypeInfo->GetDocumentation( pfuncdesc->memid, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp );
	UINT cNames = 0 ;
	hr = pMethodTypeInfo->GetNames( pfuncdesc->memid, rgbstrNames, MAX_NAMES, &cNames );	
	
	pMethod->m_returnType = TYPEDESCtoString( pMethodTypeInfo, &pfuncdesc->elemdescFunc.tdesc );
		
	if (pfuncdesc->invkind & INVOKE_PROPERTYGET) {
		cNames --;
		pMethod->m_isPropertyGetter = true;
	}
	else if ((pfuncdesc->invkind & INVOKE_PROPERTYPUT) || (pfuncdesc->invkind & INVOKE_PROPERTYPUTREF)) {		
		pMethod->m_isPropertySetter = true;
	}
	pMethod->m_methodName = rgbstrNames[0];
	method += rgbstrNames[0];
	method += "( ";
	
	for ( UINT p=0;p<pfuncdesc->cParams;p++) {
		bool paramIsPointer = false;
		if ( pfuncdesc->lprgelemdescParam ) {
			if ( pfuncdesc->lprgelemdescParam[p].idldesc.wIDLFlags ) {
				if (pfuncdesc->lprgelemdescParam[p].idldesc.wIDLFlags & IDLFLAG_FOUT) {
					paramIsPointer = true;
				}
				if (pfuncdesc->lprgelemdescParam[p].idldesc.wIDLFlags & IDLFLAG_FRETVAL) {
					paramIsPointer = true;
				}
				
				if ((pfuncdesc->lprgelemdescParam[p].tdesc.vt & 0x0FFF) == VT_CARRAY) {
					_bstr_t val = TYPEDESCtoString( pMethodTypeInfo, &pfuncdesc->lprgelemdescParam[p].tdesc.lpadesc->tdescElem );
				}
				else {
					_bstr_t type = TYPEDESCtoString( pMethodTypeInfo, &pfuncdesc->lprgelemdescParam[p].tdesc );
					_bstr_t paramName = rgbstrNames[p+1];									
					
					method += type;
					method += " ";
					if ( paramName.length() == 0 ) {
						paramName = "Val";
					}
					method += paramName;
					
					if ( p!=(pfuncdesc->cParams-1) ) {
						method += ", ";
					}

					MethodArgumentHolder methodArg( type, paramName );
					pMethod->m_arguments.push_back( methodArg );
				}
			}
		}
	}
	method += " )";
	pMethod->m_declaration = method;
	TCHAR methIDStr[256];
	memset( methIDStr, 0, 256 );
	sprintf( methIDStr, "%d", pfuncdesc->memid );
	pMethod->m_methodID = methIDStr;

	for (ui = 0 ; ui < MAX_NAMES ; ui++) {
		if (rgbstrNames[ui]) {
			SysFreeString(rgbstrNames[ui]) ;
		}
	}

	SysFreeString(bstrName) ;
	SysFreeString( bstrDoc ) ;
	SysFreeString( bstrHelp ) ;

	pMethodTypeInfo->ReleaseFuncDesc( pfuncdesc ) ;		

	return hr;
}

HRESULT GenerateInterface( ITypeInfo* pTypeInfo, TYPEATTR* pTypeAttr, InterfaceHolder* pInterface )
{
	HRESULT hr = E_FAIL;
	BSTR            bstrName = NULL ;
	BSTR            bstrDoc = NULL ;
	BSTR            bstrHelp = NULL ;
	DWORD           dwHelpID ;
	
	hr = pTypeInfo->GetDocumentation( MEMBERID_NIL, &bstrName, &bstrDoc, &dwHelpID, &bstrHelp );
	_bstr_t interfaceName = bstrName;
	OLECHAR guidStr[256];
	StringFromGUID2( pTypeAttr->guid, guidStr, 256 );
	
	SysFreeString(bstrName) ;
	SysFreeString( bstrDoc ) ;
	SysFreeString( bstrHelp ) ;

	pInterface->m_interfaceID = guidStr;
	pInterface->m_interfaceName = interfaceName;
	if ( pTypeAttr->typekind == TKIND_INTERFACE ) {
		pInterface->m_superInterfaceName = "IUnknown";
	}
	else if ( pTypeAttr->typekind == TKIND_DISPATCH ) {
		pInterface->m_superInterfaceName = "IDispatch";
	}
	for (UINT i = 0 ; i < pTypeAttr->cFuncs ; i++)	{						
		MethodHolder* methodHolder = new MethodHolder();
		
		hr = GenerateMethod( pTypeInfo, i, methodHolder );		
		CString tmp = (TCHAR*)methodHolder->m_declaration;
		//ignore IUnknown and IDispatch methods
		int pos = tmp.Find( "AddRef(" );
		int pos1 = tmp.Find( "Release(" );
		int pos2 = tmp.Find( "QueryInterface(" );
		int pos3 = tmp.Find( "GetTypeInfoCount(" );
		int pos4 = tmp.Find( "GetTypeInfo(" );
		int pos5 = tmp.Find( "GetIDsOfNames(" );
		int pos6 = tmp.Find( "Invoke(" );		
		bool addMethod = ((pos<0) && (pos1<0) && (pos2<0) && (pos3<0) && (pos4<0) && (pos5<0) && (pos6<0));

		if ( (true == addMethod) && (SUCCEEDED(hr)) ) {
			pInterface->m_methods.push_back( methodHolder );
		}
		else {
			delete methodHolder;
		}
	}
	return hr;
}