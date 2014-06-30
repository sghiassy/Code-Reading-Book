//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlltexture.cpp
// PURPOSE: in here is the textureclass implementation and the pixeldata class implementation
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// this is the file with the main control functions for running the demo.
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
// v1.3: Added new features.
// v1.2: Added new features.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//                      CPixelData Class
////////////////////////////////////////////////////////////////////////

// Purpose: Constructor for precreation of buffer
CPixelData::CPixelData(long lBitDataLength)
{
	InitMembers();

	// create datastore.
	m_pbyBitData=(byte *)malloc(lBitDataLength * sizeof(byte));
	if(m_pbyBitData)
	{
		m_lBitDataLength=lBitDataLength;
	}
}


// Purpose: constructor for an empty buffer. Will be set later
CPixelData::CPixelData()
{
	InitMembers();
}


// Purpose: destructor
CPixelData::~CPixelData()
{
	if(m_pbyBitData)
	{
		free(m_pbyBitData);
	}
}


// Purpose: Initializes all membervariables
void
CPixelData::InitMembers()
{
	m_pbyBitData=NULL;
	m_bHasAlpha=false;
	m_lBitDataLength=0;
	m_iWidth=0;
	m_iHeight=0;
	m_iAmIncludedMipMaps=0;
	m_iCompressionFormat=0;
}


// Purpose: sets m_iAmIncludedMipMaps
void
CPixelData::SetAmIncludedMipMaps(int iAmount)
{
	m_iAmIncludedMipMaps=iAmount;
}


// Purpose: gets m_iAmIncludedMipMaps
int
CPixelData::GetAmIncludedMipMaps()
{
	return m_iAmIncludedMipMaps;
}


// Purpose: sets the compressionformat of the data contained
// Only used with compressed (dds) bitdata.
// Format is like GL_COMPRESSED_RGBA_S3TC_DXT5_EXT for S3TC, DXt5 compressed bitdata.
void
CPixelData::SetCompressionFormat(GLenum iFormat)
{
	m_iCompressionFormat=iFormat;
}


// Purpose: gets the compressionformat of the data contained
GLenum
CPixelData::GetCompressionFormat()
{
	return m_iCompressionFormat;
}


// Purpose: retrieves a pointer to the actual bitmap
byte
*CPixelData::GetBitData()
{
	return m_pbyBitData;
}


// Purpose: returns the value of bHasAlpha
bool
CPixelData::GetbHasAlpha()
{
	return m_bHasAlpha;
}


// Purpose: gets the length of the datablock in bytes
long
CPixelData::GetBitDataLength()
{
	return m_lBitDataLength;
}


// Purpose: sets a pointer to a bitmap. If there is already a bitmap
// associated with this object, it's freed first
void
CPixelData::SetBitData(byte *pbyData, long lBitDataLength)
{
	if(m_pbyBitData)
	{
		free(m_pbyBitData);
	}
	InitMembers();

	m_pbyBitData=pbyData;
	m_lBitDataLength=lBitDataLength;
}


// Purpose: sets the HasAlpha flag
void
CPixelData::SetbHasAlpha(bool bYN)
{
	m_bHasAlpha=bYN;
}


