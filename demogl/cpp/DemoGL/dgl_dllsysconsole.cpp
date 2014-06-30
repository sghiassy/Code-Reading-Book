//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllsysconsole.cpp
// PURPOSE: Implementation of a readonly OpenGL console class.
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// This class delivers a console like view like Quake type engines have, only
// This one is only used for output.
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
// v1.3: For 70% recoded.
// v1.2: For 90% recoded.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

// local includes
#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

// Purpose: Constructor
CSysConsole::CSysConsole()
{
	int			i;

	// Initialize vars
	m_iWindowWidth = 0;
	m_iWindowHeight = 0;
	m_fFPS=0;
	m_bLoadingSplashEnabled=false;
	m_fCharXAdjust=0.0f;
	m_fLineYAdjust=0.0f;
	m_fCharWidth=0.0f;
	m_fCharHeight=0.0f;
	m_bRefillOGLArrays=true;
	
	// initially all buffers are not there. They're dynamically created.
	m_parrcConsoleBuffer=NULL;
	m_parrcHeaderBuffer=NULL;
	m_parrcCBLineStarts=NULL;
	m_parrcHBLineStarts=NULL;

	m_pfTexCoordArrayLeft=NULL;
	m_pfTexCoordArrayRight=NULL;
	m_pfVertexArrayLeft=NULL;
	m_pfVertexArrayRight=NULL;

	m_pLoadingPB=NULL;
	m_bLoadingPBEnabled=false;

	// Fill the buffervars with the default variables defined in the dgl_dllsysconsole.h file
	m_iAmCharsPerLine=DEFAULT_AMCHARSLINE;
	m_iAmLinesInBuffer=DEFAULT_AMLINESBUFFER;
	m_iAmLinesOnScreen=DEFAULT_AMLINESONSCREEN;
	m_iAmLinesInDebugInfo=DEFAULT_AMLINESDBGINFO;
	m_iAmLinesInHeader=DEFAULT_AMLINESHDR;

	m_iNextCleanLineinCB=0;
	m_iLinesFilledInBuffer=0;

	// Calculate the topline number visible when currentline is at the bottom of thescreen.
	CalcTopLineWithCLAtBottom();

	// create the initial buffers.
	(void)CreateConsoleBuffers();

	// Create the ascii texturedata info
	CreateCharTexData();	

	// Initialize the critical section for the logline method. that method can
	// be called by more than 1 thread.
	InitializeCriticalSection(&m_csLogLine);

	// create a utilsobject for different functions.
	for(i=0;i<CONSOLE_MAXTEXTURES;i++)
	{
		m_arriTextureNames[i]=0;
	}
}


// Purpose: Destructor
CSysConsole::~CSysConsole()
{
	// remove charbuffers if defined
	if(m_parrcConsoleBuffer)
	{
		free(m_parrcConsoleBuffer);
		free(m_parrcCBLineStarts);
	}
	if(m_parrcHeaderBuffer)
	{
		free(m_parrcHeaderBuffer);
		free(m_parrcHBLineStarts);
	}
	if(m_pfTexCoordArrayLeft)
	{
		free(m_pfTexCoordArrayLeft);
	}
	if(m_pfTexCoordArrayRight)
	{
		free(m_pfTexCoordArrayRight);
	}
	if(m_pfVertexArrayLeft)
	{
		free(m_pfVertexArrayLeft);
	}
	if(m_pfVertexArrayRight)
	{
		free(m_pfVertexArrayRight);
	}

	// Delete the mutex
	DeleteCriticalSection(&m_csLogLine);
}


// Purpose: sets the loading progressbar control pointer. The actual progressbar control is located in the
// system kernel, the console only needs a pointer for the drawing of the bar
void
CSysConsole::SetLoadingPBCntrl(CProgressBar *pLoadingPB)
{
	if(pLoadingPB)
	{
		m_bLoadingPBEnabled=true;
		m_pLoadingPB=pLoadingPB;
	}
	else
	{
		m_pLoadingPB=NULL;
		m_bLoadingPBEnabled=false;
	}
}


// Purpose: sets the amount of characters per line in the buffer. This is the same amount of characters
// that is displayed on screen per line, thus the width of the console on screen PLUS the width of the buffer.
// No changes will be made to the buffers until you call CreateConsoleBuffers. This function has effect on the
// width of the characters displayed
// Gets: iAmChars, amount of chars per line
// returns: nothing
void
CSysConsole::SetAmCharsPerLine(int iAmChars)
{
	m_iAmCharsPerLine=iAmChars;
}


// Purpose: sets the amount of lines in the consolebuffer. No changes will be made to the buffers until you call
// CreateConsoleBuffers
// Gets: iAmLines, amount of lines
// returns: nothing
void
CSysConsole::SetAmLinesInBuffer(int iAmLines)
{
	m_iAmLinesInBuffer = iAmLines;
}


// Purpose: sets the amount of lines on screen. No changes will be made to the buffers until you call
// CreateConsoleBuffers. This routine has effect on the height of characters displayed. The amount of lines
// really displayed on screen is iAmLines - 1 (for input) 
// Gets: iAmLines, amount of lines
// returns: nothing
void
CSysConsole::SetAmLinesOnScreen(int iAmLines)
{
	m_iAmLinesOnScreen = iAmLines;
}


// Purpose: sets the amount of lines in the debuginfo (F5). No changes will be made to the buffers until you call
// CreateConsoleBuffers. If iAmLines >= m_iAmLinesOnScreen + 1 it will be ignored
// Gets: iAmLines, amount of lines
// returns: nothing
void
CSysConsole::SetAmLinesInDebugInfo(int iAmLines)
{
	m_iAmLinesInDebugInfo = iAmLines;
}


// Purpose: sets the amount of lines in the header. No changes will be made to the buffers until you call
// CreateConsoleBuffers. 
// Gets: iAmLines, amount of lines
// returns: nothing
void
CSysConsole::SetAmLinesInHeader(int iAmLines)
{
	m_iAmLinesInHeader = iAmLines;
}


// Purpose: creates the buffers for the console and recalculates character variables plus vars like lineadjust.
// Gets: nothing
// Returns: nothing
// Additional Info: it relies on the buffers variables like m_iAmLinesInBuffer. If These are set to nonpractical
// values, the result of CreateConsoleBuffers will be unpleasent ;)
void
CSysConsole::CreateConsoleBuffers()
{
	char	*pCBtmp, *pHBtmp, **pCBLineStartsTmp, **pHBLineStartsTmp;
	int		i;

	// create new buffers.
	// Console contents buffer
	pCBtmp=(char *)malloc(sizeof(char)* (m_iAmCharsPerLine+1) * m_iAmLinesInBuffer);
	pCBLineStartsTmp=(char **)malloc(sizeof(char *) * m_iAmLinesInBuffer);

	// Header contents buffer
	pHBtmp=(char *)malloc(sizeof(char)* (m_iAmCharsPerLine+1) * m_iAmLinesInHeader);
	pHBLineStartsTmp=(char **)malloc(sizeof(char *) * m_iAmLinesInHeader);

	// clear them
	memset(pCBtmp,0,(sizeof(char)* (m_iAmCharsPerLine+1) * m_iAmLinesInBuffer));
	memset(pHBtmp,0,(sizeof(char)* (m_iAmCharsPerLine+1) * m_iAmLinesInHeader));

	// fill linepointerarrays
	// Console contents buffer
	for(i=0;i<m_iAmLinesInBuffer;i++)
	{
		pCBLineStartsTmp[i]=pCBtmp + (i*(m_iAmCharsPerLine+1));
	}

	// Header contents buffer
	for(i=0;i<m_iAmLinesInHeader;i++)
	{
		pHBLineStartsTmp[i]=pHBtmp + (i*(m_iAmCharsPerLine+1));
	}

	// recalculate character and lineadjustment values
	m_fCharWidth=1.0f/(float)m_iAmCharsPerLine;
	m_fCharHeight=1.0f/(float)m_iAmLinesOnScreen;
	m_fCharXAdjust=1.0f/(float)m_iAmCharsPerLine;
	m_fLineYAdjust=1.0f/(float)m_iAmLinesOnScreen;
	
	// memory not needed anymore
	free(m_parrcConsoleBuffer);
	free(m_parrcCBLineStarts);
	free(m_parrcHeaderBuffer);
	free(m_parrcHBLineStarts);

	// let bufferpointers point to new buffers
	m_parrcConsoleBuffer=pCBtmp;
	m_parrcCBLineStarts=pCBLineStartsTmp;
	m_parrcHeaderBuffer=pHBtmp;
	m_parrcHBLineStarts=pHBLineStartsTmp;

	// delete the arrays needed for opengl and recreate them.
	free(m_pfTexCoordArrayLeft);
	free(m_pfTexCoordArrayRight);
	free(m_pfVertexArrayLeft);
	free(m_pfVertexArrayRight);

	// create new buffers for the rendering, also called 'vertexarrays' in opengl.
	// All arrays are as large as holding an entire screen of characterquads, so we won't need to resize
	// them each frame. All data is tightly packed together. This can be slow for the vertexarrays
	// because a float is 4 bytes and we now have 12bytes per vertex. THis could perhaps be a point for improvement.
	// Per character we have 4 texture coords and 4 vertexcoords (we use quads). 
	m_pfTexCoordArrayLeft=(GLfloat *)malloc(sizeof(GLfloat) * 2 * 4 * (m_iAmCharsPerLine * m_iAmLinesOnScreen));
	m_pfTexCoordArrayRight=(GLfloat *)malloc(sizeof(GLfloat) * 2 * 4* (m_iAmCharsPerLine * m_iAmLinesOnScreen));
	m_pfVertexArrayLeft=(GLfloat *)malloc(sizeof(GLfloat) * 3 * 4 * (m_iAmCharsPerLine * m_iAmLinesOnScreen));
	m_pfVertexArrayRight=(GLfloat *)malloc(sizeof(GLfloat) * 3 * 4 * (m_iAmCharsPerLine * m_iAmLinesOnScreen));

	// Initialize the new buffer and don't clear it. THis is to prevent resizing loosing any data
	InitConsoleBuffer(false);
}


