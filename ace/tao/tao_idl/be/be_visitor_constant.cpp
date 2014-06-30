//
// be_visitor_constant.cpp,v 1.5 1998/08/03 17:39:53 gonzo Exp
//

// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    be_visitor_constant.cpp
//
// = DESCRIPTION
//    Visitors for generation of code for Constant
//
// = AUTHOR
//    Aniruddha Gokhale
//
// ============================================================================

#include	"idl.h"
#include	"idl_extern.h"
#include	"be.h"

#include "be_visitor_constant.h"

// include all the individual files
#include "be_visitor_constant/constant_ch.cpp"
#include "be_visitor_constant/constant_cs.cpp"

ACE_RCSID(be, be_visitor_constant, "be_visitor_constant.cpp,v 1.5 1998/08/03 17:39:53 gonzo Exp")

