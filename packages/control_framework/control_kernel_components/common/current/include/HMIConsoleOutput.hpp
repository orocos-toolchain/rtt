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

#include <control_kernel/BaseComponents.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include <execution/TemplateMethodFactory.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#endif
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>
#include <corelib/TaskNonRealTime.hpp>

#include <os/MutexLock.hpp>
#include <sstream>
#include <iostream>

#pragma interface
namespace ORO_ControlKernel
{
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    using namespace ORO_Execution;
#endif
    using ORO_OS::Mutex;
    using ORO_OS::MutexLock;
    using ORO_OS::MutexTryLock;

    /**
     * @brief This component can be used to display messages on the
     * standard output.
     *
     * It is known as the 'cout' component in scripts.
     *
     * HMI == Human-Machine Interface
     * @ingroup kcomps kcomp_support
     */
    class HMIConsoleOutput
        : public SupportComponent< MakeAspect<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                              , ExecutionExtension
#endif
                                              >::Result >,
          public ORO_CoreLib::TaskNonRealTime
    {
        typedef SupportComponent< MakeAspect<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                             , ExecutionExtension
#endif
                                             >::Result > Base;
        std::string coloron;
        std::string coloroff;
        std::string _prompt;
        std::ostringstream messages;
        std::ostringstream backup;
        //TaskNonRealtime printer;
        Mutex msg_lock;
    public :
        HMIConsoleOutput( const std::string& name = "cout")
            : Base( name ), TaskNonRealTime(0.1), coloron("\033[1;34m"), coloroff("\033[0m"),
              _prompt("HMIConsoleOutput :\n")
              {
                  this->start();
              }

        void step()
        {
            MutexLock lock1( msg_lock );
            if ( ! messages.str().empty() ) {
                std::cout << coloron << _prompt<< coloroff <<
                    messages.str() << std::endl;
                messages.rdbuf()->str("");
            }
        }

        /**
         * Enable or disable using a colored prompt.
         */
        void enableColor(bool yesno = true)
        {
            if (yesno == true) {
                coloron = "\033[1;34m";
                coloroff = "\033[0m";
            } else {
                coloron.clear();
                coloroff.clear();
            }
        }
           
        /**
         * Set the prompt text.
         */
        void setPrompt(const std::string& prompt)
        {
            _prompt = prompt;
        }
                

        /**
         * @brief Display a message on standard output.
         */
        void display(const std::string & what)
        {
            //std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
            this->enqueue( what );
        }

        /**
         * Put a message in the queue. 
         * The message must be convertible to a stream using
         * operator<<().
         */
        template<class T>
        void enqueue( const T& what )
        {
            MutexTryLock try_lock( msg_lock );
            if ( try_lock.isSuccessful() ) {
                // we got the lock, copy everything...
//                 while( !backup.empty() ) {
//                     messages.push( backup.front() );
//                     backup.pop();
//                 }
//                 messages.push( what );
                messages << backup.str();
                messages << what << std::endl;
                backup.rdbuf()->str("");
            }
            else  // no lock, backup.
                backup << what << std::endl;
        }

        /**
         * @brief Display a boolean on standard output.
         */
        void displayBool(bool what)
        {
            this->enqueue( what );
            //std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
        }

        /**
         * @brief Display an integer on standard output.
         */
        void displayInt( int what)
        {
            this->enqueue( what );
            //std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
        }

        /**
         * @brief Display a double on standard output.
         */
        void displayDouble( double what )
        {
            this->enqueue( what );
            //std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
        }

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        // Methods are display commands.
        MethodFactoryInterface* createMethodFactory()
        {
            TemplateMethodFactory< HMIConsoleOutput >* ret =
                newMethodFactory( this );
            ret->add( "display", 
                      method( &HMIConsoleOutput::display,
                               "Display a message on the console",
                               "message","The message to be displayed"
                               ) );
            ret->add( "displayBool", 
                      method( &HMIConsoleOutput::displayBool,
                               "Display a boolean on the console",
                               "boolean","The Boolean to be displayed"
                               ) );
            ret->add( "displayInt", 
                      method( &HMIConsoleOutput::displayInt,
                               "Display a integer on the console",
                               "integer","The Integer to be displayed"
                               ) );
            ret->add( "displayDouble", 
                      method( &HMIConsoleOutput::displayDouble,
                               "Display a double on the console",
                               "double","The Double to be displayed"
                               ) );
            return ret;
        }
#endif
    };

}

#endif
