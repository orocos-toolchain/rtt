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
 

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <boost/lambda/lambda.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include <execution/TemplateFactories.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#include <execution/Parser.hpp>
#endif

#include <iostream>
#include <stdio.h> // how do you do getchar with cin ??

#pragma interface

namespace ORO_ControlKernel
{
    using std::cout;
    using std::cerr;
    using std::cin;
    using std::endl;
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    using namespace ORO_Execution;
#endif

    class ExecutionExtension;

    /**
     * @brief This component allows console input to be passed
     * to the program or state scripts. It can be referenced
     * as the 'cin' component in these scripts.
     *
     * HMI == Human-Machine Interface
     * @ingroup kcomps kcomp_support
     * @deprecated by HMIReadline
     */
    class HMIConsoleInput
        : public SupportComponent< MakeAspect<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                              , ExecutionExtension
#endif
                                              >::Result >
    {
        typedef SupportComponent< MakeAspect<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                             , ExecutionExtension
#endif
                                             >::Result > Base;
        bool start;
        Event<void(void)> startEvent;
        ExecutionExtension* ee;
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        ConditionInterface* condition;
        CommandInterface*   command;
        const CommandFactoryInterface* tester;
        const DataSourceFactoryInterface* dataobject;
        int lastc; // last command's number
#endif
    public :
        HMIConsoleInput( ExecutionExtension* _ee = 0,  const std::string& name = "cin")
            : Base( name ), start(false),
              startEvent("HMIConsoleInput::StartEvent"),
              ee(_ee)
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
              , condition(0), command(0), tester(0), dataobject(0), lastc(0)
#endif
        {}

        /**
         * @brief Call this method from ORO_main() to 
         * process keyboard input.
         */
        void loop()
        {
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
            cout << endl<<
"  This simple console reader can only accept start and stop \n\
  commands to start or stop the running program and  allows \n\
  to manually type in a Component command (type 'help' for info). \n\n\
  If the ReportingExtension is used, you can track the changes \n\
  in another terminal window."<<endl<<endl;
#else
            cout << endl<<
"  The HMIConsoleInput Component was compiled without the ExecutionExtension \n\
   or without the Program Parser. You can only quit the application."<<endl<<endl;
#endif
            while (1)
                {
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                    cout << "Type 'start' or 'stop' to start/stop the current program-script " << endl;
                    if ( ee !=0 ) {
                        cout << " Or enter a command. (Status of previous command : ";
                        cout << (condition == 0 ? "none )" : condition->evaluate() == true ? "done )" : "busy )" );
                    }
#endif
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
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION

//             if ( condition != 0 && condition->evaluate() == false )
//                 {
//                     cout << "Previous command not done yet !"<<endl;
//                     return;
//                 }
//             if ( ee->isProgramRunning("Default") )
//                 {
//                     cout << "A Program is running, not accepting commands  !"<<endl;
//                     return;
//                 }
            cout << "      Got :"<< comm <<endl;

            tester = ee->getTaskContext()->commandFactory.getObjectFactory( comm );
            dataobject = ee->getTaskContext()->dataFactory.getObjectFactory( comm );
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
                return; // we are done.
                    
            if ( ee->getTaskContext()->getProcessor()->isProcessed( lastc ) ) {
                delete command;
                delete condition;
            } else
                cerr << "Warning : previous command is not yet processed." <<endl;

            ORO_Execution::Parser _parser;
            std::pair< CommandInterface*, ConditionInterface*> comcon;
            try {
                DataSourceBase::shared_ptr ds = _parser.parseExpression( comm, ee->getTaskContext() );
                if ( ds.get() != 0 )
                    this->printResult( ds.get() );
                return; // done here
            } catch ( fatal_syntactic_parse_exception& pe ) {
                // ignore it, try to parse it as a command :
                try {
                    comcon = _parser.parseCommand(comm, ee->getTaskContext() );
                } catch ( parse_exception& pe ) {
                    cerr << "Parse Error : Illegal command."<<endl;
                    cerr << pe.what() <<endl;
                    command = 0;
                    condition = 0;
                    return;
                }
            } catch ( parse_exception& pe ) {
                cerr << "Parse Error : Invalid Expression."<<endl;
                cerr << pe.what() <<endl;
            } catch ( ... ) {
                cerr << "Illegal Command."<<endl;
            }

            command = comcon.first;
            condition = comcon.second;
            if ( command == 0 ) {
                cerr << "Parse Error : Illegal command."<<endl;
                return;
            }
            lastc = ee->getTaskContext()->getProcessor()->process( command );
            if ( lastc == 0 && ee->getTaskContext()->executeCommand(command) == false ) {
                cerr << "Command not accepted by Processor ! " << endl;
                delete command;
                delete condition;
                command = 0;
                condition = 0;
                return;
            }
#endif
        }

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        void printResult( DataSourceBase* ds) {
            std::string prompt("   = ");
            // this method can print some primitive DataSource<>'s.
            DataSource<bool>* dsb = dynamic_cast<DataSource<bool>*>(ds);
            if (dsb)
                cout <<prompt<< dsb->get() <<endl;
            DataSource<int>* dsi = dynamic_cast<DataSource<int>*>(ds);
            if (dsi)
                cout <<prompt<< dsi->get() <<endl;
            DataSource<std::string>* dss = dynamic_cast<DataSource<std::string>*>(ds);
            if (dss)
                cout <<prompt<< dss->get() <<endl;
            DataSource<double>* dsd = dynamic_cast<DataSource<double>*>(ds);
            if (dsd)
                cout <<prompt<< dsd->get() <<endl;
            DataSource<char>* dsc = dynamic_cast<DataSource<char>*>(ds);
            if (dsc)
                cout <<prompt<< dsc->get() <<endl;
        }
#endif

        void printHelp()
        {
            using boost::lambda::_1;
            if (ee == 0){
                cerr << "  The HMIConsoleInput object hasn't got a reference to the Execution"<<endl;
                cerr <<"   Engine on construction. No Commands are available."<<endl;
                return;
            }
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
            cout << endl;
            cout << "  A command consists of an object, followed by a dot ('.'), the method "<<endl;
            cout << "  name, followed by the parameters. An example could be :"<<endl;
            cout << "  CartesianGenerator.moveTo( frame( vector( .75, .5, .8), rotation( 90., 0., 90. ) ), 15.0 ) [then press enter] "<<endl;
            cout << "  console_output.displayDouble( CartesianGenerator.currentTime ) [enter]" <<endl;
            cout << endl<<"  The available objects are :"<<endl;
            std::vector<std::string> objlist = ee->getTaskContext()->commandFactory.getObjectList();
            std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
            cout << "  The available DataSources are :"<<endl;
            objlist = ee->getTaskContext()->dataFactory.getObjectList();
            std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
            cout << "  For the Argument list of an object, just type the object name (eg 'kernel')" <<endl;
            cout <<endl;
#endif            
        }
        
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
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
#endif
                
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

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        // The only data we export is the user's start input.
        DataSourceFactoryInterface* createDataSourceFactory()
        {
            TemplateDataSourceFactory< HMIConsoleInput >* ret =
                newDataSourceFactory( this );
            ret->add( "startPushed", 
                      data( &HMIConsoleInput::startPushed, "Is the start button pushed ? " ) );
            return ret;
        }
#endif

    };

}

#endif
