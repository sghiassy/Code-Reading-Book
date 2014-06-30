/* -*- C++ -*- */
//=============================================================================
/**
 *  @file    Widget_Factory.cpp
 *
 *  Widget_Factory.cpp,v 1.1 2001/08/29 12:02:07 schmidt Exp
 *
 *  @author Christopher Kohlhoff <chris@kohlhoff.com>
 */
//=============================================================================

#include "Widget_Factory.h"
#include "Widget_Impl.h"

Widget *Widget_Factory::create_widget (void)
{
  return new Widget_Impl;
}
