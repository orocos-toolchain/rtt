/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:23 CEST 2004  HMIReadline.hpp 

                        HMIReadline.hpp -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
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
#ifndef HMIREADLINE_HPP
#define HMIREADLINE_HPP
 

#include <execution/TemplateCommandFactory.hpp>
#include <execution/TemplateDataSourceFactory.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <execution/Parser.hpp>
#include <execution/parse_exception.hpp>
#include <boost/lambda/lambda.hpp>

#include <iostream>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/bind.hpp>

#pragma interface

namespace ORO_ControlKernel
{
    using std::cout;
    using std::cin;
    using std::endl;
    using boost::bind;
    using namespace ORO_Execution;

    /**
     * @brief This component allows console input to be passed
     * to the program or state scripts. It can be referenced
     * as the 'console_output' component in these scripts.
     *
     * HMI == Human-Machine Interface
     * @ingroup kcomps kcomp_support
     */
#if 1
    class HMIReadline
        : public  SupportComponent< MakeAspect<KernelBaseFunction, ExecutionExtension>::CommonBase >
    {
        typedef SupportComponent< MakeAspect<KernelBaseFunction, ExecutionExtension>::CommonBase > Base;
#else
    template<class _Base=SupportComponent< MakeAspect<KernelBaseFunction, ExecutionExtension>::CommonBase > >
    class HMIReadline
        : public _Base
    {
        typedef _Base Base;
#endif

        bool start;
        Event<void(void)> startEvent;
        static ExecutionExtension* ee;
        ConditionInterface* condition;
        CommandInterface*   command;
        const CommandFactoryInterface* command_fact;
        const DataSourceFactoryInterface* datasource_fact;

        std::string prompt;
        std::string coloron;
        std::string coloroff;

        /* A static variable for holding the line. */
        char *line_read;

        /* Read a string, and return a pointer to it.
           Returns NULL on EOF. */
        char *rl_gets ()
        {
            /* If the buffer has already been allocated,
               return the memory to the free pool. */
            if (line_read)
                {
                    free (line_read);
                    line_read = 0;
                }

            /* Get a line from the user. */
            line_read = readline ( prompt.c_str() );

            /* If the line has any text in it,
               save it on the history. */
            if (line_read && *line_read)
                add_history (line_read);

            return (line_read);
        }

        static std::vector<std::string> completes;
        static std::vector<std::string>::iterator complete_iter;

        static std::string component;
        static std::string method;
        static std::string datasource;
        static std::string text;

        // helper function
        static char* dupstr( const char *s )
        {
            char * rv;
            // must be C-style :
            rv = (char*) malloc( strlen( s ) + 1 );
            strcpy( rv, s );
            return rv;
        }

        static char *command_generator( const char *_text, int state )
        {
            // first time called.
            if ( !state )
                {
                    // make a copy :
                    text = _text;
                    // rebuild a completion list
                    completes.clear();
                    find_completes();
                    complete_iter = completes.begin();
                }
            else
                ++complete_iter;
            
            // return zero if nothing more is found
            if ( complete_iter == completes.end() )
                return 0;
            // return the next completion option
            return  dupstr( complete_iter->c_str() ); // malloc !
        }

        static void find_completes() {
            std::string::size_type pos;
            std::string::size_type startpos;

            startpos = text.find_last_of(",( ");
            if ( startpos == std::string::npos )
                startpos = 0;      // if none is found, we are at the beginning of the line.
            pos = text.rfind( "." );
                
            if ( pos != std::string::npos && pos > startpos) {
                // store the compname
                std::string comp(text, startpos, pos);
                //cout << "Found : "<< comp << endl;

                // strip white spaces from comp
                while ( comp.find(" ") != std::string::npos )
                    comp.replace( comp.find(" "),1,"" );

                component = comp;
                // try if it is a component :
                if ( ee->commandFactory().getObjectFactory( comp ) ) {
                    // fill with new ones.
                    find_method( pos+1 );
                }
                if ( ee->dataFactory().factory( comp ) ) {
                    // fill with new ones.
                    find_datasource( pos+1 );
                }
                return ; // completion on the methods
            }
            // no dot found, thus build completion list based on what we got :
            std::string comp( text, startpos );
            //cout << "Found2 : "<< comp << endl;

            // strip white spaces from comp
            while ( comp.find(" ") != std::string::npos )
                comp.replace( comp.find(" "),1,"" );

            std::vector<std::string> comps = ee->commandFactory().getObjectList();
            for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
                if ( i->find( comp ) == 0  )
                    completes.push_back( *i+"." );
            }
            comps = ee->dataFactory().getObjectList();
            for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
                if ( i->find( comp ) == 0  )
                    completes.push_back( *i+"." );
            }
            if ( std::string( "start" ).find(comp) == 0 )
                completes.push_back("start");
            if ( std::string( "stop" ).find(comp) == 0 )
                completes.push_back("stop");
            if ( std::string( "help" ).find(comp) == 0 )
                completes.push_back("help");
            if ( std::string( "quit" ).find(comp) == 0 )
                completes.push_back("quit");
        }
        
