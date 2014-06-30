/* -*- C++ -*- */
//=============================================================================
/**
 *  @file    Widget_Part_Factory.cpp
 *
 *  Widget_Part_Factory.cpp,v 1.1 2001/08/29 12:02:08 schmidt Exp
 *
 *  @author Christopher Kohlhoff <chris@kohlhoff.com>
 */
//=============================================================================

#include "Widget_Part_Factory.h"
#include "Widget_Part_Impl.h"

Widget_Part *Widget_Part_Factory::create_widget_part (Widget *owner,
                                                      const char* name,
                                                      int size)
{
  return new Widget_Part_Impl (owner, name, size);
}
