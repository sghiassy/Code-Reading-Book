//////////////////////////////////////////////////////////////////////
// FILE: DemoGL_CSysConsole.h
// PURPOSE: Headerfile of DemoGL_CSysConsole.cpp
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// In this headerfile are all the declarations of the console, plus the definition of
// the console class 
//
//////////////////////////////////////////////////////////////////////
// Part of DemoGL Demo System sourcecode. See version information
//////////////////////////////////////////////////////////////////////
// COPYRIGHTS:
// Copyright (c)1999-2001 Solutions Design. All rights reserved.
// Central DemoGL Website: www.demogl.com.
// 
// Released under the following license: (BSD)
// -------------------------------------------
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met: 
//
// 1) Redistributions of source code must retain the above copyright notice, this list of 
//    conditions and the following disclaimer. 
// 2) Redistributions in binary form must reproduce the above copyright notice, this list of 
//    conditions and the following disclaimer in the documentation and/or other materials 
//    provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY SOLUTIONS DESIGN ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SOLUTIONS DESIGN OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
//
// The views and conclusions contained in the software and documentation are those of the authors 
// and should not be interpreted as representing official policies, either expressed or implied, 
// of Solutions Design. 
//
// See also License.txt in the sourcepackage.
//
//////////////////////////////////////////////////////////////////////
// Contributers to the code:
//		- Frans Bouma [FB]
//////////////////////////////////////////////////////////////////////
// VERSION INFORMATION.
//
// v1.3: Added new methods and members.
// v1.2: Added new methods and members.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLSYSCONSOLE_H
#define _DGL_DLLSYSCONSOLE_H

#include "DemoGL\dgl_dllprogressbar.h"

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
// alter value if we need more concurrent textures.
#define CONSOLE_MAXTEXTURES		10

// textureindices defines
#define CONSOLE_BACKGRND		0
#define FONTLEFT				1
#define FONTRIGHT				2
#define LOADINGSPLASH			3

// textoffset adjusts, so there is some small nonchar border around the text (and the text is not smacked to the windowborder)
#define TEXTXBASEADJ			0.0025f
#define TEXTYBASEADJ			0.002f

// Font specific values. the font coming with DemoGL has these values.
#define CHARWIDTH_PIX			26.0f
#define CHARHEIGHT_PIX			32.0f

// Display types for OGL array filling
#define DT_CONSOLE				1
#define DT_DEBUGOVERLAY			2
#define DT_TIMER				3
#define DT_FPS					4

// some Z values for the various elements of the console. The values are random, as long as the
// value is larger/smaller than the z of the element behind/in front of is.
#define	Z_BACKGROUND			-0.1f
#define Z_SPLASH				0.8f
#define Z_TEXT					0.0f
#define Z_HEADERBACKGROUND		-0.05f
#define Z_SCROLLBAR				0.1f
#define Z_SCROLLBARELEMENT		0.15f

// Console buffers defaults
#define DEFAULT_AMLINESHDR		4
#define DEFAULT_AMLINESBUFFER	500
#define DEFAULT_AMCHARSLINE		80
#define DEFAULT_AMLINESDBGINFO	8
#define DEFAULT_AMLINESONSCREEN	50

// Scrollbar constants
#define SB_WIDTH				0.05f
#define SB_POSX1				1.0f
#define SB_BORDERWIDTH			0.005f

// Inputbuffer prompt. Defined here because the inputbuffer is visualized by the console.
#define IB_PROMPT				"DemoGL>"

// Wrap prefix for system related logs in the console. (chars)
#define PREFIX_WRAP_SYSRELATED	"     ::"
#define PREFIXLENGTH_WRAP_SYSRELATED	7

////////////////////////////////////////////////////////////////////////
//                          
//						TYPEDEFS
//
////////////////////////////////////////////////////////////////////////
// Purpose: structure that holds precalculated texturecoordinate data and additional info 
// for 1 ascii character. 
typedef struct
{
	GLfloat		m_fX1, m_fY1;			// left lower point
	GLfloat		m_fX2, m_fY2;			// top right point
	bool		m_bLeftTexture;			// true if we need the left texture for this char. false if we need the right texture
} SCharTexDat;


////////////////////////////////////////////////////////////////////////
//                          
//						CLASS DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
class CSysConsole
{
	// PUBLIC MEMBERS NOT ALLOWED