// Purpose: Renders the console. First it renders the background, then the texts, and then the loadingsplash.
// (when requested). Each frame it rebuilds the textarrays if needed. If no character is changed in the buffer,
// and/or no inputbuffer is passed we won't rebuild the vertex/texcoord arrays. every state that's enabled
// is preserved using opengl functions or own testing. 
// Gets: bInputPrompt, bool, flags if sInputBuffer is valid
// Gets: sInputBuffer, pointer to char, 
// Gets: bDisplaySplashIfEnabled, bool, will display the splashscreen if true. This way the splashscreen can be rendered
//       at controlled times, f.e. at boottime
// Gets: bDisplayLoadingPBIfEnabled, bool, will display the progressbar if true. This way the progressbar can be rendered
//       at controlled times, f.e. at boottime
void
CSysConsole::RenderConsole(bool bInputPrompt,char *sInputBuffer, bool bDisplaySplashIfEnabled, bool bDisplayLoadingPBIfEnabled)
{
	bool		bVertexArraysWereEnabled, bTexCoordArraysWereEnabled;

	// check if some clientside states are already enabled. 
	if(glIsEnabled(GL_TEXTURE_COORD_ARRAY))
	{
		bTexCoordArraysWereEnabled=true;
	}
	else
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		bTexCoordArraysWereEnabled=false;
	}
	if(glIsEnabled(GL_VERTEX_ARRAY))
	{
		bVertexArraysWereEnabled=true;
	}
	else
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		bVertexArraysWereEnabled=false;
	}

	// now push serverstates to prevent ruining effectexecution when console was opened during app execution
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT);
	// check if we have to fill the vertex and texture coord arrays
	if(bInputPrompt || m_bRefillOGLArrays)
	{
		// reset element counters for both sides.	
		m_iAmElementsLeft=0;
		m_iAmElementsRight=0;

		FillOGLArrays(sInputBuffer, bInputPrompt,DT_CONSOLE);
		m_bRefillOGLArrays=false;
	}
	////////////////////////////////
	// set camera and projection matrix for 2D display
	////////////////////////////////
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();	
		glLoadIdentity();           
		glOrtho(0.0,1.0,0,1.0,-1.0,1.0); 
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			// camera info
			glLoadIdentity();
			glTranslatef(0.0f,0.0f,-0.1f);		// move camera 0.1 unit on the POSITIVE Z axe
			glPushMatrix();
			////////////////////////////////
			// render the console
			////////////////////////////////

				// Disable possible ENABLED states of OpenGL. This will save more cycles than
				// letting those states be ENABLED. We also disable states that are disabled
				// by default, because we don't know if an effect has enabled them, causing 
				// undefined results in the console.
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_ALPHA_TEST);
				glDisable(GL_COLOR_MATERIAL);
				glDisable(GL_LIGHTING);
				glDisable(GL_STENCIL_TEST);
				glDisable(GL_BLEND);
				glDisable(GL_POLYGON_SMOOTH);
				glDisable(GL_NORMALIZE);
				glDisable(GL_TEXTURE_GEN_S); 
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_SCISSOR_TEST);
				
				// enable culling for backfaces to have single sided filling.
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CCW);
				glPolygonMode(GL_FRONT,GL_FILL);
				glCullFace(GL_BACK);
				// enable texturing.
				glEnable(GL_TEXTURE_2D);
				// call display list for background rendering. (see CreateConsoleDL's)
				glCallList(m_iBGDList);

				// enable blending for the rest of the console elements
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

				// draw the header semi opaque quadbackground.
				RenderHeaderBackground();

				///////////////////////////////
				// draw the console characters: (header, buffer and inputline)
				///////////////////////////////
				glColor4f(1.0f,1.0f,1.0f,1.0f);
				// Draw left charmap elements first
				glBindTexture(GL_TEXTURE_2D,m_arriTextureNames[FONTLEFT]);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glTexCoordPointer(2, GL_FLOAT, 0, m_pfTexCoordArrayLeft);
				glVertexPointer(3, GL_FLOAT, 0, m_pfVertexArrayLeft);
				glDrawArrays(GL_QUADS,0,m_iAmElementsLeft);

				// Draw right charmap elements
				glBindTexture(GL_TEXTURE_2D,m_arriTextureNames[FONTRIGHT]);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glTexCoordPointer(2, GL_FLOAT, 0, m_pfTexCoordArrayRight);
				glVertexPointer(3, GL_FLOAT, 0, m_pfVertexArrayRight);
				glDrawArrays(GL_QUADS,0,m_iAmElementsRight);
				
				// Render scrollbar.
				RenderConsoleScrollBar();

				// draw splashpicture IF necessary.
				if(m_bLoadingSplashEnabled && bDisplaySplashIfEnabled)
				{
					// yes.
					DisplaySplash();
				}

				// draw loading progressbar if necessary.
				if(m_bLoadingPBEnabled && bDisplayLoadingPBIfEnabled)
				{
					DisplayLoadingPB();
				}

				/////////////////////////
				// ADD OTHER STUFF THAT SHOULD BE RENDERED HERE
				/////////////////////////
			glPopMatrix();
		// get original modelviewmatrix back
		glPopMatrix();
		// get back projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();

	// if the clientstate arrays weren't enabled we have to disable them, because they're disabled
	// by default and this could hurt effect execution if we leave them on.
	if(!bVertexArraysWereEnabled)
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	if(!bTexCoordArraysWereEnabled)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}


