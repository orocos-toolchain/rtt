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
 

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExtensionComposition.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include <execution/TemplateFactories.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#include <execution/Parser.hpp>
#include <execution/parse_exception.hpp>
#endif

#include <iostream>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>


#pragma interface

namespace ORO_ControlKernel
{
    using std::cout;
    using std::cerr;
    using std::cin;
    using std::endl;
    using boost::bind;
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    using namespace ORO_Execution;
#endif
    class ExecutionExtension;

    /**
     * @brief This component allows console input to be passed
     * to the program or state scripts. It can be referenced
     * as the 'console_output' component in these scripts.
     *
     * HMI == Human-Machine Interface
     * @ingroup kcomps kcomp_support
     */
    class HMIReadline
        : public  SupportComponent< MakeFacet<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                               , ExecutionExtension
#endif
                                               >::CommonBase >
    {
        typedef SupportComponent< MakeFacet<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                             , ExecutionExtension
#endif
                                             >::CommonBase > Base;

        bool start;
        Event<void(void)> startEvent;
        static ExecutionExtension* ee;

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        static TaskContext* taskcontext;
        ConditionInterface* condition;
        CommandInterface*   command;
        const CommandFactoryInterface* command_fact;
        const DataSourceFactoryInterface* datasource_fact;
        const MethodFactoryInterface* method_fact;
#endif

        std::string prompt;
        std::string coloron;
        std::string coloroff;

        /* A static variable for holding the line. */
        char *line_read;
        int lastc; // last command's number

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
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
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
                if ( taskcontext->commandFactory.getObjectFactory( comp ) ) {
                    // fill with new ones.
                    find_command( pos+1 );
                }
                if ( taskcontext->dataFactory.getObjectFactory( comp ) ) {
                    // fill with new ones.
                    find_datasource( pos+1 );
                }
                if ( taskcontext->methodFactory.getObjectFactory( comp ) ) {
                    // fill with new ones.
                    find_method( pos+1 );
                }
                return ; // completion on the methods
            }
            // no dot found, thus build completion list based on what we got :
            std::string comp( text, startpos );
            //cout << "Found2 : "<< comp << endl;

            // strip white spaces from comp
            while ( comp.find(" ") != std::string::npos )
                comp.replace( comp.find(" "),1,"" );

            std::vector<std::string> comps = taskcontext->commandFactory.getObjectList();
            for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
                if ( i->find( comp ) == 0  )
                    completes.push_back( *i+"." );
            }
            comps = taskcontext->dataFactory.getObjectList();
            for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
                if ( i->find( comp ) == 0  )
                    completes.push_back( *i+"." );
            }
            comps = taskcontext->methodFactory.getObjectList();
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
#endif
        }
        
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        static void find_command(std::string::size_type startpos)
        {
            std::string::size_type pos;
            if ( (pos = text.find( "(", startpos )) != std::string::npos ) {
                std::string _method( text, startpos, pos - 1); // remove "("
                //cout << "FoundMethod : "<< _method <<endl;

                // strip white spaces from method
                while ( _method.find(" ") != std::string::npos )
                    _method.replace( _method.find(" "),1,"" );

                // try if it is a command :
                if ( taskcontext->commandFactory.getObjectFactory( component )->hasCommand( _method ) ) {
                    // complete :
                    method = _method;
                    return; // nothing to do
                } else {
                    cerr << "Illegal method" << endl;
                }
            }
            // no brace found, thus build completion list :
            std::string _method( text, startpos );
            //cout << "FoundMethod2 : "<< _method << endl;
            
            // strip white spaces from _method
            while ( _method.find(" ") != std::string::npos )
                _method.replace( _method.find(" "),1,"" );

            std::vector<std::string> comps;
            comps = taskcontext->commandFactory.getObjectFactory( component)->getCommandList();
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
                if ( taskcontext->dataFactory.getObjectFactory( component )->hasMember( _datasource ) ) {
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
            comps = taskcontext->dataFactory.getObjectFactory(component)->getNames();
            for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
                if ( i->find( _datasource ) == 0  )
                    completes.push_back( component +"."+ *i );
            }
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
                if ( taskcontext->methodFactory.getObjectFactory( component )->hasMember( _method ) ) {
                    method = _method;
                }
            }
            // no brace found, thus build completion list :
            std::string _method( text, startpos );
            //cout << "FoundMethod2 : "<< _method << endl;
            
            // strip white spaces from _method
            while ( _method.find(" ") != std::string::npos )
                _method.replace( _method.find(" "),1,"" );

            std::vector<std::string> comps;
            comps = taskcontext->methodFactory.getObjectFactory(component)->getNames();
            for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
                if ( i->find( _method ) == 0  )
                    completes.push_back( component +"."+ *i );
            }
        }
