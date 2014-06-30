/*
 * $XFree86: xc/lib/XThrStub/UIThrStubs.c,v 3.0 1995/11/02 00:27:07 dawes Exp $
 *
 * Copyright (c) 1995 David E. Wexelblat.  All rights reserved
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL DAVID E. WEXELBLAT BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of David E. Wexelblat shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization
 * from David E. Wexelblat.
 * 
 */

/*
 * Stubs for thread functions needed by the X library.  Supports 
 * UnixWare 2.x threads; may support Solaris 2 threads as well, but not
 * tested.  Defining things this way removes the dependency of the X
 * library on the threads library, but still supports threads if the user
 * specificies the thread library on the link line.
 */

#include <thread.h>
#include <synch.h>

#pragma weak thr_self = _Xthr_self_stub_
thread_t 
_Xthr_self_stub_()
{
    return((thread_t)0);
}

#pragma weak mutex_init = _Xmutex_init_stub_
int 
_Xmutex_init_stub_(m, t, a)
    mutex_t *m;
    int t;
    void *a;
{
    return(0);
}

#pragma weak mutex_destroy = _Xmutex_destroy_stub_
int
_Xmutex_destroy_stub_(m)
    mutex_t *m;
{
    return(0);
}

#pragma weak mutex_lock = _Xmutex_lock_stub_
int
_Xmutex_lock_stub_(m)
    mutex_t *m;
{
    return(0);
}

#pragma weak mutex_unlock = _Xmutex_unlock_stub_
int
_Xmutex_unlock_stub_(m)
    mutex_t *m;
{
    return(0);
}

#pragma weak cond_init = _Xcond_init_stub_
int 
_Xcond_init_stub_(c, t, a)
    cond_t *c;
    int t;
    void *a;
{
    return(0);
}

#pragma weak cond_destroy = _Xcond_destroy_stub_
int
_Xcond_destroy_stub_(c)
    cond_t *c;
{
    return(0);
}

#pragma weak cond_wait = _Xcond_wait_stub_
int
_Xcond_wait_stub_(c,m)
    cond_t *c;
    mutex_t *m;
{
    return(0);
}

#pragma weak cond_signal = _Xcond_signal_stub_
int
_Xcond_signal_stub_(c)
    cond_t *c;
{
    return(0);
}

#pragma weak cond_broadcast = _Xcond_broadcast_stub_
int
_Xcond_broadcast_stub_(c)
    cond_t *c;
{
    return(0);
}
