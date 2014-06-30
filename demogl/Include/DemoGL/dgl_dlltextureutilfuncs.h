//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlltextureutilfuncs.h
// PURPOSE: include file of dgl_dlltextxureutilfuncs.cpp
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
// v1.3: Added to codebase
// v1.2: --
// v1.1: --
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLTEXTUREUTILFUNCS_H
#define _DGL_DLLTEXTUREUTILFUNCS_H

////////////////////////////////////////////////////////////////////////
//                          
//						FUNCTION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
extern	byte				*ConvertJPGtoRGBA(byte *pTGAFile, long lFileLength, int *iTrueWidth, int *iTrueHeight, 
										bool bCreateAlphaFromColor);
extern	int					ConvertTGAtoRGBA(byte *pTGAFile, byte *pRGBABuffer, int iWidth, int iHeight, 
										bool bCreateAlphaFromColor, bool *bHasAlpha);
extern	void				DoDeleteTexture(int iIndx);
extern	int					DoImportBMP(CPixelData **ppdPixelData, const char *pszFilename, int *piWidth, int *piHeight, 
										int i3DDepth, bool bCreateAlphaFromColor);
extern	int					DoImportDDS(CPixelData **ppdPixelData, const char *pszFilename, int *piWidth, int *piHeight);
extern	int					DoImportJPG(CPixelData **ppdPixelData, const char *pszFilename, int *piWidth, int *piHeight, 
										int i3DDepth, bool bCreateAlphaFromColor);
extern	int					DoImportTGA(CPixelData **ppdPixelData, const char *pszFilename, int *piWidth, int *piHeight, 
										int i3DDepth, bool bCreateAlphaFromColor);
extern	void				FlipRGBAVertical(byte *pbyRGBA, int iWidth, int iHeight);
extern	GLuint				GenerateTextureID(int iIndx);
extern	int					GetFirstEmptySlotInTextureStore(char *pszIdentName, bool *bIsEmpty);
extern	CTexture			*GetTexture(GLuint iTextureID);
extern	int					GetTextureStoreIndex(GLuint iTextureID);

//////////////////////////////////////////////////////////
//                    functions also exported from the dll.
//////////////////////////////////////////////////////////

extern	DLLEXPORT	GLuint	DEMOGL_TextureCreateFromBuffer(const int iWidth, const int iHeight, const int iDepth,
								const byte *pbyBuffer, const char *pszIdentName, const int iBorder, const GLint iWrapS, 
								const GLint iWrapT, const GLint iMinFilter, const GLint iMagFilter, const bool bMipMaps,	
								const GLint iTexUploadHint, const int iDimensions);
extern	DLLEXPORT	GLuint	DEMOGL_TextureCreateFromFBRegion(const int iX, const int iY, const int iWidth, const int iHeight, 
								const char *pszIdentName, const int iBorder, const GLint iWrapS, const GLint iWrapT, 
								const GLint iMinFilter, const GLint iMagFilter, const GLint iTexUploadHint, 
								const int iDimensions);
extern	DLLEXPORT	GLuint	DEMOGL_TextureCubeMapCreateDynamic(const int iWidth, const int iHeight, const char *pszIdentName, 
								const int iBorder, const GLint iMinFilter, const GLint iMagFilter, const GLint iTexUploadHint);
extern	DLLEXPORT	GLuint	DEMOGL_TextureCubeMapLoad(const char *pszFilename, const int iFileType, const GLint iMinFilter, 
								const GLint iMagFilter, const bool bMipMaps, const bool bCreateAlphaFromColor, const int iBorder, 
								const GLint iTexUploadHint);
extern	DLLEXPORT	int		DEMOGL_TextureCubeMapUpdateWithFBRegion(const GLuint iTextureID, const int iCubeSide, const int iX, 
								const int iY, const int iWidth, const int iHeight, const int iXDest, const int iYDest);
extern	DLLEXPORT	void	DEMOGL_TextureDelete(const GLuint iTextureID);
extern	DLLEXPORT	byte	*DEMOGL_TextureGetData(const GLuint iTextureID, int * const iWidth, int * const iHeight, 
								int * const iDepth, int * const iBitsPerPixel, int * const iDimensions, 
								bool * const bHasAlpha);
extern	DLLEXPORT	GLuint	DEMOGL_TextureLoad(const char *pszFilename, const int iFileType, const int iDepth, const GLint iWrapS,
								const GLint iWrapT, const GLint iMinFilter, const GLint iMagFilter, const bool bMipMaps, 
								const bool bCreateAlphaFromColor, const int iBorder, const GLint iTexUploadHint, 
								const int iDimensions);
extern	DLLEXPORT	void	DEMOGL_TextureUnUpload(const GLuint iTextureID);
extern	DLLEXPORT	int		DEMOGL_TextureUpdateWithFBRegion(const int iX, const int iY, const int iWidth, const int iHeight, 
								const int iXDest, const int iYDest, const GLuint iTextureID, const int iDimensions);
extern	DLLEXPORT	int		DEMOGL_TextureUpload(const GLuint iTextureID);

/////////////////////////////////[IMPORTANT NOTICE]/////////////////////////////////////////////////////
// The functions below are marked 'deprecated' from version 1.3 but still supported for backwards compatibility
// They are just wrappers around the new DEMOGL_ versions so they don't take much DLL space.
//
//				    >>> Please use the new DEMOGL_* functions in your new code <<<
//
/////////////////////////////////[IMPORTANT NOTICE]/////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
	extern	DLLEXPORT	GLuint	CreateTextureFromBuffer(int iWidth, int iHeight, byte *pBuffer, const char *sIdentName, 
										int iBorder, GLint iWrapS, GLint iWrapT, GLint iMinFilter, GLint iMagFilter, 
										bool bMipMaps, GLint iTexUploadHint);
	extern	DLLEXPORT	GLuint	CreateTextureFromFBRegion(int iX, int iY, int iWidth, int iHeight, const char *sIdentName, 
										int iBorder, GLint iWrapS, GLint iWrapT, GLint iMinFilter, GLint iMagFilter, 
										GLint iTexUploadHint);
	extern	DLLEXPORT	void	DeleteTexture(int iTextureName);
	extern	DLLEXPORT	GLuint	LoadTexture(const char *sFilename, int iFileType, GLint iWrapS, GLint iWrapT, 
										GLint iMinFilter, GLint iMagFilter, bool bMipMaps, bool bCreateAlphaFromColor, 
										int iBorder, GLint iTexUploadHint);
	extern	DLLEXPORT	void	UnUploadTexture(int iTextureName);
	extern	DLLEXPORT	int		UploadTexture(int iTextureName);
#ifdef __cplusplus
}
#endif	// __cplusplus
#endif // _DGL_DLLTEXTUREUTILFUNCS_H
