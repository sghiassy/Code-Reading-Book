// NewListenerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcflistener.h"
#include "NewListenerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NewListenerDlg dialog


NewListenerDlg::NewListenerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NewListenerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(NewListenerDlg)
	m_listenerClassName = _T("");
	m_eventClassName = _T("");
	//}}AFX_DATA_INIT

	m_listenerClassDecl = "";
	m_eventClassDecl = "";
	m_eventClassImpl = "";
}


void NewListenerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NewListenerDlg)
	DDX_Control(pDX, IDC_LISTENER_METH_LIST, m_listenerMethodList);
	DDX_Control(pDX, IDC_EVENT_TYPE_LIST, m_eventTypeList);
	DDX_Text(pDX, IDC_LISTENER_CLASSNAME, m_listenerClassName);
	DDX_Text(pDX, IDC_EVENT_CLASSNAME, m_eventClassName);
	//}}AFX_DATA_MAP

	if ( pDX->m_bSaveAndValidate ) {
		m_listenerClassDecl = "";
		m_listenerClassDecl += "//" + m_listenerClassName  + ".h\n\n";
		CString tmpName = m_listenerClassName;
		tmpName.MakeUpper();
		m_listenerClassDecl += "#ifndef _" + tmpName + "_H__\n";
		m_listenerClassDecl += "#define _" + tmpName + "_H__\n\n";
		m_listenerClassDecl += "#include \"VCF.h\"\n";
		m_listenerClassDecl += "#include \"Listener.h\"\n";
		m_listenerClassDecl += "#include \"" + m_eventClassName + ".h\"\n\n";

		m_listenerClassDecl += "class " +  m_listenerClassName + " : public VCF::Listener { \n";
		m_listenerClassDecl += "public:\n";
		m_listenerClassDecl += "\tvirtual ~" + m_listenerClassName + "(){};\n\n";
		

		int itemCount = this->m_listenerMethodList.GetItemCount();
		for ( int i=0;i<itemCount;i++)	{
			CString methName = m_listenerMethodList.GetItemText(i,0);
			m_listenerClassDecl += "\tvirtual void " + methName + "( " + m_eventClassName + "* event ) = 0;\n\n";
		}

		m_listenerClassDecl += "};\n\n";
		
		
		
		CString handlerClassName = m_listenerClassName;
		int pos = handlerClassName.Find( "Listener", 0 );
		if ( pos != -1 ) { 
			handlerClassName = handlerClassName.Left(pos);
		}
		handlerClassName += "Handler";
		CString HandlerBaseClass = "Handler<LISTENER_TYPE, " + m_eventClassName + ">";
		m_listenerClassDecl += "template <class LISTENER_TYPE> class " +  handlerClassName + " : public VCF::Handler<LISTENER_TYPE," + m_eventClassName + ">, public " + m_listenerClassName + " { \n";
		m_listenerClassDecl += "public:\n";
		m_listenerClassDecl += "\t" + handlerClassName + "( LISTENER_TYPE* source ):\n";
		m_listenerClassDecl += "\t\t" + HandlerBaseClass + "( source ) {\n";
		m_listenerClassDecl += "\t}\n\n";
		m_listenerClassDecl += "\tvirtual ~" + handlerClassName + "(){};\n\n";

		m_listenerClassDecl += "};\n\n";

		m_listenerClassDecl += "#endif //_" + tmpName + "_H__\n";
	}
	else {

	}
}


BEGIN_MESSAGE_MAP(NewListenerDlg, CDialog)
	//{{AFX_MSG_MAP(NewListenerDlg)
	ON_BN_CLICKED(IDC_ADD_LISTENER_METH, OnAddListenerMeth)
	ON_BN_CLICKED(IDC_ADD_EVENT_TYPE, OnAddEventType)
	ON_BN_CLICKED(IDC_DELETE_EVENT_TYPE, OnDeleteEventType)
	ON_BN_CLICKED(IDC_EDT_EVENT_TYPE, OnEdtEventType)
	ON_BN_CLICKED(IDC_DELETE_LISTENER_METH, OnDeleteListenerMeth)
	ON_BN_CLICKED(IDC_EDIT_LISTENER_METH, OnEditListenerMeth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NewListenerDlg message handlers

void NewListenerDlg::OnAddListenerMeth() 
{
		
}

void NewListenerDlg::OnAddEventType() 
{
	
}

void NewListenerDlg::OnDeleteEventType() 
{
	
}

void NewListenerDlg::OnEdtEventType() 
{
	
}

void NewListenerDlg::OnDeleteListenerMeth() 
{
	
}

void NewListenerDlg::OnEditListenerMeth() 
{
	
}

BOOL NewListenerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
