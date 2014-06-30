//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlltextureutilfuncs.cpp
// PURPOSE: in here are all the texture utility functions the system has. 
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// this is the file with the textxure utility functions, like all the texturemanager
// functions and various texture related functions. If it's texture related, it's stored
// here.
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
// v1.3: Added to codebase
// v1.2: --
// v1.1: --
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

// for dds fileloader.
#include <ddraw.h>

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//                      Public routines exported by the library
////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//        DEPRECATED FUNCTIONS. HERE FOR COMPATIBILITY REASONS. 
//            DON'T USE THESE. USE DEMOGL_* VARIANTS INSTEAD
///////////////////////////////////////////////////////////////////////
// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_DeleteTexture() instead.
void
DeleteTexture(int iTextureName)
{
	DEMOGL_TextureDelete(iTextureName);
}

// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_UnUploadTexture() instead.
void
UnUploadTexture(int iTextureName)
{
	DEMOGL_TextureUnUpload(iTextureName);
}

// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_LoadTexture() instead.
GLuint
LoadTexture(const char *sFilename, int iFileType, GLint iWrapS, GLint iWrapT, GLint iMinFilter, 
	GLint iMagFilter, bool bMipMaps, bool bCreateAlphaFromColor, int iBorder, GLint iTexUploadHint)
{
	return DEMOGL_TextureLoad(sFilename, 
				iFileType, 
				0,
				iWrapS,
				iWrapT,
				iMinFilter, 
				iMagFilter,
				bMipMaps,
				bCreateAlphaFromColor,
				iBorder,
				iTexUploadHint,
				DGL_TEXTUREDIMENSION_2D);
}

// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_UploadTexture() instead.
int
UploadTexture(int iTextureName)
{
	return DEMOGL_TextureUpload(iTextureName);
}

// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_CreateTextureFromBuffer() instead.
GLuint
CreateTextureFromBuffer(int iWidth, int iHeight, byte *pBuffer, const char *sIdentName, int iBorder, 
			GLint iWrapS, GLint iWrapT, GLint iMinFilter, GLint iMagFilter, bool bMipMaps, GLint iTexUploadHint)
{
	return DEMOGL_TextureCreateFromBuffer(iWidth, 
				iHeight,
				0,
				pBuffer,
				sIdentName,
				iBorder, 
				iWrapS,
				iWrapT,
				iMinFilter,
				iMagFilter,
				bMipMaps,
				iTexUploadHint,
				DGL_TEXTUREDIMENSION_2D);
}

// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_UploadTexture() instead.
GLuint
CreateTextureFromFBRegion(int iX, int iY, int iWidth, int iHeight, const char *sIdentName, int iBorder, 
		GLint iWrapS, GLint iWrapT, GLint iMinFilter, GLint iMagFilter, GLint iTexUploadHint)
{
	return DEMOGL_TextureCreateFromFBRegion(iX,
				iY,
				iWidth,
				iHeight,
				sIdentName,
				iBorder, 
				iWrapS,
				iWrapT,
				iMinFilter,
				iMagFilter,
				iTexUploadHint,
				DGL_TEXTUREDIMENSION_2D);
}

///////////////////////////////////////////////////////////////////////
//                     END DEPRECATED FUNCTIONS.                     //
///////////////////////////////////////////////////////////////////////

// Purpose: Deletes the data inside the textureobject and then deletes the textureobject
void
DEMOGL_TextureDelete(const GLuint iTextureID)
{
	CTexture	*pTex;
	int			iIndx;

	// first decrease the referencecounter.
	pTex = GetTexture(iTextureID);
	if(pTex)
	{
		iIndx = GetTextureStoreIndex(iTextureID);
		pTex->DecAmReferences();
		if(!pTex->GetAmReferences())
		{
			// it's zero, no more references to this texture. -> remove it.
			DoDeleteTexture(iIndx);
		}
	}
}


// Purpose: removes the texture with index iTextureID from cardmemory
void
DEMOGL_TextureUnUpload(const GLuint iTextureID)
{
	int			iIndx;

	iIndx = GetTextureStoreIndex(iTextureID);
	if(m_garrpTextures[iIndx]->GetAmUploaded() == 0 )
	{
		// not uploaded
		return;
	}
	
	// decrease uploaded counter.
	m_garrpTextures[iIndx]->DecAmUploaded();

	// check if uploaded counter is 0, then no effect referencing this texture is
	// keeping it uploaded, so it can be unuploaded.
	if(m_garrpTextures[iIndx]->GetAmUploaded() == 0 )
	{
		m_garrpTextures[iIndx]->UnUpload();
	}
}


// Purpose: same as LoadTexture but now the texture isn't loaded from disk but it's loaded from a buffer
// that is pointed by a pointer passed to the function. The 'filename' for identification by other objects
// is also passed. The name of the texture is returned.
//
// Note: Only used for normal, uncompressed textures, so no cubemaps nor compressed texturedata.
GLuint
DEMOGL_TextureCreateFromBuffer(const int iWidth, const int iHeight, const int iDepth,
			const byte *pbyBuffer, const char *pszIdentName, const int iBorder, const GLint iWrapS, 
			const GLint iWrapT, const GLint iMinFilter, const GLint iMagFilter, const bool bMipMaps,	
			const GLint iTexUploadHint, const int iDimensions)
{
    int				i, j, iIndx;
	CTexture		*pTex;
	byte			*pBitData;
	CPixelData		*ppdPixelData;
	bool			bIsEmpty, bHasAlpha;

	//////////////////////
	// NOTE: Currently 3D texturesupport is NOT implemented due to the lack of a generic 
	//       OpenGL dll with 1.2 compliance.
	//       The interface is prepared for it though.
	//////////////////////
	// REMOVE IFSTATEMENT BELOW WHEN 3D TEXTURE SUPPORT IS AVAILABLE
	if(iDimensions==DGL_TEXTUREDIMENSION_3D)
	{
		// no can do.
		return 0;
	}

	// Search for the texture in the texture store and report the first empty slot back.
	iIndx=GetFirstEmptySlotInTextureStore((char *)pszIdentName, &bIsEmpty);

	// Check if the texture was already found in the texturestore
	if((iIndx>=0)&&!bIsEmpty)
	{
		// the texture was already stored in the texture store. Return TextureID
		// and increase references
		m_garrpTextures[iIndx]->IncAmReferences();
		return m_garrpTextures[iIndx]->GetTextureID();
	}

	// check if the texturestore was full.
	if(iIndx<0)
	{
		// it was full
		return 0;
	}

	// An empty slot was found and the texture was not found in the texturestore. It should be
	// stored at the found empty slot.
	pTex = new CTexture(DGL_TEXTURETYPE_NORMAL,iWidth, iHeight, iDepth);

	// Create a pixeldata object to hold the data.
	ppdPixelData = new CPixelData(4*iWidth*iHeight);			// we only support 32bit bitmaps internally.

	// Get pointer to bitdata buffer
	pBitData=ppdPixelData->GetBitData();

	// copy data into the bitdata buffer.
	for (i = 0, bHasAlpha=false; i < iHeight; i++) 
	{
		for (j = 0; j < iWidth; j++) 
		{
			pBitData[(i*iWidth*4) + j * 4] = pbyBuffer[(i*iWidth*4) + j * 4];			// R
			pBitData[(i*iWidth*4) + j * 4+1] = pbyBuffer[(i*iWidth*4) + j * 4+1];		// G
			pBitData[(i*iWidth*4) + j * 4+2] = pbyBuffer[(i*iWidth*4) + j * 4+2];		// B
			pBitData[(i*iWidth*4) + j * 4+3] = pbyBuffer[(i*iWidth*4) + j * 4+3];		// A
			bHasAlpha|=(pBitData[(i*iWidth*4) + j * 4+3] > 0);
		}
	}

	ppdPixelData->SetbHasAlpha(bHasAlpha);
	// set width/height of texture using the width/height of the bitmap
	ppdPixelData->SetMetrics(iWidth, iHeight);

	// attach the PixelData object to the texture
	pTex->AttachToPixelData(ppdPixelData, DGL_CUBEMAPSIDE_NOCUBEMAP);

	// set texture upload hint.
	if((!iTexUploadHint)||(m_gpDemoDat->GetiTexDepth()==32))
	{
		pTex->SetOGLPixelDepthHint(m_gpDemoDat->GetTexturePixelDepthHint());
	}
	else
	{
		pTex->SetOGLPixelDepthHint(iTexUploadHint);
	}

	// increase the reference and set texture properties
	pTex->IncAmReferences();
	pTex->SetBorder(iBorder);
	pTex->SetbHasMipMaps(bMipMaps);
	pTex->SetFilename(pszIdentName);
	pTex->SetGLFilters(iMinFilter,iMagFilter);
	pTex->SetGLWrapFlags(iWrapS, iWrapT);
	pTex->SetTextureDimensions(iDimensions);
	pTex->SetTextureID(GenerateTextureID(iIndx));
	
	// Store the textureobject in the texture store
	m_garrpTextures[iIndx]=pTex;

	// Return index
	return pTex->GetTextureID();
}


