/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  HMIConsoleInput.hpp 

                        HMIConsoleInput.hpp -  description
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
#ifndef HMICONSOLEINPUT_HPP
#define HMICONSOLEINPUT_HPP
 

#include <execution/TemplateCommandFactory.hpp>
#include <execution/TemplateDataSourceFactory.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <execution/Parser.hpp>
#include <boost/lambda/lambda.hpp>

#include <iostream>
#include <stdio.h> // how do you do getchar with cin ??

#pragma interface

namespace ORO_ControlKernel
{
    using std::cout;
    using std::cin;
    using std::endl;
    using namespace ORO_Execution;

    /**
     * @brief This component allows console input to be passed
     * to the program or state scripts. It can be referenced
     * as the 'cin' component in these scripts.
     *
     * HMI == Human-Machine Interface
     * @ingroup kcomps kcomp_support
     */
    class HMIConsoleInput
        : public SupportComponent< MakeAspect<KernelBaseFunction, ExecutionExtension>::Result >
    {
        typedef SupportComponent< MakeAspect<KernelBaseFunction, ExecutionExtension>::Result > Base;
        bool start;
        Event<void(void)> startEvent;
        ExecutionExtension* ee;
        ConditionInterface* condition;
        CommandInterface*   command;
        const CommandFactoryInterface* tester;
        const DataSourceFactoryInterface* dataobject;
    public :
        HMIConsoleInput( ExecutionExtension* _ee = 0,  const std::string& name = "cin")
            : Base( name ), start(false),
              startEvent("HMIConsoleInput::StartEvent"),
              ee(_ee), condition(0), command(0), tester(0), dataobject(0) {}

        /**
         * @brief Call this method from ORO_main() to 
         * process keyboard input.
         */
        void loop()
        {
            cout << endl<<
"  This simple console reader can only accept start and stop \n\
  commands to start or stop the running program and  allows \n\
  to manually type in a Component command (type 'help' for info). \n\n\
  If the ReportingExtension is used, you can track the changes \n\
  in another terminal window."<<endl<<endl;
            while (1)
                {
                    cout << "Type 'start' or 'stop' to start/stop the current program-script " << endl;
                    if ( ee !=0 ) {
                        cout << " Or enter a command. (Status of previous command : ";
                        cout << (condition == 0 ? "none )" : condition->evaluate() == true ? "done )" : "busy )" );
                    }
                    cout << endl <<" (type 'quit' for exit) :" ;
                    std::string command;
                    getline(cin, command);
                    if ( command == "quit" ) {
                        stopButton();
                        return;
                    } else if ( command == "start") {
                        startButton();
                    } else if ( command == "stop") {
                        stopButton();
                    } else if ( command == "help") {
                        printHelp();
                    } else if ( command == "" ) { // nop
                    } else
                        evalCommand( command );
                    cout <<endl;
                }
        }

        void evalCommand(std::string& comm )
        {
            if ( ee == 0 )
                return;
//             if ( condition != 0 && condition->evaluate() == false )
//                 {
//                     cout << "Previous command not done yet !"<<endl;
//                     return;
//                 }
            if ( ee->isProgramRunning("Default") )
                {
                    cout << "A Program is running, not accepting commands  !"<<endl;
                    return;
                }
            cout << "      Got :"<< comm <<endl;

            tester = ee->commandFactory().getObjectFactory( comm );
            dataobject = ee->dataFactory().getObjectFactory( comm );
            if ( tester ) // only commandobject name was typed
                {
                    std::vector<std::string> methods = tester->getCommandList();
                    std::for_each( methods.begin(), methods.end(), boost::bind(&HMIConsoleInput::printMethod, this, _1) );
                }
                    
            if ( dataobject ) // only dataobject name was typed
                {
                    std::vector<std::string> methods = dataobject->getNames();
                    std::for_each( methods.begin(), methods.end(), boost::bind(&HMIConsoleInput::printSource, this, _1) );
                }
            if ( tester || dataobject )
                return;
                    
            delete command;
            delete condition;
            ORO_Execution::Parser _parser;
            std::pair< CommandInterface*, ConditionInterface*> comcon;
            try {
                comcon = _parser.parseCommand(comm, ee);
            } catch ( parse_exception& pe ) {
                cout << "Parse Error : Illegal command."<<endl;
                cout << pe.what() <<endl;
                return;
            }
            command = comcon.first;
            condition = comcon.second;
            if ( command == 0 ) {
                cout << "Parse Error : Illegal command."<<endl;
                return;
            }
            if ( ee->getProcessor()->process( command ) == false ) {
                cout << "Command not accepted by Processor ! " << endl;
                delete command;
                delete condition;
                command = 0;
                condition = 0;
                return;
            }
        }

        void printHelp()
        {
            using boost::lambda::_1;
            if (ee == 0){
                cout << "  The HMIConsoleInput object hasn't got a reference to the Execution"<<endl;
                cout <<"   Engine on construction. No Commands are available."<<endl;
                return;
            }
            cout << endl;
            cout << "  A command consists of an object, followed by a dot ('.'), the method "<<endl;
            cout << "  name, followed by the parameters. An example could be :"<<endl;
            cout << "  CartesianGenerator.moveTo( frame( vector( .75, .5, .8), rotation( 90., 0., 90. ) ), 15.0 ) [then press enter] "<<endl;
            cout << "  console_output.displayDouble( CartesianGenerator.currentTime ) [enter]" <<endl;
            cout << endl<<"  The available objects are :"<<endl;
            std::vector<std::string> objlist = ee->commandFactory().getObjectList();
            std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
            cout << "  The available DataSources are :"<<endl;
            objlist = ee->dataFactory().getObjectList();
            std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
            cout << "  For the Argument list of an object, just type the object name (eg 'kernel')" <<endl;
            cout <<endl;
            
        }
        
        void printMethod( const std::string m )
        {
            using boost::lambda::_1;
            std::vector<ArgumentDescription> args;
            args = tester->getArgumentList( m );
            cout << "  Method   : " << m <<endl;
            //int i = 0;
            if (args.begin() != args.end() ){
                cout << "  Argument :"<<endl;
            std::for_each( args.begin(), args.end(),
                           cout << &_1 ->* &ArgumentDescription::name << " - "
                           << &_1 ->* &ArgumentDescription::description << "\n" );
            }
            cout <<endl;
        }
                
        void printSource( const std::string m )
        {
            using boost::lambda::_1;
            std::vector<ArgumentDescription> args;
            args = dataobject->getArgumentList( m );
                cout << "  Source   : " << m <<endl;
            if (args.begin() != args.end() ){
                //int i = 0;
                cout << "  Argument :"<<endl;
                std::for_each( args.begin(), args.end(),
                               cout << &_1 ->* &ArgumentDescription::name << " - "
                               << &_1 ->* &ArgumentDescription::description << "\n" );
            }
            cout <<endl;
        }
                
        void startButton()
        {
            start = true;
            startEvent.fire();
        }

        void stopButton()
        {
            start = false;
        }

        bool startPushed() const
        {
            return start;
        }

        // The only data we export is the user's start input.
        DataSourceFactoryInterface* createDataSourceFactory()
        {
            TemplateDataSourceFactory< HMIConsoleInput >* ret =
                newDataSourceFactory( this );
            ret->add( "startPushed", 
                      data( &HMIConsoleInput::startPushed, "Is the start button pushed ? " ) );
            return ret;
        }

    };

}

#endif
