// SubClassWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SubClassWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubClassWnd

CSubClassWnd::CSubClassWnd(HWND hWndSubclass)
{
	this->m_Saved_hWnd = hWndSubclass;
	m_bIsSubclassed = false;
}

CSubClassWnd::~CSubClassWnd()
{
	DoUnsubclass();
}


BEGIN_MESSAGE_MAP(CSubClassWnd, CWnd)
	//{{AFX_MSG_MAP(CSubClassWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSubClassWnd message handlers

void CSubClassWnd::DoSubclass(HWND hWndSubclass)
{
	ASSERT_VALID(this);
    ASSERT(!hWndSubclass  ||  ::IsWindow(hWndSubclass));

    if (hWndSubclass)
    {
        m_Saved_hWnd = hWndSubclass;
    }

    if (m_Saved_hWnd  &&  !m_bIsSubclassed)
    {
        SubclassWindow(m_Saved_hWnd);
		OutputDebugString( "Subclassed Window() !\n" );
        m_bIsSubclassed = true;
    }
}

void CSubClassWnd::DoUnsubclass()
{
	// work-around for embedded windows:
    // if you open an external document (such as a WinWord file), WinWord
    // will destroy the window but our m_hWnd won't be updated
    if (!::IsWindow(m_hWnd))
    {
        m_hWnd = NULL;
        m_bIsSubclassed = false;
        return;
    }

    ASSERT_VALID(this);

    if (m_hWnd  &&  m_bIsSubclassed)
    {
        if (::GetWindowLong(m_hWnd, GWL_WNDPROC) == (LONG)AfxGetAfxWndProc()

            // patch to support the MouseImp utility 
            // (http://www.tv4studios.com/mouseimp/index.html)
            // patch courtesy of Goran Mitrovic [gmit@fly.srk.fer.hr]
            || ::GetModuleHandle("MImp.dll")
            || ::GetModuleHandle("MIPro.dll")

            )
        {
            UnsubclassWindow();
			TRACE( "UnsubclassWindow() !\n" );
        }
        else
        {
            // someone subclassed after us (very bad!) - we'll just detach,
            // hope for the best, and keep on going...

            // make a sound (even in release builds)
            MessageBeep(MB_ICONEXCLAMATION);

#ifdef _DEBUG
            // this is serious enough to warrent an intrusive error message
            // box, but only in the debug build so regular users won't be
            // bothered
            AfxMessageBox("Trying to unsubclass a window that was "
                "subclassed by someone else.", MB_OK | MB_ICONEXCLAMATION);
#endif
            
            Detach();
        }
        m_bIsSubclassed = false;
    }
}
