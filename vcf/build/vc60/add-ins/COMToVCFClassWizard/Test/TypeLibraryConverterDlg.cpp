// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Test.h"
#include "TypeLibraryConverterDlg.h"
#include "TypeLibContents.h"
#include <comdef.h>
#include "ConversionOptionsDlg.h"
#include "TypelibDump.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TypeLibraryConverterDlg dialog

TypeLibraryConverterDlg::TypeLibraryConverterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TypeLibraryConverterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TypeLibraryConverterDlg)
	m_cppDir = _T("");
	m_headerDir = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_getMethodPrefix = _T("get");
	m_setMethodPrefix = _T("set");
	m_memberPrefix = _T("m_");
	m_singleUnitPerClass = TRUE;
	m_classPrefix = _T("");

	m_fileDistributionType = -1;
}

void TypeLibraryConverterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TypeLibraryConverterDlg)
	DDX_Control(pDX, IDC_CONVERT_PROGRESS, m_conversionProgress);
	DDX_Control(pDX, IDC_PROGRESS_LABEL, m_progressLabel);
	DDX_Control(pDX, IDC_TYPELIB_LIST, m_typeLibList);
	DDX_Text(pDX, IDC_CPP_DIR, m_cppDir);
	DDX_Text(pDX, IDC_HEADER_DIR, m_headerDir);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TypeLibraryConverterDlg, CDialog)
	//{{AFX_MSG_MAP(TypeLibraryConverterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONVERT, OnConvert)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDC_BROWSE_FOR_HDR_DIR, OnBrowseForHdrDir)
	ON_BN_CLICKED(IDC_BROWSE_FOR_CPP_DIR, OnBrowseForCppDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TypeLibraryConverterDlg message handlers

BOOL TypeLibraryConverterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LoadSettingsFromRegistry();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_imageList.Create( 16, 16, ILC_COLOR4 | ILC_MASK, 0, 5 );
	CBitmap bmp;
	bmp.LoadBitmap( IDB_TYPELIB );
	m_imageList.Add( &bmp, RGB(0,255,0) );
	bmp.Detach();

	bmp.LoadBitmap( IDB_INTERFACE );
	m_imageList.Add( &bmp, RGB(0,255,0) );
	bmp.Detach();

	bmp.LoadBitmap( IDB_COCLASS );
	m_imageList.Add( &bmp, RGB(0,255,0) );
	bmp.Detach();	

	bmp.LoadBitmap( IDB_INSERTABLE_COCLASS );
	m_imageList.Add( &bmp, RGB(0,255,0) );
	bmp.Detach();

	this->m_typeLibList.SetImageList( &m_imageList, LVSIL_SMALL );

	m_typeLibList.InsertColumn( 0, "Registered Type Libraries", LVCFMT_LEFT, 400 );
	ExpandTypeLibs();
	
	TCHAR currentDir[MAX_PATH];
	memset(currentDir, 0, MAX_PATH);
	GetCurrentDirectory( MAX_PATH, currentDir );
	this->m_cppDir = currentDir;
	if ( m_cppDir.GetAt(m_cppDir.GetLength()-1) != '\\' ) {
		m_cppDir += "\\";
	}
	this->m_headerDir = currentDir;
	if ( m_headerDir.GetAt(m_headerDir.GetLength()-1) != '\\' ) {
		m_headerDir += "\\";
	}

	UpdateData( FALSE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void TypeLibraryConverterDlg::ExpandTypeLibs()
{
	USES_CONVERSION;
	HKEY    hkTypeLib ;
	if (RegOpenKey( HKEY_CLASSES_ROOT, _T("TypeLib"), &hkTypeLib) == ERROR_SUCCESS)
	{
		LONG    cb ;
		TCHAR    szVer[64] ;
		TCHAR    szName[64] ;
		TCHAR    szGUID[64] ;
		TCHAR    szBuf[256] ;
		DWORD   dwIndex, dwIndex2 ;
		HKEY    hkGUID ;

		// Enum \TypeInfo
		for ( dwIndex = 0 ;
			  RegEnumKey( hkTypeLib, dwIndex, szGUID, sizeof(szGUID)) == ERROR_SUCCESS ;
			  ++dwIndex )
		{
			// Gotta open \TypeInfo\szGUID
			if (RegOpenKey( hkTypeLib, szGUID, &hkGUID ) == ERROR_SUCCESS)
			{
				// Enum all at this level (major.minor = name)
				for (dwIndex2 = 0 ;
					RegEnumKey( hkGUID, dwIndex2, szVer, sizeof(szVer)) == ERROR_SUCCESS ;
					++dwIndex2 )
				{
					cb = sizeof(szName);
					if (RegQueryValue( hkGUID, (LPTSTR)szVer, szName, &cb) == ERROR_SUCCESS)
					{
						wsprintf( szBuf, _T("%s (Ver %s)"), (LPTSTR)szName, (LPTSTR)szVer ) ;
						
						GUID classID = {0};
						PTSTR p = strrchr( szVer, '.' ) ;
						
						::CLSIDFromString(T2OLE(szGUID), &classID);
						TypeLibEntry* pTLBEntry = new TypeLibEntry( classID, (WORD)atoi( szVer ), (WORD)atoi( p+1 ) );
						m_tlbEntrys.push_back( pTLBEntry );
						//
						//lpOD->m_szProgID[0] = '\0' ;

						//lpOD->m_uiBitmap = BMINDEX(IDB_AUTOMATION) ;
						//lpOD->m_wMajorVer =  ;
						
						//if (p)
						//	lpOD->m_wMinorVer =  ;
						//else
						//	lpOD->m_wMinorVer = 0 ;

						LVITEM item = {0} ;
						item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
						item.iItem  = dwIndex;
						item.iImage = 0;
						item.lParam = (LPARAM)pTLBEntry;
						pTLBEntry->m_tlbName = szBuf;
						item.pszText = szBuf;
						item.cchTextMax = strlen( szBuf );
						m_typeLibList.InsertItem( &item );
						
					}
				}
				RegCloseKey( hkGUID ) ;
			}
		}
		RegCloseKey( hkTypeLib ) ;
	}
}

void TypeLibraryConverterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void TypeLibraryConverterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR TypeLibraryConverterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void TypeLibraryConverterDlg::OnConvert() 
{
	ITypeLib* pTypeLib ;
	LCID      lcid = GetUserDefaultLCID() ;
	int i = m_typeLibList.GetSelectionMark();
	CString s = m_typeLibList.GetItemText( i, 0 );
	TRACE( "Item %d, text = %s\n", i, s.GetBuffer(0) );
	s.ReleaseBuffer();

	if ( i >= 0 ) {
		//
		TypeLibEntry* tlb = (TypeLibEntry*)m_typeLibList.GetItemData( i ); //m_tlbEntrys[i];
		HRESULT hr = ::LoadRegTypeLib( tlb->m_classID, tlb->m_wMajorVer,
								   tlb->m_wMinorVer, lcid, &pTypeLib ) ;
		if ( SUCCEEDED( hr ) ) {
			TypeLibContents dlg;
			CString label;
			wchar_t tmp[256];
			memset( tmp, 0, sizeof(wchar_t)*256 );
			LPOLESTR tmpOleStr = tmp;
			::StringFromCLSID( tlb->m_classID, &tmpOleStr );
			_bstr_t clsid = tmpOleStr;

			label.Format( "Listing for Type Library \"%s\", CLSID %s",  tlb->m_tlbName.GetBuffer(0), (char*)clsid );
			tlb->m_tlbName.ReleaseBuffer();

			dlg.m_typeLibLabel = label;
			dlg.m_pTypeLib = pTypeLib;
			if ( IDOK == dlg.DoModal() ) {
				m_convertCoClasses = dlg.m_convertCoClasses;
				m_useATLForCoClassImpl = dlg.m_useATLForCoClassImpl;
				m_useTLBAsNamespace = dlg.m_useTLBAsNamespace;
				TypeLibHolder tlib;
				CWaitCursor wait;
				hr = GenerateTypeLibHolder( pTypeLib, tlib );
				if ( SUCCEEDED(hr) ) {
					InitializeProgress( &tlib );
					GenerateImplementation( &tlib );
					FinishProgress();
				}
			}
			int i = pTypeLib->Release();
		}
	}
	
}

void TypeLibraryConverterDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	SaveSettingsToRegistry();

	std::vector<TypeLibEntry*>::iterator it = m_tlbEntrys.begin();
	while ( it != m_tlbEntrys.end() ) {
		TypeLibEntry* tlb = *it;
		delete tlb;
		tlb = NULL;
		it++;
	}
	m_tlbEntrys.clear();
	
}

void TypeLibraryConverterDlg::OnOptions() 
{
	ConversionOptionsDlg dlg;
	dlg.m_classPrefix = this->m_classPrefix;
	dlg.m_getMethodPrefix = this->m_getMethodPrefix;
	dlg.m_memberPrefix = this->m_memberPrefix;
	dlg.m_setMethodPrefix = this->m_setMethodPrefix;
	dlg.m_singleUnitPerClass = this->m_singleUnitPerClass;
	dlg.m_fileDistributionType = this->m_fileDistributionType;
	if ( IDOK == dlg.DoModal() ) {
		this->m_classPrefix = dlg.m_classPrefix;
		this->m_getMethodPrefix = dlg.m_getMethodPrefix;
		this->m_memberPrefix = dlg.m_memberPrefix;
		this->m_setMethodPrefix = dlg.m_setMethodPrefix;
		this->m_singleUnitPerClass = dlg.m_singleUnitPerClass;
		this->m_fileDistributionType = dlg.m_fileDistributionType;
		this->SaveSettingsToRegistry();
	}
}

CString TypeLibraryConverterDlg::GenerateCoClassHeader( CoClassHolder* pCoClass, TypeLibHolder* pTypeLibHolder )
{
	CString header = "";
	CString uuid = (TCHAR*)pCoClass->m_classID;
	uuid.Delete(0,1);
	uuid.Delete(uuid.GetLength()-1,1);
	CString coClassName = (TCHAR*)pTypeLibHolder->m_typeLibName;
	coClassName += "_" + pCoClass->m_className;
	coClassName.MakeUpper();
	
	
	header = "";
	header += "//";
	header += pCoClass->m_className;
	header += ".h\n\n";
	header += "#ifndef " + coClassName + "_H__\n";
	header += "#define " + coClassName + "_H__\n";
	header += "#include \"FoundationKit.h\"\n\n";
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += "namespace ";
		header += pTypeLibHolder->m_typeLibName;
		header += "  {\n\n";
	}
	
	coClassName += "_CLASSID";
	header += "#define " + coClassName + " \t\t\"";
	header += uuid + "\"\n";
	header += "/**\n";
	header += "*class " + pCoClass->m_className + "\n";
	header += "*UUID: " + uuid + "\n";
	header += "*ProgID: " + pCoClass->m_progID + "\n";
	header += "*/\n";
	header += "class " + pCoClass->m_className + " : public VCF::Object";
	
	CString s;
	s += "Adding CoClass " + pCoClass->m_className + "...";
	UpdateProgress( s );
	
	int interfaceCount = pCoClass->m_implementedInterfaces.size();
	if ( interfaceCount > 0 ) {
		header += ", ";
	}
	for ( int j=0;j<interfaceCount;j++) {
		InterfaceHolder* pImplInterface = pCoClass->m_implementedInterfaces[j];
		header += "public " + pImplInterface->m_interfaceName;
		s = "";
		s += "CoClass " + pCoClass->m_className + " implements " + pImplInterface->m_interfaceName;
		UpdateProgress( s );
		if ( j < (interfaceCount-1) ) {
			header += ", ";
		}
	}
	header += " {\n";
	header += "public:\n";
	header += "\tBEGIN_CLASSINFO( " + pCoClass->m_className + ", \"";
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += pTypeLibHolder->m_typeLibName + "::";
	}
	header += pCoClass->m_className + "\", \"VCF::Object\", ";
	header += coClassName + " );\n";
	header += "\tEND_CLASSINFO( " + pCoClass->m_className + " );\n\n";
	header += "\t" + pCoClass->m_className + "();\n\n";
	header += "\tvirtual ~" + pCoClass->m_className + "();\n\n";
	for ( j=0;j<interfaceCount;j++) {
		InterfaceHolder* pImplInterface = pCoClass->m_implementedInterfaces[j];
		int methodCount = pImplInterface->m_methods.size();
		for (int m=0;m<methodCount;m++) {
			s = "";					
			s.Format( "Adding method %d of %d for interface %s", m, methodCount, (char*)pImplInterface->m_interfaceName );
			UpdateProgress( s );
			
			MethodHolder* pMethod = pImplInterface->m_methods[m];
			header += "\tvirtual " + pMethod->m_returnType + " ";
			if ( pMethod->m_isPropertyGetter ) {
				header += this->m_getMethodPrefix;
			}
			else if ( pMethod->m_isPropertySetter ) {
				header += this->m_setMethodPrefix;
			}
			
			header += pMethod->m_declaration + ";";
			
			CString siName = (TCHAR*)pImplInterface->m_superInterfaceName;
			if ( siName == "IDispatch" ) {
				header += "//[id(" + pMethod->m_methodID + ")]";
			}
			header += "\n\n";
		}
	}
	header += "};\n\n";
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += "}  //end of namespace ";
		header += pTypeLibHolder->m_typeLibName;
		header += "\n\n";
	}
	
	header += "#endif //";
	header += s;
	
	header += "\n\n";

	return header;
}