#endif

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
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
              condition(0), command(0), command_fact(0), datasource_fact(0),
#endif
              prompt(" (type 'quit' for exit) :"),
              coloron("\033[1;34m"), coloroff("\033[0m"),
              line_read(0),
              lastc(0)
        {
            ee = _ee;
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
            taskcontext = ee->getTaskContext();
#endif
            rl_completion_append_character = '\0'; // avoid adding spaces
            rl_attempted_completion_function = &HMIReadline::orocos_hmi_completion;
        }

        ~HMIReadline() {
            if (line_read)
                {
                    free (line_read);
                }
        }


        /**
         * @brief Call this method from ORO_main() to 
         * process keyboard input.
         */
        void loop()
        {
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
            cout << endl<< coloron <<
"  This simple console reader can only accept start and stop \n\
  commands to start or stop the running program and  allows \n\
  to manually type in a Component command (type 'help' for info). \n\n\
  If the ReportingExtension is used, you can track the changes \n\
  in another terminal window by typing 'tail -f results.txt' ."<<endl<<endl;
            cout << "    TAB completion and HISTORY is available for Component commands" <<coloroff<<endl<<endl;
#else
            cout << endl<<
"  The HMIReadline Component was compiled without the ExecutionExtension \n\
   or without the Program Parser. You can only 'quit' the application."<<endl<<endl;
#endif
            while (1)
                {
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                    cout << "Type 'start' or 'stop' to start/stop the current program-script " << endl;
                    if ( ee !=0 ) {
                        cout << " Or enter a command. (Status of previous command : ";
                        cout << (condition == 0 ? "none )" : condition->evaluate() == true ? "done )" : "busy )" );
                    }
                    cout << endl;
#endif
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
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
//             if ( condition != 0 && condition->evaluate() == false )
//                 {
//                     cout << "Previous command not done yet !"<<endl;
//                     return;
//                 }
            if ( ee->isProgramRunning("Default") )
                {
                    cerr << "A Program is running, not accepting commands  !"<<endl;
                    return;
                }
            cout << "      Got :"<< comm <<endl;

            command_fact = taskcontext->commandFactory.getObjectFactory( comm );
            datasource_fact = taskcontext->dataFactory.getObjectFactory( comm );
            method_fact = taskcontext->methodFactory.getObjectFactory( comm );
            if ( command_fact ) // only commandobject name was typed
                {
                    std::vector<std::string> methods = command_fact->getCommandList();
                    std::for_each( methods.begin(), methods.end(), boost::bind(&HMIReadline::printCommand, this, _1) );
                }
                    
            if ( datasource_fact ) // only datasource_fact name was typed
                {
                    std::vector<std::string> methods = datasource_fact->getNames();
                    std::for_each( methods.begin(), methods.end(), boost::bind(&HMIReadline::printSource, this, _1) );
                }
            if ( method_fact ) // only method_fact name was typed
                {
                    std::vector<std::string> methods = method_fact->getNames();
                    std::for_each( methods.begin(), methods.end(), boost::bind(&HMIReadline::printMethod, this, _1) );
                }
            if ( command_fact || datasource_fact || method_fact )
                return;
                    
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
            if ( command == 0 ) { // this should not be reached
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
            if (dsb) {
                cout <<prompt<< (dsb->get() ? "true" : "false") <<endl;
                return;
            }
            DataSource<int>* dsi = dynamic_cast<DataSource<int>*>(ds);
            if (dsi) {
                cout <<prompt<< dsi->get() <<endl;
                return;
            }
            DataSource<std::string>* dss = dynamic_cast<DataSource<std::string>*>(ds);
            if (dss) {
                cout <<prompt<< dss->get() <<endl;
                return;
            }
            DataSource<double>* dsd = dynamic_cast<DataSource<double>*>(ds);
            if (dsd) {
                cout <<prompt<< dsd->get() <<endl;
                return;
            }
            DataSource<char>* dsc = dynamic_cast<DataSource<char>*>(ds);
            if (dsc) {
                cout <<prompt<< dsc->get() <<endl;
                return;
            }
            if (ds)
                ds->evaluate();
        }
#endif
        void printHelp()
        {
            using boost::lambda::_1;
            if (ee == 0){
                cerr <<coloron<< "  The HMIReadline object hasn't got a reference to the Execution"<<endl;
                cerr <<"   Engine on construction. No Commands are available."<<endl<< coloroff;
                return;
            }
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION

            cout << endl<< coloron;
            cout << "  A command consists of an object, followed by a dot ('.'), the method "<<endl;
            cout << "  name, followed by the parameters. An example could be :"<<endl;
            cout << "  CartesianGenerator.moveTo( frame( vector( .75, .5, .8), rotation( 90., 0., 90. ) ), 15.0 ) [then press enter] "<<endl;
            cout << "  cout.displayDouble( CartesianGenerator.currentTime ) [enter]" <<endl << coloroff;
            cout << endl<<"  The available Components are :"<<endl;
            std::vector<std::string> objlist = taskcontext->commandFactory.getObjectList();
            std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
            cout << "  The available DataSources are :"<<endl;
            objlist = taskcontext->dataFactory.getObjectList();
            std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
            cout << "  The available Methods are :"<<endl;
            objlist = taskcontext->methodFactory.getObjectList();
            std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
            cout << coloron <<"  For the Argument list of an object, just type the object name (eg 'kernel')" <<endl<< coloroff;
            cout <<endl;
#endif            
        }
        
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        void printCommand( const std::string m )
        {
            using boost::lambda::_1;
            std::vector<ArgumentDescription> args;
            args = command_fact->getArgumentList( m );
            cout <<coloron<< "  Command     : " << coloroff << m <<coloron<< " - ";
            cout << command_fact->getDescription(m) <<coloroff<<endl;
            //int i = 0;
            if (args.begin() != args.end() ){
                for (std::vector<ArgumentDescription>::iterator it = args.begin(); it != args.end(); ++it) {
                    cout <<coloron<< "  Argument "<< (it - args.begin()) + 1<<" : " <<coloroff;
                    cout << it->name << coloron << " - " << it->description <<coloroff<< endl;
                }
            }
            cout <<endl;
        }
                
        void printSource( const std::string m )
        {
            using boost::lambda::_1;
            std::vector<ArgumentDescription> args;
            args = datasource_fact->getArgumentList( m );
            cout <<coloron << "  Source     : " << coloroff << m << coloron<< " - ";
            cout << datasource_fact->getDescription( m )<<coloroff<<endl;
            if (args.begin() != args.end() ){
                for (std::vector<ArgumentDescription>::iterator it = args.begin(); it != args.end(); ++it) {
                    cout <<coloron<< "  Argument "<< (it - args.begin()) + 1<<" : " <<coloroff;
                    cout << it->name << coloron << " - " << it->description <<coloroff<< endl;
                }
            }
            cout <<endl;
        }
                
        void printMethod( const std::string m )
        {
            using boost::lambda::_1;
            std::vector<ArgumentDescription> args;
            args = method_fact->getArgumentList( m );
            cout <<coloron << "  Method     : " << coloroff << m << coloron<< " - ";
            cout << method_fact->getDescription( m )<<coloroff<<endl;
            if (args.begin() != args.end() ){
                for (std::vector<ArgumentDescription>::iterator it = args.begin(); it != args.end(); ++it) {
                    cout <<coloron<< "  Argument "<< (it - args.begin()) + 1<<" : " <<coloroff;
                    cout << it->name << coloron << " - " << it->description <<coloroff<< endl;
                }
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
            TemplateDataSourceFactory< HMIReadline >* ret =
                newDataSourceFactory( this );
            ret->add( "startPushed", 
                      data( &HMIReadline::startPushed, "Is the start button pushed ? " ) );
            return ret;
        }

        MethodFactoryInterface* createMethodFactory()
        {
            TemplateMethodFactory< HMIReadline >* ret =
                newMethodFactory( this );
            ret->add( "startButton", 
                      ORO_Execution::method( &HMIReadline::startButton, "Push the start button." ) );
            ret->add( "stopButton", 
                      ORO_Execution::method( &HMIReadline::stopButton, "Push the stop button." ) );
            return ret;
        }
#endif

    };

}

#endif
