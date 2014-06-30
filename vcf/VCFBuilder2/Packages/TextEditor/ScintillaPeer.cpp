//ScintillaPeer.cpp
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"
#include "ScintillaPeer.h"
#include "TextEditor.h"
#include "CodeEditor.h"
#include "TextEvent.h"
#include "TextModel.h"

#include "Scintilla.h"
#include "SciLexer.h"


#define CPP_KEYWORDS	"asm auto bool break case catch char class const const_cast continue \
default delete do double dynamic_cast else enum explicit export extern false float for \
friend goto if inline int long mutable namespace new operator private protected public \
register reinterpret_cast return short signed sizeof static static_cast struct switch \
template this throw true try typedef typeid typename union unsigned using \
virtual void volatile wchar_t while"


#define CPP_WORD_CHARS	"_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

using namespace VCF;

using namespace VCFBuilder;


LRESULT Win32ScintillaPeerWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	LRESULT result = 0;
	Win32Object* win32Obj = Win32Object::getWin32ObjectFromHWND( hWnd );
	if ( NULL != win32Obj ) {
		win32Obj->handleEventMessages( message, wParam, lParam );
		Win32ScintillaPeer* scintillaPeer = (Win32ScintillaPeer*)win32Obj;
		WNDPROC wndProc = scintillaPeer->getScintillaWndProc();
		result = CallWindowProc( wndProc, hWnd, message, wParam, lParam );
	}
	return result;
}

