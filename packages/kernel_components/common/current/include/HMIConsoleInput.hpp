/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  HMIConsoleInput.hpp 

                        HMIConsoleInput.hpp -  description
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
 
 

#include <execution/TemplateCommandFactory.hpp>
#include <execution/TemplateDataSourceFactory.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#include <execution/Parser.hpp>
#include <boost/lambda/lambda.hpp>

#include <iostream>
#include <stdio.h> // how do you do getchar with cin ??

namespace ORO_ControlKernel
{
    using std::cout;
    using std::cin;
    using std::endl;
    //using namespace boost;
    //using namespace boost::lambda;

    /**
     * @brief This component allows console input to be passed
     * to the program or state scripts. It can be referenced
     * as the 'console_output' component in these scripts.
     *
     * HMI == Human-Machine Interface
     * @ingroup kcomps kcomp_support
     */
    template< class Base>
    class HMIConsoleInput
        : public Base
    {
        bool start;
        Event startEvent;
        ExecutionExtension* ee;
        ConditionInterface* condition;
        CommandInterface*   command;
        const CommandFactoryInterface* tester;
    public :
        HMIConsoleInput( ExecutionExtension* _ee = 0)
            : Base("console_input"), start(false),
              startEvent(Event::SYNASYN,"HMIConsoleInput::StartEvent"),
              ee(_ee), condition(0), command(0), tester(0) {}

        /**
         * @brief Call this method from ORO_main() to 
         * process keyboard input.
         */
        void loop()
        {
            char c;
            cout << endl<< "This simple console reader can only accept"<<endl;
            cout << "start and stop commands to start or stop the running program"<<endl;
            while (1)
                {
                    cout << "Press s to start or p to stop the current program-script " << endl;
                    if ( ee !=0 ) {
                        cout << " Or press c to enter a command. (Status of previous command : ";
                        cout << (condition == 0 || condition->evaluate() == true ? "done )" : "busy )" );
                    }
                    cout << endl <<" (press q to quit) :" ;
                    c = getchar();
                    cin.ignore(); // remove cr
                    switch (c)
                        {
                        case 'c':
                            evalCommand();
                            break;
                        case 's':
                            startButton();
                            break;
                        case 'p':
                            stopButton();
                            break;
                        case 'q':
                            stopButton();
                            return;
                        }
                    cout <<endl;
                }
        }

        void evalCommand()
        {
            if ( ee == 0 )
                return;
            if ( condition != 0 && condition->evaluate() == false )
                {
                    cout << "Previous command not done yet !"<<endl;
                    return;
                }
            if ( ee->isProgramRunning("Default") )
                {
                    cout << "A Program is running, not accepting commands  !"<<endl;
                    return;
                }
            delete command;
            delete condition;
            std::string comm;
            cout << "Enter your command ( type help for instructions ):";
            getline(cin,comm);
            cout << "      Got :"<< comm <<endl;
            if ( comm == "help" ) {
                printHelp();
                return;
            }

            tester = ee->commandFactory().getObjectFactory( comm );
            if ( tester ) // only object name was typed
                {
                    std::vector<std::string> methods = tester->getMethodList();
                    std::for_each( methods.begin(), methods.end(), boost::bind(&HMIConsoleInput::printMethod, this, _1) );
                    return;
                }
                    
            ORO_Execution::Parser _parser;
            std::pair< CommandInterface*, ConditionInterface*> comcon;
            comcon = _parser.parseCommand(comm, ee);
            command = comcon.first;
            condition = comcon.second;
            if ( command == 0 || condition == 0 ) {
                cout << "Illegal command."<<endl;
                return;
            }
            if ( ee->getProcessor()->process( command ) == false ) {
                cout << "Command not accepted ! " << endl;
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
            cout << "A command consists of an object, followed by a dot ('.'), the method "<<endl;
            cout << "name, followed by the parameters. An example could be :"<<endl;
            cout << "cart_generator.moveTo( 0.75, 0.5, 0.8, 90, 0, 90 ) [then press enter] "<<endl;
            cout << "The available objects are :"<<endl;
            std::vector<std::string> objlist = ee->commandFactory().getObjectList();
            std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
            cout << "For the Argument list of an object, just type the object name" <<endl;
        }
        
        void printMethod( const std::string m )
        {
            using boost::lambda::_1;
            vector<ArgumentDescription> args;
            args = tester->getArgumentList( m );
            cout << "Method   : " << m <<endl;
            //int i = 0;
            std::for_each( args.begin(), args.end(),
                           cout <<"Argument : " << &_1 ->* &ArgumentDescription::name << " - "
                           << &_1 ->* &ArgumentDescription::description << "\n" );
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
        DataSourceFactory* createDataSourceFactory()
        {
            TemplateDataSourceFactory< HMIConsoleInput<Base> >* ret =
                newDataSourceFactory( this );
            ret->add( "startPushed", 
                      data( &HMIConsoleInput<Base>::startPushed, "Is the start button pushed ? " ) );
            return ret;
        }

    };
}
