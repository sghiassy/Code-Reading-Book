// ConvedrtActiveXCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "comtovcfclasswizard.h"
#include "ConvertActiveXCtrlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ConvertActiveXCtrlDlg dialog


ConvertActiveXCtrlDlg::ConvertActiveXCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ConvertActiveXCtrlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ConvertActiveXCtrlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ConvertActiveXCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ConvertActiveXCtrlDlg)
	DDX_Control(pDX, IDC_ACTIVEX_CTRL_LIST, m_activeXCtrlsList);
	//}}AFX_DATA_MAP

	if ( TRUE == pDX->m_bSaveAndValidate ) {

		int i = m_activeXCtrlsList.GetSelectionMark();
		CString selectedAxCtrlCLSID = m_activeXCtrlsList.GetItemText( i, 1 );
		memset( &m_selectedAxCtrlCLSID, 0, sizeof(CLSID) );
		_bstr_t tmpCLSID = selectedAxCtrlCLSID;
		HRESULT hr = CLSIDFromString( (LPOLESTR)tmpCLSID, &m_selectedAxCtrlCLSID );
	}
}


BEGIN_MESSAGE_MAP(ConvertActiveXCtrlDlg, CDialog)
	//{{AFX_MSG_MAP(ConvertActiveXCtrlDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConvertActiveXCtrlDlg message handlers

BOOL ConvertActiveXCtrlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	m_imageList.Create( 16, 16, ILC_COLOR4 | ILC_MASK, 0, 5 );

	CBitmap bmp;
	{
		AFX_MANAGE_STATE( AfxGetModuleState() );

		bmp.LoadBitmap( IDB_AX_CTRL );
		m_imageList.Add( &bmp, RGB(0,255,0) );
		bmp.Detach();
	}

	m_activeXCtrlsList.SetImageList( &m_imageList, LVSIL_SMALL );
	m_activeXCtrlsList.InsertColumn( 0, "Registered ActiveX Controls", LVCFMT_LEFT, 400 );
	m_activeXCtrlsList.InsertColumn( 1, "CLSID", LVCFMT_LEFT, 200 );
	
	FindAllActiveXControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL ConvertActiveXCtrlDlg::HasServer32( HKEY hkCLSID )
{
	HKEY hkTemp ;
	if ((RegOpenKey( hkCLSID, _T("InprocServer32"), &hkTemp ) == ERROR_SUCCESS) ||
		(RegOpenKey( hkCLSID, _T("InprocHandler32"), &hkTemp ) == ERROR_SUCCESS) ||
		(RegOpenKey( hkCLSID, _T("LocalServer32"), &hkTemp ) == ERROR_SUCCESS))
	{
		RegCloseKey( hkTemp ) ;
		return TRUE ;
	}

	return FALSE ;
}

void ConvertActiveXCtrlDlg::FindAllActiveXControls()
{
	USES_CONVERSION;
	
	HKEY    hkCLSID ;
	
	CWaitCursor waitCrsr;

	if (RegOpenKey( HKEY_CLASSES_ROOT, _T("CLSID"), &hkCLSID) == ERROR_SUCCESS)
	{
		LONG     cb  = 0;
		TCHAR    szBuf[_MAX_PATH] ;
		TCHAR    szName[64] ;
		TCHAR    szCLSID[64] ;
		DWORD   dwIndex ;
		long itemIndex = 0;
		for ( dwIndex = 0 ; RegEnumKey( hkCLSID, dwIndex, szCLSID, sizeof(szCLSID)) == ERROR_SUCCESS ; ++dwIndex )
		{
			cb = sizeof(szName);
			if (RegQueryValue( hkCLSID, (LPTSTR)szCLSID, szName, &cb) == ERROR_SUCCESS)
			{
				HKEY    hkClass, hkTemp ;

				if (RegOpenKey( hkCLSID, szCLSID, &hkClass ) == ERROR_SUCCESS)
				{
					BOOL    hasServer32 = HasServer32( hkClass ) ;
					if ( TRUE == hasServer32 ) {
						wsprintf( szBuf, _T("CLSID\\%s\\Control"), (LPTSTR)szCLSID) ;
						if (RegOpenKey( HKEY_CLASSES_ROOT, szBuf, &hkTemp ) == ERROR_SUCCESS)
						{						
							RegCloseKey( hkTemp ) ;
							//goto lblInsertInList ;
							int idx = m_activeXCtrlsList.InsertItem( itemIndex, szName );
							m_activeXCtrlsList.SetItemText( idx, 1, szCLSID );

							itemIndex++;
						}
					}
					
				}
				RegCloseKey( hkClass ) ;
			}
			
		}
		RegCloseKey( hkCLSID ) ;
	}
}