// Purpose: updates a texture's color fragments with a region from the framebuffer. 
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
//
// Note: Only used for normal, uncompressed textures, so no cubemaps nor compressed texturedata.
int	
DEMOGL_TextureUpdateWithFBRegion(const int iX, const int iY, const int iWidth, const int iHeight, 
			const int iXDest, const int iYDest, const GLuint iTextureID, const int iDimensions)
{
	int			iIndx;

	// check if texture exists
	iIndx=GetTextureStoreIndex(iTextureID);
	if(!m_garrpTextures[iIndx])
	{
		return SYS_NOK;
	}

	//////////////////////
	// NOTE: Currently 3D texturesupport is NOT implemented due to the lack of a generic 
	//       OpenGL dll with 1.2 compliance.
	//       The interface is prepared for it though.
	//////////////////////
	// REMOVE IFSTATEMENT BELOW WHEN 3D TEXTURE SUPPORT IS AVAILABLE
	if(iDimensions==DGL_TEXTUREDIMENSION_3D)
	{
		// no can do.
		return SYS_NOK;
	}

	if(m_garrpTextures[iIndx]->GetAmUploaded() == 0 )
	{
		// not uploaded
		return SYS_NOK;
	}
	
	switch(iDimensions)
	{
		case DGL_TEXTUREDIMENSION_1D:
		{
			// generate the texture the caller asked for.
			glBindTexture( GL_TEXTURE_1D, iTextureID);	
			glCopyTexSubImage1D(GL_TEXTURE_1D, 0, (GLint)iXDest, (GLint)iX, (GLint)iY, (GLsizei)iWidth);
		}; break;
		case DGL_TEXTUREDIMENSION_2D:
		{
			// generate the texture the caller asked for.
			glBindTexture( GL_TEXTURE_2D, iTextureID);	
			glCopyTexSubImage2D(GL_TEXTURE_2D,0, (GLint)iXDest, (GLint)iYDest,(GLint)iX, (GLint)iY, (GLsizei)iWidth, (GLsizei)iHeight);
		}; break;
		case DGL_TEXTUREDIMENSION_3D:
		{
			// not supported yet
		}; break;
	}
	return SYS_OK;
}


// Purpose: updates a cubemap texture's color fragments with a region from the framebuffer for a given side.
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
//
int	
DEMOGL_TextureCubeMapUpdateWithFBRegion(const GLuint iTextureID, const int iCubeSide, const int iX, 
				const int iY, const int iWidth, const int iHeight, const int iXDest, const int iYDest)
{
	int				iIndx;
	CStdString		sToLog;
	GLuint			iCurrentSide;

	// check if texture exists
	iIndx=GetTextureStoreIndex(iTextureID);
	if(!m_garrpTextures[iIndx])
	{
		return SYS_NOK;
	}

	if(m_garrpTextures[iIndx]->GetAmUploaded() == 0 )
	{
		// not uploaded
		return SYS_NOK;
	}

	// Test of the current renderer supports cubemaps.
	if(!(m_gpExtensions->GetExtensionAvailability(DGL_GL_EXT_texture_cube_map)||
		 m_gpExtensions->GetExtensionAvailability(DGL_GL_ARB_texture_cube_map)))
	{
		// cubemaps are not supported.
		sToLog.Format("Update action of cubemap texture: %d failed. ICD doesn't support cubemaps.",iTextureID);
		LogFeedback(&sToLog[0],true,"DEMOGL_TextureCubeMapUpdateWithFBRegion",false);
		return 0;
	}
	
	// Enable Cubemapping in OpenGL
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);

	// generate the texture the caller asked for.
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, iTextureID);	
	// Determine the OpenGL identifier for the current cubeside
	switch(iCubeSide)
	{
		// Use ARB side definitions, but EXT definitions have the same values.
		case DGL_CUBEMAPSIDE_POSX:
		{
			iCurrentSide=GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
		}; break;
		case DGL_CUBEMAPSIDE_NEGX:
		{
			iCurrentSide=GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
		}; break;
		case DGL_CUBEMAPSIDE_POSY:
		{
			iCurrentSide=GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
		}; break;
		case DGL_CUBEMAPSIDE_NEGY:
		{
			iCurrentSide=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
		}; break;
		case DGL_CUBEMAPSIDE_POSZ:
		{
			iCurrentSide=GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
		}; break;
		case DGL_CUBEMAPSIDE_NEGZ:
		{
			iCurrentSide=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
		}; break;
	}

	// Update the texture with the framebuffer contents
	glCopyTexSubImage2D(iCurrentSide,0, (GLint)iXDest, (GLint)iYDest,(GLint)iX, (GLint)iY, (GLsizei)iWidth, (GLsizei)iHeight);

	// Disable Cubemapping in OpenGL
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

	return SYS_OK;
}




// Purpose: Creates a texture from a framebuffer region. The actual texturedata is not stored in local memory, and
// therefor not reuploadable. 
//
// Note: Only used for normal, uncompressed textures, so no cubemaps nor compressed texturedata.
GLuint
DEMOGL_TextureCreateFromFBRegion(const int iX, const int iY, const int iWidth, const int iHeight, 
			const char *pszIdentName, const int iBorder, const GLint iWrapS, const GLint iWrapT, 
			const GLint iMinFilter, const GLint iMagFilter, const GLint iTexUploadHint, 
			const int iDimensions)
{
    int				iIndx;
	CTexture		*pTex;
	GLint			iTexHint;
	GLenum			iTarget;
	bool			bIsEmpty;

	//////////////////////
	// NOTE: Currently 3D texturesupport is NOT implemented due to the lack of a generic 
	//       OpenGL dll with 1.2 compliance.
	//       The interface is prepared for it though.
	//////////////////////
	// REMOVE IFSTATEMENT BELOW WHEN 3D TEXTURE SUPPORT IS AVAILABLE
	if(iDimensions==DGL_TEXTUREDIMENSION_3D)
	{
		// no can do.
		return 0;
	}

	
	// Search for the texture in the texture store and report the first empty slot back.
	iIndx=GetFirstEmptySlotInTextureStore((char *)pszIdentName, &bIsEmpty);

	// Check if the texture was already found in the texturestore
	if((iIndx>=0)&&!bIsEmpty)
	{
		// the texture was already stored in the texture store. Return TextureID
		// and increase references
		m_garrpTextures[iIndx]->IncAmReferences();
		return m_garrpTextures[iIndx]->GetTextureID();
	}

	// check if the texturestore was full.
	if(iIndx<0)
	{
		// it was full
		return 0;
	}

	// An empty slot was found and the texture was not found in the texturestore. It should be
	// stored at the found empty slot.
	pTex = new CTexture(DGL_TEXTURETYPE_NORMAL,iWidth, iHeight, 0);
	pTex->SetTextureID(GenerateTextureID(iIndx));

	if((!iTexUploadHint)||(m_gpDemoDat->GetiTexDepth()==32))
	{
		iTexHint = m_gpDemoDat->GetTexturePixelDepthHint();
	}
	else
	{
		iTexHint = iTexUploadHint;
	}

	switch(iDimensions)
	{
		case DGL_TEXTUREDIMENSION_1D:
		{
			iTarget=GL_TEXTURE_1D;
			// generate the texture the caller asked for.
			glBindTexture( GL_TEXTURE_1D, pTex->GetTextureID());	
			glCopyTexImage1D(GL_TEXTURE_1D,0,iTexHint,(GLint)iX, (GLint)iY, (GLsizei)iWidth, iBorder);
		}; break;
		case DGL_TEXTUREDIMENSION_2D:
		{
			iTarget=GL_TEXTURE_2D;
			// generate the texture the caller asked for.
			glBindTexture( GL_TEXTURE_2D, pTex->GetTextureID());	
			glCopyTexImage2D(GL_TEXTURE_2D,0,iTexHint,(GLint)iX, (GLint)iY, (GLsizei)iWidth, (GLsizei)iHeight, iBorder);
		}; break;
		case DGL_TEXTUREDIMENSION_3D:
		{
			// not supported yet
			//glBindTexture( GL_TEXTURE_3D, (iIndx + 1));	
			//glCopyTexSubImage3D(GL_TEXTURE_3D,0,iTexHint,(GLint)iX, (GLint)iY, (GLsizei)iWidth, (GLsizei)iHeight, iBorder);
		}; break;
	}

	// increase amount of times this texture is 'uploaded'.
	(void)pTex->IncAmUploaded();

	// set additional texture params. These will be stored with the texture object in OpenGL.
	glTexParameteri(iTarget, GL_TEXTURE_MIN_FILTER, iMinFilter);
	glTexParameteri(iTarget, GL_TEXTURE_MAG_FILTER, iMagFilter);
	glTexParameteri(iTarget, GL_TEXTURE_WRAP_S, iWrapS);
	glTexParameteri(iTarget, GL_TEXTURE_WRAP_T, iWrapT);

	// set name for unique identification.
	pTex->IncAmReferences();
	pTex->SetBorder(iBorder);
	pTex->SetbHasMipMaps(false);
	pTex->SetFilename(pszIdentName);
	pTex->SetGLFilters(iMinFilter,iMagFilter);
	pTex->SetGLWrapFlags(iWrapS, iWrapT);
	pTex->SetTextureDimensions(iDimensions);
	// texture can't be unuploaded and thus not re-uploaded, because the texture data is not read from buffer.
	pTex->SetbCanBeUnUploaded(false);
	m_garrpTextures[iIndx]=pTex;
	return pTex->GetTextureID();
}


