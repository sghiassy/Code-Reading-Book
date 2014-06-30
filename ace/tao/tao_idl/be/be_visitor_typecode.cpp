//
// be_visitor_typecode.cpp,v 1.5 1999/05/15 14:34:32 gokhale Exp
//

// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    be_visitor_typecode.cpp
//
// = DESCRIPTION
//    Visitors for generation of code for TypeCodes for various types
//
// = AUTHOR
//    Aniruddha Gokhale
//
// ============================================================================

#include	"idl.h"
#include	"idl_extern.h"
#include	"be.h"

#include "be_visitor_typecode.h"

// include all the individual files
#include "be_visitor_typecode/typecode_decl.cpp"
#include "be_visitor_typecode/typecode_defn.cpp"

ACE_RCSID(be, be_visitor_typecode, "be_visitor_typecode.cpp,v 1.5 1999/05/15 14:34:32 gokhale Exp")

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Node<be_visitor_typecode_defn::QNode*>;
template class ACE_Unbounded_Queue<be_visitor_typecode_defn::QNode*>;
template class ACE_Unbounded_Queue_Iterator<be_visitor_typecode_defn::QNode*>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Node<be_visitor_typecode_defn::QNode*>
#pragma instantiate ACE_Unbounded_Queue<be_visitor_typecode_defn::QNode*>
#pragma instantiate ACE_Unbounded_Queue_Iterator<be_visitor_typecode_defn::QNode*>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
