/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  DispatcherInterface.hpp 

                        DispatcherInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef DISPATCHERINTERFACE_HPP
#define DISPATCHERINTERFACE_HPP

#include "ParserInterface.hpp"

namespace ORO_Execution
{

    /**
     * An abstract base class, responsible for processing std::strings that are fed to
     * it
     * Modules (ParserInterfaces) can be registered in this Dispatcher
     */

    class DispatcherInterface
    {

        public:
            virtual ~DispatcherInterface()
            {}

            /**
             * Processes the given std::string
             *
             * @param toProcess
             *    the std::string that has to be processed
             */
            virtual void process( std::string toProcess ) = 0;

            /**
             * Registers a ParserInterface to this Dispatcher
             *
             * @param parser
             *     the ParserInterface to be registered
             */
            virtual void registerParser( ParserInterface *parser ) = 0;

            /**
             * Unregisters a ParserInterface from this Dispatcher
             * @param parser
             *     the ParserInterface to be unregistered
             */
            virtual void unregisterParser( ParserInterface *parser ) = 0;
    };

}

#endif //DISPATCHERINTERFACE_HPP