// Purpose: Creates a cubemap texture that will be dynamically updated. It basicly creates a cubemap texture of
// iWidth x iHeight pixels and uploads it to OpenGL. This will create the cubemap texture frame inside OpenGL and
// is then updateable by using DEMOGL_TextureCubeMapUpdateWithFBRegion(). The cubemap is black and not reuploadable.
// The TextureID is returned when succeeded or 0 if not.
GLuint
DEMOGL_TextureCubeMapCreateDynamic(const int iWidth, const int iHeight, const char *pszIdentName, 
			const int iBorder, const GLint iMinFilter, const GLint iMagFilter, const GLint iTexUploadHint)
{
    int				iIndx, i, iCurrentSide;
	CTexture		*pTex;
	CPixelData		*ppdCubeSide;
	GLint			iTexHint;
	bool			bIsEmpty;
	CStdString		sToLog;
	byte			*pbyBitMap;

	// Test of the current renderer supports cubemaps.
	if(!(m_gpExtensions->GetExtensionAvailability(DGL_GL_EXT_texture_cube_map)||
		 m_gpExtensions->GetExtensionAvailability(DGL_GL_ARB_texture_cube_map)))
	{
		// cubemaps are not supported.
		sToLog.Format("Creation of cubemap texture: %s failed. ICD doesn't support cubemaps.",pszIdentName);
		LogFeedback(&sToLog[0],true,"DEMOGL_TextureCubeMapCreateFromFBRegion",false);
		return 0;
	}

	// Search for the texture in the texture store and report the first empty slot back.
	iIndx=GetFirstEmptySlotInTextureStore((char *)pszIdentName, &bIsEmpty);

	// Check if the texture was already found in the texturestore
	if((iIndx>=0)&&!bIsEmpty)
	{
		// the texture was already stored in the texture store. Return TextureID
		// and increase references
		m_garrpTextures[iIndx]->IncAmReferences();
		return m_garrpTextures[iIndx]->GetTextureID();
	}

	// check if the texturestore was full.
	if(iIndx<0)
	{
		// it was full
		return 0;
	}

	// An empty slot was found and the texture was not found in the texturestore. It should be
	// stored at the found empty slot.
	pTex = new CTexture(DGL_TEXTURETYPE_CUBEMAP, iWidth, iHeight, 0);
	if((!iTexUploadHint)||(m_gpDemoDat->GetiTexDepth()==32))
	{
		iTexHint = m_gpDemoDat->GetTexturePixelDepthHint();
	}
	else
	{
		iTexHint = iTexUploadHint;
	}
	// Generate a new TextureID for this texture
	pTex->SetTextureID(GenerateTextureID(iIndx));

	// store object inside texturestore
	m_garrpTextures[iIndx]=pTex;

	pTex->IncAmReferences();
	pTex->SetFilename(pszIdentName);

	for(i=1;i<=6;i++)
	{
		// create a black bitmap for each cubeside. 
		ppdCubeSide = new CPixelData(iWidth * iHeight *4);
		pbyBitMap=ppdCubeSide->GetBitData();
		memset(pbyBitMap,0,iWidth * iHeight * 4);
		ppdCubeSide->SetMetrics(iWidth, iHeight);

		switch(i)
		{
			case DGL_CUBEMAPSIDE_POSX:
			{
				iCurrentSide=DGL_CUBEMAPSIDE_POSX;
			}; break;
			case DGL_CUBEMAPSIDE_NEGX:
			{
				iCurrentSide=DGL_CUBEMAPSIDE_NEGX;
			}; break;
			case DGL_CUBEMAPSIDE_POSY:
			{
				iCurrentSide=DGL_CUBEMAPSIDE_POSY;
			}; break;
			case DGL_CUBEMAPSIDE_NEGY:
			{
				iCurrentSide=DGL_CUBEMAPSIDE_NEGY;
			}; break;
			case DGL_CUBEMAPSIDE_POSZ:
			{
				iCurrentSide=DGL_CUBEMAPSIDE_POSZ;
			}; break;
			case DGL_CUBEMAPSIDE_NEGZ:
			{
				iCurrentSide=DGL_CUBEMAPSIDE_NEGZ;
			}; break;
		}
		pTex->AttachToPixelData(ppdCubeSide,iCurrentSide);
	}

	// set name for unique identification.
	pTex->SetBorder(iBorder);
	pTex->SetbHasMipMaps(false);
	pTex->SetGLFilters(iMinFilter,iMagFilter);

	// texture can't be unuploaded and thus not re-uploaded, because the texture data is not read from buffer.
	pTex->SetbCanBeUnUploaded(false);

	// Upload the texture to OpenGL.
	pTex->Upload();

	// return textureid
	return pTex->GetTextureID();
}



