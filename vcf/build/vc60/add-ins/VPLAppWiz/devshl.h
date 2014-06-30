// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IGenericDocument wrapper class

class IGenericDocument : public COleDispatchDriver
{
public:
	IGenericDocument() {}		// Calls COleDispatchDriver default constructor
	IGenericDocument(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IGenericDocument(const IGenericDocument& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetName();
	CString GetFullName();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();
	CString GetPath();
	BOOL GetSaved();
	LPDISPATCH GetActiveWindow();
	BOOL GetReadOnly();
	void SetReadOnly(BOOL bNewValue);
	CString GetType();
	LPDISPATCH GetWindows();
	void SetActive(BOOL bNewValue);
	BOOL GetActive();
	LPDISPATCH NewWindow();
	long Save(const VARIANT& vFilename, const VARIANT& vBoolPrompt);
	BOOL Undo();
	BOOL Redo();
	BOOL PrintOut();
	long Close(const VARIANT& vSaveChanges);
};
/////////////////////////////////////////////////////////////////////////////
// IGenericWindow wrapper class

class IGenericWindow : public COleDispatchDriver
{
public:
	IGenericWindow() {}		// Calls COleDispatchDriver default constructor
	IGenericWindow(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IGenericWindow(const IGenericWindow& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetCaption();
	CString GetType();
	void SetActive(BOOL bNewValue);
	BOOL GetActive();
	void SetLeft(long nNewValue);
	long GetLeft();
	void SetTop(long nNewValue);
	long GetTop();
	void SetHeight(long nNewValue);
	long GetHeight();
	void SetWidth(long nNewValue);
	long GetWidth();
	long GetIndex();
	LPDISPATCH GetNext();
	LPDISPATCH GetPrevious();
	void SetWindowState(long nNewValue);
	long GetWindowState();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();
	long Close(const VARIANT& boolSaveChanges);
};
/////////////////////////////////////////////////////////////////////////////
// IGenericProject wrapper class

class IGenericProject : public COleDispatchDriver
{
public:
	IGenericProject() {}		// Calls COleDispatchDriver default constructor
	IGenericProject(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IGenericProject(const IGenericProject& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetName();
	CString GetFullName();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();
	CString GetType();
};
/////////////////////////////////////////////////////////////////////////////
// IApplication wrapper class

class IApplication : public COleDispatchDriver
{
public:
	IApplication() {}		// Calls COleDispatchDriver default constructor
	IApplication(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IApplication(const IApplication& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetHeight();
	void SetHeight(long nNewValue);
	long GetWidth();
	void SetWidth(long nNewValue);
	long GetTop();
	void SetTop(long nNewValue);
	long GetLeft();
	void SetLeft(long nNewValue);
	CString GetName();
	LPDISPATCH GetTextEditor();
	CString GetVersion();
	CString GetPath();
	CString GetCurrentDirectory();
	void SetCurrentDirectory(LPCTSTR lpszNewValue);
	CString GetFullName();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();
	LPDISPATCH GetActiveDocument();
	LPDISPATCH GetWindows();
	LPDISPATCH GetDocuments();
	LPDISPATCH GetActiveWindow();
	long GetWindowState();
	void SetWindowState(long nNewValue);
	LPDISPATCH GetDebugger();
	LPDISPATCH GetProjects();
	LPDISPATCH GetActiveConfiguration();
	void SetActiveConfiguration(LPDISPATCH newValue);
	BOOL GetVisible();
	void SetVisible(BOOL bNewValue);
	LPDISPATCH GetActiveProject();
	void SetActiveProject(LPDISPATCH newValue);
	void SetActive(BOOL bNewValue);
	BOOL GetActive();
	LPDISPATCH GetPackageExtension(LPCTSTR szExtensionName);
	void Quit();
	void PrintToOutputWindow(LPCTSTR Message);
	void ExecuteCommand(LPCTSTR szCommandName);
	void AddCommandBarButton(long nButtonType, LPCTSTR szCmdName, long dwCookie);
	void AddKeyBinding(LPCTSTR szKey, LPCTSTR szCommandName, LPCTSTR szEditor);
	void Build(const VARIANT& Configuration);
	void RebuildAll(const VARIANT& Configuration);
	void ExecuteConfiguration(const VARIANT& Reserved);
	void SetAddInInfo(long nInstanceHandle, LPDISPATCH pCmdDispatch, long nIDBitmapResourceMedium, long nIDBitmapResourceLarge, long dwCookie);
	BOOL AddCommand(LPCTSTR szCmdName, LPCTSTR szMethodName, long nBitmapOffset, long dwCookie);
	void EnableModeless(BOOL bEnable);
	void Clean(const VARIANT& Configuration);
	long GetErrors();
	long GetWarnings();
	void AddProject(LPCTSTR szName, LPCTSTR szPath, LPCTSTR szType, const VARIANT& bAddDefaultFolders);
};
/////////////////////////////////////////////////////////////////////////////
// IDispApplication wrapper class

class IDispApplication : public COleDispatchDriver
{
public:
	IDispApplication() {}		// Calls COleDispatchDriver default constructor
	IDispApplication(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IDispApplication(const IDispApplication& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	long GetHeight();
	void SetHeight(long);
	long GetWidth();
	void SetWidth(long);
	long GetTop();
	void SetTop(long);
	long GetLeft();
	void SetLeft(long);
	LPDISPATCH GetTextEditor();
	CString GetVersion();
	CString GetPath();
	CString GetCurrentDirectory();
	void SetCurrentDirectory(LPCTSTR);
	CString GetFullName();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();
	LPDISPATCH GetActiveDocument();
	LPDISPATCH GetWindows();
	LPDISPATCH GetDocuments();
	LPDISPATCH GetActiveWindow();
	long GetWindowState();
	void SetWindowState(long);
	LPDISPATCH GetDebugger();
	LPDISPATCH GetProjects();
	LPDISPATCH GetActiveConfiguration();
	void SetActiveConfiguration(LPDISPATCH);
	BOOL GetVisible();
	void SetVisible(BOOL);
	LPDISPATCH GetActiveProject();
	void SetActiveProject(LPDISPATCH);
	long GetErrors();
	long GetWarnings();
	CString GetName();
	BOOL GetActive();
	void SetActive(BOOL);

// Operations
public:
	LPDISPATCH GetPackageExtension(LPCTSTR szExtensionName);
	void Quit();
	void PrintToOutputWindow(LPCTSTR Message);
	void ExecuteCommand(LPCTSTR szCommandName);
	void AddCommandBarButton(long nButtonType, LPCTSTR szCmdName, long dwCookie);
	void AddKeyBinding(LPCTSTR szKey, LPCTSTR szCommandName, LPCTSTR szEditor);
	void Build(const VARIANT& Configuration);
	void RebuildAll(const VARIANT& Configuration);
	void ExecuteConfiguration(const VARIANT& Reserved);
	void Clean(const VARIANT& Configuration);
	void AddProject(LPCTSTR szName, LPCTSTR szPath, LPCTSTR szType, const VARIANT& bAddDefaultFolders);
	void SetAddInInfo(long nInstanceHandle, LPDISPATCH pCmdDispatch, long nIDBitmapResourceMedium, long nIDBitmapResourceLarge, long dwCookie);
	BOOL AddCommand(LPCTSTR szCmdName, LPCTSTR szMethodName, long nBitmapOffset, long dwCookie);
	void EnableModeless(BOOL bEnable);
};
/////////////////////////////////////////////////////////////////////////////
// IApplicationEvents wrapper class

class IApplicationEvents : public COleDispatchDriver
{
public:
	IApplicationEvents() {}		// Calls COleDispatchDriver default constructor
	IApplicationEvents(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IApplicationEvents(const IApplicationEvents& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void BeforeBuildStart();
	void BuildFinish(long nNumErrors, long nNumWarnings);
	void BeforeApplicationShutDown();
	void DocumentOpen(LPDISPATCH theDocument);
	void BeforeDocumentClose(LPDISPATCH theDocument);
	void DocumentSave(LPDISPATCH theDocument);
	void NewDocument(LPDISPATCH theDocument);
	void WindowActivate(LPDISPATCH theWindow);
	void WindowDeactivate(LPDISPATCH theWindow);
	void WorkspaceOpen();
	void WorkspaceClose();
	void NewWorkspace();
};
/////////////////////////////////////////////////////////////////////////////
// IDispApplicationEvents wrapper class

class IDispApplicationEvents : public COleDispatchDriver
{
public:
	IDispApplicationEvents() {}		// Calls COleDispatchDriver default constructor
	IDispApplicationEvents(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IDispApplicationEvents(const IDispApplicationEvents& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void BeforeBuildStart();
	void BuildFinish(long nNumErrors, long nNumWarnings);
	void BeforeApplicationShutDown();
	void DocumentOpen(LPDISPATCH theDocument);
	void BeforeDocumentClose(LPDISPATCH theDocument);
	void DocumentSave(LPDISPATCH theDocument);
	void NewDocument(LPDISPATCH theDocument);
	void WindowActivate(LPDISPATCH theWindow);
	void WindowDeactivate(LPDISPATCH theWindow);
	void WorkspaceOpen();
	void WorkspaceClose();
	void NewWorkspace();
};
/////////////////////////////////////////////////////////////////////////////
// IDocuments wrapper class

class IDocuments : public COleDispatchDriver
{
public:
	IDocuments() {}		// Calls COleDispatchDriver default constructor
	IDocuments(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IDocuments(const IDocuments& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetCount();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();
	LPDISPATCH Item(const VARIANT& Index);
	long SaveAll(const VARIANT& vtBoolPrompt);
	long CloseAll(const VARIANT& vtSaveChanges);
	LPDISPATCH Add(LPCTSTR docType, const VARIANT& vtReserved);
	LPDISPATCH Open(LPCTSTR filename, const VARIANT& vtDocType, const VARIANT& vtBoolReadOnly);
};
/////////////////////////////////////////////////////////////////////////////
// IDispDocuments wrapper class

class IDispDocuments : public COleDispatchDriver
{
public:
	IDispDocuments() {}		// Calls COleDispatchDriver default constructor
	IDispDocuments(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IDispDocuments(const IDispDocuments& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	long GetCount();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();

// Operations
public:
	long SaveAll(const VARIANT& boolPrompt);
	long CloseAll(const VARIANT& boolSaveChanges);
	LPDISPATCH Add(LPCTSTR docType, const VARIANT& vtReserved);
	LPDISPATCH Open(LPCTSTR filename, const VARIANT& docType, const VARIANT& ReadOnly);
	LPDISPATCH Item(const VARIANT& Index);
};
/////////////////////////////////////////////////////////////////////////////
// IWindows wrapper class

class IWindows : public COleDispatchDriver
{
public:
	IWindows() {}		// Calls COleDispatchDriver default constructor
	IWindows(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IWindows(const IWindows& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetCount();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();
	LPDISPATCH Item(const VARIANT& Index);
	void Arrange(const VARIANT& vtArrangeStyle);
	long CloseAll(const VARIANT& vtSaveChanges);
};
/////////////////////////////////////////////////////////////////////////////
// IDispWindows wrapper class

class IDispWindows : public COleDispatchDriver
{
public:
	IDispWindows() {}		// Calls COleDispatchDriver default constructor
	IDispWindows(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IDispWindows(const IDispWindows& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	long GetCount();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();

// Operations
public:
	LPDISPATCH Item(const VARIANT& Index);
	long CloseAll(const VARIANT& vtSaveChanges);
};
/////////////////////////////////////////////////////////////////////////////
// IProjects wrapper class

class IProjects : public COleDispatchDriver
{
public:
	IProjects() {}		// Calls COleDispatchDriver default constructor
	IProjects(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IProjects(const IProjects& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetCount();
	LPDISPATCH GetParent();
	LPDISPATCH Item(const VARIANT& Index);
};
/////////////////////////////////////////////////////////////////////////////
// IDispProjects wrapper class

class IDispProjects : public COleDispatchDriver
{
public:
	IDispProjects() {}		// Calls COleDispatchDriver default constructor
	IDispProjects(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IDispProjects(const IDispProjects& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	LPDISPATCH GetApplication();
	long GetCount();
	LPDISPATCH GetParent();

// Operations
public:
	LPDISPATCH Item(const VARIANT& Index);
};
/////////////////////////////////////////////////////////////////////////////
// IDispGenericDocument wrapper class

class IDispGenericDocument : public COleDispatchDriver
{
public:
	IDispGenericDocument() {}		// Calls COleDispatchDriver default constructor
	IDispGenericDocument(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IDispGenericDocument(const IDispGenericDocument& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	CString GetName();
	CString GetFullName();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();
	CString GetPath();
	BOOL GetSaved();
	void SetSaved(BOOL);
	LPDISPATCH GetActiveWindow();
	void SetActiveWindow(LPDISPATCH);
	BOOL GetReadOnly();
	void SetReadOnly(BOOL);
	CString GetType();
	void SetType(LPCTSTR);
	LPDISPATCH GetWindows();
	void SetWindows(LPDISPATCH);
	BOOL GetActive();
	void SetActive(BOOL);

// Operations
public:
	LPDISPATCH NewWindow();
	long Save(const VARIANT& filename, const VARIANT& longSaveChanges);
	BOOL Undo();
	BOOL Redo();
	BOOL PrintOut();
	long Close(const VARIANT& longSaveChanges);
};
/////////////////////////////////////////////////////////////////////////////
// IDispGenericWindow wrapper class

class IDispGenericWindow : public COleDispatchDriver
{
public:
	IDispGenericWindow() {}		// Calls COleDispatchDriver default constructor
	IDispGenericWindow(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IDispGenericWindow(const IDispGenericWindow& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	CString GetCaption();
	CString GetType();
	BOOL GetActive();
	void SetActive(BOOL);
	long GetLeft();
	void SetLeft(long);
	long GetTop();
	void SetTop(long);
	long GetHeight();
	void SetHeight(long);
	long GetWidth();
	void SetWidth(long);
	long GetIndex();
	LPDISPATCH GetNext();
	LPDISPATCH GetPrevious();
	CString Get_Caption();
	long GetWindowState();
	void SetWindowState(long);

// Operations
public:
	long Close(const VARIANT& boolSaveChanges);
};
/////////////////////////////////////////////////////////////////////////////
// IDispGenericProject wrapper class

class IDispGenericProject : public COleDispatchDriver
{
public:
	IDispGenericProject() {}		// Calls COleDispatchDriver default constructor
	IDispGenericProject(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IDispGenericProject(const IDispGenericProject& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	CString GetName();
	CString GetFullName();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();
	CString GetType();

// Operations
public:
};
