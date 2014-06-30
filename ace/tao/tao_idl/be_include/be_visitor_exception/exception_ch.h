/* -*- C++ -*- */
//
// exception_ch.h,v 1.2 1998/10/20 02:32:25 levine Exp
//

// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    exception_ch.h
//
// = DESCRIPTION
//    Visitor for Exceptions.
//    For code generation in client header
//
// = AUTHOR
//    Aniruddha Gokhale
//
// ============================================================================

#ifndef _BE_VISITOR_EXCEPTION_EXCEPTION_CH_H_
#define _BE_VISITOR_EXCEPTION_EXCEPTION_CH_H_

class be_visitor_exception_ch : public be_visitor_exception
{
  //
  // = TITLE
  //   be_visitor_exception_ch
  //
  // = DESCRIPTION
  //   This is a concrete visitor to generate the client header for exception
  //
  //
public:
  be_visitor_exception_ch (be_visitor_context *ctx);
  // constructureor

  ~be_visitor_exception_ch (void);
  // destructureor

  virtual int visit_exception (be_exception *node);
  // visit exception. We provide code for this method in the derived class

};

#endif /* _BE_VISITOR_EXCEPTION_EXCEPTION_CH_H_ */