CString TypeLibraryConverterDlg::GenerateCoClassImpl( CoClassHolder* pCoClass, TypeLibHolder* pTypeLibHolder )
{
	CString coClassImpl = "";	
	coClassImpl += "//";
	coClassImpl += pCoClass->m_className;
	coClassImpl += ".cpp\n\n";
	
	if ( TRUE == m_singleUnitPerClass ) {
		coClassImpl += "#include \"" + pTypeLibHolder->m_typeLibName + ".h\"\n";
		coClassImpl += "#include \"" + pTypeLibHolder->m_typeLibName + "Interfaces.h\"\n";
	}
	else {		
		switch ( this->m_fileDistributionType ) {
			case SINGLE_FILE_FOR_TLB : {
				coClassImpl += "#include \"" + pTypeLibHolder->m_typeLibName + ".h\"\n";
			}
			break;

			case SINGLE_FILE_FOR_IFACE_COCLASS : {
				coClassImpl += "#include \"" + pTypeLibHolder->m_typeLibName + "Interfaces.h\"\n";
			}
			break;

			case SINGLE_FILE_FOR_IFACE_TYPEDEF_COCLASS : {
				coClassImpl += "#include \"" + pTypeLibHolder->m_typeLibName + "TypeDefs.h\"\n";
				coClassImpl += "#include \"" + pTypeLibHolder->m_typeLibName + "Interfaces.h\"\n";
			}
			break;
		}
	}

	coClassImpl += "#include \"" + pCoClass->m_className + ".h\"\n\n";
	coClassImpl += "using namespace VCF;\n";
	if ( TRUE == this->m_useTLBAsNamespace ) {
		coClassImpl += "using namespace " + pTypeLibHolder->m_typeLibName + ";\n\n";
	}
	
	coClassImpl += pCoClass->m_className + "::" + pCoClass->m_className + "()\n";
	coClassImpl += "{\n\n}\n\n";

	coClassImpl += pCoClass->m_className + "::~" + pCoClass->m_className + "()\n";
	coClassImpl += "{\n\n}\n\n";

	int interfaceCount = pCoClass->m_implementedInterfaces.size();
	for ( int j=0;j<interfaceCount;j++) {
		InterfaceHolder* pImplInterface = pCoClass->m_implementedInterfaces[j];
		int methodCount = pImplInterface->m_methods.size();
		for (int m=0;m<methodCount;m++) {
			CString s = "";					
			s.Format( "Adding method %d of %d for interface %s", m, methodCount, (char*)pImplInterface->m_interfaceName );
			UpdateProgress( s );
			
			MethodHolder* pMethod = pImplInterface->m_methods[m];
			coClassImpl += pMethod->m_returnType + " " + pCoClass->m_className + "::";
			if ( pMethod->m_isPropertyGetter ) {
				coClassImpl += this->m_getMethodPrefix;
			}
			else if ( pMethod->m_isPropertySetter ) {
				coClassImpl += this->m_setMethodPrefix;
			}
			
			coClassImpl += pMethod->m_declaration + "\n{\n";
			if ( pMethod->m_isPropertyGetter ) {
				coClassImpl += "\t" + pMethod->m_returnType + " result;\n\n";
				coClassImpl += "\treturn result;\n";
			}
			else {
				coClassImpl += "\n";
			}
			coClassImpl += "}\n\n";
		}
	}

	return coClassImpl;
}


