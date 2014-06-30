// NewClassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcfnewclasswiz.h"
#include "NewClassDlg.h"
#include "Rpcdce.h" //make sure to link with Rpcrt4.lib
#include "PropertyInfoDlg.h"
#include "PreHeaderDlg.h"
#include "PostHeaderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NewClassDlg dialog


NewClassDlg::NewClassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NewClassDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(NewClassDlg)
	m_className = _T("");
	m_CPPName = _T("");
	m_derivesFrom = _T("VCF::Object");
	m_headerName = _T("");
	m_supportsRTTI = TRUE;
	m_abstractClass = FALSE;
	m_useNameSpace = FALSE;
	m_nameSpaceText = _T("VCF");
	m_isClassAnInterface = FALSE;
	//}}AFX_DATA_INIT
	this->m_postHdrText = "";
	this->m_preHdrText = "";
	m_headerDir = "";
	m_cppDir = "";
}


void NewClassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NewClassDlg)
	DDX_Control(pDX, IDC_NAMESPACE_NAME, m_nameSpace);
	DDX_Control(pDX, IDC_CPP, m_cppEdit);
	DDX_Control(pDX, IDC_HEADER, m_headerEdit);
	DDX_Control(pDX, IDC_PROP_LIST, m_propList);
	DDX_Control(pDX, IDC_DERIVES_FROM, m_derivesList);
	DDX_Text(pDX, IDC_CLASS_NAME, m_className);
	DDX_Text(pDX, IDC_CPP, m_CPPName);
	DDX_CBString(pDX, IDC_DERIVES_FROM, m_derivesFrom);
	DDX_Text(pDX, IDC_HEADER, m_headerName);
	DDX_Check(pDX, IDC_SUPPORTS_VCF_RTTI, m_supportsRTTI);
	DDX_Check(pDX, IDC_ABSTRACT_CLASS, m_abstractClass);
	DDX_Check(pDX, IDC_USE_NAMESPACE, m_useNameSpace);
	DDX_CBString(pDX, IDC_NAMESPACE_NAME, m_nameSpaceText);
	DDX_Check(pDX, IDC_INTERFACE_CLASS, m_isClassAnInterface);
	//}}AFX_DATA_MAP

	if ( pDX->m_bSaveAndValidate ){		
		m_classDecl = "";
		
		UUID id;
		CString classid;
		if ( RPC_S_OK == ::UuidCreate( &id ) ){
			unsigned char *tmpid = NULL;
			
			RPC_STATUS rpcresult = UuidToString(  &id, &tmpid );
			
			if ( RPC_S_OUT_OF_MEMORY != rpcresult ) {
				classid = tmpid;
				
				RpcStringFree( &tmpid );
			}
			else {
				classid = "\"unknown\"";
			}
		}
		

		CString defName = m_className;
		defName.MakeUpper();

		defName = "_" + defName + "_H__";
		m_classDecl = "//" + m_className + ".h\n\n";
		if ( m_preHdrText.GetLength() > 0 ) {
			m_classDecl += "/**\n" + m_preHdrText + "\n*/\n\n";
		}
		m_classDecl += "#ifndef " + defName + "\n";
		m_classDecl += "#define " + defName + "\n\n\n";

		CString classIDName = m_className;
		classIDName.MakeUpper();
		classIDName += "_CLASSID";
		m_classDecl += "#include \"VCF.h\"\n";
		m_classDecl += "#include \"ClassInfo.h\"\n\n\nusing namespace VCF;\n\n\n";
		if ( this->m_supportsRTTI ){
			m_classDecl += "#define " + classIDName + "\t\t\"" + classid + "\"\n\n\n";
		}
		if ( TRUE == this->m_useNameSpace ) {
			m_classDecl += "namespace " + m_nameSpaceText + "  {\n\n";
		}
		m_classDecl += "/**\n*Class " + m_className + " documentation\n*/\n";

		m_classDecl += "class " + m_className;
		
		if ( FALSE == this->m_isClassAnInterface ) {
			m_classDecl += " : public " + m_derivesFrom + " { \n";
		}
		else {
			m_classDecl += " { \n";
		}

		m_classDecl += "public:\n";
		if ( this->m_supportsRTTI ){
			if ( this->m_abstractClass ){
				m_classDecl += "\tBEGIN_ABSTRACT_CLASSINFO(" + m_className + ", \"" + m_className + "\", \"" + m_derivesFrom + "\", " + classIDName + ")\n";	
			}
			else {
				m_classDecl += "\tBEGIN_CLASSINFO(" + m_className + ", \"" + m_className + "\", \"" + m_derivesFrom + "\", " + classIDName + ")\n";
			}
			this->fillInPropInfo();	
			m_classDecl += "\tEND_CLASSINFO(" + m_className + ")\n\n";
			
		}
		if ( FALSE == this->m_isClassAnInterface ) {
			m_classDecl += "\t" + m_className + "();\n\n";
		}
		m_classDecl += "\tvirtual ~" + m_className + "();\n\n";

		fillInPropMethods();

		m_classDecl += "protected:\n\nprivate:\n";

		fillInPropMemberVars();

		m_classDecl += "};\n\n\n";

		if ( TRUE == this->m_useNameSpace ) {
			m_classDecl += "}; //end of namespace " + m_nameSpaceText + "\n\n";
		}

		m_classDecl += "#endif //" + defName + "\n\n\n";

		if ( m_postHdrText.GetLength() > 0 ) {
			m_classDecl += "/**\n" + m_postHdrText + "\n*/\n\n";
		}

		this->m_classImpl = "";
		if ( FALSE == this->m_isClassAnInterface ) {
			this->m_classImpl = "//" + m_className + ".h\n";
			
			if ( m_preHdrText.GetLength() > 0 ) {
				m_classImpl += "/**\n" + m_preHdrText + "\n*/\n\n";
			}		
			
			m_classImpl += "#include \"" + m_className + ".h\"\n\n";
			
			m_classImpl += "using namespace VCF;\n";
			
			if ( (TRUE == this->m_useNameSpace) && (m_nameSpaceText != "VCF") ) { 
				m_classImpl += "\nusing namespace " + m_nameSpaceText + ";\n\n\n";
			}
			
			m_classImpl += m_className + "::" + m_className + "()\n{\n\n}\n\n";
			
			m_classImpl += m_className + "::~" + m_className + "()\n{\n\n}\n\n";
			
			fillInPropMethodsImpl();
		}
	}
}