Win32ScintillaPeer::Win32ScintillaPeer( CodeEditor* editorControl ):
	AbstractWin32Component(editorControl)
{
	m_editorControl = editorControl;
	m_scintillaWndProc = NULL;
	Win32ToolKit* toolkit = (Win32ToolKit*)UIToolkit::getDefaultUIToolkit();
	HWND parent = toolkit->getDummyParent();	

	this->m_styleMask = WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	this->m_exStyleMask = 0;

	TextEditorApplication* textLib = TextEditorApplication::getTextEditorApplication();

	m_hwnd = ::CreateWindowEx( m_exStyleMask, "Scintilla", "", m_styleMask, 0, 0, 0, 0,
								parent, NULL, ::GetModuleHandle(NULL), 0 );//(HINSTANCE)textLib->getPeer()->getHandleID(), 0);

	if ( NULL != m_hwnd ) {
		Win32Object::registerWin32Object( this );
		m_defaultWndProc = NULL;
		m_wndProc = (WNDPROC)Win32ScintillaPeerWndProc;
		m_scintillaWndProc = (WNDPROC)::SetWindowLong( this->m_hwnd, GWL_WNDPROC, (LONG)m_wndProc ); 
		//this->m_defaultWndProc = NULL;

		TextModel* tm = m_editorControl->getTextModel();

		TextModelEventHandler<Win32ScintillaPeer>* tmh = 
			new TextModelEventHandler<Win32ScintillaPeer>( this, Win32ScintillaPeer::onTextModelTextChanged, "TextModelHandler" );		
		
		tm->addTextModelChangedHandler( tmh );

		m_scintillaObject = (void*)SendMessage( m_hwnd, SCI_GETDIRECTPOINTER,0,0 );
		m_scintillaFunc = (ScintillaFunc)SendMessage( m_hwnd, SCI_GETDIRECTFUNCTION,0,0 );
		//set the lexer to C++
		m_scintillaFunc( m_scintillaObject, SCI_SETLEXER, SCLEX_CPP, 0 );

		m_scintillaFunc( m_scintillaObject, SCI_SETKEYWORDS, 0, (LPARAM)CPP_KEYWORDS );

		m_scintillaFunc( m_scintillaObject,SCI_SETCARETFORE,0,0);
		m_scintillaFunc( m_scintillaObject, SCI_SETCARETWIDTH, 1, 0 );
		m_scintillaFunc( m_scintillaObject, SCI_SETCARETLINEVISIBLE, 0, 0 );

		m_scintillaFunc( m_scintillaObject, SCI_CALLTIPSETBACK, RGB(0xff,0xff,0xff),0 );
		m_scintillaFunc( m_scintillaObject, SCI_AUTOCSETIGNORECASE, 0, 0 );		

		m_scintillaFunc( m_scintillaObject, SCI_SETWORDCHARS, 0, (LPARAM)CPP_WORD_CHARS );
		m_scintillaFunc( m_scintillaObject, SCI_SETUSETABS, 1, 0 );
		m_scintillaFunc( m_scintillaObject, SCI_SETTABINDENTS, 1, 0 );
		m_scintillaFunc( m_scintillaObject, SCI_SETBACKSPACEUNINDENTS, 1, 0 );
		m_scintillaFunc( m_scintillaObject, SCI_SETTABWIDTH, 4, 0 );
		m_scintillaFunc( m_scintillaObject, SCI_SETINDENT, 4, 0 );
		m_scintillaFunc( m_scintillaObject, SCI_SETFOLDFLAGS, 16, 0 );
		m_scintillaFunc( m_scintillaObject, SCI_SETMODEVENTMASK, SC_MOD_CHANGEFOLD, 0 );
		m_scintillaFunc( m_scintillaObject, SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL );
		m_scintillaFunc( m_scintillaObject, SCI_SETMARGINWIDTHN, 2, 14 );
		m_scintillaFunc( m_scintillaObject, SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS ); 
		m_scintillaFunc( m_scintillaObject, SCI_SETMARGINSENSITIVEN, 2, 1 );
		m_scintillaFunc( m_scintillaObject, SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_ARROWDOWN);
		m_scintillaFunc( m_scintillaObject, SCI_MARKERSETFORE, SC_MARKNUM_FOLDEROPEN, 0);
		m_scintillaFunc( m_scintillaObject, SCI_MARKERSETBACK, SC_MARKNUM_FOLDEROPEN, 0);
		m_scintillaFunc( m_scintillaObject, SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_ARROW);
		m_scintillaFunc( m_scintillaObject, SCI_MARKERSETFORE, SC_MARKNUM_FOLDER, 0);
		m_scintillaFunc( m_scintillaObject, SCI_MARKERSETBACK, SC_MARKNUM_FOLDER, 0);


		m_scintillaFunc( m_scintillaObject, SCI_SETSTYLEBITS, 7, 0);

		setEditorStyle( STYLE_DEFAULT, 0, 0x00A0A0A0, 11, "Lucida Console" );

		m_scintillaFunc( m_scintillaObject, SCI_STYLECLEARALL, 0, 0 );

		setEditorStyle( STYLE_DEFAULT, 0, 0x00A0A0A0, 11, "Lucida Console" );
		
		setEditorStyle( SCE_C_DEFAULT, 0, 0x00A0A0A0, 11, "Lucida Console" );
		setEditorStyle( SCE_C_COMMENT, 0x00800000, 0x00A0A0A0, 11, "Lucida Console",false,true );
		setEditorStyle( SCE_C_COMMENTLINE, 0x00800000, 0x00A0A0A0, 11, "Lucida Console",false,true );
		setEditorStyle( SCE_C_COMMENTDOC, 0x00B80000, 0x00A0A0A0, 11, "Lucida Console",false,true );
		setEditorStyle( SCE_C_NUMBER, 0x00FF00FF, 0x00A0A0A0, 11, "Lucida Console" );
		setEditorStyle( SCE_C_WORD, 0, 0x00A0A0A0, 11, "Lucida Console", true );
		setEditorStyle( SCE_C_STRING, 0x000000FF, 0x00A0A0A0, 11, "Lucida Console" );
		setEditorStyle( SCE_C_CHARACTER, 0x000000FF, 0x00A0A0A0, 11, "Lucida Console" );
		setEditorStyle( SCE_C_UUID, 0, 0x00A0A0A0, 11, "Lucida Console" );
		setEditorStyle( SCE_C_PREPROCESSOR, 0x00FF0000, 0x00A0A0A0, 11, "Lucida Console", false, true );
		setEditorStyle( SCE_C_OPERATOR, 0, 0x00A0A0A0, 11, "Lucida Console", true );
		setEditorStyle( SCE_C_IDENTIFIER, 0, 0x00A0A0A0, 11, "Lucida Console" );
		setEditorStyle( SCE_C_STRINGEOL, 0, 0x00A0A0A0, 11, "Lucida Console" );
		setEditorStyle( SCE_C_VERBATIM, 0, 0x00A0A0A0, 11, "Lucida Console" );
		setEditorStyle( SCE_C_REGEX, 0, 0x00A0A0A0, 11, "Lucida Console" );
		setEditorStyle( SCE_C_COMMENTLINEDOC, 0x00B90000, 0x00A0A0A0, 11, "Lucida Console" );

		m_scintillaFunc( m_scintillaObject, SCI_STYLESETVISIBLE, STYLE_BRACELIGHT, 1 );
		m_scintillaFunc( m_scintillaObject, SCI_STYLESETVISIBLE, STYLE_BRACEBAD, 1 );

		setEditorStyle( STYLE_BRACELIGHT, 0x00FF0000, 0x00A0A0A0, 11, "Lucida Console", true );		
		setEditorStyle( STYLE_BRACEBAD, 0x000000FF, 0x00A0A0A0, 11, "Lucida Console", true, true );

		//m_scintillaFunc( m_scintillaObject, SCI_CLEARDOCUMENTSTYLE,0,0);
		
		//m_scintillaFunc( m_scintillaObject, SCI_COLOURISE,0,-1);

		
	}
}

