////////////////////////////////////////////////////////////
//	JpegFile - A C++ class to allow reading and writing of
//	RGB and Grayscale JPEG images.
//	It is based on the IJG V.6 code.
//
//	This class Copyright 1997, Chris Losinger
//	This is free to use and modify provided my name is 
//	included.
//
//	See jpegfile.h for usage.
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
//		- Chris Losinger
//		- Frans Bouma [FB]
//////////////////////////////////////////////////////////////////////
// VERSION INFORMATION.
//
// v1.3: Some bugfixes and cleanup.
// v1.2: No changes.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////
// NOTE: This code is hardly readable. This is not my fault (FB) but Chris Losinger
// who made 98% of this file's code. This file, plus the jpeglib file can be
// replaced by a call to OleLoadPicture() and some memDC mumbo jumbo. This requires
// IE 3.0 or higher, which native windows95 machines don't have. For release 1.3 I've
// decided to keep the jpeglib because it adds just 50KB to the dll, and to keep compatible
// with windows95 native. Next releases may drop support for windows95 without IE.
//
// [FB] On 28th of november 2000
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

#ifdef __cplusplus
	extern "C" {
#endif // __cplusplus

#define HAVE_BOOLEAN 

#include "JpegLib\jpeglib.h"

#ifdef __cplusplus
	}
#endif // __cplusplus

//
//
//


/*
 * <setjmp.h> is used for the optional error recovery mechanism shown in
 * the second part of the example.
*/

#include <setjmp.h>

// error handler, to avoid those pesky exit(0)'s

struct my_error_mgr {
  struct jpeg_error_mgr pub;

  jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

//
//
//

METHODDEF(void) my_error_exit (j_common_ptr cinfo);

//
//	to handle fatal errors.
//	the original JPEG code will just exit(0). can't really
//	do that in Windows....
//

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Always display the message. */
	MessageBox(NULL,buffer,"JPEG Fatal Error",MB_ICONSTOP);


	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

// store a scanline to our data buffer
void j_putRGBScanline(BYTE *jpegline, 
						 int widthPix,
						 BYTE *outBuf,
						 int row);

void j_putGrayScanlineToRGB(BYTE *jpegline, 
						 int widthPix,
						 BYTE *outBuf,
						 int row);


//
//	constructor doesn't do much - there's no real class here...
//

JpegFile::JpegFile()
{
}

//
//	
//

JpegFile::~JpegFile()
{
}

//
//	read a JPEG file
//
//  [FB] Changed filename to FILE inputstream pointer, so we can read from data files, which are 
//       openend elsewhere.
//

BYTE * JpegFile::JpegFileToRGB(unsigned char *pFileInMemory, UINT *width, UINT *height, long lFileLength)

{
	BYTE *dataBuf;

	// This struct contains the JPEG decompression parameters and pointers to
	// working space (which is allocated as needed by the JPEG library).
	struct jpeg_decompress_struct cinfo;

	// We use our private extension JPEG error handler.
	// Note that this struct must live as long as the main JPEG parameter
	// struct, to avoid dangling-pointer problems.
	struct my_error_mgr jerr;
	// More stuff 
	JSAMPARRAY buffer;
	int row_stride;

	*width=0;
	*height=0;

	// Step 1: allocate and initialize JPEG decompression object 
	// We set up the normal JPEG error routines, then override error_exit.
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	// Establish the setjmp return context for my_error_exit to use.
	if (setjmp(jerr.setjmp_buffer)) 
	{
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	// Now we can initialize the JPEG decompression object. 
	jpeg_create_decompress(&cinfo);

	// Step 2: specify data source (eg, a file)

	// pass 'NULL' because we don't read from a file
	jpeg_stdio_src(&cinfo, NULL);

	//=======================
	// [FB]
	// Put the pointer to the src buffer into the src struct inside cinfo.
	// And set the parameters of the buffer and the data inside the buffer in the src struct
	// From now on, the lib will think it reads from the file but it reads from the buffer
	// It won't call fill_input_buffer, because there won't be a buffer underrun.

	// store pointer in src struct
	// cinfo.src->buffer = (JOCTET *) pFileInMemory;
	cinfo.src->bytes_in_buffer = lFileLength;
	cinfo.src->next_input_byte = (JOCTET *) pFileInMemory;
	//cinfo.src->start_of_file = FALSE;
	//=======================

	// Step 3: read file parameters with jpeg_read_header()

	(void) jpeg_read_header(&cinfo, TRUE);

	// Step 4: Start decompressor
	(void) jpeg_start_decompress(&cinfo);

	// get our buffer set to hold data

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	dataBuf=(BYTE *)malloc(cinfo.output_width * 3 * cinfo.output_height);
	if (dataBuf==NULL) 
	{
		MessageBox(NULL,"DemoGL :\nOut of memory. Couldn't allocate memory for jpeg file.","Jpeg Read Error",MB_ICONSTOP);
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	// how big is this thing gonna be?
	*width = cinfo.output_width;
	*height = cinfo.output_height;
	
	// JSAMPLEs per row in output buffer 
	row_stride = cinfo.output_width * cinfo.output_components;

	// Make a one-row-high sample array that will go away when done with image 
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	// Here we use the library's state variable cinfo.output_scanline as the
	// loop counter, so that we don't have to keep track ourselves.
	while (cinfo.output_scanline < cinfo.output_height) {
		// jpeg_read_scanlines expects an array of pointers to scanlines.
		// Here the array is only one element long, but you could ask for
		// more than one scanline at a time if that's more convenient.
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		// asuumer all 3-components are RGBs
		if (cinfo.out_color_components==3) {
			
			j_putRGBScanline(buffer[0], 
								*width,
								dataBuf,
								cinfo.output_scanline-1);

		} else if (cinfo.out_color_components==1) {

			// assume all single component images are grayscale
			j_putGrayScanlineToRGB(buffer[0], 
								*width,
								dataBuf,
								cinfo.output_scanline-1);

		}

	}

	// Step 7: Finish decompression

	(void) jpeg_finish_decompress(&cinfo);

	// This is an important step since it will release a good deal of memory.
	jpeg_destroy_decompress(&cinfo);

	return dataBuf;
}



//BOOL JpegFile::GetJPGDimensions(FILE *fpInput,
//								UINT *width,
//								UINT *height)
//
//{
	// basic code from IJG Jpeg Code v6 example.c

	/* This struct contains the JPEG decompression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	*/
//	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
//	struct my_error_mgr jerr;

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
//	cinfo.err = jpeg_std_error(&jerr.pub);
//	jerr.pub.error_exit = my_error_exit;


	/* Establish the setjmp return context for my_error_exit to use. */
//	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */

//		jpeg_destroy_decompress(&cinfo);
//		return FALSE;
//	}

	/* Now we can initialize the JPEG decompression object. */
//	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

//	jpeg_stdio_src(&cinfo, fpInput);

	/* Step 3: read file parameters with jpeg_read_header() */

//	(void) jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	*   (a) suspension is not possible with the stdio data source, and
	*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	* See libjpeg.doc for more info.
	*/


	// how big is this thing ?
//	*width = cinfo.image_width;
//	*height = cinfo.image_height;

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will release a good deal of memory. */
//	jpeg_destroy_decompress(&cinfo);

//	return TRUE;
//}


//
//
//

/*
BYTE *JpegFile::RGBFromDWORDAligned(BYTE *inBuf,
									UINT widthPix, 
									UINT widthBytes,
									UINT height)
{
	if (inBuf==NULL)
		return NULL;


	BYTE *tmp;
	tmp=(BYTE *)new BYTE[height * widthPix * 3];
	if (tmp==NULL)
		return NULL;

	UINT row;

	for (row=0;row<height;row++) {
		memcpy((tmp+row * widthPix * 3), 
				(inBuf + row * widthBytes), 
				widthPix * 3);
	}

	return tmp;
}
*/


void j_putRGBScanline(BYTE *jpegline, 
					 int widthPix,
					 BYTE *outBuf,
					 int row)
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) 
	{
		*(outBuf + offset + count * 3 + 0) = *(jpegline + count * 3 + 0);
		*(outBuf + offset + count * 3 + 1) = *(jpegline + count * 3 + 1);
		*(outBuf + offset + count * 3 + 2) = *(jpegline + count * 3 + 2);
	}
}

//
//	stash a gray scanline
//

void j_putGrayScanlineToRGB(BYTE *jpegline, 
							 int widthPix,
							 BYTE *outBuf,
							 int row)
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) {

		BYTE iGray;

		// get our grayscale value
		iGray = *(jpegline + count);

		*(outBuf + offset + count * 3 + 0) = iGray;
		*(outBuf + offset + count * 3 + 1) = iGray;
		*(outBuf + offset + count * 3 + 2) = iGray;
	}
}

//
// copies BYTE buffer into DWORD-aligned BYTE buffer
// return addr of new buffer
//