CString TypeLibraryConverterDlg::GenerateInterface( InterfaceHolder* pInterface, TypeLibHolder* pTypeLibHolder )
{
	CString header = "";
	CString uuid = (TCHAR*)pInterface->m_interfaceID;
	uuid.Delete(0,1);
	uuid.Delete(uuid.GetLength()-1,1);
	header += "/**\n";
	header += "*Interface " + pInterface->m_interfaceName + "\n";
	header += "*UUID: " + uuid + "\n";
	header += "*/\n";
	header += "class " + pInterface->m_interfaceName + " : public Interface { \n";
	header += "public:\n";
	header += "\tvirtual ~" + pInterface->m_interfaceName + "(){};\n\n";
	CString s;
	s += "Adding Interface " + pInterface->m_interfaceName + "...";
	UpdateProgress( s );
	int methodCount = pInterface->m_methods.size();
	for (int i=0;i<methodCount;i++) {
		MethodHolder* pMethod = pInterface->m_methods[i];
		s = "";
		s.Format( "Adding method %d of %d for interface %s", i, methodCount, (char*)pInterface->m_interfaceName );
		UpdateProgress( s );
		header += "\tvirtual " + pMethod->m_returnType + " ";
		if ( pMethod->m_isPropertyGetter ) {
			header += this->m_getMethodPrefix;
		}
		else if ( pMethod->m_isPropertySetter ) {
			header += this->m_setMethodPrefix;
		}
		
		header += pMethod->m_declaration + " = 0;";
		
		CString siName = (TCHAR*)pInterface->m_superInterfaceName;
		if ( siName == "IDispatch" ) {
			header += "//[id(" + pMethod->m_methodID + ")]";
		}
		header += "\n\n";
	}
	header += "};\n\n\n";
	return header;
}