Win32ScintillaPeer::~Win32ScintillaPeer()
{

}

void Win32ScintillaPeer::setEditorStyle( const int& style, const ulong32& foreColor, const ulong32& backColor, 
										const ulong32& size, const String& fontFace, const bool& bold, const bool& italic )
{
	m_scintillaFunc( m_scintillaObject, SCI_STYLESETFORE, style, foreColor);
	m_scintillaFunc( m_scintillaObject, SCI_STYLESETBACK, style, backColor);
	if (size >= 1) {
		m_scintillaFunc( m_scintillaObject, SCI_STYLESETSIZE, style, size);
	}
	if (false == fontFace.empty()) { 
		m_scintillaFunc( m_scintillaObject, SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(fontFace.c_str()));
	}

	m_scintillaFunc( m_scintillaObject, SCI_STYLESETBOLD, style, bold );
		
	m_scintillaFunc( m_scintillaObject, SCI_STYLESETITALIC, style, italic );
}

bool Win32ScintillaPeer::findMatchingBracePosition( int &braceAtCaret, int &braceOpposite, const bool& sloppy )
{
	/*
	bool isInside = false;	
	int bracesStyleCheck = editor ? bracesStyle : 0;
	int caretPos = m_scintillaFunc( m_scintillaObject, SCI_GETCURRENTPOS, 0, 0);
	braceAtCaret = -1;
	braceOpposite = -1;
	char charBefore = '\0';
	char styleBefore = '\0';
	//WindowAccessor acc(win.GetID(), props);
	if (caretPos > 0) {
		charBefore = m_scintillaFunc( m_scintillaObject, SCI_GETCHARAT,caretPos - 1, 0 );
		styleBefore = static_cast<char>(m_scintillaFunc( m_scintillaObject, SCI_GETSTYLEAT, caretPos - 1, 0) & 31);
	}

	// Priority goes to character before caret
	if (charBefore && strchr("[](){}", charBefore) &&
	        ((styleBefore == bracesStyleCheck) || (!bracesStyle))) {
		braceAtCaret = caretPos - 1;
	}
	bool colonMode = false;
	if (lexLanguage == SCLEX_PYTHON && ':' == charBefore) {
		braceAtCaret = caretPos - 1;
		colonMode = true;
	}
	bool isAfter = true;
	if (sloppy && (braceAtCaret < 0)) {
		// No brace found so check other side
		char charAfter = acc[caretPos];
		char styleAfter = static_cast<char>(acc.StyleAt(caretPos) & 31);
		if (charAfter && strchr("[](){}", charAfter) && (styleAfter == bracesStyleCheck)) {
			braceAtCaret = caretPos;
			isAfter = false;
		}
		if (lexLanguage == SCLEX_PYTHON && ':' == charAfter) {
			braceAtCaret = caretPos;
			colonMode = true;
		}
	}
	if (braceAtCaret >= 0) {
		if (colonMode) {
			int lineStart = Platform::SendScintilla(win.GetID(), SCI_LINEFROMPOSITION, braceAtCaret);
			int lineMaxSubord = Platform::SendScintilla(win.GetID(), SCI_GETLASTCHILD, lineStart, -1);
			braceOpposite = Platform::SendScintilla(win.GetID(), SCI_GETLINEENDPOSITION, lineMaxSubord);
		} else {
			braceOpposite = Platform::SendScintilla(win.GetID(), SCI_BRACEMATCH, braceAtCaret, 0);
		}
		if (braceOpposite > braceAtCaret) {
			isInside = isAfter;
		} else {
			isInside = !isAfter;
		}
	}
	return isInside;
	*/
	return false;
}

