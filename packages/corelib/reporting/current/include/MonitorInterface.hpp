/***************************************************************************
  tag: Peter Soetens  Wed Apr 17 16:15:59 CEST 2002  MonitorInterface.hpp 

                        MonitorInterface.hpp -  description
                           -------------------
    begin                : Wed April 17 2002
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


#ifndef MONITORINTERFACE_HPP
#define MONITORINTERFACE_HPP

#include <string>

namespace ORO_CoreLib
{

    /**
     * This interfaces defines an object, acting as a server, that will read
     * the contents of a variable (from the client) for reporting purposes.
     * The server will descide how many times and when the variable is read. Since
     * no protection (like mutexes) are available, this interface can only be used
     * when the server works in a way synchronised with the client.
     */
    class MonitorInterface
    {
        public:
            /**
             * Monitor a certain variable.
             * 
             * @param d
             *        The (address of the) variable to be monitored
             * @param name
             *        The name of this variable
             * @param description
             *        An elaborate description of this variable
             */
            virtual void monitor(const double* d, const std::string& name, const std::string& description="")=0;

            /**
             * Monitor a certain variable.
             * 
             * @param i
             *        The (address of the) variable to be monitored
             * @param name
             *        The name of this variable
             * @param description
             *        An elaborate description of this variable
             */
            virtual void monitor(const int* i, const std::string& name, const std::string& description="")=0;

            /**
             * Stops a previously monitored variable from being monitored.
             */ 
            virtual void remove(const std::string& name)=0;
    };

}

#endif