        static void find_method(std::string::size_type startpos)
        {
            std::string::size_type pos;
            if ( (pos = text.find( "(", startpos )) != std::string::npos ) {
                std::string _method( text, startpos, pos - 1); // remove "("
                //cout << "FoundMethod : "<< _method <<endl;

                // strip white spaces from method
                while ( _method.find(" ") != std::string::npos )
                    _method.replace( _method.find(" "),1,"" );

                // try if it is a command :
                if ( ee->commandFactory().getObjectFactory( component )->hasCommand( _method ) ) {
                    // complete :
                    method = _method;
                    return; // nothing to do
                } else {
                    cout << "Illegal method" << endl;
                }
            }
            // no brace found, thus build completion list :
            std::string _method( text, startpos );
            //cout << "FoundMethod2 : "<< _method << endl;
            
            // strip white spaces from _method
            while ( _method.find(" ") != std::string::npos )
                _method.replace( _method.find(" "),1,"" );

            std::vector<std::string> comps;
            comps = ee->commandFactory().getObjectFactory( component)->getMethodList();
            for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
                if ( i->find( _method ) == 0  )
                    completes.push_back( component +"."+ *i );
            }
        }

        static void find_datasource(std::string::size_type startpos)
        {
            std::string::size_type pos;
            if ( (pos = text.find( "(", startpos )) != std::string::npos ) {
                std::string _datasource( text, startpos, pos - 1); // remove "("
                //cout << "FoundDatasource : "<< _datasource <<endl;

                // strip white spaces from datasource
                while ( _datasource.find(" ") != std::string::npos )
                    _datasource.replace( _datasource.find(" "),1,"" );

                // try if it is a command :
                if ( ee->dataFactory().factory( component )->hasData( _datasource ) ) {
                    datasource = _datasource;
                }
            }
            // no brace found, thus build completion list :
            std::string _datasource( text, startpos );
            //cout << "FoundDatasource2 : "<< _datasource << endl;
            
            // strip white spaces from _datasource
            while ( _datasource.find(" ") != std::string::npos )
                _datasource.replace( _datasource.find(" "),1,"" );

            std::vector<std::string> comps;
            comps = ee->dataFactory().factory(component)->dataNames();
            for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
                if ( i->find( _datasource ) == 0  )
                    completes.push_back( component +"."+ *i );
            }
        }

        static char ** orocos_hmi_completion ( const char *text, int start, int end )
        {
            char **matches;
            matches = ( char ** ) 0;

            matches = rl_completion_matches ( text, &HMIReadline::command_generator );
            
            return ( matches );
        }

    public :
        HMIReadline( ExecutionExtension* _ee = 0, const std::string& name = "cin")
            : Base( name ), start(false),
              startEvent("HMIReadline::StartEvent"),
              condition(0), command(0), command_fact(0), datasource_fact(0),
              prompt(" (type 'quit' for exit) :"),
              coloron("\033[1;34m"), coloroff("\033[0m"),
              line_read(0)
        {
            ee = _ee;
            rl_completion_append_character = '\0'; // avoid adding spaces
            rl_attempted_completion_function = &HMIReadline::orocos_hmi_completion;
        }

        /**
         * @brief Call this method from ORO_main() to 
         * process keyboard input.
         */
        void loop()
        {
            cout << endl<< coloron <<
"  This simple console reader can only accept start and stop \n\
  commands to start or stop the running program and  allows \n\
  to manually type in a Component command (type 'help' for info). \n\n\
  If the ReportingExtension is used, you can track the changes \n\
  in another terminal window by typing 'tail -f results.txt' ."<<endl<<endl;
            cout << "    TAB completion and HISTORY is available for Component commands" <<coloroff<<endl<<endl;
            while (1)
                {
                    cout << "Type 'start' or 'stop' to start/stop the current program-script " << endl;
                    if ( ee !=0 ) {
                        cout << " Or enter a command. (Status of previous command : ";
                        cout << (condition == 0 ? "none )" : condition->evaluate() == true ? "done )" : "busy )" );
                    }
                    cout << endl;
                    // Call readline wrapper :
                    std::string command( rl_gets() ); // copy over to string
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

            command_fact = ee->commandFactory().getObjectFactory( comm );
            datasource_fact = ee->dataFactory().factory( comm );
            if ( command_fact ) // only commandobject name was typed
                {
                    std::vector<std::string> methods = command_fact->getMethodList();
                    std::for_each( methods.begin(), methods.end(), boost::bind(&HMIReadline::printMethod, this, _1) );
                }
                    
            if ( datasource_fact ) // only datasource_fact name was typed
                {
                    std::vector<std::string> methods = datasource_fact->dataNames();
                    std::for_each( methods.begin(), methods.end(), boost::bind(&HMIReadline::printSource, this, _1) );
                }
            if ( command_fact || datasource_fact )
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
            if ( command == 0 ) { // this should not be reached
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
                cout <<coloron<< "  The HMIReadline object hasn't got a reference to the Execution"<<endl;
                cout <<"   Engine on construction. No Commands are available."<<endl<< coloroff;
                return;
            }
            cout << endl<< coloron;
            cout << "  A command consists of an object, followed by a dot ('.'), the method "<<endl;
            cout << "  name, followed by the parameters. An example could be :"<<endl;
            cout << "  CartesianGenerator.moveTo( frame( vector( .75, .5, .8), rotation( 90., 0., 90. ) ), 15.0 ) [then press enter] "<<endl;
            cout << "  cout.displayDouble( CartesianGenerator.currentTime ) [enter]" <<endl << coloroff;
            cout << endl<<"  The available Components are :"<<endl;
            std::vector<std::string> objlist = ee->commandFactory().getObjectList();
            std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
            cout << "  The available DataSources are :"<<endl;
            objlist = ee->dataFactory().getObjectList();
            std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
            cout << coloron <<"  For the Argument list of an object, just type the object name (eg 'kernel')" <<endl<< coloroff;
            cout <<endl;
            
        }
        
        void printMethod( const std::string m )
        {
            using boost::lambda::_1;
            std::vector<ArgumentDescription> args;
            args = command_fact->getArgumentList( m );
            cout <<coloron<< "  Method   : " << coloroff << m <<endl;
            //int i = 0;
            if (args.begin() != args.end() ){
                cout <<coloron<< "  Argument :"<<coloroff<<endl;
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
            args = datasource_fact->getArgumentList( m );
            cout <<coloron << "  Source   : " << coloroff << m <<endl;
            if (args.begin() != args.end() ){
                //int i = 0;
                cout <<coloron<< "  Argument :"<< coloroff<<endl;
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

#if 1
        // The only data we export is the user's start input.
        DataSourceFactoryInterface* createDataSourceFactory()
        {
            TemplateDataSourceFactory< HMIReadline >* ret =
                newDataSourceFactory( this );
            ret->add( "startPushed", 
                      data( &HMIReadline::startPushed, "Is the start button pushed ? " ) );
            return ret;
        }
#endif

    };

#if 0
        template<class B>
    std::vector<std::string> HMIReadline<B>::completes;
        template<class B>
    std::vector<std::string>::iterator HMIReadline<B>::complete_iter;
        template<class B>
    std::string HMIReadline<B>::component;
        template<class B>
    std::string HMIReadline<B>::method;
        template<class B>
    std::string HMIReadline<B>::datasource;
        template<class B>
    std::string HMIReadline<B>::text;
        template<class B>
    ExecutionExtension* HMIReadline<B>::ee = 0;
#endif
}

#endif
