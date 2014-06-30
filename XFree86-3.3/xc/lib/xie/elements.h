/* $XConsortium: elements.h,v 1.5 95/06/08 23:20:39 gildea Exp $ */

/*

Copyright (c) 1993, 1994  X Consortium

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

#ifndef _ELEMENTS_H_
#define _ELEMENTS_H_

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define ELEM_NAME(_name_) xieFlo##_name_
#define ELEM_SIZE(_name_) sz_xieFlo##_name_
#else
#define ELEM_NAME(_name_) xieFlo/**/_name_
#define ELEM_SIZE(_name_) sz_xieFlo/**/_name_
#endif


#ifndef WORD64

#define BEGIN_ELEM_HEAD(_name, _elemSrc, _elemLength, _bufDest, _elemDest) \
    _elemDest = (ELEM_NAME(_name) *) _bufDest; \
    _elemDest->elemType = elemSrc->elemType; \
    _elemDest->elemLength = _elemLength;

#define END_ELEM_HEAD(_name, _bufDest, _elemDest) \
    _bufDest += ELEM_SIZE(_name);

#else /* WORD64 */

#define BEGIN_ELEM_HEAD(_name, _elemSrc, _elemLength, _bufDest, _elemDest) \
{ \
    ELEM_NAME(_name) tElem; \
    _elemDest = &tElem; \
    _elemDest->elemType = elemSrc->elemType; \
    _elemDest->elemLength = _elemLength;

#define END_ELEM_HEAD(_name, _bufDest, _elemDest) \
    memcpy (_bufDest, _elemDest, ELEM_SIZE(_name)); \
    _bufDest += ELEM_SIZE(_name); \
}

#endif /* WORD64 */


#ifndef WORD64

#define STORE_LISTOF_TILES(_tiles, _count, _pBuf) \
{ \
    xieTypTile *tileDest = (xieTypTile *) _pBuf; \
    int     i; \
\
    for (i = 0; i < _count; i++) \
    { \
	tileDest->src  = _tiles[i].src; \
        tileDest->dstX = _tiles[i].dst_x; \
        tileDest->dstY = _tiles[i].dst_y; \
        tileDest++; \
    } \
    _pBuf += (SIZEOF (xieTypTile) * _count); \
}

#else /* WORD64 */

#define STORE_LISTOF_TILES(_tiles, _count, _pBuf) \
{ \
    xieTypTile temp; \
    int     i; \
\
    for (i = 0; i < _count; i++) \
    { \
	temp.src  = _tiles[i].src; \
        temp.dstX = _tiles[i].dst_x; \
        temp.dstY = _tiles[i].dst_y; \
	memcpy (_pBuf, &temp, SIZEOF (xieTypTile)); \
        _pBuf += SIZEOF (xieTypTile); \
    } \
}

#endif

#endif /* _ELEMENTS_H_ */