// Purpose: moves the visual window through the console buffer with 1 line or a page. 
// Gets: iMoveType, int, one of the predefined movetypes in dgl_dllmain.h
void
CSysConsole::MoveConsoleViewWindow(int iMoveType)
{
	int		iStartOfBuffer, iLowestTopLine;
	bool	bStartOfBufferIsBelow, bMoveUp, bLowestTopIsAbove;

	bStartOfBufferIsBelow=false;

	// Calculate startline in buffer. This is line 0 when the currentline pointer hasn't
	// passed the end of the buffer. Otherwise can be anywhere in the buffer.
	if(m_iLinesFilledInBuffer < (m_iAmLinesInBuffer-1))
	{
		iStartOfBuffer=0;
	}
	else
	{
		iStartOfBuffer=(m_iNextCleanLineinCB+1)%m_iAmLinesInBuffer;
	}

	iLowestTopLine=m_iNextCleanLineinCB-GetAmountOfCBLinesOnScreen();

	if(iLowestTopLine<0)
	{
		// use '+' because iLowestTopLine is negative. Calculation is needed to wrap around
		// physical bufferend
		iLowestTopLine=m_iAmLinesInBuffer + iLowestTopLine;
	}

	// test if the start of the buffer is below the current topline. This is needed for testing if we've passed
	// the start of the buffer, if the topline position wraps around 0
	if(m_iTopLineOnScreen>=0)
	{
		bStartOfBufferIsBelow=(iStartOfBuffer>m_iTopLineOnScreen);
		bLowestTopIsAbove=(iLowestTopLine<m_iTopLineOnScreen);
	}
	else
	{
		// start of system: topline is negative.
		// No need to set variables. We won't need them because we won't scroll.
	}

	switch(iMoveType)
	{
		case CONSOLE_MOVE1LINEUP:
		{
			m_iTopLineOnScreen--;
			bMoveUp=true;
		}; break;
		case CONSOLE_MOVE1LINEDOWN:
		{
			m_iTopLineOnScreen++;
			bMoveUp=false;
		}; break;
		case CONSOLE_MOVE1PAGEUP:
		{
			m_iTopLineOnScreen-=GetAmountOfCBLinesOnScreen();
			bMoveUp=true;
		}; break;
		case CONSOLE_MOVE1PAGEDOWN:
		{
			m_iTopLineOnScreen+=GetAmountOfCBLinesOnScreen();
			bMoveUp=false;
		}; break;
		default:
		{
			// nothing. return
			return;
		}
	}
	// we now can have several situations. We have to clamp the topline to the current state of the buffer
	// plus if we're in the startup state where there are Less lines logged than there can be printed on screen,
	// we shouldn't wrap, because we're displaying the buffer bottom at the top of the screen.
	if(m_iLinesFilledInBuffer<=GetAmountOfCBLinesOnScreen())
	{
		// startup situation. DONT SCROLL
		// set m_iTopLineOnScreen back to start position (so the currentline is at the bottom)
		CalcTopLineWithCLAtBottom();
		// because we don't scroll, No need to refill the arrays.
	}
	else
	{
		// not startup situation, clamp the topline to the current status of the buffer. Always should the buffer
		// scroll up so that the maximum topline in the buffer is the topline on screen, and down so that the bottom
		// line can't pass current line
		if(bMoveUp)
		{
			// moved up
			if(bStartOfBufferIsBelow)
			{	
				// we only wrap if the startofbuffer was below the topline
				if(m_iTopLineOnScreen<0)
				{
					// wrap
					m_iTopLineOnScreen=m_iTopLineOnScreen + m_iAmLinesInBuffer;
					if(m_iTopLineOnScreen<iStartOfBuffer)
					{
						m_iTopLineOnScreen=iStartOfBuffer;
					}
				}
			}
			else
			{
				if(m_iTopLineOnScreen<iStartOfBuffer)
				{
					m_iTopLineOnScreen=iStartOfBuffer;
				}
			}
		}
		else
		{
			// move down.
			if(bLowestTopIsAbove)
			{
				if(m_iTopLineOnScreen>=m_iAmLinesInBuffer)
				{
					// wrap if necessary
					m_iTopLineOnScreen%=m_iAmLinesInBuffer;
					if(m_iTopLineOnScreen>iLowestTopLine)
					{
						m_iTopLineOnScreen=iLowestTopLine;
					}
				}
			}
			else
			{
				if(m_iTopLineOnScreen>iLowestTopLine)
				{
					m_iTopLineOnScreen=iLowestTopLine;
				}
			}
		}
		// Now refill the vertexarray buffers. 
		m_bRefillOGLArrays=true;
	}
}


// Purpose: renders a scrollbar that visualizes the current position of the console contents in the
// consolebuffer.
void
CSysConsole::RenderConsoleScrollBar()
{
	float	fX1, fY1, fX2, fY2, fHeight, fControlHeight, fControlTop, fControlBottom;
	int		iAmLinesCBOnScreen, iAmLinesAboveTopLine, iStartOfBuffer;
	bool	bTexturingEnabled;

	fX1=(1.0f-(2.0f * m_fCharWidth))-TEXTXBASEADJ;
	fX2=1.0f;
	fControlBottom=m_fCharHeight * (m_iAmLinesInHeader-2);
	fControlTop=1.0f - (m_fCharHeight * m_iAmLinesInHeader);
	fControlHeight=(fControlTop-fControlBottom);

	bTexturingEnabled=false;
	if(glIsEnabled(GL_TEXTURE_2D))
	{
		bTexturingEnabled=true;
		glDisable(GL_TEXTURE_2D);
	}
	glPushMatrix();
		// Draw scrollbar background
		glColor4f(1.0f,1.0f,1.0f,0.2f);
		glBegin(GL_QUADS);
			glVertex3f(fX1, fControlBottom, Z_SCROLLBAR);
			glVertex3f(fX2, fControlBottom, Z_SCROLLBAR);
			glVertex3f(fX2, fControlTop, Z_SCROLLBAR);
			glVertex3f(fX1, fControlTop, Z_SCROLLBAR);
		glEnd();

		// Calculate the scollbar element hightsize and position.
		fX1+=SB_BORDERWIDTH;
		fX2-=SB_BORDERWIDTH;
		// calculate height in percent. 100% is full scrollbar element, 0% is tiny small scrollbar element
		iAmLinesCBOnScreen=GetAmountOfCBLinesOnScreen();
		if(m_iLinesFilledInBuffer <= iAmLinesCBOnScreen)
		{
			// special case: there are less lines logged to buffer than there fit on the screen.
			fHeight=1.0f;
		}
		else
		{
			// begin shrink
			fHeight=(float)iAmLinesCBOnScreen/(float)m_iLinesFilledInBuffer;
		}
		fHeight*=fControlHeight;
		
		// calculate vertical position using m_iTopLineOnScreen
		if(m_iLinesFilledInBuffer < (m_iAmLinesInBuffer-1))
		{
			iStartOfBuffer=0;
		}
		else
		{
			iStartOfBuffer=(m_iNextCleanLineinCB+1)%m_iAmLinesInBuffer;
		}

		// calculate the amount of lines above the topline in the logical buffer
		if(iStartOfBuffer <= m_iTopLineOnScreen)
		{
			iAmLinesAboveTopLine=m_iTopLineOnScreen-iStartOfBuffer;
		}
		else
		{
			iAmLinesAboveTopLine=m_iTopLineOnScreen + (m_iAmLinesInBuffer-iStartOfBuffer);
		}

		// calculate the fY2 coordinate, the Y coordinate of the topright corner.
		if(m_iLinesFilledInBuffer <= iAmLinesCBOnScreen)
		{
			fY2=fControlTop;
		}
		else
		{
			// top right corner's Y coordinate (fY2) == fControlTop - V. Calculate V using iAmLinesAboveTopLine and then fY2
			fY2=fControlTop - (fControlHeight * ((float)iAmLinesAboveTopLine/(float)m_iLinesFilledInBuffer));
		}
		fY1=fY2-fHeight;

		// Draw actual scrollbar element that shrinks and moves.
		glColor4f(1.0f,1.0f,1.0f,0.5f);
		glBegin(GL_QUADS);
			glVertex3f(fX1, fY1, Z_SCROLLBARELEMENT);
			glVertex3f(fX2, fY1, Z_SCROLLBARELEMENT);
			glVertex3f(fX2, fY2, Z_SCROLLBARELEMENT);
			glVertex3f(fX1, fY2, Z_SCROLLBARELEMENT);
		glEnd();
	glPopMatrix();

	if(bTexturingEnabled)
	{
		glEnable(GL_TEXTURE_2D);
	}
}