void TypeLibraryConverterDlg::GenerateSingleUnitperClassImpl( TypeLibHolder* pTypeLibHolder )
{
	CString header;
	header += "//";
	header += pTypeLibHolder->m_typeLibName.copy();
	header += ".h\n\n";
	header += "#ifndef ";
	
	CString s = (char*)pTypeLibHolder->m_typeLibName;
	s.MakeUpper();
	s = "_" + s + "_H__";
	header += s.GetBuffer(0);
	s.ReleaseBuffer();
	header += "\n";
	header += "#define ";
	header += s.GetBuffer(0);
	s.ReleaseBuffer();
	header += "\n\n";
	header += "#include \"FoundationKit.h\"\n\n";
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += "namespace ";
		header += pTypeLibHolder->m_typeLibName;
		header += "  {\n\n";
	}

	header += GenerateClassRegFunction( pTypeLibHolder );

	header += "//Typelibrary typedefs\n\n";
	header += pTypeLibHolder->m_typeDefs;
	header += "\n//End of Typelibrary typedefs\n\n";
	
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += "}  //end of namespace ";
		header += pTypeLibHolder->m_typeLibName;
		header += "\n\n";
	}
	
	header += "#endif //";
	header += s.GetBuffer(0);
	s.ReleaseBuffer();
	
	header += "\n\n";
	
	CString headerFileName = "foo";
	headerFileName = m_headerDir;
	headerFileName += pTypeLibHolder->m_typeLibName;
	headerFileName += ".h";
	CStdioFile headerFile( headerFileName, CFile::modeCreate | CFile::modeWrite );
	headerFile.WriteString( header.GetBuffer(0) );
	header.ReleaseBuffer();
	headerFile.Close();
	
	//do interfaces
	header = "";
	header += "//";
	header += pTypeLibHolder->m_typeLibName.copy();
	header += ".h\n\n";
	header += "#ifndef ";
	
	s = (char*)pTypeLibHolder->m_typeLibName;
	s += "Interfaces";
	s.MakeUpper();
	s = "_" + s + "_H__";
	header += s;
	header += "\n";
	header += "#define ";
	header += s;
	header += "\n\n";
	header += "#include \"FoundationKit.h\"\n\n";
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += "namespace ";
		header += pTypeLibHolder->m_typeLibName;
		header += "  {\n\n";
	}
	
	std::map<_bstr_t,InterfaceHolder*>::iterator it = pTypeLibHolder->m_interfaces.begin();
	while ( it != pTypeLibHolder->m_interfaces.end() ) {
		InterfaceHolder* pInterface = it->second;
		header += GenerateInterface( pInterface, pTypeLibHolder );
		it++;
	}
	
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += "}  //end of namespace ";
		header += pTypeLibHolder->m_typeLibName;
		header += "\n\n";
	}
	
	header += "#endif //";
	header += s;
	
	header += "\n\n";
	
	headerFileName = m_headerDir;
	headerFileName += pTypeLibHolder->m_typeLibName;
	headerFileName += "Interfaces.h";
	headerFile.Open( headerFileName, CFile::modeCreate | CFile::modeWrite );
	headerFile.WriteString( header.GetBuffer(0) );
	header.ReleaseBuffer();
	headerFile.Close();
	
	//do coClasses
	int coClassCount = pTypeLibHolder->m_coClasses.size();
	for ( int i=0;i<coClassCount;i++) {
		CoClassHolder* pCoClass = pTypeLibHolder->m_coClasses[i];
		
		header = GenerateCoClassHeader( pCoClass, pTypeLibHolder );
		
		headerFileName = m_headerDir;
		headerFileName += pCoClass->m_className;
		headerFileName += ".h";
		headerFile.Open( headerFileName, CFile::modeCreate | CFile::modeWrite );
		headerFile.WriteString( header.GetBuffer(0) );
		header.ReleaseBuffer();
		headerFile.Close();

		CString coClassImpl = GenerateCoClassImpl( pCoClass, pTypeLibHolder );
		
		headerFileName = m_cppDir;
		headerFileName += pCoClass->m_className;
		headerFileName += ".cpp";
		headerFile.Open( headerFileName, CFile::modeCreate | CFile::modeWrite );
		headerFile.WriteString( coClassImpl.GetBuffer(0) );
		coClassImpl.ReleaseBuffer();
		headerFile.Close(); 
	}

	CString tlbImpl = GenerateTLBImpl( pTypeLibHolder );
	headerFileName = m_cppDir;
	headerFileName += pTypeLibHolder->m_typeLibName;
	headerFileName += ".cpp";
	headerFile.Open( headerFileName, CFile::modeCreate | CFile::modeWrite );
	headerFile.WriteString( tlbImpl.GetBuffer(0) );
	tlbImpl.ReleaseBuffer();
	headerFile.Close();	
}

