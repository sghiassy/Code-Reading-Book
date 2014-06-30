/* $XConsortium: pl_lut.h,v 1.4 94/04/17 20:22:33 mor Exp $ */
/*

Copyright (c) 1992  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

#ifndef WORD64

#define BEGIN_LUTENTRY_HEADER(_name, _pBuf, _pEntry) \
    _pEntry = (_name *) _pBuf; 

#define END_LUTENTRY_HEADER(_name, _pBuf, _pEntry) \
    _pBuf += SIZEOF (_name);

#else /* WORD64 */

#define BEGIN_LUTENTRY_HEADER(_name, _pBuf, _pEntry) \
{ \
    _name tEntry; \
    _pEntry = &tEntry;

#define END_LUTENTRY_HEADER(_name, _pBuf, _pEntry) \
    memcpy (_pBuf, _pEntry, SIZEOF (_name)); \
    _pBuf += SIZEOF (_name); \
}

#endif /* WORD64 */


static PEXPointer _PEXRepackLUTEntries();

#define GetLUTEntryBuffer(_numEntries, _entryType, _buf) \
    (_buf) = (PEXPointer) Xmalloc ( \
	(unsigned) ((_numEntries) * (sizeof (_entryType))));