// Purpose: renders the background for the header in the console, and also the background for the
// input line in the console.
void
CSysConsole::RenderHeaderBackground()
{
	float	fY1;
	bool	bTexturingEnabled;

	fY1=1.0f - (m_fCharHeight * m_iAmLinesInHeader);

	bTexturingEnabled=false;
	if(glIsEnabled(GL_TEXTURE_2D))
	{
		bTexturingEnabled=true;
		glDisable(GL_TEXTURE_2D);
	}
	glPushMatrix();
		glBegin(GL_QUADS);
			glColor4f(0.6f,0.6f,0.6f,0.3f);
			glVertex3f(0.0f, fY1, Z_HEADERBACKGROUND);
			glVertex3f(1.0f, fY1, Z_HEADERBACKGROUND);
			glColor4f(0.5f,0.5f,0.5f,0.7f);
			glVertex3f(1.0f, 1.0f, Z_HEADERBACKGROUND);
			glVertex3f(0.0f, 1.0f, Z_HEADERBACKGROUND);
		glEnd();
		// draw background for inputline
		fY1=m_fCharHeight * 2;
		glBegin(GL_QUADS);
			glColor4f(0.5f,0.5f,0.5f,0.7f);
			glVertex3f(0.0f, 0.0f, Z_HEADERBACKGROUND);
			glVertex3f(1.0f, 0.0f, Z_HEADERBACKGROUND);
			glColor4f(0.6f,0.6f,0.6f,0.3f);
			glVertex3f(1.0f, fY1, Z_HEADERBACKGROUND);
			glVertex3f(0.0f, fY1, Z_HEADERBACKGROUND);
		glEnd();
	glPopMatrix();
	if(bTexturingEnabled)
	{
		glEnable(GL_TEXTURE_2D);
	}
}


// Purpose: renders the background for the debuginfo. It takes the bool bInputPrompt to calculate
// the amount of real lines to cover. 
// If bInputPrompt is true, the amount of lines is 2 bigger than when it's false. 
// Amount of lines to cover = m_iAmLinesInDebugInfo + 1 /* for time */ + (2 * (int)bInputPrompt)
void
CSysConsole::RenderDebugInfoBackground(bool bInputPrompt)
{
	float	fYTime, fYConsoleLines, fYInputBuffer;
	bool	bTexturingEnabled;

	fYTime=1.0f - m_fCharHeight;
	fYConsoleLines=fYTime - (m_fCharHeight * m_iAmLinesInDebugInfo);
	fYInputBuffer=fYConsoleLines - (m_fCharHeight * 2);

	bTexturingEnabled=false;
	if(glIsEnabled(GL_TEXTURE_2D))
	{
		bTexturingEnabled=true;
		glDisable(GL_TEXTURE_2D);
	}
	glPushMatrix();
		// first draw the BG for the time.
		glBegin(GL_QUADS);
			glColor4f(0.6f,0.6f,0.6f,0.3f);
			glVertex3f(0.0f, fYTime, Z_HEADERBACKGROUND);
			glVertex3f(1.0f, fYTime, Z_HEADERBACKGROUND);
			glVertex3f(1.0f, 1.0f, Z_HEADERBACKGROUND);
			glVertex3f(0.0f, 1.0f, Z_HEADERBACKGROUND);
		glEnd();
		// Then draw the BG for the consolelines
		glBegin(GL_QUADS);
			glColor4f(0.6f,0.6f,0.6f,0.3f);
			glVertex3f(0.0f, fYConsoleLines, Z_HEADERBACKGROUND);
			glVertex3f(1.0f, fYConsoleLines, Z_HEADERBACKGROUND);
			glColor4f(0.5f,0.5f,0.5f,0.7f);
			glVertex3f(1.0f, fYTime, Z_HEADERBACKGROUND);
			glVertex3f(0.0f, fYTime, Z_HEADERBACKGROUND);
		glEnd();
		if(bInputPrompt)
		{
			// Then draw the BG for the Inputprompt.
			glBegin(GL_QUADS);
				glColor4f(0.6f,0.6f,0.6f,0.2f);
				glVertex3f(0.0f, fYInputBuffer, Z_HEADERBACKGROUND);
				glVertex3f(1.0f, fYInputBuffer, Z_HEADERBACKGROUND);
				glColor4f(0.7f,0.7f,0.7f,0.4f);
				glVertex3f(1.0f, fYConsoleLines, Z_HEADERBACKGROUND);
				glVertex3f(0.0f, fYConsoleLines, Z_HEADERBACKGROUND);
			glEnd();
		}
	glPopMatrix();
	if(bTexturingEnabled)
	{
		glEnable(GL_TEXTURE_2D);
	}
}


// Purpose: renders the background for the FPS counter at the bottom of the screen
void
CSysConsole::RenderFPSBackground()
{
	float	fY1;
	bool	bTexturingEnabled;

	bTexturingEnabled=false;
	if(glIsEnabled(GL_TEXTURE_2D))
	{
		bTexturingEnabled=true;
		glDisable(GL_TEXTURE_2D);
	}
	glPushMatrix();
		// draw background (just 1 line).
		fY1=m_fCharHeight;
		glBegin(GL_QUADS);
			glColor4f(0.5f,0.5f,0.5f,0.7f);
			glVertex3f(0.0f, 0.0f, Z_HEADERBACKGROUND);
			glVertex3f(1.0f, 0.0f, Z_HEADERBACKGROUND);
			glColor4f(0.6f,0.6f,0.6f,0.3f);
			glVertex3f(1.0f, fY1, Z_HEADERBACKGROUND);
			glVertex3f(0.0f, fY1, Z_HEADERBACKGROUND);
		glEnd();
	glPopMatrix();
	if(bTexturingEnabled)
	{
		glEnable(GL_TEXTURE_2D);
	}
}


// Purpose: adds a line to the header. All contents currently on line in header with no iLineNo will be deleted.
// Lines start at 0. if there are less lines in the header than iLineNo+1, the line in sLine is ignored.
void
CSysConsole::AddLineToHeader(char *sLine, int iLineNo)
{
	if((iLineNo+1) > m_iAmLinesInHeader)
	{
		// not that much lines in buffer
		return;
	}
	// erase the headerline
	memset(m_parrcHBLineStarts[iLineNo],0,m_iAmCharsPerLine);
	// copy data
	strncpy(m_parrcHBLineStarts[iLineNo],sLine,m_iAmCharsPerLine);
}


