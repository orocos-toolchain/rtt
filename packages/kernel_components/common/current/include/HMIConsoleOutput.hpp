/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  HMIConsoleOutput.hpp 

                        HMIConsoleOutput.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 
#ifndef HMI_CONSOLE_OUTPUT_HPP
#define HMI_CONSOLE_OUTPUT_HPP

#include <execution/TemplateCommandFactory.hpp>
#include <execution/TemplateDataSourceFactory.hpp>
#include <iostream>

namespace ORO_ControlKernel
{

    /**
     * This component can be used to display messages on the
     * standard output. It should not be used in hard realtime
     * programs, but is nice for testing program and state scripts.
     * HMI == Human-Machine Interface
     */
    template< class Base>
    class HMIConsoleOutput
        : public Base
    {
    public :
        HMIConsoleOutput()
            : Base("console_output")
              {}

        /**
         * Display a message on standard output.
         */
        void display(const std::string & what)
        {
            std::cout << "HMIConsoleOutput : "<< what <<std::endl;
        }

        template< class T >
        bool true_gen(const T& t) const
        {
            return true;
        }

        // Commands are display commands.
        CommandFactoryInterface* createCommandFactory()
        {
            TemplateCommandFactory< HMIConsoleOutput<Base> >* ret =
                newCommandFactory( this );
            ret->add( "display", 
                      command( &HMIConsoleOutput<Base>::display,
                               &HMIConsoleOutput<Base>::true_gen,
                               "Display a message on the console",
                               "message","The message to be displayed"
                               ) );
            return ret;
        }
    };

}

#endif
