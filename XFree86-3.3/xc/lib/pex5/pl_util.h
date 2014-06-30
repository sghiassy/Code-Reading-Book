/* $XConsortium: pl_util.h,v 1.6 94/04/17 20:22:51 mor Exp $ */
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

#define ZERO_TOLERANCE 1.0e-30

#define ABS(_x) ((_x) < 0.0 ? -(_x) : (_x))

#define NEAR_ZERO(_s) (ABS (_s) < ZERO_TOLERANCE)

#define ZERO_MAG(_s) ((_s) < ZERO_TOLERANCE)

#define IN_RANGE(_low, _high, _val) ((_val) >= (_low) && (_val) <= (_high))

#define MAG_V3(_v) \
    (sqrt ((_v)->x * (_v)->x + (_v)->y * (_v)->y + (_v)->z * (_v)->z))

#define MAG_V2(_v) \
    (sqrt ((_v)->x * (_v)->x + (_v)->y * (_v)->y))

#define BAD_SUBVOLUME(_volume) \
    (!IN_RANGE (0.0, 1.0, _volume->min.x) || \
     !IN_RANGE (0.0, 1.0, _volume->max.x) || \
     !IN_RANGE (0.0, 1.0, _volume->min.y) || \
     !IN_RANGE (0.0, 1.0, _volume->max.y) || \
     !IN_RANGE (0.0, 1.0, _volume->min.z) || \
     !IN_RANGE (0.0, 1.0, _volume->max.z) || \
     !(_volume->min.x <  _volume->max.x) || \
     !(_volume->min.y <  _volume->max.y) || \
     !(_volume->min.z <= _volume->max.z))


/*
 * Dot product of v1 * v2
 */

#define DOT_PRODUCT(_v1, _v2) \
    ((_v1)->x * (_v2)->x + (_v1)->y * (_v2)->y + (_v1)->z * (_v2)->z)


/*
 * Cross product of p1p2 x p3p4
 */

#define CROSS_PRODUCT(_p1, _p2, _p3, _p4, _v) \
{ \
    (_v)->x = (((_p2)->y - (_p1)->y) * ((_p4)->z - (_p3)->z)) - \
              (((_p2)->z - (_p1)->z) * ((_p4)->y - (_p3)->y)); \
    (_v)->y = (((_p2)->z - (_p1)->z) * ((_p4)->x - (_p3)->x)) - \
              (((_p2)->x - (_p1)->x) * ((_p4)->z - (_p3)->z)); \
    (_v)->z = (((_p2)->x - (_p1)->x) * ((_p4)->y - (_p3)->y)) - \
              (((_p2)->y - (_p1)->y) * ((_p4)->x - (_p3)->x)); \
}


/*
 * Normalize vector and return length
 */

#define NORMALIZE_VECTOR(_v, _len) \
{ \
    _len = DOT_PRODUCT (_v, _v); \
    _len = sqrt (_len); \
    if (!NEAR_ZERO (_len)) \
    { \
      (_v)->x /= _len; \
      (_v)->y /= _len; \
      (_v)->z /= _len; \
    } \
}


/*
 * Determine if a point is inside of a view
 */

#define POINT3D_IN_VIEW(_point, _view) \
    (_point.x >= _view.clip_limits.min.x && \
     _point.x <= _view.clip_limits.max.x && \
     _point.y >= _view.clip_limits.min.y && \
     _point.y <= _view.clip_limits.max.y && \
     _point.z >= _view.clip_limits.min.z && \
     _point.z <= _view.clip_limits.max.z)

#define POINT2D_IN_VIEW(_point, _view) \
    (_point.x >= _view.clip_limits.min.x && \
     _point.x <= _view.clip_limits.max.x && \
     _point.y >= _view.clip_limits.min.y && \
     _point.y <= _view.clip_limits.max.y)