	// public method declarations
public:
							CSysConsole(void);
		virtual				~CSysConsole(void);
		void				AddLineToHeader(char *sLine, int iLineNo);
		void				CreateConsoleBuffers(void);
		void				End(void);
		int					Init(int iWidth, int iHeight, char *sLoadingSplashTexture, int iLoadingSplashTextureType, float fOpacity);
		void				LogLineToConsole(const char *sString, bool bScroll, bool bSystemRelated);
		void				MoveConsoleViewWindow(int iMoveType);
		void				Prepare(void);
		void				RenderConsole(bool bInputPrompt, char *sInputBuffer, bool bDisplaySplashIfEnabled, bool bDisplayLoadingPBIfEnabled);
		void				RenderOverlay(bool bDisplayFPS, bool bDisplayDebugInfo, bool bInputPrompt, long lMillisecondsPassed, char *sInputBuffer);
		void				Reshape(int iWidth, int iHeight);
		// REMEMBER: CALLING ANY OF THESE BUFFERFUNCTIONS WILL HAVE NO EFFECT ON THE BUFFER UNLESS YOU CALL CreateConsoleBuffers()
		// THERE IS NO WRAP IN THE HEADER. BE SURE TO SET THE WIDTH OF THE SCREEN TO THE MAXWIDTH OF THE HEADER
		void				SetAmCharsPerLine(int iAmChars);				// amount of chars per line is same for buffer and screen
		void				SetAmLinesInBuffer(int iAmLines);				
		void				SetAmLinesOnScreen(int iAmLines);				// Lines on screen is >= (lines in header + 2)
		void				SetAmLinesInDebugInfo(int iAmLines);			// Lines in debuginfo is at least 1 (for inputline)
		void				SetAmLinesInHeader(int iAmLines);				// Lines in header is at least 1, but at most 4 at the moment.
		void				SetFPS(float fFPS);
		void				SetLoadingPBCntrl(CProgressBar *pLoadingPB);

	// private method declarations
private:
		inline void			CalcTopLineWithCLAtBottom(void);
		void				CreateCharTexData(void);
		void				CreateConsoleDLs(void);
		void				DisplayLoadingPB(void);
		void				DisplaySplash(void);
		void				FillOGLArrays(char *sInputBuffer, bool bInputPrompt, int iDisplayType);
		inline int			GetAmountOfCBLinesOnScreen(void);
		void				InitConsoleBuffer(bool bClearBuffer);
		void				RenderConsoleScrollBar(void);
		void				RenderDebugInfoBackground(bool bInputPrompt);
		void				RenderFPSBackground(void);
		void				RenderHeaderBackground(void);
		void				ScrollConsoleBuffer(void);
		void				WriteLine(char *pszString, int iLineNo, GLfloat **pCurTexCoordL, GLfloat **pCurTexCoordR, GLfloat **pCurVertexL, GLfloat **pCurVertexR);

	// private member declarations
private:
		int					m_iWindowWidth;
		int					m_iWindowHeight; 
		GLuint				m_iBGDList;
		GLuint				m_arriTextureNames[CONSOLE_MAXTEXTURES];
		float				m_fFPS;
		long				m_lMSecPassedSinceStart;
		CRITICAL_SECTION	m_csLogLine;
		bool				m_bLoadingSplashEnabled;
		bool				m_bLoadingPBEnabled;
		float				m_fLoadingSplashOpacity;
		CProgressBar		*m_pLoadingPB;

		SCharTexDat			m_arrCharTexData[128];	// for every ascii member we calculate the values.

		// charbuffers.
		char				*m_parrcConsoleBuffer;
		char				*m_parrcHeaderBuffer;
		char				**m_parrcCBLineStarts;	// array of linepointers in ConsoleBuffer
		char				**m_parrcHBLineStarts;	// array of linepointers in Header

		// parameters needed for characterpositioning on screen
		float				m_fCharXAdjust;
		float				m_fLineYAdjust;
		float				m_fCharWidth;
		float				m_fCharHeight;
		
		// maintenance parameters which control runtimebehaviour
		int					m_iNextCleanLineinCB;
		int					m_iLinesFilledInBuffer;		// the amount of lines with actual content to display. That's max m_iAmLinesInBuffer-1
		int					m_iTopLineOnScreen;
		
		// parameters in control of the sizes of the buffers
		int					m_iAmCharsPerLine;
		int					m_iAmLinesInBuffer;
		int					m_iAmLinesOnScreen;
		int					m_iAmLinesInDebugInfo;
		int					m_iAmLinesInHeader;

		// OpenGL primitive buffers for glDrawElements usage. Buffers for left and right are
		// equal in size. No elements arrays needed because we don't share any vertex between
		// the characters on screen
		// There is also a flag that signales if the arrays should be refilled. The arrays
		// are only refilled if there is an inputbuffer or the screen has been scrolled.
		bool				m_bRefillOGLArrays;
		GLfloat				*m_pfTexCoordArrayLeft;		// array for left chartexture elements
		GLfloat				*m_pfTexCoordArrayRight;	// array for right chartexture elements
		GLfloat				*m_pfVertexArrayLeft;		// array for chars with leftchartexture textures
		GLfloat				*m_pfVertexArrayRight;		// array for chars with rightchartexture textures

		GLfloat				*m_pfTmpTCArrayLeft;		// Pointer IN TexCoordarray left, pointing at the current position
		GLfloat				*m_pfTmpTCArrayRight;		// Pointer IN TexCoordarray right, pointing at the current position
		GLfloat				*m_pfTmpVArrayLeft;			// Pointer IN Vertexarray left, pointing at the current position
		GLfloat				*m_pfTmpVArrayRight;		// Pointer IN Vertexarray right, pointing at the current position
		int					m_iAmElementsLeft;
		int					m_iAmElementsRight;
};


#endif	// _DGL_DLLSYSCONSOLE_H