// Purpose: loads a cubemap as a texture. It returns the TextureID of the cubemap texture,which is directly usable with
// OpenGL. It acts as DEMOGL_TextureLoad, but works only with 2D textures, since cubemaps are implemented in OpenGL as
// 2D textures. 
// Cubemap textures consist of 6 bitmaps which are treatened as 1 texture. The 6 bitmaps should have a common filename
// part, like 'mycubemap_'. This routine then adds negx, posx, negy, posy, negz and posz to load all 6 bitmapfiles.
// All files should be of the same type, f.e. JPG. 
//
// Cubemap textures are identified by their pszFilename, not by their whole filename (thus not with the negx etc part).
// Bitmaps in a cubemap texture can't be used separately. If the bitmap has to be used as a separate texture, it should
// be loaded separately. 
GLuint
DEMOGL_TextureCubeMapLoad(const char *pszFilename, const int iFileType, const GLint iMinFilter, const GLint iMagFilter, 
			const bool bMipMaps, const bool bCreateAlphaFromColor, const int iBorder, const GLint iTexUploadHint)
{
	int				i, iIndx, iResult, iWidth, iHeight, iCurrentSide;
	CTexture		*pTex;
	CStdString		sToLog, sCurrentFilename, sFileExtension;
	CPixelData		*ppdPixelData;
	bool			bIsEmpty;

	// Test of the current renderer supports cubemaps.
	if(!(m_gpExtensions->GetExtensionAvailability(DGL_GL_EXT_texture_cube_map)||
		 m_gpExtensions->GetExtensionAvailability(DGL_GL_ARB_texture_cube_map)))
	{
		// cubemaps are not supported.
		sToLog.Format("Loading of cubemap texture: %s* failed. ICD doesn't support cubemaps.",pszFilename);
		LogFeedback(&sToLog[0],true,"DEMOGL_TextureCubeMapLoad",false);
		return 0;
	}

	// Search for the texture in the texture store and report the first empty slot back.
	iIndx=GetFirstEmptySlotInTextureStore((char *)pszFilename, &bIsEmpty);

	// Check if the texture was already found in the texturestore
	if((iIndx>=0)&&!bIsEmpty)
	{
		// the texture was already stored in the texture store. Return TextureID
		// and increase references
		m_garrpTextures[iIndx]->IncAmReferences();
		return m_garrpTextures[iIndx]->GetTextureID();
	}

	// check if the texturestore was full.
	if(iIndx<0)
	{
		// it was full
		return 0;
	}

	switch(iFileType)
	{
		case DGL_FTYPE_JPGFILE:
		{
			sFileExtension.Format(".jpg");
		}; break;
		case DGL_FTYPE_BMPFILE:
		{
			sFileExtension.Format(".bmp");
		}; break;
		case DGL_FTYPE_TGAFILE:
		{
			sFileExtension.Format(".tga");
		}; break;
		///////////////////////////////
		// ADD MORE TYPES HERE
		///////////////////////////////
		default:
		{	
			// error, return
			iResult = SYS_NOK;
			return 0;
		}; break;
	}

	// Create new texture object. No depth. width and height will be adjusted by the attachment of the pixeldata
	// of the bitmaps.
	pTex = new CTexture(DGL_TEXTURETYPE_CUBEMAP, 0, 0, 0);

	// add params to textureobject. These will be used when the texture is actually created inside OpenGL
	if((!iTexUploadHint)||(m_gpDemoDat->GetiTexDepth()==32))
	{
		pTex->SetOGLPixelDepthHint(m_gpDemoDat->GetTexturePixelDepthHint());
	}
	else
	{
		pTex->SetOGLPixelDepthHint(iTexUploadHint);
	}
	// set properties of texture
	pTex->SetTextureID(GenerateTextureID(iIndx));
	pTex->SetbHasMipMaps(bMipMaps);
	pTex->SetTextureDimensions(DGL_TEXTUREDIMENSION_2D);
	pTex->SetGLFilters(iMinFilter,iMagFilter);
	pTex->IncAmReferences();
	pTex->SetFilename(pszFilename);
	pTex->SetBorder(iBorder);

	// now load the data from the 6 bitmap files. The routines will return in ppdPixelData NULL if failed or
	// a pointer to a pixeldata object with the bitmap loaded.
	for(i=1;i<=6;i++)
	{
		switch(i)
		{
			case DGL_CUBEMAPSIDE_POSX:
			{
				sCurrentFilename.Format("%sposx%s",pszFilename, &sFileExtension[0]);
				iCurrentSide=DGL_CUBEMAPSIDE_POSX;
			}; break;
			case DGL_CUBEMAPSIDE_NEGX:
			{
				sCurrentFilename.Format("%snegx%s",pszFilename, &sFileExtension[0]);
				iCurrentSide=DGL_CUBEMAPSIDE_NEGX;
			}; break;
			case DGL_CUBEMAPSIDE_POSY:
			{
				sCurrentFilename.Format("%sposy%s",pszFilename, &sFileExtension[0]);
				iCurrentSide=DGL_CUBEMAPSIDE_POSY;
			}; break;
			case DGL_CUBEMAPSIDE_NEGY:
			{
				sCurrentFilename.Format("%snegy%s",pszFilename, &sFileExtension[0]);
				iCurrentSide=DGL_CUBEMAPSIDE_NEGY;
			}; break;
			case DGL_CUBEMAPSIDE_POSZ:
			{
				sCurrentFilename.Format("%sposz%s",pszFilename, &sFileExtension[0]);
				iCurrentSide=DGL_CUBEMAPSIDE_POSZ;
			}; break;
			case DGL_CUBEMAPSIDE_NEGZ:
			{
				sCurrentFilename.Format("%snegz%s",pszFilename, &sFileExtension[0]);
				iCurrentSide=DGL_CUBEMAPSIDE_NEGZ;
			}; break;
		}

		// load the bitmap using hte routine that can handle the type of the bitmap
		switch(iFileType)
		{
			case DGL_FTYPE_JPGFILE:
			{
				iResult = DoImportJPG(&ppdPixelData, &sCurrentFilename[0], &iWidth, &iHeight, 0, bCreateAlphaFromColor);
			}; break;
			case DGL_FTYPE_BMPFILE:
			{
				iResult = DoImportBMP(&ppdPixelData, &sCurrentFilename[0], &iWidth, &iHeight, 0, bCreateAlphaFromColor);
			}; break;
			case DGL_FTYPE_TGAFILE:
			{
				iResult = DoImportTGA(&ppdPixelData, &sCurrentFilename[0], &iWidth, &iHeight, 0, bCreateAlphaFromColor);
			}; break;
			///////////////////////////////
			// ADD MORE TYPES HERE
			///////////////////////////////
		}

		// test the result of the loading to perform further actions.
		switch(iResult)
		{
			case SYS_OK:
			{
				// set width/height of texture using the width/height of the bitmap
				ppdPixelData->SetMetrics(iWidth, iHeight);

				// flip the bitmap vertically so Cubic Envmapping hardware handles the textures correctly.
				FlipRGBAVertical(ppdPixelData->GetBitData(),iWidth, iHeight);

				// attach the PixelData object to the texture. 
				pTex->AttachToPixelData(ppdPixelData, iCurrentSide);

			}; break;
			default:
			{
				// someting went wrong. 
				sToLog.Format("Loading of cubemap texture bitmap: %s didn't succeed. Errorcode: %d.",&sCurrentFilename[0], iResult);
				LogFeedback(&sToLog[0],true,"LoadTexture",false);
				// remove the texture object
				delete pTex;
				// return error, i.e texture ID 0;
				return 0;
			}; break;
		}
	}

	// Everything went allright. Add the texture object to the store and return the textureID
	m_garrpTextures[iIndx]=pTex;
	return pTex->GetTextureID();
}



