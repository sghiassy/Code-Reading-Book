// PreHeaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcfnewclasswiz.h"
#include "PreHeaderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CString GPLText = 	
	"This program is free software; you can redistribute it and/or\r"\
	"modify it under the terms of the GNU General Public License\r"\
	"as published by the Free Software Foundation; either version 2\r"\
	"of the License, or (at your option) any later version.\r"\
	"\r"\
	"This program is distributed in the hope that it will be useful,\r"\
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\r"\
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\r"\
	"GNU General Public License for more details.\r"\
	"\r"\
	"You should have received a copy of the GNU General Public License\r"\
	"along with this program; if not, write to the Free Software\r"\
	"Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\r";
	


CString VCFLicenseText = "Copyright (c) 2000-2001, Jim Crafton\r"\
						"All rights reserved.\r"\
						"Redistribution and use in source and binary forms, with or without\r"\
						"modification, are permitted provided that the following conditions\r"\
						"are met:\r"\
						"	Redistributions of source code must retain the above copyright\r"\
						"	notice, this list of conditions and the following disclaimer.\r"\
						"\r"\
						"	Redistributions in binary form must reproduce the above copyright\r"\
						"	notice, this list of conditions and the following disclaimer in \r"\
						"	the documentation and/or other materials provided with the distribution.\r"\
						"\r"\
						"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" \r"\
						"AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\r"\
						"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\r"\
						"A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS\r"\
						"OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\r"\
						"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\r"\
						"PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR\r"\
						"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF\r"\
						"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\r"\
						"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS \r"\
						"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\r"\
						"\r"\
						"NB: This software will not save the world.\r";


/////////////////////////////////////////////////////////////////////////////
// PreHeaderDlg dialog


PreHeaderDlg::PreHeaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PreHeaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PreHeaderDlg)
	m_preDefinedHdrsSel = 0;
	m_preHdrText = _T("");
	//}}AFX_DATA_INIT
}


void PreHeaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PreHeaderDlg)
	DDX_CBIndex(pDX, IDC_PREDEFINED_HDRS, m_preDefinedHdrsSel);
	DDX_Text(pDX, IDC_PRE_HDR_TEXT, m_preHdrText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PreHeaderDlg, CDialog)
	//{{AFX_MSG_MAP(PreHeaderDlg)
	ON_CBN_SELCHANGE(IDC_PREDEFINED_HDRS, OnSelchangePredefinedHdrs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PreHeaderDlg message handlers

void PreHeaderDlg::OnSelchangePredefinedHdrs() 
{
	this->UpdateData();
	switch ( this->m_preDefinedHdrsSel ) 
	{
		case 0 :{
			this->m_preHdrText = "";
		}
		break;

		case 1 :{
			this->m_preHdrText = GPLText;
		}
		break;

		case 2 :{
			this->m_preHdrText = VCFLicenseText;
		}
		break;
	}
	this->UpdateData( FALSE );
}



BOOL PreHeaderDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CFont font;
	font.CreatePointFont( 80, "Courier New" );
	CWnd* wnd = this->GetDlgItem( IDC_PRE_HDR_TEXT );
	wnd->SetFont( &font );
	font.Detach();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
