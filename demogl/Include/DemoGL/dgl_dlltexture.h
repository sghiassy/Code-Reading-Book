//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlltexture.h
// PURPOSE: include file of implementation of the CTexture class.
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
// v1.3: Added new methods and members
// v1.2: Added new methods and members
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLTEXTURE_H
#define _DGL_DLLTEXTURE_H

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
// TextureCompression type flags.
#define	DGL_TC_NONE					1

// cubemap side flags.
#define DGL_CUBEMAPSIDE_NOCUBEMAP	0
#define DGL_CUBEMAPSIDE_POSX		1
#define DGL_CUBEMAPSIDE_NEGX		2
#define DGL_CUBEMAPSIDE_POSY		3
#define DGL_CUBEMAPSIDE_NEGY		4
#define DGL_CUBEMAPSIDE_POSZ		5
#define DGL_CUBEMAPSIDE_NEGZ		6

// TextureType
#define DGL_TEXTURETYPE_NORMAL		1
#define DGL_TEXTURETYPE_COMPRESSED	2
#define DGL_TEXTURETYPE_CUBEMAP		3

// max number of pixeldatablocks supported
#define MAX_PIXELDATABLOCKS			7				// 0 for normal textures, 1-6 for the 6 cubemap sides or other special maps.

////////////////////////////////////////////////////////////////////////
//                          
//						CLASS DEFINITIONS
//
////////////////////////////////////////////////////////////////////////

// Purpose: the pixeldata class used to store the pixeldata for a 2D or 3D bitmap
// usable as a texture. 
class CPixelData
{
	public:
						CPixelData(long lBitDataLength);
						CPixelData(void);
		virtual			~CPixelData(void);
		bool			GetbHasAlpha(void);
		int				GetAmIncludedMipMaps(void);
		byte			*GetBitData(void);
		long			GetBitDataLength(void);
		int				GetHeight(void);
		GLenum			GetCompressionFormat(void);
		int				GetWidth(void);
		int				Rescale(void);
		void			SetBitData(byte *pbyData, long lBitDataLength);
		void			SetMetrics(int iWidth, int iHeight);	// 2D bitmaps supported. If 3D textures are supported, extend it.
		void			SetAmIncludedMipMaps(int iAmount);
		void			SetbHasAlpha(bool bYN);
		void			SetCompressionFormat(GLenum iFormat);

	// private method declarations
	private:
		void			InitMembers(void);
	
	// private member declarations
	private:
		byte			*m_pbyBitData;
		bool			m_bHasAlpha;
		long			m_lBitDataLength;
		int				m_iWidth;							// Pixeldata is 2D. When 3D textures are supported, this should be extended with a m_iDepth
		int				m_iHeight;							// Pixeldata is 2D. When 3D textures are supported, this should be extended with a m_iDepth
		int				m_iAmIncludedMipMaps;				// Only used with compressed dds textures
		GLenum			m_iCompressionFormat;				// Only used with compressed dds textures. DXT1 has 3 components, others 4, per pixel
};


// Purpose: texture class for texture objects in the texturestore.
// CTexture uses CPixelData objects to store the pixeldata of the bitmap(s) used as
// the texture(s). Normal textures have just 1 CPixelData object, Cube Maps for cubic environment mapping
// have more, usaly 6.
class CTexture
{
	// PUBLIC MEMBERS NOT ALLOWED

	// public method declarations
	public:
						CTexture(int iTextureType, int iWidth, int iHeight, int iDepth);
		virtual			~CTexture(void);
		void			AttachToPixelData(CPixelData *ppdPixelData, int iCubeSide);
		void			Clear(void);
		void			DecAmReferences(void);
		int				DecAmUploaded(void);
		int				GetDepth(void);
		int				GetAmReferences(void);
		int				GetAmUploaded(void);
		bool			GetbCanBeUnUploaded(void);
		bool			GetbHasMipMaps(void);
		CPixelData		*GetPixelData(int iCubeSide);
		int				GetBorder(void);
		int				GetCompressionType(void);
		char			*GetFilename(void);
		GLint			GetGLWrapS(void);
		GLint			GetGLWrapT(void);
		GLint			GetGLMagFilter(void);
		GLint			GetGLMinFilter(void);
		int				GetHeight(void);
		GLint			GetOGLPixelDepthHint(void);
		int				GetTextureDimensions(void);
		GLuint			GetTextureID(void);
		int				GetTextureType(void);
		int				GetWidth(void);
		void			IncAmReferences(void);
		int				IncAmUploaded(void);
		bool			PixelDataPresent(int iCubeSide);
		void			SetbCanBeUnUploaded(bool bYN);
		void			SetbHasMipMaps(bool bYN);
		void			SetBorder(int iBorder);
		void			SetCompressionType(int iType);	
		void			SetFilename(const char *pszFilename);
		void			SetGLFilters(GLint iMinFilter, GLint iMagFilter);
		void			SetGLWrapFlags(GLint iWrapS, GLint iWrapT);
		void			SetOGLPixelDepthHint(GLint iPixelDepthHint);
		void			SetTextureDimensions(int iDimensions);
		void			SetTextureID(GLuint iTextureID);
		void			SetTextureType(int iTextureType);
		int				UnUpload(void);
		int				Upload(void);

	// private method declarations
	private:
		void			InitMembers(void);
		int				DoUploadCubeMap(void);
		int				DoUploadNormal(void);
		int				DoUploadCompressed(void);

	
	// private member declarations
	private:
		int				m_iTextureType;
		CPixelData		*m_ppdPixelData[MAX_PIXELDATABLOCKS];
		int				m_iCompressionType;			// The type of compression (S3TC, DX1T, DX3T) is set here.
		int				m_iDimensions;
		int				m_iAmReferences;
		int				m_iAmUploaded;
		GLuint			m_iTextureID;
		int				m_iOrgWidth;
		int				m_iOrgHeight;
		int				m_iOrgDepth;
		GLint			m_iPixelDepthHint;
		bool			m_bHasMipMaps;
		GLint			m_iGLMinFilter;
		GLint			m_iGLMagFilter;
		GLint			m_iGLWrapS;
		GLint			m_iGLWrapT;
		CStdString		m_sFilename;
		int				m_iBorder;
		bool			m_bCanBeUnUploaded;			// true for all textures NOT derived from framebuffer (i.e.: the pixeldata is in the textureobject)
};

#endif	// _DGL_DLLTEXTURE_H