// Purpose: loads a texture with the filename sFilename into the texture base and returns the name of
//          this texture so the caller can refer to that texture with that name.
//          If the texture is already loaded into memory, the name of that texture is returned and the reference
//		    is increased by one, if not in core, it's loaded and referencecount is 1.
//
// Additional info: the texturename is the index in m_arrpTextures + 1. 
//
// Note: Only used for normal, uncompressed textures, so no cubemaps nor compressed texturedata.
GLuint
DEMOGL_TextureLoad(const char *pszFilename, const int iFileType, const int iDepth, const GLint iWrapS,
			const GLint iWrapT, const GLint iMinFilter, const GLint iMagFilter, const bool bMipMaps, 
			const bool bCreateAlphaFromColor, const int iBorder, const GLint iTexUploadHint, 
			const int iDimensions)
{
	int				iIndx, iResult, iWidth, iHeight, iTextureType;
	CTexture		*pTex;
	CStdString		sToLog;
	CPixelData		*ppdPixelData;
	bool			bIsEmpty;

	//////////////////////
	// NOTE: Currently 3D texturesupport is NOT implemented due to the lack of a generic 
	//       OpenGL dll with 1.2 compliance.
	//       The interface is prepared for it though.
	//////////////////////
	// REMOVE IFSTATEMENT BELOW WHEN 3D TEXTURE SUPPORT IS AVAILABLE
	if(iDimensions==DGL_TEXTUREDIMENSION_3D)
	{
		// no can do.
		return 0;
	}

	// Search for the texture in the texture store and report the first empty slot back.
	iIndx=GetFirstEmptySlotInTextureStore((char *)pszFilename, &bIsEmpty);

	// Check if the texture was already found in the texturestore
	if((iIndx>=0)&&!bIsEmpty)
	{
		// the texture was already stored in the texture store. Return TextureID
		// and increase references
		m_garrpTextures[iIndx]->IncAmReferences();
		return m_garrpTextures[iIndx]->GetTextureID();
	}

	// check if the texturestore was full.
	if(iIndx<0)
	{
		// it was full
		return 0;
	}

	// now load the data from the file. The routines will return in ppdPixelData NULL if failed or
	// a pointer to a pixeldata object with the bitmap loaded.
	switch(iFileType)
	{
		case DGL_FTYPE_JPGFILE:
		{
			iTextureType=DGL_TEXTURETYPE_NORMAL;
			iResult = DoImportJPG(&ppdPixelData,pszFilename, &iWidth, &iHeight, iDepth, bCreateAlphaFromColor);
		}; break;
		case DGL_FTYPE_BMPFILE:
		{
			iTextureType=DGL_TEXTURETYPE_NORMAL;
			iResult = DoImportBMP(&ppdPixelData,pszFilename, &iWidth, &iHeight, iDepth, bCreateAlphaFromColor);
		}; break;
		case DGL_FTYPE_TGAFILE:
		{
			iTextureType=DGL_TEXTURETYPE_NORMAL;
			iResult = DoImportTGA(&ppdPixelData,pszFilename, &iWidth, &iHeight, iDepth ,bCreateAlphaFromColor);
		}; break;
		case DGL_FTYPE_DDSFILE:
		{

			// Check if the needed extensions are supported
			if(!(m_gpExtensions->GetExtensionAvailability(DGL_GL_ARB_texture_compression)&&
				 m_gpExtensions->GetExtensionAvailability(DGL_GL_EXT_texture_compression_s3tc)))
			{
				// nope. 
				LogFeedback("Compressed textures are not supported by the ICD.",true, "DEMOGL_TextureLoad",false);

				// return now.
				return 0;
			}

			// Not all options are available for DDS compressed bitmaps.
			iTextureType=DGL_TEXTURETYPE_COMPRESSED;
			iResult = DoImportDDS(&ppdPixelData,pszFilename, &iWidth, &iHeight);
		}; break;
		///////////////////////////////
		// ADD MORE TYPES HERE
		///////////////////////////////
		default:
		{
			iResult = SYS_NOK;
		}; break;
	}

	// test the result of the loading to perform further actions.
	switch(iResult)
	{
		case SYS_OK:
		{
			// all went well.
			pTex = new CTexture(iTextureType, iWidth, iHeight, iDepth);

			// set width/height of texture using the width/height of the bitmap
			ppdPixelData->SetMetrics(iWidth, iHeight);

			// attach the PixelData object to the texture
			pTex->AttachToPixelData(ppdPixelData, DGL_CUBEMAPSIDE_NOCUBEMAP);

			// add params to textureobject. These will be used when the texture is actually created inside OpenGL
			if((!iTexUploadHint)||(m_gpDemoDat->GetiTexDepth()==32))
			{
				pTex->SetOGLPixelDepthHint(m_gpDemoDat->GetTexturePixelDepthHint());
			}
			else
			{
				pTex->SetOGLPixelDepthHint(iTexUploadHint);
			}
			// set properties of texture
			pTex->SetTextureID(GenerateTextureID(iIndx));

			// This will be true also if the caller specified 'mipmaps', while the bitmap is a
			// DDS file and doesn't contain any mipmaps. That's ok, the DDS (compressed texture) uploader
			// helper routine will look at the pixelstore properties anyway.
			pTex->SetbHasMipMaps(bMipMaps || (ppdPixelData->GetAmIncludedMipMaps()>0));

			pTex->SetTextureDimensions(iDimensions);
			pTex->SetGLFilters(iMinFilter,iMagFilter);
			pTex->SetGLWrapFlags(iWrapS, iWrapT);
			pTex->IncAmReferences();
			pTex->SetFilename(pszFilename);
			pTex->SetBorder(iBorder);
			m_garrpTextures[iIndx]=pTex;
			return pTex->GetTextureID();
		}; break;
		default:
		{
			// someting went wrong. 
			sToLog.Format("Loading of texture %s didn't succeed. Errorcode: %d.",pszFilename, iResult);
			LogFeedback(&sToLog[0],true,"DEMOGL_TextureLoad",false);
			return 0;
		}; break;
	}
}


// Purpose: returns the texturedata of the texture with the id iTextureID. If the texture is not found, 
// or the texture is not build from an inmemory buffer, NULL is returned, 
// otherwise a pointer to the texturedata is returned, plus the width, height, depth, hasalpha, dimensions and bitdepth parameters
// are filled in.
//
// Returns NULL also when a pointer passed is NULL. 
//
// Note: Only used for normal, uncompressed textures, so no cubemaps nor compressed texturedata.
byte
*DEMOGL_TextureGetData(const GLuint iTextureID, int * const iWidth, int * const iHeight, 
			int * const iDepth, int * const iBitsPerPixel, int * const iDimensions, 
			bool * const bHasAlpha)
{
	CTexture		*pTex;
	CPixelData		*ppdPixelData;

	// check submitted pointers.
	if((!iWidth)||(!iHeight)||(!iDepth)||(!iBitsPerPixel)||(!iDimensions)||(!bHasAlpha))
	{
		// pointer error.
		return NULL;
	}

	// get the texture.
	pTex=GetTexture(iTextureID);
	if(!pTex)
	{
		// return NULL, coz not found
		return NULL;
	}
	
	// check if the texture is unuploadable, thus has a buffer in memory and is not created from a framebufferpart
	if(pTex->GetbCanBeUnUploaded())
	{
		// yes, fill in params.
		*iWidth=pTex->GetWidth();
		*iHeight=pTex->GetHeight();
		*iDepth=pTex->GetDepth();
		*iBitsPerPixel=32;					// We only support 32bpp internally. 
		*iDimensions=pTex->GetTextureDimensions();
		ppdPixelData=pTex->GetPixelData(DGL_CUBEMAPSIDE_NOCUBEMAP);
		*bHasAlpha=ppdPixelData->GetbHasAlpha();
		return ppdPixelData->GetBitData();
	}
	else
	{
		return NULL;
	}
}


// Purpose: Does the uploading of a texture with the number iTextureID. It will bubble the uploading result.
int
DEMOGL_TextureUpload(const GLuint iTextureID)
{
	int				iResult;
	CTexture		*pTex;

	// Get the textureobject
	pTex = GetTexture(iTextureID);
	if(!pTex)
	{
		// not loaded
		return SYS_NOK;
	}
	if(!pTex->GetbCanBeUnUploaded())
	{
		// texture can't be reuploaded.
		return SYS_NOK;
	}
	iResult = pTex->Upload();
	return iResult;
}


////////////////////////////////////////////////////////////////////////
//                         Library ONLY routines
////////////////////////////////////////////////////////////////////////

// Purpose: finds the first empty slot usable for a new texture.
// Returns:	- -1 and false in *bIsEmpty if no empty slots were found and pszIdentName is not found
//			- Index in m_garrpTextures and true in *bIsEmpty if the pszIdentName is NOT found and 
//            there is an empty slot found
//			- Index in m_garrpTextures and false in *bIsEmpty if the pszIdentName was found.
int
GetFirstEmptySlotInTextureStore(char *pszIdentName, bool *bIsEmpty)
{
	int		iEmptySlot, iIndx;
	
	for(iIndx=0,iEmptySlot = -1;iIndx<MAXTEXTURES;iIndx++)
	{
		// check if the slot is empty
		if(!m_garrpTextures[iIndx])
		{
			// it's empty
			if(iEmptySlot<0)
			{
				// flag this slot as the first one empty and thus if the texture isn't found
				// in the base, use this slot.
				iEmptySlot = iIndx;
			}
			// check next slot.
			continue;
		}

		// it's not empty, check filename
		if(!strcmp(pszIdentName, m_garrpTextures[iIndx]->GetFilename()))
		{
			// yup, we found the texture, it's already there. return index and false as empty
			*bIsEmpty=false;
			return iIndx;
		}
	}
	*bIsEmpty=(iEmptySlot>=0);
	return iEmptySlot;
}


// Purpose: cold textureobject removal routine. Used by DEMOGL_AppEnd and DEMOGL_TextureDelete()
void
DoDeleteTexture(int iIndx)
{
	CStdString		sToLog;

	// unUpload the texture from cardmemory. This is NOT a download ;P
	m_garrpTextures[iIndx]->UnUpload();

	if(m_garrpTextures[iIndx]->GetbCanBeUnUploaded())
	{
		// Whipe clean the pixelmaps, so delete them from memory.
		m_garrpTextures[iIndx]->Clear();
	}

	// get rid of it
	delete m_garrpTextures[iIndx];

	// clear pointer.
	m_garrpTextures[iIndx]=NULL;
}


