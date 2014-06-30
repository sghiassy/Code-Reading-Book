//ControlsKit.h
#ifndef _CONTROLSKIT_H__
#define _CONTROLSKIT_H__

#ifdef WIN32
	#ifdef CONTROLSKITDLL_EXPORTS
		#define CONTROLKIT_API __declspec(dllexport)
	#else
		#define CONTROLKIT_API __declspec(dllimport)
	#endif //CONTROLSKITDLL_EXPORTS
#else
	#define CONTROLKIT_API
#endif //WIN32


#include "ApplicationKit.h"



#endif //_CONTROLSKIT_H__