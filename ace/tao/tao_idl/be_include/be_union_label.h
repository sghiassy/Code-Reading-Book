// be_union_label.h,v 1.6 2000/09/20 21:09:16 parsons Exp

#ifndef BE_UNION_LABEL_H
#define BE_UNION_LABEL_H

#include "ast_union_label.h"

class be_visitor;

class be_union_label : public virtual AST_UnionLabel
{
public:
  be_union_label (void);
  // Default constructor.

  be_union_label (AST_UnionLabel::UnionLabel ul, 
                  AST_Expression *v);
  // Constructor.

  // Visiting.
  virtual int accept (be_visitor *visitor);
};

#endif