// Purpose: returns a pointer to a textureobject previously loaded. If the index of iTextureID >=MAXTEXTURES or < 0 or
//          the texture is not loaded, NULL is returned.
CTexture
*GetTexture(GLuint iTextureID)
{
	int		iIndx;

	iIndx=GetTextureStoreIndex(iTextureID);

	if((iIndx >=MAXTEXTURES)||(iIndx < 0))
	{
		return NULL;
	}
	return m_garrpTextures[iIndx];
}


// Purpose: loads a bmp file into the passed texture object txTexture.
// In: ppdPixelData : CPixelData pointer, which will point to the created CPixelData object with the bitdata.
//     pszFilename  : bmp file to load
//	   piWidth		: pointer to int wherein the width of the picture loaded should be stored
//	   piHeight		: pointer to int wherein the height of the picture loaded should be stored
//     i3DDepth       : 3rd dimension in pixels. For 3D texture support.
//	   bCreateAlphaFromColor: creates a greyscale pixel from the RGB value and makes the alpha: 255-greyscale, so the 
//                            blacker the picture the higher the alpha.
// Out: SYS_OK   : ppdPixelData is filled.
//      Errorcode: if something went wrong.
//      ppdPixelData will point to the CPixelData object with the bitdata loaded.
//		piWidth contains width
//      piHeight contains height
// Additional Info: - Converts all bitmap data to 24bit RGB triplets (because OpenGL wants that)
int
DoImportBMP(CPixelData **ppdPixelData, const char *pszFilename, int *piWidth, int *piHeight, int i3DDepth, bool bCreateAlphaFromColor)
{
	BITMAPFILEHEADER	*bmFileHeader;
	BITMAPINFOHEADER	*bmInfoHeader;
	RGBQUAD				*rgbPalette;
	long				lBmpBits, lPalBytes;
	int					i, j,  iTrueWidth, iHeight, iWidth, iBytesRead;
	bool				bEightBit;
	byte				*pFileData, *pBitData, *pBitDataFromFile;
	byte				byR, byG, byB, byAlpha;
	CPixelData			*pPD;

	// Set ppdPixelData pointer to NULL so it will reflect a failure if the routine has found an error
	*ppdPixelData=NULL;

	// load file.
	pFileData = DEMOGL_FileLoad(pszFilename);
	if(!pFileData)
	{
		// open didn't succeed
		return SYS_BMP_LOAD_FAILED;
	}
	
	// point bmFileheader to the header.
	bmFileHeader = (BITMAPFILEHEADER *)pFileData;
	iBytesRead = sizeof(BITMAPFILEHEADER);

	// Check if the file header is correct
	if(!(bmFileHeader->bfReserved1 == 0 && bmFileHeader->bfReserved2 == 0))
	{
		DEMOGL_FileFree(pFileData);
		return SYS_BMP_INVALID_HEADER;
	}

	// point the infoheader to the right datablock in the file.
	bmInfoHeader = (BITMAPINFOHEADER *)(pFileData + sizeof(BITMAPFILEHEADER));
	iBytesRead+=sizeof(BITMAPINFOHEADER);

	// Check if the info header is correct
	if(!(bmInfoHeader->biSize == sizeof(BITMAPINFOHEADER) && bmInfoHeader->biPlanes == 1))
	{
		DEMOGL_FileFree(pFileData);
		return SYS_BMP_INVALID_HEADER;
	}

	// Check if the # of colors is unacceptable (i.e. < 256)
	if(bmInfoHeader->biBitCount < 8)
	{
		DEMOGL_FileFree(pFileData);
		return SYS_BMP_INVALID_COLORS;
	}
	
	// Check the compression parameter of the file
	if (bmInfoHeader->biCompression != BI_RGB)
	{
		DEMOGL_FileFree(pFileData);
		return SYS_BMP_INVALID_COMPRESSED;
	}

	// beware of the adding of zerobits to align the bits in the file.
	iTrueWidth = (bmInfoHeader->biWidth + 3) & ~3;
	iHeight = bmInfoHeader->biHeight;
	iWidth = bmInfoHeader->biWidth;

	// now we have to check if the bitdepth is 8 or > 8 (i.e.24). If it's 8, we
	// have to look into the colortable for the 3 bytes, otherwise we will just get the
	// bitmap bytes for the 24 bit colorinfo. Add a '0xFF' alphabyte.
	bEightBit = (bmInfoHeader->biBitCount==8);
	if(bEightBit)
	{
		// create a colorbuffer, and read palette. 
		// Figure out how many entries are actually in the table
		if (bmInfoHeader->biClrUsed == 0)
		{
			// there are 256 different colors
			bmInfoHeader->biClrUsed = 256;
			lPalBytes = (1 << bmInfoHeader->biBitCount) * sizeof(RGBQUAD);
		}
		else 
		{
			
			lPalBytes = bmInfoHeader->biClrUsed * sizeof(RGBQUAD);
		}

		// point the RGBpalet to the right datablock
		rgbPalette = (RGBQUAD *)(pFileData + iBytesRead);
		iBytesRead+=lPalBytes;
	}

	// check out how many bytes we have to read from the file.
	lBmpBits = bmFileHeader->bfSize - iBytesRead;

	// create a texture inside the textureobject. This routine converts to 24bit so 
	// supply that as depth for the texture, plus add 8 bits for the alpha.
	pPD = new CPixelData(iWidth * iHeight * 4);
	// check if creation did go well...
	if(!pPD)
	{
		DEMOGL_FileFree(pFileData);
		return SYS_MEM_ALLOC_FAIL;
	}

	// Get a pointer to the bitdata memblock
	pBitData = pPD->GetBitData();

	// point the bitdata pointer to the right block
	pBitDataFromFile = pFileData + iBytesRead;

	// reverse the order of the data by copying the data reverse to the texturememory reserved in the
	// texture object. Copy pixel for pixel the data towards the texture space in the texture object
	// because we don't support 1 and 4 color bmp files, we don't have to worry about alignbits.
	for(i=0; i<iHeight;i++)
	{
		for(j=0;j<iWidth;j++)
		{
			// check if we've to convert 8 bit crap to 24bit pixels
			if(bEightBit)
			{
				// yup, so get the colorbytes from the colortable instead.
				byR = rgbPalette[pBitDataFromFile[(i * iTrueWidth) + (j * 4)]].rgbRed;		// R
				byG = rgbPalette[pBitDataFromFile[(i * iTrueWidth) + (j * 4)+1]].rgbGreen;	// G
				byB = rgbPalette[pBitDataFromFile[(i * iTrueWidth) + (j * 4)+2]].rgbBlue;	// B
				pBitData[(i * iWidth * 4) + (j * 4)] = byR;
				pBitData[(i * iWidth * 4) + (j * 4)+1] = byG;
				pBitData[(i * iWidth * 4) + (j * 4)+2] = byB;
				if(bCreateAlphaFromColor)
				{
					byAlpha = (byte)((0.35f * byR) + (0.45f * byG) + (0.20f * byB));
				}
				else
				{
					byAlpha = 0xff;
				}
				pBitData[(i * iWidth * 4) + (j * 4)+3] = byAlpha;
			}
			else
			{
				// is stored as BGRA
				byR = pBitDataFromFile[(i * iTrueWidth*3) + (j * 3)+2];		// R
				byG = pBitDataFromFile[(i * iTrueWidth *3) + (j * 3)+1];	// G
				byB = pBitDataFromFile[(i * iTrueWidth *3) + (j * 3)];		// B
				pBitData[(i * iWidth * 4) + (j * 4)] = byR;		// R
				pBitData[(i * iWidth * 4) + (j * 4)+1] = byG;	// G
				pBitData[(i * iWidth * 4) + (j * 4)+2] = byB;	// B
				if(bCreateAlphaFromColor)
				{
					byAlpha = (byte)((0.35f * byR) + (0.45f * byG) + (0.20f * byB));
				}
				else
				{
					byAlpha = 0xff;
				}
				pBitData[(i * iWidth * 4) + (j * 4)+3] = byAlpha;
			}
		}
	}
	// free the memory of the loaded file in memory.
	DEMOGL_FileFree(pFileData);

	// Set hasalpha bit. A BMP is loaded, which doesn't contain any alpha channel, so the only way to 
	// add an alphachannel is to speficy bCreateAlphaFromColor as true
	pPD->SetbHasAlpha(bCreateAlphaFromColor);

	*ppdPixelData=pPD;

	// report back the width and height of the bitmap loaded
	*piWidth=iWidth;
	*piHeight=iHeight;

	return SYS_OK;
}