// Purpose: renders an overlay layer with info when the console is NOT visible. On this layer are visible: FPS, inputprompt and DEBUG info.
// Additional info: if there are no options set to display any information this routine is NOT called. All decisions WHAT
// to draw are taken in the main system that calls this routine. 
//
// It fills the vertexarrays with the data requested. It then renders these arrays (just like the console)
// This routine looks the same as the RenderConsole routine, but we keep it separate to have full control of what the
// overlay should do.
void
CSysConsole::RenderOverlay(bool bDisplayFPS, bool bDisplayDebugInfo, bool bInputPrompt, long lMillisecondsPassed, char *sInputBuffer)
{
	bool		bVertexArraysWereEnabled, bTexCoordArraysWereEnabled;

	// check if some clientside states are already enabled. 
	if(glIsEnabled(GL_TEXTURE_COORD_ARRAY))
	{
		bTexCoordArraysWereEnabled=true;
	}
	else
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		bTexCoordArraysWereEnabled=false;
	}
	if(glIsEnabled(GL_VERTEX_ARRAY))
	{
		bVertexArraysWereEnabled=true;
	}
	else
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		bVertexArraysWereEnabled=false;
	}

	// now push serverstates to prevent ruining effectexecution when console was opened during app execution
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT);

	////////////////////////////////
	// set camera and projection matrix for 2D display
	////////////////////////////////
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();	
		glLoadIdentity();           
		glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0); 
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			// camera info
			glLoadIdentity();
			glTranslatef(0.0f,0.0f,-0.1f);		// move camera 0.1 unit on the POSITIVE Z axe
			glPushMatrix();
				// Disable possible ENABLED states of OpenGL. This will save more cycles than
				// letting those states be ENABLED. We also disable states that are disabled
				// by default, because we don't know if an effect has enabled them, causing 
				// undefined results in the console.
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_ALPHA_TEST);
				glDisable(GL_COLOR_MATERIAL);
				glDisable(GL_LIGHTING);
				glDisable(GL_STENCIL_TEST);
				glDisable(GL_BLEND);
				glDisable(GL_POLYGON_SMOOTH);
				glDisable(GL_NORMALIZE);
				glDisable(GL_TEXTURE_GEN_S); 
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_SCISSOR_TEST);
				
				// enable culling for backfaces to have single sided filling.
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CCW);
				glPolygonMode(GL_FRONT,GL_FILL);
				glCullFace(GL_BACK);

				// enable blending for the rest of the console elements
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

				// reset element counters for both sides.	
				m_iAmElementsLeft=0;
				m_iAmElementsRight=0;

				// store time passed since start in milliseconds in variable for TIME display in debuginfo.
				m_lMSecPassedSinceStart=lMillisecondsPassed;

				// Initialize array temp pointers for current positions. (because we're filling the arrays with several calls
				m_pfTmpTCArrayLeft=m_pfTexCoordArrayLeft;
				m_pfTmpTCArrayRight=m_pfTexCoordArrayRight;
				m_pfTmpVArrayLeft=m_pfVertexArrayLeft;
				m_pfTmpVArrayRight=m_pfVertexArrayRight;

				// for every requested item in the overlay, fill the contents of this item in the
				// vertex arrays.
				if(bDisplayDebugInfo)
				{
					// draw background for Debuginfo.
					RenderDebugInfoBackground(bInputPrompt);

					// insert timer into vertexbuffers
					FillOGLArrays(NULL,false,DT_TIMER);
					// insert part of console contents in arrays
					FillOGLArrays(sInputBuffer,bInputPrompt,DT_DEBUGOVERLAY);
				}
				if(bDisplayFPS)
				{
					// Draw background for FPS counter
					RenderFPSBackground();

					// Insert FPS counter into vertex buffers
					FillOGLArrays(NULL,false,DT_FPS);
				}

				///////////////////////////////
				// draw the characters in the overlay
				///////////////////////////////
				// enable texturing.
				glEnable(GL_TEXTURE_2D);

				glColor4f(1.0f,1.0f,1.0f,1.0f);
				// Draw left charmap elements first
				glBindTexture(GL_TEXTURE_2D,m_arriTextureNames[FONTLEFT]);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glTexCoordPointer(2, GL_FLOAT, 0, m_pfTexCoordArrayLeft);
				glVertexPointer(3, GL_FLOAT, 0, m_pfVertexArrayLeft);
				glDrawArrays(GL_QUADS,0,m_iAmElementsLeft);

				// Draw right charmap elements
				glBindTexture(GL_TEXTURE_2D,m_arriTextureNames[FONTRIGHT]);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glTexCoordPointer(2, GL_FLOAT, 0, m_pfTexCoordArrayRight);
				glVertexPointer(3, GL_FLOAT, 0, m_pfVertexArrayRight);
				glDrawArrays(GL_QUADS,0,m_iAmElementsRight);

				// Set the refill arrays flag, so the console gets refilled when it's brought up by the user.
				m_bRefillOGLArrays=true;
				
				/////////////////////////
				// ADD OTHER STUFF THAT SHOULD BE RENDERED HERE
				/////////////////////////
			glPopMatrix();
		// get original modelviewmatrix back
		glPopMatrix();
		// get back projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();

	// if the clientstate arrays weren't enabled we have to disable them, because they're disabled
	// by default and this could hurt effect execution if we leave them on.
	if(!bVertexArraysWereEnabled)
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	if(!bTexCoordArraysWereEnabled)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}


// Purpose: fills the texcoord and vertex arrays with the right primitives.
// Gets: sInputBuffer, pointer to char, pointer to contents of current inputbuffer if bInputPrompt is true
// Gets: bInputPrompt, bool, true if inputbuffer is valid
// Gets: iDisplayType, int, can be DT_CONSOLE,DT_DEBUGOVERLAY, DT_TIMER or DT_FPS. Needed for source of what to
// put into the buffer and how many lines we need.
// DT_CONSOLE has more lines and a header, DT_DEBUGOVERLAY doesn't have the header and less lines. The others
// are single line small strings.
//
// Elementcounters will be increased during the fill by the utilty routines.
void
CSysConsole::FillOGLArrays(char *sInputBuffer, bool bInputPrompt, int iDisplayType)
{
	GLfloat		*pCurTexCoordL,*pCurTexCoordR, *pCurVertexL, *pCurVertexR;
	int			iLineNo, iCurLineToDraw, i, iAmCBLinesToWrite, iLength, iIBPromptLength, iH, iM, iS;
	long		lMSTmp;
	CStdString	sToWrite;

	// Protect buffer access via mutex. So just 1 thread can access the buffer. (f.e. thread A is filling the
	// OGL arrays here and another one is writing in the buffer via LogLineToConsole. That thread is then blocked..)
	EnterCriticalSection(&m_csLogLine);
		switch(iDisplayType)
		{
			case DT_CONSOLE:
			{
				iLineNo=0;
				pCurTexCoordL=m_pfTexCoordArrayLeft;
				pCurTexCoordR=m_pfTexCoordArrayRight;
				pCurVertexL=m_pfVertexArrayLeft;
				pCurVertexR=m_pfVertexArrayRight;

				////////////////////////
				// Insert header
				////////////////////////

				for(i=0;i<m_iAmLinesInHeader;iLineNo++, i++)
				{
					// get a line and write it into the arrays
					WriteLine(m_parrcHBLineStarts[i],iLineNo, 
									&pCurTexCoordL, &pCurTexCoordR, &pCurVertexL, &pCurVertexR);
				}
				////////////////////////
				// Insert console content
				////////////////////////

				// From m_iTopLineOnScreen till we have enough lines on screen, write the
				// line to the arrays, adjusting the element counters.
				iAmCBLinesToWrite=(m_iAmLinesOnScreen-m_iAmLinesInHeader)-2;
				for(i=0;i<iAmCBLinesToWrite;iLineNo++, i++)
				{
					iCurLineToDraw=m_iTopLineOnScreen+i;
					if(iCurLineToDraw<0)
					{
						iCurLineToDraw=m_iAmLinesInBuffer-(-iCurLineToDraw);	
					}
					iCurLineToDraw%=m_iAmLinesInBuffer;
					// get a line and write it into the arrays
					WriteLine(m_parrcCBLineStarts[iCurLineToDraw],iLineNo, 
									&pCurTexCoordL, &pCurTexCoordR, &pCurVertexL, &pCurVertexR);
				}

				////////////////////////
				// Insert inputbuffer if available
				////////////////////////
				if(bInputPrompt)
				{
					// WriteLine will check length
					sToWrite.Format("%s%s_\0",IB_PROMPT,sInputBuffer);
					WriteLine(&sToWrite[0],iLineNo,
									&pCurTexCoordL, &pCurTexCoordR, &pCurVertexL, &pCurVertexR);
					// we can support 2 lines of inputbuffer. If we need to wrap the inputbuffer, 
					// print the 2nd line also, the rest is not printed.
					iLength=strlen(sInputBuffer);
					iIBPromptLength=strlen(IB_PROMPT);
					if(iLength > (m_iAmCharsPerLine - iIBPromptLength))
					{
						// print the second line of the inputbuffer.
						iLineNo++;
						// use m_iAmCharsPerLine as offset, because the IBprompt is put in front of
						// the string sInputBuffer and is thus part of sToWrite!
						WriteLine(&sToWrite[m_iAmCharsPerLine],iLineNo,
										&pCurTexCoordL, &pCurTexCoordR, &pCurVertexL, &pCurVertexR);
					}
				}
			}; break;
			case DT_DEBUGOVERLAY:
			{
				////////////////////////
				// Insert part of console content
				////////////////////////
				pCurTexCoordL=m_pfTmpTCArrayLeft;
				pCurTexCoordR=m_pfTmpTCArrayRight;
				pCurVertexL=m_pfTmpVArrayLeft;
				pCurVertexR=m_pfTmpVArrayRight;

				// insert m_iAmLinesInDebugInfo into the vertexbuffers
				// We start at line 1, the time is at line 0.
				iLineNo=1;
				if(m_iAmLinesInDebugInfo>=m_iAmLinesInBuffer)
				{
					iAmCBLinesToWrite=m_iAmLinesInBuffer-1;
				}
				else
				{
					iAmCBLinesToWrite=m_iAmLinesInDebugInfo;
				}
				for(i=0;i<iAmCBLinesToWrite;iLineNo++, i++)
				{
					iCurLineToDraw=m_iNextCleanLineinCB-((iAmCBLinesToWrite+1)-iLineNo);
					if(iCurLineToDraw<0)
					{
						iCurLineToDraw=m_iAmLinesInBuffer-(-iCurLineToDraw);	
					}
					// get a line and write it into the arrays
					WriteLine(m_parrcCBLineStarts[iCurLineToDraw],iLineNo, 
									&pCurTexCoordL, &pCurTexCoordR, &pCurVertexL, &pCurVertexR);
				}

				////////////////////////
				// Insert inputbuffer if available
				////////////////////////
				if(bInputPrompt)
				{
					// WriteLine will check length
					sToWrite.Format("%s%s_\0",IB_PROMPT,sInputBuffer);
					WriteLine(&sToWrite[0],iLineNo,
									&pCurTexCoordL, &pCurTexCoordR, &pCurVertexL, &pCurVertexR);
					// we can support 2 lines of inputbuffer. If we need to wrap the inputbuffer, 
					// print the 2nd line also, the rest is not printed.
					iLength=strlen(sInputBuffer);
					iIBPromptLength=strlen(IB_PROMPT);
					if(iLength > (m_iAmCharsPerLine - iIBPromptLength))
					{
						// print the second line of the inputbuffer.
						iLineNo++;
						// use m_iAmCharsPerLine as offset, because the IBprompt is put in front of
						// the string sInputBuffer and is thus part of sToWrite!
						WriteLine(&sToWrite[m_iAmCharsPerLine],iLineNo,
										&pCurTexCoordL, &pCurTexCoordR, &pCurVertexL, &pCurVertexR);
					}
				}


				// store updated pointers in the tmp pointers.
				m_pfTmpTCArrayLeft=pCurTexCoordL;
				m_pfTmpTCArrayRight=pCurTexCoordR;
				m_pfTmpVArrayLeft=pCurVertexL;
				m_pfTmpVArrayRight=pCurVertexR;
			}; break;
			case DT_TIMER:
			{
				////////////////////////
				// Insert timer.
				////////////////////////
				lMSTmp = m_lMSecPassedSinceStart;

				// calculate hours
				iH=(int)(lMSTmp / (1000*60*60));
				if(iH!=0)
				{
					lMSTmp %= (1000*60*60);
				}
				// calc minutes
				iM=lMSTmp / (1000*60);
				if(iM!=0)
				{
					lMSTmp %= (1000*60);
				}
				// calc seconds
				iS=lMSTmp / 1000;
				if(iS!=0)
				{
					lMSTmp %= 1000;
				}

				// Fill the pointers we're using with the pointers left by a previous call to this fill routine.
				// so we continue in the vertexbuffers instead of overwriting them
				pCurTexCoordL=m_pfTmpTCArrayLeft;
				pCurTexCoordR=m_pfTmpTCArrayRight;
				pCurVertexL=m_pfTmpVArrayLeft;
				pCurVertexR=m_pfTmpVArrayRight;
				sToWrite.Format("Time elapsed since application start: %04dh:%02dm:%02ds:%03dms\0",iH,iM,iS,lMSTmp);
				WriteLine(&sToWrite[0],0, &pCurTexCoordL, &pCurTexCoordR, &pCurVertexL, &pCurVertexR);
				// store updated pointers in the tmp pointers.
				m_pfTmpTCArrayLeft=pCurTexCoordL;
				m_pfTmpTCArrayRight=pCurTexCoordR;
				m_pfTmpVArrayLeft=pCurVertexL;
				m_pfTmpVArrayRight=pCurVertexR;
			}; break;
			case DT_FPS:
			{
				////////////////////////
				// Insert fps counter
				////////////////////////
				// Fill the pointers we're using with the pointers left by a previous call to this fill routine.
				// so we continue in the vertexbuffers instead of overwriting them
				pCurTexCoordL=m_pfTmpTCArrayLeft;
				pCurTexCoordR=m_pfTmpTCArrayRight;
				pCurVertexL=m_pfTmpVArrayLeft;
				pCurVertexR=m_pfTmpVArrayRight;
				sToWrite.Format("Frames per second (FPS): %3.2f\0",m_fFPS);
				WriteLine(&sToWrite[0],m_iAmLinesOnScreen-1,&pCurTexCoordL, &pCurTexCoordR, &pCurVertexL, &pCurVertexR);
				// store updated pointers in the tmp pointers.
				m_pfTmpTCArrayLeft=pCurTexCoordL;
				m_pfTmpTCArrayRight=pCurTexCoordR;
				m_pfTmpVArrayLeft=pCurVertexL;
				m_pfTmpVArrayRight=pCurVertexR;
			}; break;
		}
	LeaveCriticalSection(&m_csLogLine);
}


