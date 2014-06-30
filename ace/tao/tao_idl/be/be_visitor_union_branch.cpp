//
// be_visitor_union_branch.cpp,v 1.12 1999/05/28 02:16:42 coryan Exp
//
// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    be_visitor_union_branch.cpp
//
// = DESCRIPTION
//    Concrete visitor for the base "BE_Union_Branch" node
//
// = AUTHOR
//    Aniruddha Gokhale
//
// ============================================================================

#include        "idl.h"
#include        "idl_extern.h"
#include        "be.h"

#include "be_visitor_union_branch.h"

// include all the individual files
#include "be_visitor_union_branch/private_ch.cpp"
#include "be_visitor_union_branch/public_assign_cs.cpp"
#include "be_visitor_union_branch/public_reset_cs.cpp"
#include "be_visitor_union_branch/public_access_cs.cpp"
#include "be_visitor_union_branch/public_ch.cpp"
#include "be_visitor_union_branch/public_ci.cpp"
#include "be_visitor_union_branch/public_cs.cpp"
#include "be_visitor_union_branch/cdr_op_ch.cpp"
#include "be_visitor_union_branch/cdr_op_ci.cpp"
#include "be_visitor_union_branch/cdr_op_cs.cpp"

ACE_RCSID(be, be_visitor_union_branch, "be_visitor_union_branch.cpp,v 1.12 1999/05/28 02:16:42 coryan Exp")
