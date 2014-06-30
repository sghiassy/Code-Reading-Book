#ifndef _DEBUG_H
#define _DEBUG_H

extern int debuglevel;	// bitmapped int describing debug info to show

// masks for debuglevel (best viewed with TABSTOP=3)
#define DEBUGLEVEL_VERBOSITY			(15)
#define DEBUGLEVEL_CONSTRUCTOR		(1<<4)
#define DEBUGLEVEL_COPYCONSTRUCTOR	(1<<5)
#define DEBUGLEVEL_DESTRUCTOR			(1<<6)
#define DEBUGLEVEL_NOTICE				(1<<7)
#define DEBUGLEVEL_WARNING				(1<<8)
#define DEBUGLEVEL_ERROR				(1<<9)
#define DEBUGLEVEL_FATAL				(1<<10)
#define DEBUGLEVEL_NETWORK				(1<<11)

#endif	// _DEBUG_H
