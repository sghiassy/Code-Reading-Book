//DexterLib.h

#ifndef _DEXTERLIB_H__
#define _DEXTERLIB_H__

#include "FoundationKit.h"

namespace DexterLib  {

void registerDexterLibClasses();
//Typelibrary typedefs

typedef __MIDL___MIDL_itf_qedit_0000_0002 DEXTER_PARAM;

	struct __MIDL___MIDL_itf_qedit_0000_0002 {
		BSTR Name;
		long dispID;
		long nValues;
	};
typedef __MIDL___MIDL_itf_qedit_0000_0003 DEXTER_VALUE;

	struct __MIDL___MIDL_itf_qedit_0000_0003 {
		VARIANT v;
		int64 rt;
		unsigned long dwInterp;
	};
typedef __MIDL___MIDL_itf_qedit_0000_0007 TIMELINE_MAJOR_TYPE;

	enum __MIDL___MIDL_itf_qedit_0000_0007 {
		 TIMELINE_MAJOR_TYPE_COMPOSITE = 1,
		 TIMELINE_MAJOR_TYPE_TRACK = 2,
		 TIMELINE_MAJOR_TYPE_SOURCE = 4,
		 TIMELINE_MAJOR_TYPE_TRANSITION = 8,
		 TIMELINE_MAJOR_TYPE_EFFECT = 16,
		 TIMELINE_MAJOR_TYPE_GROUP = 128
	};
	struct _AMMediaType {
		GUID majortype;
		GUID subtype;
		long bFixedSizeSamples;
		long bTemporalCompression;
		unsigned long lSampleSize;
		GUID formattype;
		IUnknown* pUnk;
		unsigned long cbFormat;
		unsigned char* pbFormat;
	};
typedef long LONG_PTR;

	struct _LARGE_INTEGER {
		int64 QuadPart;
	};
	struct _ULARGE_INTEGER {
		uint64 QuadPart;
	};
	struct tagSTATSTG {
		String pwcsName;
		unsigned long Type;
		_ULARGE_INTEGER cbSize;
		_FILETIME mtime;
		_FILETIME ctime;
		_FILETIME atime;
		unsigned long grfMode;
		unsigned long grfLocksSupported;
		GUID clsid;
		unsigned long grfStateBits;
		unsigned long reserved;
	};
	struct _FILETIME {
		unsigned long dwLowDateTime;
		unsigned long dwHighDateTime;
	};
	enum _FilterState {
		 State_Stopped = 0,
		 State_Paused = 1,
		 State_Running = 2
	};
typedef unsigned long ULONG_PTR;

	struct _PinInfo {
		IBaseFilter* pFilter;
		_PinDirection dir;
		unsigned short[128] achName;
	};
	enum _PinDirection {
		 PINDIR_INPUT = 0,
		 PINDIR_OUTPUT = 1
	};
	struct _FilterInfo {
		unsigned short[128] achName;
		IFilterGraph* pGraph;
	};

//End of Typelibrary typedefs

}  //end of namespace DexterLib

#endif //_DEXTERLIB_H__