// Purpose: same as DoImportBMP, only now it loads a jpg file using jpeglib.
// See DoImportBMP.
int
DoImportJPG(CPixelData **ppdPixelData, const char *pszFilename, int *piWidth, int *piHeight, int i3DDepth, bool bCreateAlphaFromColor)
{
	int			iTrueWidth, iTrueHeight;
	byte		*pFileData, *pPixelData;
	long		lFileLength;
	CPixelData	*pPD;

	// load file.
	pFileData = DEMOGL_FileLoad(pszFilename);
	if(!pFileData)
	{
		// open didn't succeed
		return SYS_JPG_LOAD_FAILED;
	}

	lFileLength=FFileLength(pszFilename);

	// convert the data from compressed JPG data to RGBA
	pPixelData = ConvertJPGtoRGBA(pFileData,lFileLength,&iTrueWidth, &iTrueHeight, bCreateAlphaFromColor);
	if(!pPixelData)
	{
		DEMOGL_FileFree(pFileData);
		return SYS_JPG_LOAD_FAILED;
	}

	// Create pixeldata object
	pPD = new CPixelData();

	// Attach textureobject to returned buffer.
	pPD->SetBitData(pPixelData, iTrueWidth * iTrueHeight * 4);

	// Set hasalpha bit. A BMP is loaded, which doesn't contain any alpha channel, so the only way to 
	// add an alphachannel is to speficy bCreateAlphaFromColor as true
	pPD->SetbHasAlpha(bCreateAlphaFromColor);

	// report back the width and height of the bitmap loaded
	*piWidth=iTrueWidth;
	*piHeight=iTrueHeight;

	*ppdPixelData=pPD;

	DEMOGL_FileFree(pFileData);
	return SYS_OK;	
}


// Purpose: converts a JPG file in the buffer passed to RGBA. 
// Gets: pFileData, pointer to jpeg compressed file in memory
// Gets: lFileLength, length of file in memory in bytes
// Gets: bCreateAlphaFromColor, if true the color intensity per pixel will be used to calculate the alphavalue
// otherwise 0xFF is used as alphavalue.
// Returns: pointer to the pixelbuffer, pixels in RGBA format, 32bits per pixel. NULL is returned if
// something went wrong
//
// Additional info: this function differs from the ConvertTGAtoRGBA routine because we don't know the
// size of the buffer because JpegFileToRGB will return the width and height. So we can't pass a pre-allocated
// buffer to this routine as we do with ConvertJPGtoRGBA
// 
byte
*ConvertJPGtoRGBA(byte *pTGAFile, long lFileLength, int *iTrueWidth, int *iTrueHeight, bool bCreateAlphaFromColor)
{
	int			i,j, iD;
	byte		*pRGBAData, *pBitDataFromFile;
	byte		byR, byG, byB, byAlpha;
	UINT		uiW, uiH;

	// read data
	pBitDataFromFile=JpegFile::JpegFileToRGB(pTGAFile,&uiW, &uiH, lFileLength);

	// store true height and width 
	*iTrueWidth=uiW;
	*iTrueHeight=uiH;

	// alloc resultbuffer. We use a new buffer because data returned is upside down.
	pRGBAData=(byte *)malloc(4 * uiW * uiH);

	// data has to be upside down...
	// copy data in resultbuffer
	for(i=0, iD=uiH-1;i<uiH;i++, iD--)
	{
		for(j=0;j<uiW;j++)
		{
			byR = pBitDataFromFile[(iD * uiW*3) + (j * 3)];	// R
			byG = pBitDataFromFile[(iD * uiW *3) + (j * 3)+1];	// G
			byB = pBitDataFromFile[(iD * uiW *3) + (j * 3)+2];	// B

			pRGBAData[(i * uiW * 4) + (j * 4)] = byR;	// R
			pRGBAData[(i * uiW * 4) + (j * 4)+1] = byG;	// G
			pRGBAData[(i * uiW * 4) + (j * 4)+2] = byB;	// B
			if(bCreateAlphaFromColor)
			{
				byAlpha = (byte)((0.35f * byR) + (0.45f * byG) + (0.20f * byB));
			}
			else
			{
				byAlpha = 0xff;
			}
			pRGBAData[(i * uiW * 4) + (j * 4)+3] = byAlpha;
		}
	}
	// done
	free(pBitDataFromFile);
	return pRGBAData;
}


// Purpose: loads a tga file into the passed texture object txTexture.
// In: ppdPixelData : CPixelData pointer, which will point to the created CPixelData object with the bitdata.
//     pszFilename  : tga file to load
//	   piWidth		: pointer to int wherein the width of the picture loaded should be stored
//	   piHeight		: pointer to int wherein the height of the picture loaded should be stored
//	   i3DDepth		: 3D depth of texture. For 3D texture support.
//     bCreateAlphaFromColor: if true, the greyvalue of RGB is used to create the A(lpha) value
// Out: SYS_OK   : ppdPixelData is filled.
//      Errorcode: if something went wrong.
//      ppdPixelData will point to the CPixelData object with the bitdata loaded.
// Additional Info: - Converts all bitmap data to 24bit RGB triplets (because OpenGL wants that)
//					- Only true color (24 bit/32 bit) are supported. (type 2) TGA is added for alphareading purposes.
//                  - When bCreateAlphaFromColor is true, the eventually available alphabyte is ignored.
//					- XYorigin is ignored. (0,0) is left lower corner) 
//
// Uses ConvertTGAtoRGBA() for dataconversion.
int
DoImportTGA(CPixelData **ppdPixelData, const char *pszFilename, int *piWidth, int *piHeight, int i3DDepth, bool bCreateAlphaFromColor)
{
	int			iHeight, iWidth, iResult;
	byte		*pTextureBuffer, *pTGAFile;
	bool		bHasAlpha;
	CPixelData	*pPD;

	// load file.
	pTGAFile = DEMOGL_FileLoad(pszFilename);
	if(!pTGAFile)
	{
		// open didn't succeed
		return SYS_TGA_LOAD_FAILED;
	}

	// Check if we have the right type of file.
	if(pTGAFile[2] !=2)
	{
		// invalid format
		DEMOGL_FileFree(pTGAFile);
		return SYS_TGA_INVALID_FORMAT;
	}
	// SHITMACINTELCRAP. An integer is 4 bytes. lo-hi shit is reversed due to intel weirdness.
	iWidth = int(pTGAFile[12] | ((int)pTGAFile[13] << 8));
	iHeight = int(pTGAFile[14] | ((int)pTGAFile[15] << 8));
	bHasAlpha=false;

	// Create pixeldata object for the bitdata
	pPD = new CPixelData(iWidth * iHeight * 4);

	// check if creation did go well...
	if(!pPD)
	{
		DEMOGL_FileFree(pTGAFile);
		return SYS_MEM_ALLOC_FAIL;
	}

	pTextureBuffer = pPD->GetBitData();

	// Convert. Retrieve if the texturedata contains an alphachannel
	iResult=ConvertTGAtoRGBA(pTGAFile, pTextureBuffer,iWidth, iHeight, bCreateAlphaFromColor, &bHasAlpha);

	// clean up
	DEMOGL_FileFree(pTGAFile);

	// Set HasAlpha bit. 
	pPD->SetbHasAlpha(bHasAlpha);

	*ppdPixelData=pPD;

	// report back the width and height of the bitmap loaded
	*piWidth=iWidth;
	*piHeight=iHeight;

	return iResult;
}