// Purpose: Rescales the texturedata to the maximum width and height which is possible
// with the used videocard. Will replace the bitdata with a rescaled bitmap and will
// adjust width and height.
// Returns SYS_OK if succeeded, SYS_NOK if not. 
// Will return SYS_OK if no rescaling should be done (nor requested nor needed)
int
CPixelData::Rescale()
{
	int			iMaxTexSize, iNewHeight, iNewWidth;
	byte		*pbyScaledPixelData;

	// first check if there is any data.
	if(!m_lBitDataLength)
	{
		// no texture data available... 
		return SYS_NOK;
	}

	iMaxTexSize=m_gpDemoDat->GetOGLMaxTextureSize();

	// If the pixeldatasize is > than iMaxTexSize AND the user was allowed to select
	// 'rescale textures if needed', we have to rescale the pixeldata to match the iMaxTexSize.
	if(((m_iHeight > iMaxTexSize) || (m_iWidth > iMaxTexSize)) &&
		(m_gpDemoDat->GetStartupDat()->m_bRescaleTextures))
	{
		// rescale the texture. Use gluScaleImage(). Keep the width-height aspectratio.
		if(m_iHeight > iMaxTexSize)
		{
			// height is larger. rescale height and use that for rescaleing width
			iNewHeight=iMaxTexSize;
			iNewWidth=(int)((float)m_iWidth * ((float)iNewHeight/(float)m_iHeight));
		}
		else
		{
			// width is larger, rescale width and use that for rescaling height.
			iNewWidth=iMaxTexSize;
			iNewHeight=(int)((float)m_iHeight * ((float)iNewWidth/(float)m_iWidth));
		}
		// now rescale the pixeldata with new sizes. first allocate the destinationbuffer
		pbyScaledPixelData=(byte *)malloc(4 * iNewWidth * iNewHeight);
		// Scale it
		gluScaleImage(GL_RGBA, m_iWidth, m_iHeight, GL_UNSIGNED_BYTE, 
				m_pbyBitData, iNewWidth, iNewHeight, GL_UNSIGNED_BYTE, pbyScaledPixelData);

		// now free the old bitdata and replace the pointer with the scaled datablock
		free(m_pbyBitData);
		m_pbyBitData=pbyScaledPixelData;
		m_iWidth=iNewWidth;
		m_iHeight=iNewHeight;
		m_lBitDataLength=4*iNewHeight*iNewWidth;
		// done
		return SYS_OK;
	}
	else
	{
		// return 'OK', allthough nothing has been scaled.
		return SYS_OK;
	}
}


// Purpose: gets the width of the pixeldata block. In pixels
int
CPixelData::GetWidth()
{
	return m_iWidth;
}


// Purpose: gets the height of the pixeldata block. In pixels
int
CPixelData::GetHeight()
{
	return m_iHeight;
}


// Purpose: sets the width/height of the pixeldata currently inside the pixeldata block.
// This routine is used in combination of a pixeldata storage in the bitdata buffer or in
// combination with setbitdata
void
CPixelData::SetMetrics(int iWidth, int iHeight)
{
	m_iWidth=iWidth;
	m_iHeight=iHeight;
}


////////////////////////////////////////////////////////////////////////
//                      CTexture Class
////////////////////////////////////////////////////////////////////////
// Purpose: CTexture constructor
CTexture::CTexture(int iTextureType, int iWidth, int iHeight, int iDepth)
{
	// initialize members.
	InitMembers();

	// set passed values
	m_iTextureType=iTextureType;
	m_iOrgWidth=iWidth;
	m_iOrgHeight=iHeight;
	m_iOrgDepth=iDepth;
}

// Purpose: CTexture destructor
CTexture::~CTexture()
{
	(void) Clear();
}


// Purpose: gets the dimensions of this texture, for uploading
int
CTexture::GetTextureDimensions()
{
	return m_iDimensions;
}


// Purpose: sets the texturedimensions.
void
CTexture::SetTextureDimensions(int iDimensions)
{
	m_iDimensions=iDimensions;
}


// Attaches a CPixelData object to the textureobject. iCubeSide should be DGL_CUBEMAPSIDE_NOCUBEMAP
// when the texture is infact a normal texture and not a cubemap.
void
CTexture::AttachToPixelData(CPixelData *ppdPixelData, int iCubeSide)
{
	if((iCubeSide<0)||(iCubeSide>=MAX_PIXELDATABLOCKS))
	{
		// cubeside is illegal
		return;
	}

	// see if the metrics of the bitmap attached are larger than the current set metrics of this
	// texture
	if(m_iOrgWidth < ppdPixelData->GetWidth())
	{
		m_iOrgWidth=ppdPixelData->GetWidth();
	}
	if(m_iOrgHeight < ppdPixelData->GetHeight())
	{
		m_iOrgHeight=ppdPixelData->GetHeight();
	}

	if(m_ppdPixelData[iCubeSide])
	{
		// remove the pixeldata object
		delete m_ppdPixelData[iCubeSide];
	}

	m_ppdPixelData[iCubeSide]=ppdPixelData;

	// rescale the texture bitmap if needed.
	m_ppdPixelData[iCubeSide]->Rescale();
}