BEGIN_MESSAGE_MAP(NewClassDlg, CDialog)
	//{{AFX_MSG_MAP(NewClassDlg)
	ON_BN_CLICKED(IDC_CHANGE_CPP_BTN, OnChangeCppBtn)
	ON_BN_CLICKED(IDC_CHANGE_HDR_BTN, OnChangeHdrBtn)
	ON_EN_CHANGE(IDC_CLASS_NAME, OnChangeClassName)
	ON_BN_CLICKED(IDC_SUPPORTS_VCF_RTTI, OnSupportsVcfRtti)
	ON_NOTIFY(HDN_ITEMDBLCLICK, IDC_PROP_LIST, OnItemdblclickPropList)
	ON_NOTIFY(NM_RCLICK, IDC_PROP_LIST, OnRclickPropList)
	ON_COMMAND(ID_PROPSPOPUP_EDIT, OnPropspopupEdit)
	ON_COMMAND(ID_PROPSPOPUP_REMOVE, OnPropspopupRemove)
	ON_COMMAND(ID_PROPSPOPUP_ADD, OnPropspopupAdd)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_PROPSPOPUP_EDIT, OnUpdatePropspopupEdit)
	ON_UPDATE_COMMAND_UI(ID_PROPSPOPUP_REMOVE, OnUpdatePropspopupRemove)
	ON_BN_CLICKED(IDC_EDIT_PRE_HDR, OnEditPreHdr)
	ON_BN_CLICKED(IDC_EDIT_POST_HDR, OnEditPostHdr)
	ON_BN_CLICKED(IDC_USE_NAMESPACE, OnUseNamespace)
	ON_BN_CLICKED(IDC_INTERFACE_CLASS, OnInterfaceClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NewClassDlg message handlers

void NewClassDlg::OnChangeCppBtn() 
{
	CFileDialog saveAs( FALSE, NULL, this->m_CPPName, 
		                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						"C++ Files|*.cpp||" );

	if ( IDOK == saveAs.DoModal() ){
		m_CPPName = saveAs.GetPathName ();
		int pos = m_CPPName.ReverseFind( '\\' );
		if ( -1 != pos ){
			m_cppDir = m_CPPName.Left( pos );
			if ( '\\' != m_cppDir.GetAt(m_cppDir.GetLength()-1) ){
				m_cppDir += "\\";
			}
		}
		this->m_cppEdit.EnableWindow();
		this->UpdateData(FALSE);
	}
}

void NewClassDlg::OnChangeHdrBtn() 
{
	CFileDialog saveAs( FALSE, NULL, this->m_headerName, 
		                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						"C++ Header Files|*.h||" );

	if ( IDOK == saveAs.DoModal() ){
		m_headerName = saveAs.GetPathName ();
		int pos = m_headerName.ReverseFind( '\\' );
		if ( -1 != pos ){
			m_headerDir = m_headerName.Left( pos );
			if ( '\\' != m_headerDir.GetAt(m_headerDir.GetLength()-1) ){
				m_headerDir += "\\";
			}
		}
		this->m_headerEdit.EnableWindow();
		this->UpdateData(FALSE);
	}
}

void NewClassDlg::OnChangeClassName() 
{
	this->UpdateData();

	this->m_CPPName =  m_cppDir + this->m_className + ".cpp";
	this->m_headerName = m_headerDir + this->m_className + ".h";

	this->UpdateData(FALSE);
}

void NewClassDlg::OnSupportsVcfRtti() 
{
	this->UpdateData();
	CWnd* wnd = this->GetDlgItem(IDC_ABSTRACT_CLASS);	
	wnd->EnableWindow( this->m_supportsRTTI );
	wnd = this->GetDlgItem(IDC_STATIC_PROPS);	
	wnd->EnableWindow( this->m_supportsRTTI );
	wnd = this->GetDlgItem(IDC_PROP_LIST);	
	wnd->EnableWindow( this->m_supportsRTTI );
}

void NewClassDlg::OnItemdblclickPropList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	
	
	*pResult = 0;
}

