#include        "idl.h"
#include        "idl_extern.h"
#include        "be.h"

#include "be_visitor_interface.h"

ACE_RCSID (be_visitor_interface, base_proxy_broker_impl_ch, "proxy_impls_ch.cpp,v 1.2 2001/05/15 15:48:35 parsons Exp")

be_visitor_interface_proxy_impls_ch::be_visitor_interface_proxy_impls_ch (be_visitor_context *ctx)
  : be_visitor_interface (ctx)
{
  // No-Op.
}


be_visitor_interface_proxy_impls_ch::~be_visitor_interface_proxy_impls_ch (void)
{
  // No-Op.
}

int
be_visitor_interface_proxy_impls_ch::visit_interface (be_interface *node)
{
  TAO_OutStream *os = this->ctx_->stream ();

  // Generate Guards.
  *os << "// The Proxy Implementations are used by each interface to" 
      << be_nl
      << "// perform a call. Each different implementation encapsulate" 
      << be_nl
      << "// an invocation logics." << be_nl << be_nl;

  // Code Generation for the proxy imlpementations base class.
  be_visitor *visitor = 0;
  be_visitor_context ctx (*this->ctx_);
  ctx.state (TAO_CodeGen::TAO_INTERFACE_BASE_PROXY_IMPL_CH);
  visitor = tao_cg->make_visitor (&ctx);

  if (!visitor || (node->accept (visitor) == -1))
    {
      delete visitor;
      ACE_ERROR_RETURN ((LM_ERROR,
                         "be_visitor_interface_ch::"
                         "visit_interface - "
                         "codegen for Base Proxy Impl. class failed\n"),
                        -1);
    }

  delete visitor;

  ctx = *this->ctx_;
  ctx.state (TAO_CodeGen::TAO_INTERFACE_REMOTE_PROXY_IMPL_CH);
  visitor = tao_cg->make_visitor (&ctx);

  if (!visitor || (node->accept (visitor) == -1))
    {
      delete visitor;
      ACE_ERROR_RETURN ((LM_ERROR,
                         "be_visitor_interface_ch::"
                         "visit_interface - "
                         "codegen for Remote Proxy Broker class failed\n"),
                        -1);
    }

  delete visitor;

  return 0;
}