// Purpose: clears all bitmap data inside the textureobject. 
void
CTexture::Clear()
{
	int		i;

	for(i=0;i<MAX_PIXELDATABLOCKS;i++)
	{
		if(PixelDataPresent(i))
		{
			delete m_ppdPixelData[i];
		}
	}
	InitMembers();
}


// Purpose: returns true if there is a pixeldata object present on the cubeside specified. 
// False otherwise
bool
CTexture::PixelDataPresent(int iCubeSide)
{
	return(m_ppdPixelData[iCubeSide] != NULL);
}


// Purpose: set the uploaded parameter TextureID of the texture has for binding
// In: iTexureID: the number of the texture, used for OpenGL
void
CTexture::SetTextureID(GLuint iTextureID)
{
	m_iTextureID = iTextureID;
}

// Purpose: retrieve the textureID of this texture for binding.
// In: none
// Out: the ID of the texture
GLuint
CTexture::GetTextureID()
{
	return m_iTextureID;
}


// Purpose: sets the pixeldepth hint for texture upload routine
void
CTexture::SetOGLPixelDepthHint(GLint iPixelDepthHint)
{
	m_iPixelDepthHint = iPixelDepthHint;
}


// Purpose: gets the pixeldepth hint for texture upload routine
GLint
CTexture::GetOGLPixelDepthHint()
{
	return m_iPixelDepthHint;
}


// Purpose: retrieve width of texture in pixels. Returns width of first pixeldata block
// In: none
// Out: width in pixels
int
CTexture::GetWidth(void)
{
	return m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetWidth();
}

// Purpose: retrieve height of texture in pixels. Returns width of first pixeldata block
// In: none
// Out: height in pixels
int
CTexture::GetHeight(void)
{
	return m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetHeight();
}

// Purpose: returns the depth of the texture in pixels. (the 3rd dimension in a 3D texture)
int
CTexture::GetDepth()
{
	return m_iOrgDepth;
}



// Purpose: retrieves pointer to selected pixeldata object
CPixelData 
*CTexture::GetPixelData(int iCubeSide)
{
	return m_ppdPixelData[iCubeSide];
}


// Purpose: increases how many times this texture is 'uploaded' to OpenGL. It's ofcourse uploaded
// only ONCE, but when more than 1 effect references this texture and all upload this texture, it shouldn't
// be UNuploaded when the last referencing effect unuploads it.
// Returns: the new value
int
CTexture::IncAmUploaded()
{
	m_iAmUploaded++;
	return m_iAmUploaded;
}


// Purpose: decreases how many times this texture is 'uploaded' to OpenGL. It's ofcourse uploaded
// only ONCE, but when more than 1 effect references this texture and all upload this texture, it shouldn't
// be UNuploaded when the last referencing effect unuploads it.
// Returns: the new value
int
CTexture::DecAmUploaded()
{
	m_iAmUploaded--;
	if(m_iAmUploaded<0)
	{
		m_iAmUploaded=0;
	}
	return m_iAmUploaded;
}


// Purpose: gets how many times this texture is 'uploaded'. When an effect unuploads this texture but it's also
// referenced by another effect which still has it uploaded its AmUploaded counter will be decreased. This counter
// is returned by this function
int
CTexture::GetAmUploaded()
{
	return m_iAmUploaded;
}


