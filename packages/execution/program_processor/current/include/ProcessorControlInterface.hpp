/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  ProcessorControlInterface.hpp 

                        ProcessorControlInterface.hpp -  description
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
 
 
#ifndef PROCESSORCONTROLINTERFACE_HPP
#define PROCESSORCONTROLINTERFACE_HPP

namespace ORO_Execution
{

    /**
     * This abstract class provides functions that control 
     * its configuration state.
     */
    class ProcessorControlInterface
    {
    public:
        
        virtual ~ProcessorControlInterface()
        {}

    };

}

#endif
