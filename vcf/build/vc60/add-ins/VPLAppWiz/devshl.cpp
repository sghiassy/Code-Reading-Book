// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "devshl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// IGenericDocument properties

/////////////////////////////////////////////////////////////////////////////
// IGenericDocument operations

CString IGenericDocument::GetName()
{
	CString result;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IGenericDocument::GetFullName()
{
	CString result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericDocument::GetApplication()
{
	LPDISPATCH result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericDocument::GetParent()
{
	LPDISPATCH result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

CString IGenericDocument::GetPath()
{
	CString result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

BOOL IGenericDocument::GetSaved()
{
	BOOL result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericDocument::GetActiveWindow()
{
	LPDISPATCH result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

BOOL IGenericDocument::GetReadOnly()
{
	BOOL result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IGenericDocument::SetReadOnly(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

CString IGenericDocument::GetType()
{
	CString result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericDocument::GetWindows()
{
	LPDISPATCH result;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

void IGenericDocument::SetActive(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL IGenericDocument::GetActive()
{
	BOOL result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericDocument::NewWindow()
{
	LPDISPATCH result;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

long IGenericDocument::Save(const VARIANT& vFilename, const VARIANT& vBoolPrompt)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0xc, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&vFilename, &vBoolPrompt);
	return result;
}

BOOL IGenericDocument::Undo()
{
	BOOL result;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGenericDocument::Redo()
{
	BOOL result;
	InvokeHelper(0xe, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGenericDocument::PrintOut()
{
	BOOL result;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

long IGenericDocument::Close(const VARIANT& vSaveChanges)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&vSaveChanges);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IGenericWindow properties

/////////////////////////////////////////////////////////////////////////////
// IGenericWindow operations

CString IGenericWindow::GetCaption()
{
	CString result;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IGenericWindow::GetType()
{
	CString result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IGenericWindow::SetActive(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL IGenericWindow::GetActive()
{
	BOOL result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IGenericWindow::SetLeft(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IGenericWindow::GetLeft()
{
	long result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IGenericWindow::SetTop(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IGenericWindow::GetTop()
{
	long result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IGenericWindow::SetHeight(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IGenericWindow::GetHeight()
{
	long result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IGenericWindow::SetWidth(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IGenericWindow::GetWidth()
{
	long result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

long IGenericWindow::GetIndex()
{
	long result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericWindow::GetNext()
{
	LPDISPATCH result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericWindow::GetPrevious()
{
	LPDISPATCH result;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

void IGenericWindow::SetWindowState(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IGenericWindow::GetWindowState()
{
	long result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericWindow::GetApplication()
{
	LPDISPATCH result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericWindow::GetParent()
{
	LPDISPATCH result;
	InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

long IGenericWindow::Close(const VARIANT& boolSaveChanges)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&boolSaveChanges);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IGenericProject properties

/////////////////////////////////////////////////////////////////////////////
// IGenericProject operations

CString IGenericProject::GetName()
{
	CString result;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IGenericProject::GetFullName()
{
	CString result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericProject::GetApplication()
{
	LPDISPATCH result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGenericProject::GetParent()
{
	LPDISPATCH result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

CString IGenericProject::GetType()
{
	CString result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IApplication properties

/////////////////////////////////////////////////////////////////////////////
// IApplication operations

long IApplication::GetHeight()
{
	long result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IApplication::SetHeight(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IApplication::GetWidth()
{
	long result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IApplication::SetWidth(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IApplication::GetTop()
{
	long result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IApplication::SetTop(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IApplication::GetLeft()
{
	long result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IApplication::SetLeft(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

CString IApplication::GetName()
{
	CString result;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

LPDISPATCH IApplication::GetTextEditor()
{
	LPDISPATCH result;
	InvokeHelper(0x12, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

CString IApplication::GetVersion()
{
	CString result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IApplication::GetPath()
{
	CString result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IApplication::GetCurrentDirectory()
{
	CString result;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IApplication::SetCurrentDirectory(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

CString IApplication::GetFullName()
{
	CString result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

LPDISPATCH IApplication::GetApplication()
{
	LPDISPATCH result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IApplication::GetParent()
{
	LPDISPATCH result;
	InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IApplication::GetActiveDocument()
{
	LPDISPATCH result;
	InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IApplication::GetWindows()
{
	LPDISPATCH result;
	InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IApplication::GetDocuments()
{
	LPDISPATCH result;
	InvokeHelper(0xf, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IApplication::GetActiveWindow()
{
	LPDISPATCH result;
	InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

long IApplication::GetWindowState()
{
	long result;
	InvokeHelper(0x11, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IApplication::SetWindowState(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x11, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

LPDISPATCH IApplication::GetDebugger()
{
	LPDISPATCH result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IApplication::GetProjects()
{
	LPDISPATCH result;
	InvokeHelper(0x13, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IApplication::GetActiveConfiguration()
{
	LPDISPATCH result;
	InvokeHelper(0x14, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

void IApplication::SetActiveConfiguration(LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x14, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

BOOL IApplication::GetVisible()
{
	BOOL result;
	InvokeHelper(0x1d, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IApplication::SetVisible(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x1d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

LPDISPATCH IApplication::GetActiveProject()
{
	LPDISPATCH result;
	InvokeHelper(0x1e, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

void IApplication::SetActiveProject(LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x1e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

void IApplication::SetActive(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x20003, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL IApplication::GetActive()
{
	BOOL result;
	InvokeHelper(0x20003, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

LPDISPATCH IApplication::GetPackageExtension(LPCTSTR szExtensionName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		szExtensionName);
	return result;
}

void IApplication::Quit()
{
	InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IApplication::PrintToOutputWindow(LPCTSTR Message)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x17, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Message);
}

void IApplication::ExecuteCommand(LPCTSTR szCommandName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x18, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szCommandName);
}

void IApplication::AddCommandBarButton(long nButtonType, LPCTSTR szCmdName, long dwCookie)
{
	static BYTE parms[] =
		VTS_I4 VTS_BSTR VTS_I4;
	InvokeHelper(0x19, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nButtonType, szCmdName, dwCookie);
}

void IApplication::AddKeyBinding(LPCTSTR szKey, LPCTSTR szCommandName, LPCTSTR szEditor)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BSTR;
	InvokeHelper(0x1a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szKey, szCommandName, szEditor);
}

void IApplication::Build(const VARIANT& Configuration)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x1b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Configuration);
}

void IApplication::RebuildAll(const VARIANT& Configuration)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x1c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Configuration);
}

void IApplication::ExecuteConfiguration(const VARIANT& Reserved)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x1f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Reserved);
}

void IApplication::SetAddInInfo(long nInstanceHandle, LPDISPATCH pCmdDispatch, long nIDBitmapResourceMedium, long nIDBitmapResourceLarge, long dwCookie)
{
	static BYTE parms[] =
		VTS_I4 VTS_DISPATCH VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x20001, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nInstanceHandle, pCmdDispatch, nIDBitmapResourceMedium, nIDBitmapResourceLarge, dwCookie);
}

BOOL IApplication::AddCommand(LPCTSTR szCmdName, LPCTSTR szMethodName, long nBitmapOffset, long dwCookie)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4 VTS_I4;
	InvokeHelper(0x20002, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		szCmdName, szMethodName, nBitmapOffset, dwCookie);
	return result;
}

void IApplication::EnableModeless(BOOL bEnable)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x20004, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 bEnable);
}

void IApplication::Clean(const VARIANT& Configuration)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x20, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Configuration);
}

long IApplication::GetErrors()
{
	long result;
	InvokeHelper(0x21, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

long IApplication::GetWarnings()
{
	long result;
	InvokeHelper(0x22, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IApplication::AddProject(LPCTSTR szName, LPCTSTR szPath, LPCTSTR szType, const VARIANT& bAddDefaultFolders)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x23, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szName, szPath, szType, &bAddDefaultFolders);
}


/////////////////////////////////////////////////////////////////////////////
// IDispApplication properties

long IDispApplication::GetHeight()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}

void IDispApplication::SetHeight(long propVal)
{
	SetProperty(0x1, VT_I4, propVal);
}

long IDispApplication::GetWidth()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void IDispApplication::SetWidth(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

long IDispApplication::GetTop()
{
	long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}

void IDispApplication::SetTop(long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}

long IDispApplication::GetLeft()
{
	long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}

void IDispApplication::SetLeft(long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}

LPDISPATCH IDispApplication::GetTextEditor()
{
	LPDISPATCH result;
	GetProperty(0x12, VT_DISPATCH, (void*)&result);
	return result;
}

CString IDispApplication::GetVersion()
{
	CString result;
	GetProperty(0x7, VT_BSTR, (void*)&result);
	return result;
}

CString IDispApplication::GetPath()
{
	CString result;
	GetProperty(0x8, VT_BSTR, (void*)&result);
	return result;
}

CString IDispApplication::GetCurrentDirectory()
{
	CString result;
	GetProperty(0x9, VT_BSTR, (void*)&result);
	return result;
}

void IDispApplication::SetCurrentDirectory(LPCTSTR propVal)
{
	SetProperty(0x9, VT_BSTR, propVal);
}

CString IDispApplication::GetFullName()
{
	CString result;
	GetProperty(0xa, VT_BSTR, (void*)&result);
	return result;
}

LPDISPATCH IDispApplication::GetApplication()
{
	LPDISPATCH result;
	GetProperty(0xb, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispApplication::GetParent()
{
	LPDISPATCH result;
	GetProperty(0xc, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispApplication::GetActiveDocument()
{
	LPDISPATCH result;
	GetProperty(0xd, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispApplication::GetWindows()
{
	LPDISPATCH result;
	GetProperty(0xe, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispApplication::GetDocuments()
{
	LPDISPATCH result;
	GetProperty(0xf, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispApplication::GetActiveWindow()
{
	LPDISPATCH result;
	GetProperty(0x10, VT_DISPATCH, (void*)&result);
	return result;
}

long IDispApplication::GetWindowState()
{
	long result;
	GetProperty(0x11, VT_I4, (void*)&result);
	return result;
}

void IDispApplication::SetWindowState(long propVal)
{
	SetProperty(0x11, VT_I4, propVal);
}

LPDISPATCH IDispApplication::GetDebugger()
{
	LPDISPATCH result;
	GetProperty(0x6, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispApplication::GetProjects()
{
	LPDISPATCH result;
	GetProperty(0x13, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispApplication::GetActiveConfiguration()
{
	LPDISPATCH result;
	GetProperty(0x14, VT_DISPATCH, (void*)&result);
	return result;
}

void IDispApplication::SetActiveConfiguration(LPDISPATCH propVal)
{
	SetProperty(0x14, VT_DISPATCH, propVal);
}

BOOL IDispApplication::GetVisible()
{
	BOOL result;
	GetProperty(0x1d, VT_BOOL, (void*)&result);
	return result;
}

void IDispApplication::SetVisible(BOOL propVal)
{
	SetProperty(0x1d, VT_BOOL, propVal);
}

LPDISPATCH IDispApplication::GetActiveProject()
{
	LPDISPATCH result;
	GetProperty(0x1e, VT_DISPATCH, (void*)&result);
	return result;
}

void IDispApplication::SetActiveProject(LPDISPATCH propVal)
{
	SetProperty(0x1e, VT_DISPATCH, propVal);
}

long IDispApplication::GetErrors()
{
	long result;
	GetProperty(0x21, VT_I4, (void*)&result);
	return result;
}

long IDispApplication::GetWarnings()
{
	long result;
	GetProperty(0x22, VT_I4, (void*)&result);
	return result;
}

CString IDispApplication::GetName()
{
	CString result;
	GetProperty(0x0, VT_BSTR, (void*)&result);
	return result;
}

BOOL IDispApplication::GetActive()
{
	BOOL result;
	GetProperty(0x20003, VT_BOOL, (void*)&result);
	return result;
}

void IDispApplication::SetActive(BOOL propVal)
{
	SetProperty(0x20003, VT_BOOL, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// IDispApplication operations

LPDISPATCH IDispApplication::GetPackageExtension(LPCTSTR szExtensionName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		szExtensionName);
	return result;
}

void IDispApplication::Quit()
{
	InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IDispApplication::PrintToOutputWindow(LPCTSTR Message)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x17, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Message);
}

void IDispApplication::ExecuteCommand(LPCTSTR szCommandName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x18, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szCommandName);
}

void IDispApplication::AddCommandBarButton(long nButtonType, LPCTSTR szCmdName, long dwCookie)
{
	static BYTE parms[] =
		VTS_I4 VTS_BSTR VTS_I4;
	InvokeHelper(0x19, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nButtonType, szCmdName, dwCookie);
}

void IDispApplication::AddKeyBinding(LPCTSTR szKey, LPCTSTR szCommandName, LPCTSTR szEditor)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BSTR;
	InvokeHelper(0x1a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szKey, szCommandName, szEditor);
}

void IDispApplication::Build(const VARIANT& Configuration)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x1b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Configuration);
}

void IDispApplication::RebuildAll(const VARIANT& Configuration)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x1c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Configuration);
}

void IDispApplication::ExecuteConfiguration(const VARIANT& Reserved)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x1f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Reserved);
}

void IDispApplication::Clean(const VARIANT& Configuration)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x20, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Configuration);
}

void IDispApplication::AddProject(LPCTSTR szName, LPCTSTR szPath, LPCTSTR szType, const VARIANT& bAddDefaultFolders)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x23, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szName, szPath, szType, &bAddDefaultFolders);
}

void IDispApplication::SetAddInInfo(long nInstanceHandle, LPDISPATCH pCmdDispatch, long nIDBitmapResourceMedium, long nIDBitmapResourceLarge, long dwCookie)
{
	static BYTE parms[] =
		VTS_I4 VTS_DISPATCH VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x20001, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nInstanceHandle, pCmdDispatch, nIDBitmapResourceMedium, nIDBitmapResourceLarge, dwCookie);
}

BOOL IDispApplication::AddCommand(LPCTSTR szCmdName, LPCTSTR szMethodName, long nBitmapOffset, long dwCookie)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4 VTS_I4;
	InvokeHelper(0x20002, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		szCmdName, szMethodName, nBitmapOffset, dwCookie);
	return result;
}

void IDispApplication::EnableModeless(BOOL bEnable)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x20004, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 bEnable);
}


/////////////////////////////////////////////////////////////////////////////
// IApplicationEvents properties

/////////////////////////////////////////////////////////////////////////////
// IApplicationEvents operations

void IApplicationEvents::BeforeBuildStart()
{
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IApplicationEvents::BuildFinish(long nNumErrors, long nNumWarnings)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nNumErrors, nNumWarnings);
}

void IApplicationEvents::BeforeApplicationShutDown()
{
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IApplicationEvents::DocumentOpen(LPDISPATCH theDocument)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theDocument);
}

void IApplicationEvents::BeforeDocumentClose(LPDISPATCH theDocument)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theDocument);
}

void IApplicationEvents::DocumentSave(LPDISPATCH theDocument)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theDocument);
}

void IApplicationEvents::NewDocument(LPDISPATCH theDocument)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theDocument);
}

void IApplicationEvents::WindowActivate(LPDISPATCH theWindow)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theWindow);
}

void IApplicationEvents::WindowDeactivate(LPDISPATCH theWindow)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theWindow);
}

void IApplicationEvents::WorkspaceOpen()
{
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IApplicationEvents::WorkspaceClose()
{
	InvokeHelper(0xb, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IApplicationEvents::NewWorkspace()
{
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// IDispApplicationEvents properties

/////////////////////////////////////////////////////////////////////////////
// IDispApplicationEvents operations

void IDispApplicationEvents::BeforeBuildStart()
{
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IDispApplicationEvents::BuildFinish(long nNumErrors, long nNumWarnings)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nNumErrors, nNumWarnings);
}

void IDispApplicationEvents::BeforeApplicationShutDown()
{
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IDispApplicationEvents::DocumentOpen(LPDISPATCH theDocument)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theDocument);
}

void IDispApplicationEvents::BeforeDocumentClose(LPDISPATCH theDocument)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theDocument);
}

void IDispApplicationEvents::DocumentSave(LPDISPATCH theDocument)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theDocument);
}

void IDispApplicationEvents::NewDocument(LPDISPATCH theDocument)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theDocument);
}

void IDispApplicationEvents::WindowActivate(LPDISPATCH theWindow)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theWindow);
}

void IDispApplicationEvents::WindowDeactivate(LPDISPATCH theWindow)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 theWindow);
}

void IDispApplicationEvents::WorkspaceOpen()
{
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IDispApplicationEvents::WorkspaceClose()
{
	InvokeHelper(0xb, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IDispApplicationEvents::NewWorkspace()
{
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// IDocuments properties

/////////////////////////////////////////////////////////////////////////////
// IDocuments operations

long IDocuments::GetCount()
{
	long result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IDocuments::GetApplication()
{
	LPDISPATCH result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IDocuments::GetParent()
{
	LPDISPATCH result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IDocuments::Item(const VARIANT& Index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		&Index);
	return result;
}

long IDocuments::SaveAll(const VARIANT& vtBoolPrompt)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&vtBoolPrompt);
	return result;
}

long IDocuments::CloseAll(const VARIANT& vtSaveChanges)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&vtSaveChanges);
	return result;
}

LPDISPATCH IDocuments::Add(LPCTSTR docType, const VARIANT& vtReserved)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		docType, &vtReserved);
	return result;
}

LPDISPATCH IDocuments::Open(LPCTSTR filename, const VARIANT& vtDocType, const VARIANT& vtBoolReadOnly)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		filename, &vtDocType, &vtBoolReadOnly);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IDispDocuments properties

long IDispDocuments::GetCount()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}

LPDISPATCH IDispDocuments::GetApplication()
{
	LPDISPATCH result;
	GetProperty(0x2, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispDocuments::GetParent()
{
	LPDISPATCH result;
	GetProperty(0x3, VT_DISPATCH, (void*)&result);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// IDispDocuments operations

long IDispDocuments::SaveAll(const VARIANT& boolPrompt)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&boolPrompt);
	return result;
}

long IDispDocuments::CloseAll(const VARIANT& boolSaveChanges)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&boolSaveChanges);
	return result;
}

LPDISPATCH IDispDocuments::Add(LPCTSTR docType, const VARIANT& vtReserved)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		docType, &vtReserved);
	return result;
}

LPDISPATCH IDispDocuments::Open(LPCTSTR filename, const VARIANT& docType, const VARIANT& ReadOnly)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		filename, &docType, &ReadOnly);
	return result;
}

LPDISPATCH IDispDocuments::Item(const VARIANT& Index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		&Index);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IWindows properties

/////////////////////////////////////////////////////////////////////////////
// IWindows operations

long IWindows::GetCount()
{
	long result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IWindows::GetApplication()
{
	LPDISPATCH result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IWindows::GetParent()
{
	LPDISPATCH result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IWindows::Item(const VARIANT& Index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		&Index);
	return result;
}

void IWindows::Arrange(const VARIANT& vtArrangeStyle)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &vtArrangeStyle);
}

long IWindows::CloseAll(const VARIANT& vtSaveChanges)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&vtSaveChanges);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IDispWindows properties

long IDispWindows::GetCount()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}

LPDISPATCH IDispWindows::GetApplication()
{
	LPDISPATCH result;
	GetProperty(0x2, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispWindows::GetParent()
{
	LPDISPATCH result;
	GetProperty(0x3, VT_DISPATCH, (void*)&result);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// IDispWindows operations

LPDISPATCH IDispWindows::Item(const VARIANT& Index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		&Index);
	return result;
}

long IDispWindows::CloseAll(const VARIANT& vtSaveChanges)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&vtSaveChanges);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IProjects properties

/////////////////////////////////////////////////////////////////////////////
// IProjects operations

LPDISPATCH IProjects::GetApplication()
{
	LPDISPATCH result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

long IProjects::GetCount()
{
	long result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IProjects::GetParent()
{
	LPDISPATCH result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IProjects::Item(const VARIANT& Index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		&Index);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IDispProjects properties

LPDISPATCH IDispProjects::GetApplication()
{
	LPDISPATCH result;
	GetProperty(0x1, VT_DISPATCH, (void*)&result);
	return result;
}

long IDispProjects::GetCount()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

LPDISPATCH IDispProjects::GetParent()
{
	LPDISPATCH result;
	GetProperty(0x4, VT_DISPATCH, (void*)&result);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// IDispProjects operations

LPDISPATCH IDispProjects::Item(const VARIANT& Index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		&Index);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IDispGenericDocument properties

CString IDispGenericDocument::GetName()
{
	CString result;
	GetProperty(0x0, VT_BSTR, (void*)&result);
	return result;
}

CString IDispGenericDocument::GetFullName()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}

LPDISPATCH IDispGenericDocument::GetApplication()
{
	LPDISPATCH result;
	GetProperty(0x2, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispGenericDocument::GetParent()
{
	LPDISPATCH result;
	GetProperty(0x3, VT_DISPATCH, (void*)&result);
	return result;
}

CString IDispGenericDocument::GetPath()
{
	CString result;
	GetProperty(0x4, VT_BSTR, (void*)&result);
	return result;
}

BOOL IDispGenericDocument::GetSaved()
{
	BOOL result;
	GetProperty(0x5, VT_BOOL, (void*)&result);
	return result;
}

void IDispGenericDocument::SetSaved(BOOL propVal)
{
	SetProperty(0x5, VT_BOOL, propVal);
}

LPDISPATCH IDispGenericDocument::GetActiveWindow()
{
	LPDISPATCH result;
	GetProperty(0x6, VT_DISPATCH, (void*)&result);
	return result;
}

void IDispGenericDocument::SetActiveWindow(LPDISPATCH propVal)
{
	SetProperty(0x6, VT_DISPATCH, propVal);
}

BOOL IDispGenericDocument::GetReadOnly()
{
	BOOL result;
	GetProperty(0x7, VT_BOOL, (void*)&result);
	return result;
}

void IDispGenericDocument::SetReadOnly(BOOL propVal)
{
	SetProperty(0x7, VT_BOOL, propVal);
}

CString IDispGenericDocument::GetType()
{
	CString result;
	GetProperty(0x8, VT_BSTR, (void*)&result);
	return result;
}

void IDispGenericDocument::SetType(LPCTSTR propVal)
{
	SetProperty(0x8, VT_BSTR, propVal);
}

LPDISPATCH IDispGenericDocument::GetWindows()
{
	LPDISPATCH result;
	GetProperty(0x9, VT_DISPATCH, (void*)&result);
	return result;
}

void IDispGenericDocument::SetWindows(LPDISPATCH propVal)
{
	SetProperty(0x9, VT_DISPATCH, propVal);
}

BOOL IDispGenericDocument::GetActive()
{
	BOOL result;
	GetProperty(0xa, VT_BOOL, (void*)&result);
	return result;
}

void IDispGenericDocument::SetActive(BOOL propVal)
{
	SetProperty(0xa, VT_BOOL, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// IDispGenericDocument operations

LPDISPATCH IDispGenericDocument::NewWindow()
{
	LPDISPATCH result;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

long IDispGenericDocument::Save(const VARIANT& filename, const VARIANT& longSaveChanges)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0xc, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&filename, &longSaveChanges);
	return result;
}

BOOL IDispGenericDocument::Undo()
{
	BOOL result;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IDispGenericDocument::Redo()
{
	BOOL result;
	InvokeHelper(0xe, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IDispGenericDocument::PrintOut()
{
	BOOL result;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

long IDispGenericDocument::Close(const VARIANT& longSaveChanges)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&longSaveChanges);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IDispGenericWindow properties

CString IDispGenericWindow::GetCaption()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}

CString IDispGenericWindow::GetType()
{
	CString result;
	GetProperty(0x2, VT_BSTR, (void*)&result);
	return result;
}

BOOL IDispGenericWindow::GetActive()
{
	BOOL result;
	GetProperty(0x3, VT_BOOL, (void*)&result);
	return result;
}

void IDispGenericWindow::SetActive(BOOL propVal)
{
	SetProperty(0x3, VT_BOOL, propVal);
}

long IDispGenericWindow::GetLeft()
{
	long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}

void IDispGenericWindow::SetLeft(long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}

long IDispGenericWindow::GetTop()
{
	long result;
	GetProperty(0x5, VT_I4, (void*)&result);
	return result;
}

void IDispGenericWindow::SetTop(long propVal)
{
	SetProperty(0x5, VT_I4, propVal);
}

long IDispGenericWindow::GetHeight()
{
	long result;
	GetProperty(0x6, VT_I4, (void*)&result);
	return result;
}

void IDispGenericWindow::SetHeight(long propVal)
{
	SetProperty(0x6, VT_I4, propVal);
}

long IDispGenericWindow::GetWidth()
{
	long result;
	GetProperty(0x7, VT_I4, (void*)&result);
	return result;
}

void IDispGenericWindow::SetWidth(long propVal)
{
	SetProperty(0x7, VT_I4, propVal);
}

long IDispGenericWindow::GetIndex()
{
	long result;
	GetProperty(0x8, VT_I4, (void*)&result);
	return result;
}

LPDISPATCH IDispGenericWindow::GetNext()
{
	LPDISPATCH result;
	GetProperty(0x9, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispGenericWindow::GetPrevious()
{
	LPDISPATCH result;
	GetProperty(0xa, VT_DISPATCH, (void*)&result);
	return result;
}

CString IDispGenericWindow::Get_Caption()
{
	CString result;
	GetProperty(0x0, VT_BSTR, (void*)&result);
	return result;
}

long IDispGenericWindow::GetWindowState()
{
	long result;
	GetProperty(0xb, VT_I4, (void*)&result);
	return result;
}

void IDispGenericWindow::SetWindowState(long propVal)
{
	SetProperty(0xb, VT_I4, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// IDispGenericWindow operations

long IDispGenericWindow::Close(const VARIANT& boolSaveChanges)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&boolSaveChanges);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IDispGenericProject properties

CString IDispGenericProject::GetName()
{
	CString result;
	GetProperty(0x0, VT_BSTR, (void*)&result);
	return result;
}

CString IDispGenericProject::GetFullName()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}

LPDISPATCH IDispGenericProject::GetApplication()
{
	LPDISPATCH result;
	GetProperty(0x2, VT_DISPATCH, (void*)&result);
	return result;
}

LPDISPATCH IDispGenericProject::GetParent()
{
	LPDISPATCH result;
	GetProperty(0x3, VT_DISPATCH, (void*)&result);
	return result;
}

CString IDispGenericProject::GetType()
{
	CString result;
	GetProperty(0x4, VT_BSTR, (void*)&result);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// IDispGenericProject operations