void Win32ScintillaPeer::braceMatch()
{
	int braceAtCaret = -1;
	int braceOpposite = -1;
}

LRESULT Win32ScintillaPeer::handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc )
{	
	LRESULT result = 0;
	switch ( message ) {
		case SCN_UPDATEUI: {
			braceMatch();
		}
		break;

		default : {
			result = AbstractWin32Component::handleEventMessages( message, wParam, lParam );
		}
		break;
	}
	return result;
}

String Win32ScintillaPeer::getText()
{
	String result = "";

	int textLen = SendMessage( this->m_hwnd, SCI_GETTEXTLENGTH, 0, 0 );
	if ( textLen > 0 ) {
		VCFChar* tmpText = new VCFChar[textLen+1];
		memset( tmpText, 0, textLen+1 );
		SendMessage( this->m_hwnd, SCI_SETTEXT, 0, (LPARAM)tmpText );

		result = tmpText;
		delete tmpText;
		tmpText = NULL;
	}

	return result;
}

void Win32ScintillaPeer::setText( const VCF::String& text )
{
	SendMessage( this->m_hwnd, SCI_SETTEXT, 0, (LPARAM)text.c_str() );
}

void Win32ScintillaPeer::setControl( VCF::Control* component )
{
	AbstractWin32Component::setControl( component );
	this->m_editorControl = (CodeEditor*)component;
}

void Win32ScintillaPeer::setRightMargin( const double & rightMargin )
{

}

void Win32ScintillaPeer::setLeftMargin( const double & leftMargin )
{

}

unsigned long Win32ScintillaPeer::getLineCount()
{
	return 0;
}

void Win32ScintillaPeer::getCurrentLinePosition()
{
	
}

double Win32ScintillaPeer::getLeftMargin()
{
	return 0.0;
}

double Win32ScintillaPeer::getRightMargin()
{
	return 0.0;
}

Point* Win32ScintillaPeer::getPositionFromCharIndex( const unsigned long& index )
{
	return NULL;
}

unsigned long Win32ScintillaPeer::getCharIndexFromPosition( Point* point )
{
	return 0;
}

unsigned long Win32ScintillaPeer::getCaretPosition()
{
	return 0;
}

void Win32ScintillaPeer::onTextModelTextChanged( TextEvent* event )
{
	String text = event->getTextModel()->getText();

	m_scintillaFunc( m_scintillaObject, SCI_SETTEXT, 0, (LPARAM)text.c_str() );
}