// Purpose: this sets the value of m_fFPS
void
CSysConsole::SetFPS(float fFPS)
{
	m_fFPS = fFPS;
}


// Purpose: creates the char texdata for the characters 32-128. These are stored at place 0-(128-32) in m_arrCharTexData.
// This data is used by WriteLine to write the correct data into the OGL arrays
void
CSysConsole::CreateCharTexData()
{
	GLfloat		fX1, fX2, fY1, fY2, fI;
	int		i;

	for(i=0;i<(128-32);i++)
	{
		fI=(GLfloat)i;
		// calculate the start x,y pair (left bottom corner) and the top right corner.
		// we use floats, so 1.0 is 256 pixels wide. Be also aware that the actual texturebitmap is
		// cut in halve and spread over 2 textures. For these calculations (0,0) is top left corner)
		fX1 = (GLfloat)((i % 16) * 32);
		fY1 = (GLfloat)((i / 16) + 1) * 32.0f;
		fX2 = fX1 + CHARWIDTH_PIX;									
		fY2 = fY1 - CHARHEIGHT_PIX;			
		// Coords are now in pixels

		m_arrCharTexData[i].m_bLeftTexture=true;

		if(fX1 >= 256.0f)
		{
			// we have to use the right texture. adjust x's
			fX1 -= 256.0f;
			fX2 -= 256.0f;
			m_arrCharTexData[i].m_bLeftTexture=false;
		}

		// calculate the real x, to get the floating point value we need and that is correctly clamped.
		fX1 = fX1 / 256.0f;
		fX2 = fX2 / 256.0f;
		// Calculate real y (clamp to 0, 1.0f)
		fY1 = 1.0f - (fY1/256.0f);
		fY2 = 1.0f - (fY2/256.0f);
	
		// fill structure
		m_arrCharTexData[i].m_fX1=fX1;
		m_arrCharTexData[i].m_fX2=fX2;
		m_arrCharTexData[i].m_fY1=fY1;
		m_arrCharTexData[i].m_fY2=fY2;
	}
}


// Purpose: renders the loading progressbar. It just calls the renderlogic inside the progressbar object
// stored as loading progressbar
void
CSysConsole::DisplayLoadingPB()
{
	if(m_pLoadingPB)
	{
		m_pLoadingPB->Paint();
	}
}


// Purpose: blends a poly with the LoadingSplashtexture on it ONTO the console contents.
void
CSysConsole::DisplaySplash()
{
	glBindTexture(GL_TEXTURE_2D, m_arriTextureNames[LOADINGSPLASH]);
	// render background
	glBegin(GL_QUADS);
		glColor4f(1.0f,1.0f,1.0f,m_fLoadingSplashOpacity);
		glTexCoord2f(0.0f,0.0f);glVertex3f(0.0f,0.0f,Z_SPLASH);
		glTexCoord2f(1.0f,0.0f);glVertex3f(1.0f,0.0f,Z_SPLASH);
		glTexCoord2f(1.0f,1.0f);glVertex3f(1.0f,1.0f,Z_SPLASH);
		glTexCoord2f(0.0f,1.0f);glVertex3f(0.0f,1.0f,Z_SPLASH);
	glEnd();
}