void TypeLibraryConverterDlg::GenerateSingleFileForTLB( TypeLibHolder* pTypeLibHolder )
{
	CString header;
	header += "//";
	header += pTypeLibHolder->m_typeLibName.copy();
	header += ".h\n\n";
	header += "#ifndef ";
	
	CString s = (char*)pTypeLibHolder->m_typeLibName;
	s.MakeUpper();
	s = "_" + s + "_H__";
	header += s.GetBuffer(0);
	s.ReleaseBuffer();
	header += "\n";
	header += "#define ";
	header += s.GetBuffer(0);
	s.ReleaseBuffer();
	header += "\n\n";
	header += "#include \"FoundationKit.h\"\n\n";
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += "namespace ";
		header += pTypeLibHolder->m_typeLibName;
		header += "  {\n\n";
	}

	header += GenerateClassRegFunction( pTypeLibHolder );

	header += "//Typelibrary typedefs\n\n";
	header += pTypeLibHolder->m_typeDefs;
	header += "\n//End of Typelibrary typedefs\n\n";
	
	UpdateProgress( "Type Defs completed..." );
	
	std::map<_bstr_t,InterfaceHolder*>::iterator it = pTypeLibHolder->m_interfaces.begin();
	while ( it != pTypeLibHolder->m_interfaces.end() ) {
		InterfaceHolder* pInterface = it->second;
		CString uuid = (TCHAR*)pInterface->m_interfaceID;
		uuid.Delete(0,1);
		uuid.Delete(uuid.GetLength()-1,1);
		header += "/**\n";
		header += "*Interface " + pInterface->m_interfaceName + "\n";
		header += "*UUID: " + uuid + "\n";
		header += "*/\n";
		header += "class " + pInterface->m_interfaceName + " : public Interface { \n";
		header += "public:\n";
		header += "\tvirtual ~" + pInterface->m_interfaceName + "(){};\n\n";
		CString s;
		s += "Adding Interface " + pInterface->m_interfaceName + "...";
		UpdateProgress( s );
		int methodCount = pInterface->m_methods.size();
		for (int i=0;i<methodCount;i++) {
			MethodHolder* pMethod = pInterface->m_methods[i];
			s = "";
			s.Format( "Adding method %d of %d for interface %s", i, methodCount, (char*)pInterface->m_interfaceName );
			UpdateProgress( s );
			header += "\tvirtual " + pMethod->m_returnType + " ";
			if ( pMethod->m_isPropertyGetter ) {
				header += this->m_getMethodPrefix;
			}
			else if ( pMethod->m_isPropertySetter ) {
				header += this->m_setMethodPrefix;
			}
			
			header += pMethod->m_declaration + " = 0;";
			
			CString siName = (TCHAR*)pInterface->m_superInterfaceName;
			if ( siName == "IDispatch" ) {
				header += "//[id(" + pMethod->m_methodID + ")]";
			}
			header += "\n\n";
		}
		header += "};\n\n\n";
		it++;
	}		
	
	
	int coClassCount = pTypeLibHolder->m_coClasses.size();
	for ( int i=0;i<coClassCount;i++) {
		CoClassHolder* pCoClass = pTypeLibHolder->m_coClasses[i];
		
		CString uuid = (TCHAR*)pCoClass->m_classID;
		uuid.Delete(0,1);
		uuid.Delete(uuid.GetLength()-1,1);
		CString coClassName = (TCHAR*)pTypeLibHolder->m_typeLibName;
		coClassName += "_" + pCoClass->m_className;
		coClassName.MakeUpper();
		coClassName += "_CLASSID";
		header += "#define " + coClassName + " \t\t\"";
		header += uuid + "\"\n";
		header += "/**\n";
		header += "*class " + pCoClass->m_className + "\n";
		header += "*UUID: " + uuid + "\n";
		header += "*ProgID: " + pCoClass->m_progID + "\n";
		header += "*/\n";
		header += "class " + pCoClass->m_className + " : public VCF::Object";
		
		CString s;
		s += "Adding CoClass " + pCoClass->m_className + "...";
		UpdateProgress( s );
		
		int interfaceCount = pCoClass->m_implementedInterfaces.size();
		if ( interfaceCount > 0 ) {
			header += ", ";
		}
		for ( int j=0;j<interfaceCount;j++) {
			InterfaceHolder* pImplInterface = pCoClass->m_implementedInterfaces[j];
			header += "public " + pImplInterface->m_interfaceName;
			s = "";
			s += "CoClass " + pCoClass->m_className + " implements " + pImplInterface->m_interfaceName;
			UpdateProgress( s );
			if ( j < (interfaceCount-1) ) {
				header += ", ";
			}
		}
		header += " {\n";
		header += "public:\n";
		header += "\tBEGIN_CLASSINFO( " + pCoClass->m_className + ", \"";
		if ( TRUE == this->m_useTLBAsNamespace ) {
			header += pTypeLibHolder->m_typeLibName + "::";
		}
		header += pCoClass->m_className + "\", \"VCF::Object\", ";
		header += coClassName + " );\n";
		header += "\tEND_CLASSINFO( " + pCoClass->m_className + " );\n\n";
		header += "\t" + pCoClass->m_className + "();\n\n";
		header += "\tvirtual ~" + pCoClass->m_className + "();\n\n";
		for ( j=0;j<interfaceCount;j++) {
			InterfaceHolder* pImplInterface = pCoClass->m_implementedInterfaces[j];
			int methodCount = pImplInterface->m_methods.size();
			for (int m=0;m<methodCount;m++) {
				s = "";					
				s.Format( "Adding method %d of %d for interface %s", m, methodCount, (char*)pImplInterface->m_interfaceName );
				UpdateProgress( s );
				
				MethodHolder* pMethod = pImplInterface->m_methods[m];
				header += "\tvirtual " + pMethod->m_returnType + " ";
				if ( pMethod->m_isPropertyGetter ) {
					header += this->m_getMethodPrefix;
				}
				else if ( pMethod->m_isPropertySetter ) {
					header += this->m_setMethodPrefix;
				}
				
				header += pMethod->m_declaration + ";";
				
				CString siName = (TCHAR*)pImplInterface->m_superInterfaceName;
				if ( siName == "IDispatch" ) {
					header += "//[id(" + pMethod->m_methodID + ")]";
				}
				header += "\n\n";
			}
		}
		header += "};\n\n";
		
	}
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += "}  //end of namespace ";
		header += pTypeLibHolder->m_typeLibName;
		header += "\n\n";
	}
	
	header += "#endif //";
	header += s.GetBuffer(0);
	s.ReleaseBuffer();
	
	header += "\n\n";
	
	CString headerFileName = "foo";
	headerFileName = m_headerDir;
	headerFileName += pTypeLibHolder->m_typeLibName;
	headerFileName += ".h";
	CStdioFile headerFile( headerFileName, CFile::modeCreate | CFile::modeWrite );
	headerFile.WriteString( header.GetBuffer(0) );
	header.ReleaseBuffer();
	headerFile.Close();

	CString tlbImpl = GenerateTLBImpl( pTypeLibHolder );
	headerFileName = m_cppDir;
	headerFileName += pTypeLibHolder->m_typeLibName;
	headerFileName += ".cpp";
	headerFile.Open( headerFileName, CFile::modeCreate | CFile::modeWrite );
	headerFile.WriteString( tlbImpl.GetBuffer(0) );
	tlbImpl.ReleaseBuffer();
	headerFile.Close();	
}

