//
// be_visitor_structure.cpp,v 1.20 1999/05/28 02:16:42 coryan Exp
//

// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    be_visitor_structure.cpp
//
// = DESCRIPTION
//    Visitors for generation of code for Structure
//
// = AUTHOR
//    Aniruddha Gokhale
//
// ============================================================================

#include "idl.h"
#include "idl_extern.h"
#include "be.h"

#include "be_visitor_structure.h"

// include all the individual files
#include "be_visitor_structure/structure.cpp"
#include "be_visitor_structure/structure_ch.cpp"
#include "be_visitor_structure/structure_ci.cpp"
#include "be_visitor_structure/structure_cs.cpp"
#include "be_visitor_structure/any_op_ch.cpp"
#include "be_visitor_structure/any_op_cs.cpp"
#include "be_visitor_structure/cdr_op_ch.cpp"
#include "be_visitor_structure/cdr_op_ci.cpp"
#include "be_visitor_structure/cdr_op_cs.cpp"

ACE_RCSID(be, be_visitor_structure, "be_visitor_structure.cpp,v 1.20 1999/05/28 02:16:42 coryan Exp")