// Purpose: creates display lists of console drawings. This creates a display list for the
//          console background drawing plus console setup
void
CSysConsole::CreateConsoleDLs()
{
	// get listname for console background
	m_iBGDList = glGenLists(1);

	glNewList(m_iBGDList,GL_COMPILE);
		glColor3f(1.0f,1.0f,1.0f);
		glBindTexture(GL_TEXTURE_2D, m_arriTextureNames[CONSOLE_BACKGRND]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		// render background
		glBegin(GL_QUADS);
			glColor4f(1.0f,1.0f,1.0f,0.6f);
			glTexCoord2f(0.0f,0.0f);glVertex3f(0.0f,-0.15f,Z_BACKGROUND);
			glTexCoord2f(1.0f,0.0f);glVertex3f(1.0f,-0.15f,Z_BACKGROUND);
			glTexCoord2f(1.0f,1.0f);glVertex3f(1.0f,1.15f,Z_BACKGROUND);
			glTexCoord2f(0.0f,1.0f);glVertex3f(0.0f,1.15f,Z_BACKGROUND);
		glEnd();
	glEndList();
}


// Purpose: initializes the consolebuffer. This is a 2 dimensional array, and not a set of pointers to strings.
//          this is done because otherwise we get heavily memory fragmentation when a lot of text is dumped to the
//          console. Now the buffer is used cyclic. newer lines overwrite older lines.
// Gets: bClearBuffer. bool. If true, the consolebuffer will be cleared, by writing 0's on every position.
//
// It also initializes the per row pointers, and the pointers in the buffer to lines used for displaying and
// inserting new lines.
void
CSysConsole::InitConsoleBuffer(bool bClearBuffer)
{
	int			i;
	char		*pCurLine;

	if(bClearBuffer)
	{
		m_iLinesFilledInBuffer=0;
		m_iNextCleanLineinCB=0;
		// clear the console buffer completely. Put zeros on every charspot.
		memset(m_parrcConsoleBuffer,0,(m_iAmCharsPerLine + 1) * m_iAmLinesInBuffer);
	}
	else
	{
		m_iNextCleanLineinCB%=m_iAmLinesInBuffer;
	}

	// build linestart array
	for(i=0, pCurLine=m_parrcConsoleBuffer;i<m_iAmLinesInBuffer;i++)
	{
		m_parrcCBLineStarts[i]=pCurLine;
		pCurLine+=(m_iAmCharsPerLine+1);
	}
	// init the linepointers for logline and display
	CalcTopLineWithCLAtBottom();
}


// Purpose: inline function that calculates the topline position in the buffer looking at the current line
// Will modify m_iTopLineOnScreen
inline void
CSysConsole::CalcTopLineWithCLAtBottom()
{
	// see GetAmountOfCBLinesOnScreen() (below)
	m_iTopLineOnScreen=(m_iNextCleanLineinCB-((m_iAmLinesOnScreen - m_iAmLinesInHeader)-3)) % m_iAmLinesInBuffer;
}


// Purpose: calculates the amount of ConsoleBuffer lines on screen. Is inline function
inline int
CSysConsole::GetAmountOfCBLinesOnScreen()
{
	// Amount of Console Buffer lines on screen = m_iAmLinesOnScreen - #lines in header - #lines in input buffer - 1
	// line for the noscroll lines, makes:
	return (m_iAmLinesOnScreen-3)-m_iAmLinesInHeader;
}


// Purpose: Scrolls the consolebuffer write pointer 1 up. Because the consolebuffer is a cyclic buffer
// we have to make sure the garbage on the line we now call 'current line' is clean. the line will be cleared.
void
CSysConsole::ScrollConsoleBuffer()
{
	int		iAmLinesCBOnScreen, iStartOfBuffer;

	iAmLinesCBOnScreen=GetAmountOfCBLinesOnScreen();

	if(m_iLinesFilledInBuffer < (m_iAmLinesInBuffer-1))
	{
		iStartOfBuffer=0;
	}
	else
	{
		iStartOfBuffer=(m_iNextCleanLineinCB+1)%m_iAmLinesInBuffer;
	}

	// check if the topline is moved, thus the currentline is not visible on the screen.
	// or check if the currentline is the line above the topline. then we have to move the topline too.
	if((((m_iTopLineOnScreen + iAmLinesCBOnScreen)%m_iAmLinesInBuffer)==m_iNextCleanLineinCB)||
		(m_iTopLineOnScreen==iStartOfBuffer))
	{
		// current line is visible at the bottom of the screen. Scroll up the topline too.
		m_iTopLineOnScreen++;
		m_iTopLineOnScreen%=m_iAmLinesInBuffer;
	}
	m_iNextCleanLineinCB++;
	m_iNextCleanLineinCB%=m_iAmLinesInBuffer;
	
	m_iLinesFilledInBuffer++;
	// clamp to m_iAmLinesInBuffer-1. We have to calculate with the m_iAmLinesInBuffer-1 value, because we can't
	// display the m_iNextCleanLineInCB, so we have effectively m_iAmLinesInBuffer to display
	if(m_iLinesFilledInBuffer>=(m_iAmLinesInBuffer-1))
	{
		m_iLinesFilledInBuffer=(m_iAmLinesInBuffer-1);
	}
	
	// clear current line so the next logged line will be on a clean line.
	memset(m_parrcCBLineStarts[m_iNextCleanLineinCB],0,m_iAmCharsPerLine+1);
}


// Purpose: stores the passed line of chars in the lowermost slot of the console buffer. It wraps too long lines
// to new lines if needed. If bSystemRelated is true, then the wrapping will be special, because the systemlogger funcitons in
// DemoGL will add the systemstate shortcut before the line. Wrapped lines are more readable if the wrapped part is prefixed
// by a small string of spaces.
void
CSysConsole::LogLineToConsole(const char *sString, bool bScroll, bool bSystemRelated)
{
	int			i,iLenString;
	
	// thread safety lock. Only 1 thread can enter the critical section below.
	// other threads wait. 
	EnterCriticalSection(&m_csLogLine);
		iLenString=strlen(sString);
		// check length of string and write the line in the last slot.
		if(iLenString > m_iAmCharsPerLine)
		{
			// check for a console of less than PREFIXLENGTH_WRAP_SYSRELATED chars per line (?) :)
			if(PREFIXLENGTH_WRAP_SYSRELATED<m_iAmCharsPerLine)
			{
				// allowed
				// the string is longer than the width of a line. Do it in parts. 
				for(i=0;i<iLenString;)
				{
					// copy the chars
					if((i==0) || ((i>0)&&(!bSystemRelated)))
					{
						// first time or not systemrelated, don't add prefix in any case
						strncpy(m_parrcCBLineStarts[m_iNextCleanLineinCB],&sString[i],m_iAmCharsPerLine);
						i+=m_iAmCharsPerLine;
					}
					else
					{
						// add prefix first. Safe
						strncpy(m_parrcCBLineStarts[m_iNextCleanLineinCB],PREFIX_WRAP_SYSRELATED,PREFIXLENGTH_WRAP_SYSRELATED);
						// then add wrapped part.
						strncpy(m_parrcCBLineStarts[m_iNextCleanLineinCB]+PREFIXLENGTH_WRAP_SYSRELATED,
									&sString[i],(m_iAmCharsPerLine-PREFIXLENGTH_WRAP_SYSRELATED));
						i+=(m_iAmCharsPerLine-PREFIXLENGTH_WRAP_SYSRELATED);
					}

					// pad with 0 character.
					m_parrcCBLineStarts[m_iNextCleanLineinCB][m_iAmCharsPerLine]=(char)0;
					if(bScroll)
					{
						ScrollConsoleBuffer();
					}
				}
			}
		}
		else
		{
			// this is save, because we only get here if the len is <= m_iAmCharsPerLine and the len of the
			// available stringspace is m_iAmCharsPerLine+1.
			strcpy(m_parrcCBLineStarts[m_iNextCleanLineinCB],sString);
			if(bScroll)
			{
				ScrollConsoleBuffer();
			}
		}
		// Set flag to refill the vertex arrays.
		m_bRefillOGLArrays=true;
	LeaveCriticalSection(&m_csLogLine);
}



// Purpose: writes the given string to the OGL vertex/texcoord arrays. 
// Gets: pszString, pointer to zero terminated string, the string to write
// Gets: iLineNo, int, the current linenumber. line 0 is top line on screen. Needed for vertexcoord calculations
// Gets: pCurTexCoordL, pCurTexCoordR, pCurVertexL, pCurVertexR, pointers to pointers in the OGL arrays. 
// These pointers are local variables in the caller routine to keep track of the current position in these arrays.
void
CSysConsole::WriteLine(char *pszString, int iLineNo, GLfloat **pCurTexCoordL, GLfloat **pCurTexCoordR, GLfloat **pCurVertexL, GLfloat **pCurVertexR)
{
	int				iLen, i;
	GLfloat			fZ, fX, fY;
	SCharTexDat		*pCurChar;
	GLfloat			*pCurTCL, *pCurTCR, *pCurVL, *pCurVR;

	iLen = strlen(pszString);
	// protect against too long lines
	if(iLen>m_iAmCharsPerLine)
	{
		// clamp
		iLen=m_iAmCharsPerLine;
	}

	fZ = Z_TEXT;					// so characters won't overlap and ruine eachother
	fY=1.0f - ((iLineNo+1) * m_fLineYAdjust);
	fX=TEXTXBASEADJ;

	pCurTCL=*pCurTexCoordL;
	pCurTCR=*pCurTexCoordR;
	pCurVL=*pCurVertexL;
	pCurVR=*pCurVertexR;

	for(i=0;i<iLen;i++)
	{
		if(pszString[i]==0)
		{
			// end of line reached before end.. (?)
			break;
		}

		pCurChar=&m_arrCharTexData[(pszString[i]-32)];
		// write char into right array, and update counters and other related info
		if(pCurChar->m_bLeftTexture)
		{
			m_iAmElementsLeft+=4;	// add 4, we're drawing quads
			// write texture coords.
			*pCurTCL++=pCurChar->m_fX1;
			*pCurTCL++=pCurChar->m_fY1;
			*pCurTCL++=pCurChar->m_fX2;
			*pCurTCL++=pCurChar->m_fY1;
			*pCurTCL++=pCurChar->m_fX2;
			*pCurTCL++=pCurChar->m_fY2;
			*pCurTCL++=pCurChar->m_fX1;
			*pCurTCL++=pCurChar->m_fY2;
			// write vertexcoords
			*pCurVL++=fX;
			*pCurVL++=fY;
			*pCurVL++=fZ;
			*pCurVL++=fX + m_fCharWidth;
			*pCurVL++=fY;
			*pCurVL++=fZ;
			*pCurVL++=fX + m_fCharWidth;
			*pCurVL++=fY + m_fCharHeight;
			*pCurVL++=fZ;
			*pCurVL++=fX;
			*pCurVL++=fY + m_fCharHeight;
			*pCurVL++=fZ;
		}
		else
		{
			// right texture
			m_iAmElementsRight+=4;	// add 4, we're drawing quads
			// write texture coords.
			*pCurTCR++=pCurChar->m_fX1;
			*pCurTCR++=pCurChar->m_fY1;
			*pCurTCR++=pCurChar->m_fX2;
			*pCurTCR++=pCurChar->m_fY1;
			*pCurTCR++=pCurChar->m_fX2;
			*pCurTCR++=pCurChar->m_fY2;
			*pCurTCR++=pCurChar->m_fX1;
			*pCurTCR++=pCurChar->m_fY2;
			// write vertexcoords
			*pCurVR++=fX;;
			*pCurVR++=fY;;
			*pCurVR++=fZ;;
			*pCurVR++=fX + m_fCharWidth;;
			*pCurVR++=fY;;
			*pCurVR++=fZ;;
			*pCurVR++=fX + m_fCharWidth;;
			*pCurVR++=fY + m_fCharHeight;;
			*pCurVR++=fZ;;
			*pCurVR++=fX;;
			*pCurVR++=fY + m_fCharHeight;;
			*pCurVR++=fZ;;
		}
		// increase Z adjust
		fZ+=0.0001f;
		// increate current X
		fX+=m_fCharXAdjust;
	}
	*pCurVertexL=pCurVL;
	*pCurVertexR=pCurVR;
	*pCurTexCoordL=pCurTCL;
	*pCurTexCoordR=pCurTCR;
}


// Purpose: init the console. This routine is called in the system init routine, because the console
//          is a special effectobject that is rendered last.
int
CSysConsole::Init(int iWidth, int iHeight, char *sLoadingSplashTexture, int iLoadingSplashTextureType, float fOpacity)
{
	GLuint		iTextureName;

	// load data needed for console and initialize buffers and fill title buffer.
	m_iWindowWidth = iWidth;
	m_iWindowHeight = iHeight;

	m_iNextCleanLineinCB = 0;

	// load textures. Use DemoGL's textureloading routine so it is also registered.
	if(!sLoadingSplashTexture)
	{
		// splashtexture not enabled.
		m_bLoadingSplashEnabled=false;
	}
	else
	{
		// load loadingsplash texture
		iTextureName = DEMOGL_TextureLoad(sLoadingSplashTexture,iLoadingSplashTextureType,0,GL_CLAMP,GL_CLAMP,GL_LINEAR,
							GL_LINEAR,false,false,0,GL_RGB5_A1,DGL_TEXTUREDIMENSION_2D);
		if(iTextureName>0)
		{
			// all went well...
			m_bLoadingSplashEnabled=true;
			m_arriTextureNames[LOADINGSPLASH]=iTextureName;
			m_fLoadingSplashOpacity=fOpacity;
		}
	}

	// load Background texture
	iTextureName = DEMOGL_TextureLoad("systex\\cnslbg.jpg",JPGFILE,0,GL_REPEAT,GL_REPEAT,GL_LINEAR,
							GL_LINEAR, false,false,0,GL_RGB5_A1,DGL_TEXTUREDIMENSION_2D);
	if(iTextureName>0)
	{
		// all went well...
		m_arriTextureNames[CONSOLE_BACKGRND]=iTextureName;
	}
	
	iTextureName = DEMOGL_TextureLoad("systex\\fnt_cnl.bmp", BMPFILE, 0, GL_CLAMP,GL_CLAMP,GL_LINEAR,
							GL_LINEAR,true,true,0,GL_RGBA4, DGL_TEXTUREDIMENSION_2D);
	if(iTextureName>0)
	{
		// all went well...
		m_arriTextureNames[FONTLEFT]=iTextureName;
	}

	iTextureName = DEMOGL_TextureLoad("systex\\fnt_cnr.bmp",BMPFILE, 0, GL_CLAMP,GL_CLAMP,GL_LINEAR,
							GL_LINEAR,false,true,0,GL_RGBA4, DGL_TEXTUREDIMENSION_2D);
	if(iTextureName>0)
	{
		// all went well...
		m_arriTextureNames[FONTRIGHT]=iTextureName;
	}

	InitConsoleBuffer(true);
	return SYS_OK;
}


// Purpose: the object has to prepare itself that it will be shown very soon. In here is the right place to
//          upload our textures to the card and to do last minute stuff.
void
CSysConsole::Prepare()
{
	int			i, iResult;

	for(i=0;i<CONSOLE_MAXTEXTURES;i++)
	{
		if(m_arriTextureNames[i]>0)
		{
			iResult = UploadTexture(m_arriTextureNames[i]);
			if(iResult!=SYS_OK)
			{
				// uploading didn't went well...
			}
		}
	}

	// create display lists for console rendering
	CreateConsoleDLs();
}


// Purpose: is called by the DemoGL object when the mainwindow is reshaped.
void
CSysConsole::Reshape(int iWidth, int iHeight)
{
	m_iWindowWidth = iWidth;
	m_iWindowHeight = iHeight;
}


// Purpose: End the system. remove all allocated objects.
void
CSysConsole::End()
{
	free(m_parrcConsoleBuffer);
	free(m_parrcCBLineStarts);
	free(m_parrcHeaderBuffer);
	free(m_parrcHBLineStarts);

	free(m_pfTexCoordArrayLeft);
	free(m_pfTexCoordArrayRight);
	free(m_pfVertexArrayLeft);
	free(m_pfVertexArrayRight);

	m_parrcConsoleBuffer=NULL;
	m_parrcHeaderBuffer=NULL;
	m_parrcCBLineStarts=NULL;
	m_parrcHBLineStarts=NULL;

	m_pfTexCoordArrayLeft=NULL;
	m_pfTexCoordArrayRight=NULL;
	m_pfVertexArrayLeft=NULL;
	m_pfVertexArrayRight=NULL;
}
