/*
 * 
Copyright (c) 1989  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
 * */

/***********************************************************************
 *
 * $XConsortium: iconmgr.h,v 1.12 94/04/17 20:38:13 jim Exp $
 *
 * Icon Manager includes
 *
 * 09-Mar-89 Tom LaStrange		File Created
 *
 ***********************************************************************/

#ifndef _ICONMGR_
#define _ICONMGR_

typedef struct WList
{
    struct WList *next;
    struct WList *prev;
    struct TwmWindow *twm;
    struct IconMgr *iconmgr;
    Window w;
    Window icon;
    int x, y, width, height;
    int row, col;
    int me;
    Pixel fore, back, highlight;
    unsigned top, bottom;
    short active;
    short down;
} WList;

typedef struct IconMgr
{
    struct IconMgr *next;		/* pointer to the next icon manager */
    struct IconMgr *prev;		/* pointer to the previous icon mgr */
    struct IconMgr *lasti;		/* pointer to the last icon mgr */
    struct WList *first;		/* first window in the list */
    struct WList *last;			/* last window in the list */
    struct WList *active;		/* the active entry */
    TwmWindow *twm_win;			/* back pointer to the new parent */
    struct ScreenInfo *scr;		/* the screen this thing is on */
    Window w;				/* this icon manager window */
    char *geometry;			/* geometry string */
    char *name;
    char *icon_name;
    int x, y, width, height;
    int columns, cur_rows, cur_columns;
    int count;
} IconMgr;

extern int iconmgr_textx;
extern WList *DownIconManager;

extern void CreateIconManagers();
extern IconMgr *AllocateIconManager();
extern void MoveIconManager();
extern void JumpIconManager();
extern WList *AddIconManager();
extern void InsertInIconManager();
extern void RemoveFromIconManager();
extern void RemoveIconManager();
extern void ActiveIconManager();
extern void NotActiveIconManager();
extern void DrawIconManagerBorder();
extern void SortIconManager();
extern void PackIconManager();


#endif /* _ICONMGR_ */
