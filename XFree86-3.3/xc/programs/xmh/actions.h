/*
 * $XConsortium: actions.h,v 1.10 94/04/17 20:24:00 converse Exp $
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
 *
 */

	/* from compfuncs.c */

extern void	XmhResetCompose();
extern void	XmhSend();
extern void	XmhSave();

	/* from folder.c */

extern void	XmhClose();
extern void	XmhComposeMessage();
extern void 	XmhOpenFolder();
extern void	XmhOpenFolderInNewWindow();
extern void	XmhCreateFolder();
extern void	XmhDeleteFolder();
extern void	XmhPopupFolderMenu();
extern void	XmhSetCurrentFolder();
extern void	XmhLeaveFolderButton();
extern void 	XmhPushFolder();
extern void	XmhPopFolder();
extern void	XmhWMProtocols();

	/* from msg.c */

extern void	XmhInsert();

	/* from popup.c */

extern void	XmhPromptOkayAction();

	/* from toc.c */

extern void	XmhPushSequence();
extern void	XmhPopSequence();
extern void	XmhReloadSeqLists();

	/* from tocfuncs.c */

extern void	XmhCheckForNewMail();
extern void	XmhIncorporateNewMail();
extern void	XmhCommitChanges();
extern void	XmhPackFolder();
extern void	XmhSortFolder();
extern void	XmhForceRescan();
extern void	XmhViewNextMessage();
extern void	XmhViewPreviousMessage();
extern void	XmhMarkDelete();
extern void	XmhMarkMove();
extern void	XmhMarkCopy();
extern void	XmhUnmark();
extern void	XmhViewInNewWindow();
extern void	XmhReply();
extern void	XmhForward();
extern void	XmhUseAsComposition();
extern void	XmhPrint();
extern void	XmhShellCommand();
extern void	XmhPickMessages();
extern void	XmhOpenSequence();
extern void	XmhAddToSequence();
extern void	XmhRemoveFromSequence();
extern void	XmhDeleteSequence();

	/* from viewfuncs.c */

extern void	XmhCloseView();
extern void	XmhViewReply();
extern void	XmhViewForward();
extern void	XmhViewUseAsComposition();
extern void	XmhEditView();
extern void	XmhSaveView();
extern void	XmhPrintView();
extern void	XmhViewMarkDelete();
