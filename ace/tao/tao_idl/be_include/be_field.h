// be_field.h,v 1.12 2000/09/20 21:09:16 parsons Exp

// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    be_field.h
//
// = DESCRIPTION
//    Extension of class AST_Field that provides additional means for C++
//    mapping.
//
// = AUTHOR
//    Copyright 1994-1995 by Sun Microsystems, Inc.
//    and
//    Aniruddha Gokhale
//
// ============================================================================

#ifndef BE_FIELD_H
#define BE_FIELD_H

#include "be_decl.h"
#include "ast_field.h"

class UTL_StrList;
class be_visitor;

class be_field : public virtual AST_Field,
                 public virtual be_decl
{
public:
  be_field (void);
  // Default constructor.

  be_field (AST_Type *ft, 
            UTL_ScopedName *n, 
            UTL_StrList *p,
            Visibility vis = vis_NA);
  // Constructor.

  // Visiting.
  virtual int accept (be_visitor *visitor);

  // Narrowing.
  DEF_NARROW_METHODS2 (be_field, AST_Field, be_decl);
  DEF_NARROW_FROM_DECL (be_field);

protected:
  virtual int compute_size_type (void);
  // Compute the size type if it is unknown.
};

#endif
