// ------------------
// Purenum 0.4e alpha
// ------------------
// int.h
// ------------------


#ifndef INT_H
#define INT_H

#define PURENUM_SETTING_INT_DEFAULT

#ifdef PURENUM_INT_DEFAULT_HEADER
#include PURENUM_INT_DEFAULT_HEADER
#else    // PURENUM_INT_DEFAULT_HEADER
#include "integer.h"
#endif    // PURENUM_INT_DEFAULT_HEADER

#undef PURENUM_SETTING_INT_DEFAULT

#endif    // INT_H