void TypeLibraryConverterDlg::GenerateSingleFileForIFaceSeparateCoClasses( TypeLibHolder* pTypeLibHolder )
{
	CString header;
	header += "//";
	header += pTypeLibHolder->m_typeLibName.copy();
	header += "Interfaces.h";
	header += ".h\n\n";
	header += "#ifndef ";
	
	CString s = (char*)pTypeLibHolder->m_typeLibName;
	s += "_Interfaces";
	s.MakeUpper();
	s = "_" + s + "_H__";
	header += s.GetBuffer(0);
	s.ReleaseBuffer();
	header += "\n";
	header += "#define ";
	header += s.GetBuffer(0);
	s.ReleaseBuffer();
	header += "\n\n";
	header += "#include \"FoundationKit.h\"\n\n";
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += "namespace ";
		header += pTypeLibHolder->m_typeLibName;
		header += "  {\n\n";
	}

	header += GenerateClassRegFunction( pTypeLibHolder );

	header += "//Typelibrary typedefs\n\n";
	header += pTypeLibHolder->m_typeDefs;
	header += "\n//End of Typelibrary typedefs\n\n";
	
	
	std::map<_bstr_t,InterfaceHolder*>::iterator it = pTypeLibHolder->m_interfaces.begin();
	while ( it != pTypeLibHolder->m_interfaces.end() ) {
		InterfaceHolder* pInterface = it->second;
		header += GenerateInterface( pInterface, pTypeLibHolder );
		it++;
	}
	
	if ( TRUE == this->m_useTLBAsNamespace ) {
		header += "}  //end of namespace ";
		header += pTypeLibHolder->m_typeLibName;
		header += "\n\n";
	}
	
	header += "#endif //";
	header += s;
	
	header += "\n\n";
	
	CString headerFileName = m_headerDir;
	headerFileName += pTypeLibHolder->m_typeLibName;
	headerFileName += "Interfaces.h";
	CStdioFile headerFile;
	headerFile.Open( headerFileName, CFile::modeCreate | CFile::modeWrite );
	headerFile.WriteString( header.GetBuffer(0) );
	header.ReleaseBuffer();
	headerFile.Close();

	//do coClasses
	int coClassCount = pTypeLibHolder->m_coClasses.size();
	for ( int i=0;i<coClassCount;i++) {
		CoClassHolder* pCoClass = pTypeLibHolder->m_coClasses[i];
		
		header = GenerateCoClassHeader( pCoClass, pTypeLibHolder );
		
		headerFileName = m_headerDir;
		headerFileName += pCoClass->m_className;
		headerFileName += ".h";
		headerFile.Open( headerFileName, CFile::modeCreate | CFile::modeWrite );
		headerFile.WriteString( header.GetBuffer(0) );
		header.ReleaseBuffer();
		headerFile.Close();

		CString coClassImpl = GenerateCoClassImpl( pCoClass, pTypeLibHolder );

		headerFileName = m_cppDir;
		headerFileName += pCoClass->m_className;
		headerFileName += ".cpp";
		headerFile.Open( headerFileName, CFile::modeCreate | CFile::modeWrite );
		headerFile.WriteString( coClassImpl.GetBuffer(0) );
		coClassImpl.ReleaseBuffer();
		headerFile.Close();
	}

	CString tlbImpl = GenerateTLBImpl( pTypeLibHolder );
	headerFileName = m_cppDir;
	headerFileName += pTypeLibHolder->m_typeLibName;
	headerFileName += ".cpp";
	headerFile.Open( headerFileName, CFile::modeCreate | CFile::modeWrite );
	headerFile.WriteString( tlbImpl.GetBuffer(0) );
	tlbImpl.ReleaseBuffer();
	headerFile.Close();	
	
}

CString TypeLibraryConverterDlg::GenerateTLBImpl( TypeLibHolder* pTypeLibHolder )
{
	CString result = "";
	result += "//" + pTypeLibHolder->m_typeLibName + ".cpp\n\n";
	if ( TRUE == m_singleUnitPerClass ) {
		result += "#include \"" + pTypeLibHolder->m_typeLibName + ".h\"\n";
		result += "#include \"" + pTypeLibHolder->m_typeLibName + "Interfaces.h\"\n";
	}
	else {		
		switch ( this->m_fileDistributionType ) {
			case SINGLE_FILE_FOR_TLB : {
				result += "#include \"" + pTypeLibHolder->m_typeLibName + ".h\"\n";
			}
			break;

			case SINGLE_FILE_FOR_IFACE_COCLASS : {
				result += "#include \"" + pTypeLibHolder->m_typeLibName + "Interfaces.h\"\n";
			}
			break;

			case SINGLE_FILE_FOR_IFACE_TYPEDEF_COCLASS : {
				result += "#include \"" + pTypeLibHolder->m_typeLibName + "TypeDefs.h\"\n";
				result += "#include \"" + pTypeLibHolder->m_typeLibName + "Interfaces.h\"\n";
			}
			break;
		}
	}

	int coClassCount = pTypeLibHolder->m_coClasses.size();
	for ( int i=0;i<coClassCount;i++) {
		CoClassHolder* pCoClass = pTypeLibHolder->m_coClasses[i];
		result += "#include \"" + pCoClass->m_className + ".h\"\n";
	}

	result += "\n\n";

	result += "using namespace VCF;\n";
	if ( TRUE == this->m_useTLBAsNamespace ) {
		result += "using namespace " + pTypeLibHolder->m_typeLibName + ";\n";
	}

	result += "\n\n";
	
	result += GenerateClassRegFunctionImpl( pTypeLibHolder );

	return result;
}

void TypeLibraryConverterDlg::GenerateSingleFileForIFaceEnumsSeparateCoClasses( TypeLibHolder* pTypeLibHolder )
{

}

