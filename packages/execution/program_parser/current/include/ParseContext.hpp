/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ParseContext.hpp 

                        ParseContext.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef PARSECONTEXT_HPP
#define PARSECONTEXT_HPP

#include "parser-common.hpp"
#include "ValueParser.hpp"

namespace ORO_Execution
{

    /**
     * A Helper class to store parser relevant
     * objects.
     */
    struct ParseContext
    {
        ParseContext( Processor* proc,
                      const GlobalFactory* se )
            : processor( proc ), scriptableextension( se )
        {
        }

        ValueParser valueparser;
        Processor* processor;
        const GlobalFactory* scriptableextension;
  };
}

#endif
