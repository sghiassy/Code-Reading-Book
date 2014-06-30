//
// be_visitor_field.cpp,v 1.9 1999/05/28 02:16:42 coryan Exp
//
// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    be_visitor_field.cpp
//
// = DESCRIPTION
//    Concrete visitor for the base "BE_Field" node
//
// = AUTHOR
//    Aniruddha Gokhale
//
// ============================================================================

#include "idl.h"
#include "idl_extern.h"
#include "be.h"

#include "be_visitor_field.h"

// include all the individual files
#include "be_visitor_field/field_ch.cpp"
#include "be_visitor_field/field_ci.cpp"
#include "be_visitor_field/field_cs.cpp"
#include "be_visitor_field/cdr_op_ch.cpp"
#include "be_visitor_field/cdr_op_ci.cpp"
#include "be_visitor_field/cdr_op_cs.cpp"

ACE_RCSID(be, be_visitor_field, "be_visitor_field.cpp,v 1.9 1999/05/28 02:16:42 coryan Exp")