void TypeLibraryConverterDlg::GenerateImplementation( TypeLibHolder* pTypeLibHolder )
{
	if ( m_cppDir.GetAt(m_cppDir.GetLength()-1) != '\\' ) {
		m_cppDir += "\\";
	}

	if ( m_headerDir.GetAt(m_headerDir.GetLength()-1) != '\\' ) {
		m_headerDir += "\\";
	}
	if ( TRUE == m_singleUnitPerClass ) {
		GenerateSingleUnitperClassImpl( pTypeLibHolder );		
	}
	else {		
		switch ( this->m_fileDistributionType ) {
			case SINGLE_FILE_FOR_TLB : {
				GenerateSingleFileForTLB( pTypeLibHolder );
			}
			break;

			case SINGLE_FILE_FOR_IFACE_COCLASS : {
				GenerateSingleFileForIFaceSeparateCoClasses( pTypeLibHolder );		
			}
			break;

			case SINGLE_FILE_FOR_IFACE_TYPEDEF_COCLASS : {
				GenerateSingleFileForIFaceEnumsSeparateCoClasses( pTypeLibHolder );		
			}
			break;
		}
	}
}

void TypeLibraryConverterDlg::LoadSettingsFromRegistry()
{
	HKEY currentUser = HKEY_CURRENT_USER;
	HKEY settings;
	CString regSettings;
	regSettings.LoadString( IDS_REG_SETTINGS );
	
	int err = RegOpenKeyEx( currentUser, regSettings, 0, KEY_ALL_ACCESS, &settings );
	if ( err != ERROR_SUCCESS ) {
		CreateRegistrySettings();
	}
	else {
		DWORD type = 0;
		BYTE* buf = NULL;
		DWORD size = 0;		
		err = RegQueryValueEx( settings, "getMethodPrefix", 0, &type, NULL, &size );
		if ( err == ERROR_SUCCESS ){
			if ( (type == REG_SZ) && (size > 0) ){
				buf = new BYTE[size];
				memset( buf, 0, size );
				err = RegQueryValueEx( settings, "getMethodPrefix", 0, &type, buf, &size );
				if ( err == ERROR_SUCCESS ){
					m_getMethodPrefix = (char*)buf;
				}				
				delete [] buf;
			}
		}	

		err = RegQueryValueEx( settings, "setMethodPrefix", 0, &type, NULL, &size );
		if ( err == ERROR_SUCCESS ){
			if ( (type == REG_SZ) && (size > 0) ){
				buf = new BYTE[size];
				memset( buf, 0, size );
				err = RegQueryValueEx( settings, "setMethodPrefix", 0, &type, buf, &size );
				if ( err == ERROR_SUCCESS ){
					m_setMethodPrefix = (char*)buf;
				}				
				delete [] buf;
			}
		}

		err = RegQueryValueEx( settings, "memberPrefix", 0, &type, NULL, &size );
		if ( err == ERROR_SUCCESS ){
			if ( (type == REG_SZ) && (size > 0) ){
				buf = new BYTE[size];
				memset( buf, 0, size );
				err = RegQueryValueEx( settings, "memberPrefix", 0, &type, buf, &size );
				if ( err == ERROR_SUCCESS ){
					m_memberPrefix = (char*)buf;
				}				
				delete [] buf;
			}
		}

		err = RegQueryValueEx( settings, "classPrefix", 0, &type, NULL, &size );
		if ( err == ERROR_SUCCESS ){
			if ( (type == REG_SZ) && (size > 0) ){
				buf = new BYTE[size];
				memset( buf, 0, size );
				err = RegQueryValueEx( settings, "classPrefix", 0, &type, buf, &size );
				if ( err == ERROR_SUCCESS ){
					m_classPrefix = (char*)buf;
				}				
				delete [] buf;
			}
		}
		size = 4;
		type = REG_DWORD;
		DWORD boolVal = 0;
		err = RegQueryValueEx( settings, "singleUnitPerClass", 0, &type, (BYTE*)&boolVal, &size );		
		m_singleUnitPerClass = boolVal;

		size = 4;
		type = REG_DWORD;
		DWORD dwVal = 0;
		err = RegQueryValueEx( settings, "fileDistribution", 0, &type, (BYTE*)&dwVal, &size );		

		m_fileDistributionType = dwVal;
	}

	RegCloseKey( settings );
}

void TypeLibraryConverterDlg::CreateRegistrySettings()
{
	HKEY currentUser = HKEY_CURRENT_USER;
	HKEY settings;
	CString regSettings;
	regSettings.LoadString( IDS_REG_SETTINGS );
	DWORD disposition = 0;
	int err = RegCreateKeyEx( currentUser, regSettings, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &settings, &disposition );
	if ( err == ERROR_SUCCESS ) {
		const unsigned char* val = (const unsigned char*)m_getMethodPrefix.GetBuffer(0);
		err = RegSetValueEx( settings, "getMethodPrefix", 0, REG_SZ, val, m_getMethodPrefix.GetLength() );
		m_getMethodPrefix.ReleaseBuffer();

		val = (const unsigned char*)m_setMethodPrefix.GetBuffer(0);
		err = RegSetValueEx( settings, "setMethodPrefix", 0, REG_SZ, val, m_setMethodPrefix.GetLength() );
		m_setMethodPrefix.ReleaseBuffer();

		val = (const unsigned char*)m_memberPrefix.GetBuffer(0);
		err = RegSetValueEx( settings, "memberPrefix", 0, REG_SZ, val, m_memberPrefix.GetLength() );
		m_memberPrefix.ReleaseBuffer();

		val = (const unsigned char*)m_classPrefix.GetBuffer(0);
		err = RegSetValueEx( settings, "classPrefix", 0, REG_SZ, val, m_classPrefix.GetLength() );
		m_classPrefix.ReleaseBuffer();

		DWORD boolval = (DWORD)m_singleUnitPerClass;
		err = RegSetValueEx( settings, "singleUnitPerClass", 0, REG_DWORD, (BYTE*)&boolval, sizeof(boolval) );

		DWORD dwVal = m_fileDistributionType;
		err = RegSetValueEx( settings, "fileDistribution", 0, REG_DWORD, (BYTE*)&dwVal, sizeof(dwVal) );
		
		RegCloseKey( settings );
	}
}