void NewClassDlg::OnRclickPropList(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	CMenu popup;
	popup.LoadMenu( IDR_PROPSMNU );
	POINT pt = {0};
	::GetCursorPos( &pt );
	popup.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN, pt.x, pt.y, this );
	*pResult = 0;
}

void NewClassDlg::OnPropspopupEdit() 
{
	int index = this->m_propList.GetSelectionMark();
	if ( index > -1 ){
		PropertyInfoDlg dlg;
		
		dlg.m_propName = m_propList.GetItemText( index, 0 );
		dlg.m_propType = m_propList.GetItemText( index, 1 );
		CString readOnly = m_propList.GetItemText( index, 2 );
		dlg.m_readOnlyProp = (readOnly == "Yes") ? TRUE : FALSE;
		if ( IDOK == dlg.DoModal() ){
			CString name = dlg.m_propName;
			CString type = dlg.m_propType;
				
			PropInfo* info = (PropInfo*)m_propList.GetItemData( index );
			info->Name = name;
			info->Type = type;
			info->readOnly = (bool)dlg.m_readOnlyProp;
			
			m_propList.SetItemText( index, 0, name );
			
			m_propList.SetItemText( index, 1, type );
			CString readOnly = dlg.m_readOnlyProp ? "Yes" : "No";
				
			m_propList.SetItemText( index, 2, readOnly );				
		}	
	}
}

