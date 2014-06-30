/* -*- C++ -*- */
//=============================================================================
/**
 *  @file    Gadget_Factory.cpp
 *
 *  Gadget_Factory.cpp,v 1.1 2001/08/29 12:02:05 schmidt Exp
 *
 *  @author Christopher Kohlhoff <chris@kohlhoff.com>
 */
//=============================================================================

#include "Gadget_Factory.h"
#include "Gadget_Impl.h"

Gadget_var Gadget_Factory::create_gadget (void)
{
  return Gadget_var (new Gadget_Impl);
}