void TypeLibraryConverterDlg::SaveSettingsToRegistry()
{
	HKEY currentUser = HKEY_CURRENT_USER;
	HKEY settings;
	CString regSettings;
	regSettings.LoadString( IDS_REG_SETTINGS );
	DWORD disposition = 0;
	int err = RegOpenKeyEx( currentUser, regSettings, 0, KEY_ALL_ACCESS, &settings );
	if ( err == ERROR_SUCCESS ) {
		const unsigned char* val = (const unsigned char*)m_getMethodPrefix.GetBuffer(0);
		err = RegSetValueEx( settings, "getMethodPrefix", 0, REG_SZ, val, m_getMethodPrefix.GetLength() );
		m_getMethodPrefix.ReleaseBuffer();

		val = (const unsigned char*)m_setMethodPrefix.GetBuffer(0);
		err = RegSetValueEx( settings, "setMethodPrefix", 0, REG_SZ, val, m_setMethodPrefix.GetLength() );
		m_setMethodPrefix.ReleaseBuffer();

		val = (const unsigned char*)m_memberPrefix.GetBuffer(0);
		err = RegSetValueEx( settings, "memberPrefix", 0, REG_SZ, val, m_memberPrefix.GetLength() );
		m_memberPrefix.ReleaseBuffer();

		val = (const unsigned char*)m_classPrefix.GetBuffer(0);
		err = RegSetValueEx( settings, "classPrefix", 0, REG_SZ, val, m_classPrefix.GetLength() );
		m_classPrefix.ReleaseBuffer();

		DWORD boolval = (DWORD)m_singleUnitPerClass;
		err = RegSetValueEx( settings, "singleUnitPerClass", 0, REG_DWORD, (BYTE*)&boolval, sizeof(boolval) );

		DWORD dwVal = m_fileDistributionType;
		err = RegSetValueEx( settings, "fileDistribution", 0, REG_DWORD, (BYTE*)&dwVal, sizeof(dwVal) );

		RegCloseKey( settings );
	}
	else {
		CreateRegistrySettings();
	}
}

void TypeLibraryConverterDlg::InitializeProgress( TypeLibHolder* pTypeLibHolder )
{
	m_progressLabel.SetWindowText( "" );	
	m_conversionProgress.SetPos( 0 );
	int max = pTypeLibHolder->m_coClasses.size();	
	int count = max;
	for (int i=0;i<count;i++) {
		CoClassHolder* coClass = pTypeLibHolder->m_coClasses[i];
		max += coClass->m_implementedInterfaces.size();
		int ic = coClass->m_implementedInterfaces.size();
		for ( int j=0;j<ic;j++) {
			max += coClass->m_implementedInterfaces[j]->m_methods.size();
		}
	}
	max += pTypeLibHolder->m_interfaces.size();
	std::map<_bstr_t,InterfaceHolder*>::iterator it = pTypeLibHolder->m_interfaces.begin();
	while ( it != pTypeLibHolder->m_interfaces.end() ) {
		max += (it->second)->m_methods.size();
		it ++;
	}
	
		
	
	m_conversionProgress.SetRange( 0, max );
	m_conversionProgress.SetStep( 1 );

	m_progressLabel.ShowWindow( SW_SHOW );
	m_conversionProgress.ShowWindow( SW_SHOW );
}

void TypeLibraryConverterDlg::FinishProgress()
{
	m_progressLabel.ShowWindow( SW_HIDE );
	m_conversionProgress.ShowWindow( SW_HIDE );
}

void TypeLibraryConverterDlg::UpdateProgress( CString message )
{
	m_progressLabel.SetWindowText( message );	
	m_conversionProgress.StepIt();
	m_progressLabel.Invalidate();
	m_conversionProgress.Invalidate();
}

CString TypeLibraryConverterDlg::GenerateClassRegFunction( TypeLibHolder* pTypeLibHolder )
{
	CString result = "";
	result += "void register"+ pTypeLibHolder->m_typeLibName + "Classes();\n";
	return result;
}
	
CString TypeLibraryConverterDlg::GenerateClassRegFunctionImpl( TypeLibHolder* pTypeLibHolder )
{
	CString result = "";
	result += "void register"+ pTypeLibHolder->m_typeLibName + "Classes()\n";
	result += "{\n";
	int coClassCount = pTypeLibHolder->m_coClasses.size();
	for (int i=0;i<coClassCount;i++) {
		CoClassHolder* coClass = pTypeLibHolder->m_coClasses[i];
		result += "\tREGISTER_CLASSINFO( " + coClass->m_className + " );\n";
	}
	result += "}\n\n";
	return result;
}

void TypeLibraryConverterDlg::OnBrowseForHdrDir() 
{
	
	BROWSEINFO info = {0};
	TCHAR displayName[MAX_PATH];
	memset(displayName,0,MAX_PATH);
	CString title = "Browse for Header(*.h) Directory";
		

	info.hwndOwner = ::GetActiveWindow();
	info.pidlRoot = NULL;
	info.pszDisplayName = displayName;
	info.lpszTitle = title.GetBuffer(0);
	info.ulFlags = 0; 

	LPITEMIDLIST itemIDList = SHBrowseForFolder( &info );
	if ( NULL != itemIDList ){
		TCHAR path[MAX_PATH];
		memset(path,0,MAX_PATH);
		if ( TRUE == SHGetPathFromIDList( itemIDList, path ) ){
			this->m_headerDir = path;
			if ( m_headerDir.GetAt(m_headerDir.GetLength()-1) != '\\' ) {
				m_headerDir += "\\";
			}
			UpdateData( FALSE );
		}
	}

	title.ReleaseBuffer();
}

void TypeLibraryConverterDlg::OnBrowseForCppDir() 
{
	BROWSEINFO info = {0};
	TCHAR displayName[MAX_PATH];
	memset(displayName,0,MAX_PATH);
	CString title = "Browse for CPP(*.cpp) Directory";
		

	info.hwndOwner = ::GetActiveWindow();
	info.pidlRoot = NULL;
	info.pszDisplayName = displayName;
	info.lpszTitle = title.GetBuffer(0);
	info.ulFlags = 0; 

	LPITEMIDLIST itemIDList = SHBrowseForFolder( &info );
	if ( NULL != itemIDList ){
		TCHAR path[MAX_PATH];
		memset(path,0,MAX_PATH);
		if ( TRUE == SHGetPathFromIDList( itemIDList, path ) ){
			this->m_cppDir = path;
			if ( m_cppDir.GetAt(m_cppDir.GetLength()-1) != '\\' ) {
				m_cppDir += "\\";
			}
			UpdateData( FALSE );
		}
	}

	title.ReleaseBuffer();
}