// Purpose: initialize the membervariables. This is used more than once, so therefor a routine
void
CTexture::InitMembers()
{
	int			i;

	for(i=0;i<MAX_PIXELDATABLOCKS;i++)
	{
		m_ppdPixelData[i]=NULL;
	}

	m_iAmUploaded = 0;
	m_iTextureID = -1;
	m_iOrgWidth=0;
	m_iOrgHeight=0;
	m_iOrgDepth=0;
	m_iPixelDepthHint=GL_RGBA8;	
	m_bHasMipMaps=true;
	m_iGLMinFilter = GL_LINEAR_MIPMAP_LINEAR;
	m_iGLMagFilter = GL_LINEAR;
	m_iGLWrapS = GL_CLAMP;
	m_iGLWrapT = GL_CLAMP;
	m_iAmReferences=0;
	m_sFilename="";
	m_iBorder = 0;
	m_bCanBeUnUploaded = true;
	m_iDimensions=DGL_TEXTUREDIMENSION_2D;
	m_iCompressionType=DGL_TC_NONE;
	m_iTextureType=DGL_TEXTURETYPE_NORMAL;
}


// Purpose: sets the compression type. 
void
CTexture::SetCompressionType(int iType)
{
	m_iCompressionType=iType;
}


// Purpose: Gets the compressiontype
int
CTexture::GetCompressionType()
{
	return m_iCompressionType;
}



// Purpose: gets the bCanBeUnUploaded membervar. 
bool
CTexture::GetbCanBeUnUploaded()
{
	return m_bCanBeUnUploaded;
}


// Purpose: set bCanBeUnUploaded membervar
void
CTexture::SetbCanBeUnUploaded(bool bYN)
{
	m_bCanBeUnUploaded = bYN;
}


// Purpose: increase references to this textureobject by 1
void
CTexture::IncAmReferences()
{
	m_iAmReferences++;
}


// Purpose: decrease references to this textureobject by 1
void
CTexture::DecAmReferences()
{
	if(!m_iAmReferences)
	{
		// is zero, we can't have negatives.
		return;
	}
	m_iAmReferences--;
}


// Purpose: get the referencevalue
int
CTexture::GetAmReferences()
{
	return m_iAmReferences;
}


// Purpose: Get the filename of this texture. this is used to determine if the texture currently requested
// is already in core
char
*CTexture::GetFilename()
{
	return &m_sFilename[0];
}


// Purpose: Set the filename where this texture is read from.
void
CTexture::SetFilename(const char *pszFilename)
{
	m_sFilename.Format("%s",pszFilename);
}


// Purpose: sets the HasMipMaps flag. If true, there will be mipmaps created when uploading.
void
CTexture::SetbHasMipMaps(bool bYN)
{
	m_bHasMipMaps = bYN;
}


// Purpose: sets the filters for this texture. These filters will be used when uploading
// the texture (thus when defining the textureobject inside OpenGL. These filters are
// used by the filteringhardware. MinFilter is the texture minifying function and used whenever 
// the pixel being textured maps to an area greater than one texture element. MagFilter is
// the oposite.
// MagFilter is always using the largest mipmap, therefor you can only set that one to
// GL_NEAREST (ugly but fast) or GL_LINEAR (bilinear filtering)
void
CTexture::SetGLFilters(GLint iMinFilter, GLint iMagFilter)
{
	m_iGLMinFilter = iMinFilter;
	m_iGLMagFilter = iMagFilter;
}


// Purpose: Sets the textureclamping flags for X and Y texture coords. if you want a
// repeating texture, set these to GL_REPEAT, (duh) if want to stretch the total texture
// over the total poly, use GL_CLAMP.
void
CTexture::SetGLWrapFlags(GLint iWrapS, GLint iWrapT)
{
	m_iGLWrapS = iWrapS;
	m_iGLWrapT = iWrapT;
}


// Purpose: gets m_bHasMipMaps
bool
CTexture::GetbHasMipMaps()
{
	return m_bHasMipMaps;
}


// Purpose: gets WrapS flag
GLint
CTexture::GetGLWrapS()
{
	return m_iGLWrapS;
}


// Purpose: gets WrapT flag
GLint
CTexture::GetGLWrapT()
{
	return m_iGLWrapT;
}