/*
BYTE * JpegFile::MakeDwordAlignedBuf(BYTE *dataBuf,
									 UINT widthPix,				// pixels!!
									 UINT height,
									 UINT *uiOutWidthBytes)		// bytes!!!
{
	////////////////////////////////////////////////////////////
	// what's going on here? this certainly means trouble 
	if (dataBuf==NULL)
		return NULL;

	////////////////////////////////////////////////////////////
	// how big is the smallest DWORD-aligned buffer that we can use?
	UINT uiWidthBytes;
	uiWidthBytes = WIDTHBYTES(widthPix * 24);

	DWORD dwNewsize=(DWORD)((DWORD)uiWidthBytes * 
							(DWORD)height);
	BYTE *pNew;

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	pNew=(BYTE *)new BYTE[dwNewsize];
	if (pNew==NULL) {
		return NULL;
	}
	
	////////////////////////////////////////////////////////////
	// copy row-by-row
	UINT uiInWidthBytes = widthPix * 3;
	UINT uiCount;
	for (uiCount=0;uiCount < height;uiCount++) {
		BYTE * bpInAdd;
		BYTE * bpOutAdd;
		ULONG lInOff;
		ULONG lOutOff;

		lInOff=uiInWidthBytes * uiCount;
		lOutOff=uiWidthBytes * uiCount;

		bpInAdd= dataBuf + lInOff;
		bpOutAdd= pNew + lOutOff;

		memcpy(bpOutAdd,bpInAdd,uiInWidthBytes);
	}

	*uiOutWidthBytes=uiWidthBytes;
	return pNew;
}

*/

/*
//
//	vertically flip a buffer 
//	note, this operates on a buffer of widthBytes bytes, not pixels!!!
//

BOOL JpegFile::VertFlipBuf(BYTE  * inbuf, 
					   UINT widthBytes, 
					   UINT height)
{   
	BYTE  *tb1;
	BYTE  *tb2;

	if (inbuf==NULL)
		return FALSE;

	UINT bufsize;

	bufsize=widthBytes;

	tb1= (BYTE *)new BYTE[bufsize];
	if (tb1==NULL) {
		return FALSE;
	}

	tb2= (BYTE *)new BYTE [bufsize];
	if (tb2==NULL) {
		delete [] tb1;
		return FALSE;
	}
	
	UINT row_cnt;     
	ULONG off1=0;
	ULONG off2=0;

	for (row_cnt=0;row_cnt<(height+1)/2;row_cnt++) {
		off1=row_cnt*bufsize;
		off2=((height-1)-row_cnt)*bufsize;   
		
		memcpy(tb1,inbuf+off1,bufsize);
		memcpy(tb2,inbuf+off2,bufsize);	
		memcpy(inbuf+off1,tb2,bufsize);
		memcpy(inbuf+off2,tb1,bufsize);
	}	

	delete [] tb1;
	delete [] tb2;

	return TRUE;
}        

*/

//
//	swap Rs and Bs
//
//	Note! this does its stuff on buffers with a whole number of pixels
//	per data row!!
//


/*
BOOL JpegFile::BGRFromRGB(BYTE *buf, UINT widthPix, UINT height)
{
	if (buf==NULL)
		return FALSE;

	UINT col, row;
	for (row=0;row<height;row++) {
		for (col=0;col<widthPix;col++) {
			LPBYTE pRed, pGrn, pBlu;
			pRed = buf + row * widthPix * 3 + col * 3;
			pGrn = buf + row * widthPix * 3 + col * 3 + 1;
			pBlu = buf + row * widthPix * 3 + col * 3 + 2;

			// swap red and blue
			BYTE tmp;
			tmp = *pRed;
			*pRed = *pBlu;
			*pBlu = tmp;
		}
	}
	return TRUE;
}

*/


/*
//
//	Note! this does its stuff on buffers with a whole number of pixels
//	per data row!!
//

BOOL JpegFile::MakeGrayScale(BYTE *buf, UINT widthPix, UINT height)
{
	if (buf==NULL)
		return FALSE;

	UINT row,col;
	for (row=0;row<height;row++) {
		for (col=0;col<widthPix;col++) {
			LPBYTE pRed, pGrn, pBlu;
			pRed = buf + row * widthPix * 3 + col * 3;
			pGrn = buf + row * widthPix * 3 + col * 3 + 1;
			pBlu = buf + row * widthPix * 3 + col * 3 + 2;

			// luminance
			int lum = (int)(.299 * (double)(*pRed) + .587 * (double)(*pGrn) + .114 * (double)(*pBlu));

			*pRed = (BYTE)lum;
			*pGrn = (BYTE)lum;
			*pBlu = (BYTE)lum;
		}
	}
	return TRUE;
}

*/