// Purpose: imports a dds file, which is a DX compressed image. (DXT1, DXT3 or DXT5. No cubemaps.)
// Some code based on nVidia examplecode.
// Returns SYS_OK if succeeded, Errorcode if not.
int
DoImportDDS(CPixelData **ppdPixelData, const char *pszFilename, int *piWidth, int *piHeight) 
{
	DDSURFACEDESC2	*pdImageLoaded;
	int				iCompressionFactor;
	byte			*pFileData, *pBitData;
	long			lFileLength;
	CPixelData		*pPD;
	GLenum			iPixelFormat;

	// Set ppdPixelData pointer to NULL so it will reflect a failure if the routine has found an error
	*ppdPixelData=NULL;

	// load file.
	pFileData = DEMOGL_FileLoad(pszFilename);
	if(!pFileData)
	{
		// open didn't succeed
		return SYS_DDS_LOAD_FAILED;
	}

	lFileLength=FFileLength(pszFilename);

	if(strncmp((char *)pFileData, "DDS ", 4)!=0)
	{
		// no DDS file.
		DEMOGL_FileFree(pFileData);
		return SYS_DDS_NODDSFILE;
	}

	// get a ddsd pointer to the data. the actual ddsd struct starts at position 4 in the
	// file, since it's prefixed with 'DDS '.
	pdImageLoaded=(DDSURFACEDESC2 *)(pFileData+4);

	// check the pixelformat
	switch(pdImageLoaded->ddpfPixelFormat.dwFourCC)
	{
		case FOURCC_DXT1:
		{
			iPixelFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			iCompressionFactor = 2;
		}; break;
		case FOURCC_DXT3:
		{
			  iPixelFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			  iCompressionFactor = 4;
		}; break;
		case FOURCC_DXT5:
		{
			  iPixelFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			  iCompressionFactor = 4;
		}; break;
		default:
		{
			// illegal format. no can do.
			DEMOGL_FileFree(pFileData);
			return SYS_DDS_UNSUPPORTEDFORMAT;
		}; break;
	}

	// create pixelstore and set flags.
	if(pdImageLoaded->dwMipMapCount > 1)
	{
		// there are mipmaps included in the bitdata.
		pPD = new CPixelData((int)pdImageLoaded->dwLinearSize * iCompressionFactor);
	}
	else
	{
		pPD = new CPixelData(pdImageLoaded->dwLinearSize);
	}
	pPD->SetAmIncludedMipMaps((int)pdImageLoaded->dwMipMapCount);

	// set format of compression.
	pPD->SetCompressionFormat(iPixelFormat);
	pPD->SetMetrics((int)pdImageLoaded->dwWidth, (int)pdImageLoaded->dwHeight);

	*piWidth=(int)pdImageLoaded->dwWidth;
	*piHeight=(int)pdImageLoaded->dwHeight;
	
	// Get buffer so we can copy the dds data into it.
	pBitData = pPD->GetBitData();

	// Copy data
	memcpy(pBitData, pFileData+sizeof(DDSURFACEDESC2)+4, lFileLength - (sizeof(DDSURFACEDESC2)+4));

	// Free resources
	DEMOGL_FileFree(pFileData);
	
	// Return value.
	*ppdPixelData=pPD;

	// Done
	return SYS_OK;
}



// Purpose: converts a buffer that contains a TGA file to RGBA data.
// Gets: pTGAFile, bytepointer, pointer to the TGAfile in memory
// Gets: pRGBABuffer, bytepointer, pointer to pre-allocated RGBAbuffer. Should be of the size iWidth*iHeight*4
//                    No boundary checks are made!
// Gets: iWidth, iHeight, ints, with and height of picture
// Gets: bCreateAlphaFromColor, bool, if true the color of the pixel will be used to create the alphabyte, otherwise
// the alphabyte in the pixelitself is used, if present, otherwise 0xFF is used.
// Returns: SYS_OK if all went well, SYS_NOK otherwise. 
// Returns in bHasAlpha true if the texturedata contains one or more pixels with an alphachannel or false if not.
int
ConvertTGAtoRGBA(byte *pTGAFile, byte *pRGBABuffer, int iWidth, int iHeight, bool bCreateAlphaFromColor, bool *bHasAlpha)
{
	int					i,j,iBytesPerPixel;
	bool				bAlphaBytesInFile;
	byte				*pPixelData;
	byte				byR, byG, byB, byAlpha;

	if(!pRGBABuffer)
	{
		return SYS_NOK;
	}

	if(!pTGAFile)
	{
		return SYS_NOK;
	}

	iBytesPerPixel = pTGAFile[16] / 8;
	bAlphaBytesInFile = ((pTGAFile[17] & 0x0F) == 8);

	// now we have to skip the colormap and identification IF present.
	pPixelData = pTGAFile;
	if(pTGAFile[1]!=0)
	{
		// there is a colormap. Ignore it. Get the length and add this to pPixelData
		pPixelData += (int)(pTGAFile[3] | ((int)pTGAFile[4] << 8)) + ((int)(pTGAFile[5] | ((int)pTGAFile[6] << 8)) * pTGAFile[7]);
	}
	else
	{
		// there is no colormap. perhaps an identification. skip it.
		pPixelData = pTGAFile + pTGAFile[0] + 18;
	}
	
	// reverse the order of the data by copying the data reverse to the texturememory reserved in the
	// texture object. Copy pixel for pixel the data towards the texture space in the texture object
	// because we don't support 1 and 4 color tga files, we don't have to worry about alignbits.
	for(i=0, *bHasAlpha=bCreateAlphaFromColor; i<iHeight;i++)
	{
		for(j=0;j<iWidth;j++)
		{
			// is stored as BGRA
			byR = pPixelData[(i * iWidth*iBytesPerPixel) + (j * iBytesPerPixel)+2];	// R
			byG = pPixelData[(i * iWidth*iBytesPerPixel) + (j * iBytesPerPixel)+1];	// G
			byB = pPixelData[(i * iWidth*iBytesPerPixel) + (j * iBytesPerPixel)];	// B
			pRGBABuffer[(i * iWidth * 4) + (j * 4)] = byR;	// R
			pRGBABuffer[(i * iWidth * 4) + (j * 4)+1] = byG;	// G
			pRGBABuffer[(i * iWidth * 4) + (j * 4)+2] = byB;	// B
			if(bCreateAlphaFromColor)
			{
				byAlpha = (byte)((0.35f * byR) + (0.45f * byG) + (0.20f * byB));
			}
			else
			{
				if(bAlphaBytesInFile)
				{
					byAlpha = pPixelData[(i * iWidth * iBytesPerPixel) + (j * iBytesPerPixel)+3];
					*bHasAlpha|=(byAlpha<0xFF);
				}
				else
				{
					byAlpha = 0xFF;
				}
			}
			pRGBABuffer[(i * iWidth * 4) + (j * 4)+3] = byAlpha;
		}
	}
	return SYS_OK;
}


// Purpose: calculates the index in the Texture store m_garrpTextures[] from the 
// texture id given.
// Returns the index in the m_garrpTextures[] texture store, or 0 if it's below 0
int
GetTextureStoreIndex(GLuint iTextureID)
{
	int		iResult;

	iResult= iTextureID-m_gpDemoDat->GetTexNameTexIDDelta();

	if(iResult<0)
	{
		return 0;
	}
	return iResult;
}


// Purpose: generates the TextureID used in OpenGL and also in DemoGL to identify a texture.
// THis is done by using the index in m_garrpTextures[] and the TexNameTexIDDelta stored in DemoDat
// Gets: the index in m_garrpTextures of the textureobject the TextureID is for.
GLuint
GenerateTextureID(int iIndx)
{
	return (GLuint)(iIndx+m_gpDemoDat->GetTexNameTexIDDelta());
}


// Purpose: flips the passed buffer with RGBA fragments vertically. The width and height are in pixels.
// This routine assumes height is even and even a power of two, since opengl can't handle bitmaps that
// are not a power of 2. 
void
FlipRGBAVertical(byte *pbyRGBA, int iWidth, int iHeight)
{
	int				iS, iD, j;
	unsigned long	lRGBA;
	unsigned long	*pulRGBA;

	if(!pbyRGBA)
	{
		return;
	}
	
	pulRGBA=(unsigned long *)pbyRGBA;

	// process half the lines, we walk with 2 pointers, one from the front, one from the back
	// and swap the bytes, per line.
	for(iS=0, iD=iHeight-1;iS<(iHeight/2);iS++, iD--)
	{
		for(j=0;j<iWidth;j++)
		{
			// first read the RGBA fragments of the destination pixel
			lRGBA=pulRGBA[(iD * iWidth) + j];
			// write source in destination
			pulRGBA[(iD * iWidth) + j] = pulRGBA[(iS * iWidth) + j];
			// write lRGBA in source
			pulRGBA[(iS * iWidth) + j] = lRGBA;
		}
	}
}




