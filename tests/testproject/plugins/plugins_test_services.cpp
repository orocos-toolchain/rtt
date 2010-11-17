/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  plugins_test_services.cpp

                        plugins_test_services.cpp -  description
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


#include <rtt/Service.hpp>
#include <rtt/plugin/ServicePlugin.hpp>

using namespace RTT;

class ServicePluginTest : public Service
{
public:
    ServicePluginTest(TaskContext* tc) : Service("the_service", tc) {}
};

ORO_SERVICE_NAMED_PLUGIN( ServicePluginTest, "serviceplugin")
