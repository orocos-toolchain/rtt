/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  GlobalFactory.hpp 

                        GlobalFactory.hpp -  description
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
 
 
#ifndef GLOBAL_FACTORY_HPP
#define GLOBAL_FACTORY_HPP

#include "CommandFactoryInterface.hpp"
#include "GlobalCommandFactory.hpp"
#include "DataSourceFactory.hpp"


namespace ORO_Execution
{

    /**
     * This factory is used by the Parser ( and sub-Parsers )
     * to retrieve existing commands and data structures.
     * It merely stores the GlobalCommandFactory and GlobalDataSourceFactory
     * for a given application.
     */
    class GlobalFactory
    {
        GlobalCommandFactory commandFact;
        GlobalDataSourceFactory dataFact;

    public:
        const GlobalCommandFactory& commandFactory() const
        {
            return commandFact;
        }

        GlobalCommandFactory& commandFactory()
        {
            return commandFact;
        }

        GlobalDataSourceFactory& dataFactory()
        {
            return dataFact;
        }

        const GlobalDataSourceFactory& dataFactory() const
        {
            return dataFact;
        }

    };

}

#endif