void NewClassDlg::OnPropspopupRemove() 
{
	int index = this->m_propList.GetSelectionMark();
	if ( index > -1 ){
		PropInfo* info = (PropInfo*)m_propList.GetItemData( index );
		if ( NULL != info ){
			delete info;
			info = NULL;
		}

		m_propList.DeleteItem( index );
	}
}

void NewClassDlg::OnPropspopupAdd() 
{
	PropertyInfoDlg dlg;
	if ( IDOK == dlg.DoModal() ){
		CString name = dlg.m_propName;
		CString type = dlg.m_propType;

		PropInfo* info = new PropInfo();
		info->Name = name;
		info->Type = type;
		info->readOnly = (bool)dlg.m_readOnlyProp;
		
		LVITEM item = {0};
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.cchTextMax = name.GetLength();
		item.pszText = name.GetBuffer(0);
		item.lParam = (LPARAM)info;
		int i = m_propList.InsertItem( &item );
		
		m_propList.SetItemText( i, 1, type );
		CString readOnly = dlg.m_readOnlyProp ? "Yes" : "No";
		
		m_propList.SetItemText( i, 2, readOnly );
		
	}	
}

int NewClassDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

BOOL NewClassDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	this->m_propList.InsertColumn( 0, "Name" );
	this->m_propList.SetColumnWidth(0, 100 );
	this->m_propList.InsertColumn( 1, "Type" );
	this->m_propList.SetColumnWidth(1, 80 );

	this->m_propList.InsertColumn( 2, "Read Only" );
	this->m_propList.SetColumnWidth(2, 50 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void NewClassDlg::fillInPropInfo()
{
	int count = m_propList.GetItemCount();
	for (int i = 0;i<count;i++){
		PropInfo* info = (PropInfo*)m_propList.GetItemData( i );
		if ( NULL != info ){
			CString name = m_propList.GetItemText(i , 0 );
			CString type = m_propList.GetItemText(i , 1 );
			char c = name.GetAt(0);
			c = toupper(c);
			CString s_funcName = this->m_className + "::set";
			s_funcName += c;
			CString tmp = name;
			tmp.Delete(0);
			s_funcName += tmp;

			CString g_funcName = this->m_className + "::get";
			g_funcName += c;
			g_funcName += tmp;


			CString typeName;
			if ( type == "int" ){
				typeName = "PROP_INT";	
			}
			else if ( type == "long" ){
				typeName = "PROP_LONG";	
			}
			else if ( type == "short" ){
				typeName = "PROP_SHORT";	
			}
			else if ( type == "unsigned long" ){
				typeName = "PROP_ULONG";	
			}
			else if ( type == "float" ){
				typeName = "PROP_FLOAT";	
			}
			else if ( type == "char" ){
				typeName = "PROP_CHAR";	
			}
			else if ( type == "double" ){
				typeName = "PROP_DOUBLE";	
			}
			else if ( type == "bool" ){
				typeName = "PROP_BOOL";	
			}
			else if ( type == "String" ){
				typeName = "PROP_STRING";	
			}

			CString prop;
			if ( info->readOnly ){
				prop = "\tREADONLY_PROPERTY(" + type + ", " + "\"" + name + "\", " + g_funcName;
			}
			else {
				prop = "\tPROPERTY(" + type + ", " + "\"" + name + "\", " + g_funcName;
			}
			
			if ( !info->readOnly ){
				prop += ", " + s_funcName;
			}
			prop += ", " + typeName + ");\n";
			m_classDecl += prop;
		}	
	}
}

void NewClassDlg::fillInPropMethods()
{
	if ( this->m_supportsRTTI ){
		int count = m_propList.GetItemCount();
		for (int i = 0;i<count;i++){
			PropInfo* info = (PropInfo*)m_propList.GetItemData( i );
			if ( NULL != info ){
				CString name = m_propList.GetItemText(i , 0 );
				CString type = m_propList.GetItemText(i , 1 );
				char c = name.GetAt(0);
				c = toupper(c);
				CString s_funcName = "set";
				s_funcName += c;
				CString tmp = name;
				tmp.Delete(0);
				s_funcName += tmp;
				
				CString g_funcName = "get";
				g_funcName += c;
				g_funcName += tmp;				
				
				CString propGetMethod = "\t" + type + " " + g_funcName + "();\n\n";
				m_classDecl += 	propGetMethod;
				if ( !info->readOnly ){
					CString propSetMethod = "\tvoid " + s_funcName + "( const " + type + "& " + name + " );\n\n";
					m_classDecl += 	propSetMethod;
				}
			}	
		}
	}
}

void NewClassDlg::fillInPropMethodsImpl()
{
	if ( this->m_supportsRTTI ){
		int count = m_propList.GetItemCount();
		for (int i = 0;i<count;i++){
			PropInfo* info = (PropInfo*)m_propList.GetItemData( i );
			if ( NULL != info ){
				CString name = m_propList.GetItemText(i , 0 );
				CString type = m_propList.GetItemText(i , 1 );
				char c = name.GetAt(0);
				c = toupper(c);
				CString s_funcName = m_className + "::set";
				s_funcName += c;
				CString tmp = name;
				tmp.Delete(0);
				s_funcName += tmp;
				
				CString g_funcName = m_className + "::get";
				g_funcName += c;
				g_funcName += tmp;				
				
				CString propGetMethod = type + " " + g_funcName + "()\n{\n\treturn m_" + name + ";\n}\n\n";
				m_classImpl += 	propGetMethod;
				if ( !info->readOnly ){
					CString propSetMethod = "void " + s_funcName + "( const " + type + "& " + name + " )\n{\n\tm_" + name + " = " + name + ";\n}\n\n";
					m_classImpl += 	propSetMethod;
				}
			}	
		}
	}
}

void NewClassDlg::fillInPropMemberVars()
{
	if ( this->m_supportsRTTI ){
		int count = m_propList.GetItemCount();
		for (int i = 0;i<count;i++){
			PropInfo* info = (PropInfo*)m_propList.GetItemData( i );
			if ( NULL != info ){
				CString name = m_propList.GetItemText(i , 0 );
				CString type = m_propList.GetItemText(i , 1 );

				m_classDecl += "\t" + type + " m_" + name + ";\n";
			}	
		}
	}
}

void NewClassDlg::OnUpdatePropspopupEdit(CCmdUI* pCmdUI) 
{
	//int index = this->m_propList.GetSelectionMark();
	//pCmdUI->Enable( (this->m_propList.GetItemCount() > 0) && (index > -1) );	
}

void NewClassDlg::OnUpdatePropspopupRemove(CCmdUI* pCmdUI) 
{
	//int index = this->m_propList.GetSelectionMark();
	//pCmdUI->Enable( (this->m_propList.GetItemCount() > 0) && (index > -1) );	
}

void NewClassDlg::OnEditPreHdr() 
{
	PreHeaderDlg dlg;
	dlg.m_preHdrText = m_preHdrText;
	if ( IDOK == dlg.DoModal() ) {		
		m_preHdrText = dlg.m_preHdrText;		
		this->UpdateData();
	}	
}

void NewClassDlg::OnEditPostHdr() 
{
	PostHeaderDlg dlg;
	dlg.m_postHeaderText = m_postHdrText;
	if ( IDOK == dlg.DoModal() ) {		
		m_postHdrText = dlg.m_postHeaderText;		
	}
}

void NewClassDlg::OnUseNamespace() 
{
	this->UpdateData();
	this->m_nameSpace.EnableWindow( m_useNameSpace );
}

void NewClassDlg::OnInterfaceClass() 
{
	this->UpdateData();	
	if ( TRUE == this->m_isClassAnInterface ) {
		this->m_abstractClass = FALSE;
		this->m_supportsRTTI = FALSE;
	}
	this->UpdateData();	
	this->UpdateData(FALSE);	
}
