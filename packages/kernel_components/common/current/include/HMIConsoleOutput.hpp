/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  HMIConsoleOutput.hpp 

                        HMIConsoleOutput.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
#ifndef HMI_CONSOLE_OUTPUT_HPP
#define HMI_CONSOLE_OUTPUT_HPP

#include <execution/TemplateCommandFactory.hpp>
#include <execution/TemplateDataSourceFactory.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#include <iostream>

#pragma interface
namespace ORO_ControlKernel
{
    using namespace ORO_Execution;

    /**
     * @brief This component can be used to display messages on the
     * standard output.
     *
     * It should not be used in hard realtime
     * programs, but is nice for testing program and state scripts.
     * HMI == Human-Machine Interface
     * @ingroup kcomps kcomp_support
     */
    template< class Base = SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >
    class HMIConsoleOutput
        : public Base
    {
        std::string coloron;
        std::string coloroff;
    public :
        HMIConsoleOutput()
            : Base("console_output"), coloron("\033[1;34m"), coloroff("\033[0m")
              {}

        /**
         * @brief Display a message on standard output.
         */
        void display(const std::string & what)
        {
            std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
        }

        /**
         * @brief Display a boolean on standard output.
         */
        void displayBool(bool what)
        {
            std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
        }

        /**
         * @brief Display an integer on standard output.
         */
        void displayInt( int what)
        {
            std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
        }

        /**
         * @brief Display a double on standard output.
         */
        void displayDouble( double what )
        {
            std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
        }



        bool true_gen() const
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
            ret->add( "displayBool", 
                      command( &HMIConsoleOutput<Base>::displayBool,
                               &HMIConsoleOutput<Base>::true_gen,
                               "Display a boolean on the console",
                               "boolean","The Boolean to be displayed"
                               ) );
            ret->add( "displayInt", 
                      command( &HMIConsoleOutput<Base>::displayInt,
                               &HMIConsoleOutput<Base>::true_gen,
                               "Display a integer on the console",
                               "integer","The Integer to be displayed"
                               ) );
            ret->add( "displayDouble", 
                      command( &HMIConsoleOutput<Base>::displayDouble,
                               &HMIConsoleOutput<Base>::true_gen,
                               "Display a double on the console",
                               "double","The Double to be displayed"
                               ) );
            return ret;
        }
    };

    extern template class HMIConsoleOutput<>;

#if 0
    /**
     * @brief This component can be used to display messages on the
     * standard output. 
     *
     * It should not be used in hard realtime
     * programs, but is nice for testing program and state scripts.
     * HMI == Human-Machine Interface
     * @ingroup kcomps kcomp_support
     */
    class HMIConsoleOutputImpl
        : public SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase >
    {
        std::string coloron;
        std::string coloroff;
        typedef SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > Base;
    public :
        HMIConsoleOutputImpl();

        /**
         * @brief Display a message on standard output.
         */
        void display(const std::string & what);

        /**
         * @brief Display a boolean on standard output.
         */
        void displayBool(bool what);

        /**
         * @brief Display an integer on standard output.
         */
        void displayInt( int what);

        /**
         * @brief Display a double on standard output.
         */
        void displayDouble( double what );

        bool true_gen() const { return true; }

        // Commands are display commands.
        CommandFactoryInterface* createCommandFactory();
    };

}

#endif
