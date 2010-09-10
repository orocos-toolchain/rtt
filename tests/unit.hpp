/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  unit.hpp

                        unit.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include <rtt-config.h>
#include <iostream>
#include <Logger.hpp>
#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>
#include <RTT.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using namespace RTT;
using namespace RTT::detail;
using namespace boost;
using namespace std;

#if defined(__GNUG__) && defined(__unix__)

# if defined(RTT_UNIT_DLL_EXPORT)
   // Use RTT_UNIT_API for normal function exporting
#  define RTT_UNIT_API    __attribute__((visibility("default")))

   // Use RTT_UNIT_EXPORT for static template class member variables
   // They must always be 'globally' visible.
#  define RTT_UNIT_EXPORT __attribute__((visibility("default")))

   // Use RTT_UNIT_HIDE to explicitly hide a symbol
#  define RTT_UNIT_HIDE   __attribute__((visibility("hidden")))

# else
#  define RTT_UNIT_API
#  define RTT_UNIT_EXPORT __attribute__((visibility("default")))
#  define RTT_UNIT_HIDE   __attribute__((visibility("hidden")))
# endif
#else
   // Win32 and NOT GNU
# if defined( WIN32 ) && !defined ( __MINGW32__ )
#  if defined(RTT_UNIT_DLL_EXPORT)
#   define RTT_UNIT_API    __declspec(dllexport)
#   define RTT_UNIT_EXPORT __declspec(dllexport)
#   define RTT_UNIT_HIDE   
#  else
#   define RTT_UNIT_API	 __declspec(dllimport)
#   define RTT_UNIT_EXPORT __declspec(dllimport)
#   define RTT_UNIT_HIDE 
#  endif
# else
#  define RTT_UNIT_API
#  define RTT_UNIT_EXPORT
#  define RTT_UNIT_HIDE
# endif
#endif
