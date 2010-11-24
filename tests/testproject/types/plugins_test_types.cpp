/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  plugins_test_types.cpp

                        plugins_test_types.cpp -  description
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


/**
 * plugins_test_types.cpp
 *
 *  Created on: May 25, 2010
 *      Author: kaltan
 */

#include <rtt/types/TypekitPlugin.hpp>

class TypesPluginTest : public RTT::types::TypekitPlugin
{
public:
    virtual bool loadTypes() {
        return true;
    }

    virtual bool loadOperators() {
        return true;
    }

    virtual bool loadConstructors() {
        return true;
    }

    virtual bool loadGlobals() { return true; }

    virtual std::string getName() { return "TypesPluginTest"; }

};

ORO_TYPEKIT_PLUGIN( TypesPluginTest )
