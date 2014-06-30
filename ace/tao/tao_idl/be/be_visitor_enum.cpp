//
// be_visitor_enum.cpp,v 1.15 1999/05/28 02:16:42 coryan Exp
//

// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    be_visitor_enum.cpp
//
// = DESCRIPTION
//    Visitors for generation of code for Enum
//
// = AUTHOR
//    Aniruddha Gokhale
//
// ============================================================================

#include        "idl.h"
#include        "idl_extern.h"
#include        "be.h"

#include "be_visitor_enum.h"

// include all the individual files
#include "be_visitor_enum/enum_ch.cpp"
#include "be_visitor_enum/enum_cs.cpp"
#include "be_visitor_enum/any_op_ch.cpp"
#include "be_visitor_enum/any_op_cs.cpp"
#include "be_visitor_enum/cdr_op_ch.cpp"
#include "be_visitor_enum/cdr_op_ci.cpp"

ACE_RCSID(be, be_visitor_enum, "be_visitor_enum.cpp,v 1.15 1999/05/28 02:16:42 coryan Exp")