// Purpose: gets MinFilter flag
GLint
CTexture::GetGLMinFilter()
{
	return m_iGLMinFilter;
}


// Purpose: gets WrapS flag
GLint
CTexture::GetGLMagFilter()
{
	return m_iGLMagFilter;
}


// purpose: sets border
void
CTexture::SetBorder(int iBorder)
{
	m_iBorder = iBorder;
}


// purpose: gets border
int
CTexture::GetBorder()
{
	return m_iBorder;
}


// Purpose: Uploads a texture to the OpenGL texture space. Will upload all pixeldata stores 
// of the texture to opengl for this one texture. So a cubemap f.e. is uploaded at once.
// In  : Nothing
// Out : SYS_OK : everything was ok
//     : Errorcode : some error occured.
int
CTexture::Upload()
{
	int		iResult;

	// first check if the texture has any data. 
	if(!PixelDataPresent(DGL_CUBEMAPSIDE_NOCUBEMAP)&&
		!PixelDataPresent(DGL_CUBEMAPSIDE_POSX))
	{
		// no texture data available... 
		return SYS_UT_NO_TEXTURE;
	}

	// check if it's already uploaded.
	if(m_iAmUploaded > 0)
	{
		// already uploaded. Just increase counter
		IncAmUploaded();

		// return as if we've uploaded it.
		return SYS_OK;
	}

	// Set pixelstore alignment.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	switch(m_iTextureType)
	{
		case DGL_TEXTURETYPE_NORMAL:
		{
			iResult=DoUploadNormal();
		}; break;
		case DGL_TEXTURETYPE_COMPRESSED:
		{
			iResult=DoUploadCompressed();
		}; break;
		case DGL_TEXTURETYPE_CUBEMAP:
		{
			iResult=DoUploadCubeMap();
		}; break;
		////////////
		// ADD MORE TEXTURE TYPES HERE
		////////////
	}

	return iResult;
}


// Purpose: does the uploading of a cubemap texture, 6 RGBA bitmaps will be uploaded.
int
CTexture::DoUploadCubeMap()
{
	int			i;
	CPixelData	*ppdCurrentPixelData;
	GLuint		iCurrentSide;
	bool		bHWMipMapGenAvailable;

	// Cubemaps are 2D. Check extensions first.
	if(!(m_gpExtensions->GetExtensionAvailability(DGL_GL_EXT_texture_cube_map)||
		 m_gpExtensions->GetExtensionAvailability(DGL_GL_ARB_texture_cube_map)))
	{
		// nope. return
		LogFeedback("Cubemap textures are not supported by the ICD.",true, "DoUploadCubeMap",false);
		return SYS_NOK;
	}

	bHWMipMapGenAvailable = DEMOGL_ExtensionCheckIfAvailable(DGL_GL_SGIS_generate_mipmap);

	
	// Enable Cubemapping in OpenGL
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);

	// Bind to the texturename inside the OpenGL ICD
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, m_iTextureID);

	// switch on hardware generated mipmaps if available and needed.
	if(bHWMipMapGenAvailable && m_bHasMipMaps)
	{
		// For now, only hardware generated mipmaps are supported.
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	}

	// Now upload all 6 sides.
	for(i=1;i<=6;i++)
	{
		switch(i)
		{
			// Use ARB side definitions, but EXT definitions have the same values.
			case DGL_CUBEMAPSIDE_POSX:
			{
				iCurrentSide=GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
				ppdCurrentPixelData=m_ppdPixelData[DGL_CUBEMAPSIDE_POSX];
			}; break;
			case DGL_CUBEMAPSIDE_NEGX:
			{
				iCurrentSide=GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
				ppdCurrentPixelData=m_ppdPixelData[DGL_CUBEMAPSIDE_NEGX];
			}; break;
			case DGL_CUBEMAPSIDE_POSY:
			{
				iCurrentSide=GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
				ppdCurrentPixelData=m_ppdPixelData[DGL_CUBEMAPSIDE_POSY];
			}; break;
			case DGL_CUBEMAPSIDE_NEGY:
			{
				iCurrentSide=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
				ppdCurrentPixelData=m_ppdPixelData[DGL_CUBEMAPSIDE_NEGY];
			}; break;
			case DGL_CUBEMAPSIDE_POSZ:
			{
				iCurrentSide=GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
				ppdCurrentPixelData=m_ppdPixelData[DGL_CUBEMAPSIDE_POSZ];
			}; break;
			case DGL_CUBEMAPSIDE_NEGZ:
			{
				iCurrentSide=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
				ppdCurrentPixelData=m_ppdPixelData[DGL_CUBEMAPSIDE_NEGZ];
			}; break;
		}

		glTexImage2D(iCurrentSide,0, m_iPixelDepthHint, ppdCurrentPixelData->GetWidth(), 
						ppdCurrentPixelData->GetHeight(), m_iBorder, GL_RGBA, 
						GL_UNSIGNED_BYTE, ppdCurrentPixelData->GetBitData());
	}

	// increase uploaded amount, so next reference to this texture that will upload it won't really upload it
	// but just increase the counter.
	IncAmUploaded();

	// set additional texture params. These will be stored with the texture object in OpenGL.
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, m_iGLMinFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, m_iGLMagFilter);

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	
	return SYS_OK;
}


// Purpose: does the uploading of a compressed (DXT1, DXT3, DXT5) texture (loaded from a DDS file)
// Some code based on nVidia examplecode.
// Only 2D DDS files are supported, no 3D, no 1D, no Cubemaps.
int
CTexture::DoUploadCompressed()
{
	int				iWidthBitmap, iHeightBitmap, iOffset, iBitmapSize, iBlockSize, i, iAmMipMaps;
	byte			*pbyBitData;
	PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    glCompressedTexImage2DARB;

	// Check if the needed extensions are supported
	if(!(m_gpExtensions->GetExtensionAvailability(DGL_GL_ARB_texture_compression)&&
		 m_gpExtensions->GetExtensionAvailability(DGL_GL_EXT_texture_compression_s3tc)))
	{
		// nope. return
		LogFeedback("Compressed textures are not supported by the ICD.",true, "DoUploadCompressed",false);
		return SYS_NOK;
	}

	// Get the pointer to the glCompressedTexImage2DARB routine.
	glCompressedTexImage2DARB=(PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");

	// get information of the current pixeldata store which should be uploaded
	iWidthBitmap=m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetWidth();
	iHeightBitmap=m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetHeight();
	pbyBitData=m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetBitData();

	iBlockSize=16;
	if(m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetCompressionFormat()==GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
	{
		iBlockSize=8;
	}

	// Bind to the texturename inside the OpenGL ICD
	glBindTexture(GL_TEXTURE_2D, m_iTextureID);	

	iAmMipMaps=m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetAmIncludedMipMaps();

	// Upload all mipmaps (if any), or just the single bitmap.
	for(i=0, iOffset=0; (i<iAmMipMaps) && (iWidthBitmap || iHeightBitmap); i++)
	{
		if(!iWidthBitmap)
		{
			iWidthBitmap=1;
		}
		if(!iHeightBitmap)
		{
			iHeightBitmap=1;
		}
		iBitmapSize=((iWidthBitmap + 3)/4)*((iHeightBitmap + 3)/4)*iBlockSize;

		// Upload the compressed texturedata.
		glCompressedTexImage2DARB(
			GL_TEXTURE_2D, i, m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetCompressionFormat(),
			iWidthBitmap, iHeightBitmap, 0, iBitmapSize, pbyBitData + iOffset);

		// Prepare vars for next mipmap.
		iOffset+=iBitmapSize;
		iWidthBitmap>>=1;
		iHeightBitmap>>=1;
	}

	// increase uploaded amount, so next reference to this texture that will upload it won't really upload it
	// but just increase the counter.
	IncAmUploaded();

	// set additional texture params. These will be stored with the texture object in OpenGL.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_iGLMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_iGLMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_iGLWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_iGLWrapT);

	return SYS_OK;
}





// Purpose: does the uploading of normal texture, just 1 RGBA bitmap, not compressed.
int
CTexture::DoUploadNormal()
{
	GLenum		iTarget;
	int			iWidthBitmap, iHeightBitmap;
	byte		*pbyBitData;
	bool		bHWMipMapGenAvailable;

	// get information of the current pixeldata store which should be uploaded
	iWidthBitmap=m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetWidth();
	iHeightBitmap=m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetHeight();
	pbyBitData=m_ppdPixelData[DGL_CUBEMAPSIDE_NOCUBEMAP]->GetBitData();

	bHWMipMapGenAvailable = DEMOGL_ExtensionCheckIfAvailable(DGL_GL_SGIS_generate_mipmap);
	switch(m_iDimensions)
	{
		case DGL_TEXTUREDIMENSION_1D:
		{
			iTarget=GL_TEXTURE_1D;
			// Bind to the texturename inside the OpenGL ICD
			glBindTexture(iTarget, m_iTextureID);
			if(!bHWMipMapGenAvailable && m_bHasMipMaps)
			{
				// no hardware mipmap generation available. use glu function.
				gluBuild1DMipmaps(iTarget, m_iPixelDepthHint, iWidthBitmap, 
									GL_RGBA, GL_UNSIGNED_BYTE, pbyBitData);
			}
			else
			{
				if(m_bHasMipMaps)
				{
					// Switch on hardware generated mipmaps for this texture.
					glTexParameteri(iTarget, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
				}
				glTexImage1D(iTarget, 0, m_iPixelDepthHint, iWidthBitmap, 
									m_iBorder,GL_RGBA, GL_UNSIGNED_BYTE, pbyBitData);
			}
		}; break;
		case DGL_TEXTUREDIMENSION_2D:
		{
			iTarget=GL_TEXTURE_2D;

			// Bind to the texturename inside the OpenGL ICD
			glBindTexture(iTarget, m_iTextureID);		

			if(!bHWMipMapGenAvailable && m_bHasMipMaps)
			{
				// no hardware mipmap generation available. use glu function.
				gluBuild2DMipmaps(iTarget,m_iPixelDepthHint, iWidthBitmap, 
									iHeightBitmap,GL_RGBA, GL_UNSIGNED_BYTE, pbyBitData);
			}
			else
			{
				if(m_bHasMipMaps)
				{
					// Switch on hardware generated mipmaps for this texture.
					glTexParameteri(iTarget, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
				}
				glTexImage2D(iTarget,0, m_iPixelDepthHint, iWidthBitmap, 
									iHeightBitmap, m_iBorder, GL_RGBA, GL_UNSIGNED_BYTE, pbyBitData);
			}
		}; break;
		case DGL_TEXTUREDIMENSION_3D:
		{
			// not supported yet.
		}; break;
	}

	// increase uploaded amount, so next reference to this texture that will upload it won't really upload it
	// but just increase the counter.
	IncAmUploaded();

	// set additional texture params. These will be stored with the texture object in OpenGL.
	glTexParameteri(iTarget, GL_TEXTURE_MIN_FILTER, m_iGLMinFilter);
	glTexParameteri(iTarget, GL_TEXTURE_MAG_FILTER, m_iGLMagFilter);
	glTexParameteri(iTarget, GL_TEXTURE_WRAP_S, m_iGLWrapS);
	glTexParameteri(iTarget, GL_TEXTURE_WRAP_T, m_iGLWrapT);
	
	return SYS_OK;
}


// Purpose: removes this texture from OpenGL's texturespace. It has to be re-uploaded
// next time it needs to be used.
// Returns: SYS_OK if succeeded, SYS_NOK if not. (not implemented at the moment.)
int
CTexture::UnUpload()
{
	GLuint		arrTex[1];
	
	// get name and delete it
	arrTex[0]=(GLuint) m_iTextureID;
	glDeleteTextures(1,&arrTex[0]);
	return SYS_OK;
}
