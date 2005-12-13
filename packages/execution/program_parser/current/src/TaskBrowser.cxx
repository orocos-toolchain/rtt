/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  TaskBrowser.cxx 

                        TaskBrowser.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include <corelib/Logger.hpp>
#include <corelib/MultiVector.hpp>
#include <corelib/TypeStream.hpp>
#include "execution/TaskBrowser.hpp"

#include "execution/TryCommand.hpp"
#include <execution/TemplateFactories.hpp>
#include <execution/TaskContext.hpp>
#include <execution/Parser.hpp>
#include <execution/ProgramLoader.hpp>
#include <execution/parse_exception.hpp>
#include <execution/PeerParser.hpp>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <deque>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
#include <geometry/frames.h>
#include <geometry/frames_io.h>
using namespace ORO_Geometry;
#endif

#include <signal.h>

namespace ORO_Execution
{
    using namespace detail;
    std::vector<std::string> TaskBrowser::candidates;
    std::vector<std::string> TaskBrowser::completes;
    std::vector<std::string>::iterator TaskBrowser::complete_iter;
    std::string TaskBrowser::component;
    std::string TaskBrowser::object;
    std::string TaskBrowser::peerpath;
    std::string TaskBrowser::method;
    std::string TaskBrowser::datasource;
    std::string TaskBrowser::text;
    TaskContext* TaskBrowser::taskcontext = 0;
    TaskContext* TaskBrowser::peer = 0;

    using boost::bind;
    using namespace ORO_CoreLib;
    using namespace std;

    /**
     * Our own defined "\n"
     */
    static std::ostream&
    nl(std::ostream& __os)
    { return __os.put(__os.widen('\n')); }

    // catch ctrl+c signal
    void ctrl_c_catcher(int sig)
    {
        signal(sig, SIG_IGN);
        cerr <<nl<<"TaskBrowser intercepted Ctrl-C. Type 'quit' to exit."<<endl;
        signal(SIGINT, ctrl_c_catcher);
    }

    char *TaskBrowser::rl_gets ()
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

    char* TaskBrowser::dupstr( const char *s )
    {
        char * rv;
        // must be C-style :
        rv = (char*) malloc( strlen( s ) + 1 );
        strcpy( rv, s );
        return rv;
    }

    char *TaskBrowser::command_generator( const char *_text, int state )
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
        return  dupstr( complete_iter->c_str() ); // RL requires malloc !
    }

    void TaskBrowser::find_completes() {
        std::string::size_type pos;
        std::string::size_type startpos;
        std::string line( rl_line_buffer );

        // complete on 'cd' or 'ls' :
        if ( line.find(std::string("cd ")) == 0 || line.find(std::string("ls ")) == 0) { 
            //cerr <<endl<< "switch to :" << text<<endl;
//             pos = text.rfind(".");
            pos = line.find(" ");      // pos+1 is first peername
            startpos = line.find_last_of(". "); // find last peer
            //cerr <<"startpos :"<<startpos<<endl;
            TaskContext* peer;
            if ( pos+1 != line.length() ) // bounds check
                peer = findPeer( line.substr(pos+1) );
            else
                peer = taskcontext;
            //std::string peername = text.substring( pos+1, std::string::npos );
            TaskContext::PeerList v = peer->getPeerList();
            for (TaskContext::PeerList::iterator i = v.begin(); i != v.end(); ++i) {
                std::string path;
                if ( !( pos+1 > startpos) )
                    path = line.substr(pos+1, startpos - pos);
                //cerr << "path :"<<path<<endl;
                if ( startpos == std::string::npos || startpos+1 == line.length() || i->find( line.substr(startpos+1)) == 0 )
                     completes.push_back( path + *i + "." );
            }
            return; // do not add component names.
        }

        // TaskBrowser commands :
        if ( line.find(std::string(".")) == 0 ) { 
            // first make a list of all sensible completions.
            std::vector<std::string> tbcoms;
            tbcoms.push_back(".loadProgram "); 
            tbcoms.push_back(".unloadProgram "); 
            tbcoms.push_back(".loadStateMachine "); 
            tbcoms.push_back(".unloadStateMachine "); 

            // then see which one matches the already typed line :
            for( std::vector<std::string>::iterator it = tbcoms.begin();
                 it != tbcoms.end();
                 ++it)
                if ( it->find(line) == 0 )
                    completes.push_back( *it ); // if partial match, add.
            return;
        }

        if ( line.find(std::string("list ")) == 0 ) { 
            // first make a list of all sensible completions.
            std::vector<std::string> progs = taskcontext->getExecutionEngine()->getProgramProcessor()->getProgramList();
            // then see which one matches the already typed line :
            for( std::vector<std::string>::iterator it = progs.begin();
                 it != progs.end();
                 ++it) {
                string res = "list " + *it;
                if ( res.find(line) == 0 )
                    completes.push_back( *it ); // if partial match, add.
            }
            progs = taskcontext->getExecutionEngine()->getStateMachineProcessor()->getStateMachineList();
            for( std::vector<std::string>::iterator it = progs.begin();
                 it != progs.end();
                 ++it) {
                string res = "list " + *it;
                if ( res.find(line) == 0 )
                    completes.push_back( *it ); // if partial match, add.
            }
            return;
        }

        // check if the user is tabbing on an empty command, then add the console commands :
        if (  line.empty() ) {
            completes.push_back("cd "); 
            completes.push_back("cd ..");
            completes.push_back("ls"); 
            completes.push_back("peers");
            completes.push_back("help");
            completes.push_back("quit");
            completes.push_back("list");
            // go on below to add components and tasks as well.
        }

        startpos = text.find_last_of(",( ");
        if ( startpos == std::string::npos )
            startpos = 0;      // if none is found, start at beginning

        // complete on peers, and find the peer the user wants completion for
        find_peers(startpos);
        // now proceed with 'this->peer' as TC and component as object :

        // locate the last dot :
        startpos = text.rfind(".");
        if ( startpos == std::string::npos )
            startpos = 0;      // if none is found, start at beginning
        else if (startpos  != text.length() )
            startpos++;        // skip dot, if possible.
        else
            startpos = std::string::npos; // empty 'method'

        // store the partial compname or peername
        std::string comp = component;
        //cout << "Found : "<< comp << endl;

            // strip white spaces from comp
//             while ( comp.find(" ") != std::string::npos )
//                 comp.replace( comp.find(" "),1,"" );

        // try if it is an object :
        if ( peer->commandFactory.getObjectFactory( object ) ) {
            // fill with new ones.
            find_command( startpos );
        }
        if ( peer->dataFactory.getObjectFactory( object ) ) {
            // fill with new ones.
            find_datasource( startpos );
        }
        if ( peer->methodFactory.getObjectFactory( object ) ) {
            // fill with new ones.
            find_method( startpos );
        }

        find_attribute( startpos );

        std::vector<std::string> comps = peer->attributeRepository.attributes();
        for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( comp ) == 0 )
                completes.push_back( peerpath+*i );
        }

        if (peer->attributeRepository.properties() != 0 ) {
            peer->attributeRepository.properties()->list(comps);
            for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
                if ( i->find( comp ) == 0 )
                    completes.push_back( peerpath+*i );
            }
        }
        // Only complete peers and objects, not "this" methods.
        comps = peer->commandFactory.getObjectList();
        for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( comp ) == 0 )//&& *i != "this" )
                completes.push_back( peerpath+*i + "." ); // +"."
        }
        comps = peer->dataFactory.getObjectList();
        for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( comp ) == 0 )//&& *i != "this"  )
                completes.push_back( peerpath+*i + "." ); // +"."
        }
        comps = peer->methodFactory.getObjectList();
        for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( comp ) == 0 )//&& *i != "this"  )
                completes.push_back( peerpath+*i + "." ); // +"."
        }
        comps = peer->getPeerList();
        for (TaskContext::PeerList::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( comp ) == 0 )//&& *i != "this" )
                completes.push_back( peerpath+ *i + "." ); // +"."
        }

        // only try this if text is not empty.
        if ( !text.empty() ) {
            if ( std::string( "cd " ).find(text) == 0 )
                completes.push_back("cd "); 
            if ( std::string( "ls" ).find(text) == 0 )
                completes.push_back("ls"); 
            if ( std::string( "cd .." ).find(text) == 0 )
                completes.push_back("cd ..");
            if ( std::string( "peers" ).find(text) == 0 )
                completes.push_back("peers");
            if ( std::string( "help" ).find(text) == 0 )
                completes.push_back("help");
            if ( std::string( "quit" ).find(text) == 0 )
                completes.push_back("quit");
            if ( std::string( "list " ).find(text) == 0 )
                completes.push_back("list ");
        }
    }
        
    void TaskBrowser::find_command(std::string::size_type startpos)
    {
#if 0
        std::string::size_type pos;
        if ( (pos = text.find( "(", startpos )) != std::string::npos ) {
            std::string _method( text, startpos, pos - 1); // remove "("
            //cout << "FoundMethod : "<< _method <<endl;

            // strip white spaces from method
            while ( _method.find(" ") != std::string::npos )
                _method.replace( _method.find(" "),1,"" );

            // try if it is a command :
            if ( peer->commandFactory.getObjectFactory( component )->hasCommand( _method ) ) {
                // complete :
                method = _method;
                return; // nothing to do
            } else {
                cerr << "Illegal method" << endl;
            }
        }
#endif
        // no brace found, thus build completion list :
        std::string _method;
        if (startpos != std::string::npos )
            _method = text.substr( startpos );
        //cout << "FoundMethod2 : "<< _method << endl;
            
        // strip white spaces from _method
        while ( _method.find(" ") != std::string::npos )
            _method.replace( _method.find(" "),1,"" );

        std::vector<std::string> comps;
        comps = peer->commandFactory.getObjectFactory( object )->getCommandList();
        for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( _method ) == 0  )
                if ( object == "this" )
                    completes.push_back( peerpath + *i );
                else
                    completes.push_back( peerpath +object+"."+ *i );
        }
    }

    void TaskBrowser::find_datasource(std::string::size_type startpos)
    {
#if 0
        std::string::size_type pos;
        if ( (pos = text.find( "(", startpos )) != std::string::npos ) {
            std::string _datasource( text, startpos, pos - 1); // remove "("
            //cout << "FoundDatasource : "<< _datasource <<endl;

            // strip white spaces from datasource
            while ( _datasource.find(" ") != std::string::npos )
                _datasource.replace( _datasource.find(" "),1,"" );

            // try if it is a command :
            if ( peer->dataFactory.getObjectFactory( component )->hasMember( _datasource ) ) {
                datasource = _datasource;
            }
        }
#endif
        // no brace found, thus build completion list :
        std::string _datasource;
        if (startpos != std::string::npos)
            _datasource = text.substr( startpos );
        //cout << "FoundDatasource2 : "<< _datasource << endl;
            
        // strip white spaces from _datasource
        while ( _datasource.find(" ") != std::string::npos )
            _datasource.replace( _datasource.find(" "),1,"" );

        std::vector<std::string> comps;
        comps = peer->dataFactory.getObjectFactory(object)->getNames();
        for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( _datasource ) == 0  )
                if ( object == "this" )
                    completes.push_back( peerpath + *i );
                else
                    completes.push_back( peerpath +object+"."+ *i );
        }
    }

    void TaskBrowser::find_method(std::string::size_type startpos)
    {
#if 0
        std::string::size_type pos;
        if ( (pos = text.find( "(", startpos )) != std::string::npos ) {
            std::string _method( text, startpos, pos - 1); // remove "("
            //cout << "FoundMethod : "<< _method <<endl;

            // strip white spaces from method
            while ( _method.find(" ") != std::string::npos )
                _method.replace( _method.find(" "),1,"" );

            // try if it is a command :
            if ( peer->methodFactory.getObjectFactory( object )->hasMember( _method ) ) {
                method = _method;
            }
        }
        // no brace found, thus build completion list :
#endif
        std::string _method;
        if (startpos != std::string::npos)
            _method = text.substr( startpos );
        //cout << "FoundMethod2 : "<< _method << endl;
            
        // strip white spaces from _method
        while ( _method.find(" ") != std::string::npos )
            _method.replace( _method.find(" "),1,"" );

        std::vector<std::string> comps;
        comps = peer->methodFactory.getObjectFactory(object)->getNames();
        for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( _method ) == 0  )
                if ( object == "this" )
                    completes.push_back( peerpath + *i );
                else
                    completes.push_back( peerpath +object+"."+ *i );
        }
    }

    void TaskBrowser::find_attribute(std::string::size_type startpos)
    {
        // attribute to complete :
        std::string _attribute;
        if (startpos != std::string::npos)
            _attribute = text.substr( startpos );

        //cout << "startpos "<<startpos<<" attribute : "<< _attribute << endl;
            
        // strip white spaces from _attribute
        while ( _attribute.find(" ") != std::string::npos )
            _attribute.replace( _attribute.find(" "),1,"" );

        // all attributes :
        std::vector<std::string> attrs;
        attrs = peer->attributeRepository.attributes();
        for (std::vector<std::string>::iterator i = attrs.begin(); i!= attrs.end(); ++i ) {
            if ( i->find( _attribute ) == 0 && !_attribute.empty() )
                completes.push_back( peerpath + *i );
        }
        // all properties :
        if (peer->attributeRepository.properties() != 0 ) {
            std::vector<std::string> props;
            peer->attributeRepository.properties()->list(props);
            for (std::vector<std::string>::iterator i = props.begin(); i!= props.end(); ++i ) {
                if ( i->find( _attribute ) == 0 && !_attribute.empty() )
                    completes.push_back( peerpath + *i );
            }
        }
    }

    void TaskBrowser::find_peers( std::string::size_type startpos )
    {
        peerpath.clear();
        peer = taskcontext;

        //cerr <<" text : "<<text<<endl;
        findPeer( text );

        // there is only a peerpath, if there is another peer :
        if ( peer != taskcontext ) {
            std::string::size_type pos = 0;
            pos = text.rfind("."); // last dot
            assert( pos != std::string::npos );
            // after last dot is for sure the method :
            component = text.substr( pos+1, std::string::npos);

            // on 'this' object, all before last dot is peerpath
            if ( object == "this" ) {
                peerpath = text.substr( startpos, pos )+".";
            }  // with normal object, all before last-but-one dot is peerpath, 
            else {
                pos = text.rfind(".", pos-1); // starting from pos-1, since there is a dot on pos
                peerpath = text.substr( startpos, pos )+".";
            }
        } else {
            std::string::size_type pos = 0;
            pos = text.rfind("."); // last dot
            if ( pos == std::string::npos ) // no dot
                component = text.substr(startpos);
            else
                component = text.substr(pos);
        }

//         cerr <<" peer : "<<peer->getName()<<endl;
//         cerr << "path :"<<peerpath<<endl;
//         cerr <<" comp : "<<component<<endl;
//         cerr <<" obj  : "<<object<<endl;

        TaskContext::PeerList v = peer->getPeerList();
        for (TaskContext::PeerList::iterator i = v.begin(); i != v.end(); ++i) {
            if ( i->find( component ) == 0 ) { // only add if match 
                completes.push_back( peerpath + *i + "." );
                //cerr << "added " << peerpath+*i+"."<<endl;
            }
        }
    }

    char ** TaskBrowser::orocos_hmi_completion ( const char *text, int start, int end )
    {
        char **matches;
        matches = ( char ** ) 0;

        matches = rl_completion_matches ( text, &TaskBrowser::command_generator );
            
        return ( matches );
    }

    TaskBrowser::TaskBrowser( TaskContext* _c )
        :  condition(0), command(0), command_fact(0), datasource_fact(0),
           debug(0),
           line_read(0),
           lastc(0), storedname(""), storedline(-1)
    {
        taskcontext = _c;
        rl_completion_append_character = '\0'; // avoid adding spaces
        rl_attempted_completion_function = &TaskBrowser::orocos_hmi_completion;
    }

    TaskBrowser::~TaskBrowser() {
        if (line_read)
            {
                free (line_read);
            }
    }


    /**
     * @brief Call this method from ORO_main() to 
     * process keyboard input.
     */
    void TaskBrowser::loop()
    {
        using boost::lambda::_1;

        // Intercept Ctrl-C
        signal( SIGINT, ctrl_c_catcher );

        cout << nl<<
            coloron <<
            "  This console reader allows you to browse and manipulate TaskContexts."<<nl<<
            "  You can type in a command, datasource, method, expression or change variables."<<nl;
        cout <<"  (type '"<<underline<<"help"<<coloron<<"' for instructions)"<<nl;
        cout << "    TAB completion and HISTORY is available ('bash' like)" <<coloroff<<nl<<nl;
        while (1)
            {
                cout << " In Task "<< taskcontext->getName() << ". (Status of last Command : ";
                if ( !taskcontext->engine()->commands()->isProcessed( lastc ) )
                    cout << "queued )";
                else {
                    cout << (command == 0 ? "none )" : ( condition == 0 ? "done )"  : (accepted->get() == false ? "fail )" : ( condition->evaluate() == true ? "done )" : "busy )"))));
                    if ( condition != 0 && accepted->get() && condition->evaluate() ) {
                        // at this point the command is out of the Processor queue...
                        // dispose condition, condition might evaluate an invalidated object,
                        // like a deleted program or state machine.
                        delete condition;
                        accepted = 0;
                        condition = 0;
                        // we leave command to non-zero such that the above cout displays "done"
                    }
                }
                // This 'endl' is important because it flushes the whole output to screen of all
                // processing that previously happened, which was using 'nl'.
                cout << endl;

                // Call readline wrapper :
                std::string command( rl_gets() ); // copy over to string
                if ( command == "quit" ) {
                    // Intercept no Ctrl-C
                    signal( SIGINT, SIG_DFL );
                    return;
                } else if ( command == "help") {
                    printHelp();
                } else if ( command == "#debug") {
                    debug = !debug;
                } else if ( command.find("list ") == 0 || command == "list" ) {
                    browserAction(command);
                } else if ( command.find("ls") == 0 ) {
                    std::string::size_type pos = command.find("ls")+2;
                    command = std::string(command, pos, command.length());
                    printInfo( command );
                } else if ( command == "" ) { // nop
                } else if ( command == "peers" ) {
                    std::vector<std::string> objlist;
                    cout <<endl<< "  This task's peer tasks are :"<<endl;
                    objlist = taskcontext->getPeerList();
                    std::for_each( objlist.begin(), objlist.end(), cout << _1 << " " );
                } else if ( command.find("cd ..") == 0  ) {
                    this->switchBack( );
                } else if ( command.find("cd ") == 0  ) {
                    std::string::size_type pos = command.find("cd")+2;
                    command = std::string(command, pos, command.length());
                    this->switchTaskContext( command );
                } else if ( command.find(".") == 0  ) {
                    command = std::string(command, 1, command.length());
                    this->browserAction( command );
                } else {
                    this->evalCommand( command );
                    // a command was typed... clear storedline such that a next 'list'
                    // shows the 'IP' again.
                    storedline = -1;
                }
                cout <<endl;
            }
    }

    std::deque<TaskContext*> taskHistory;
    std::string TaskBrowser::prompt(" (type 'ls' for context info) :");
    std::string TaskBrowser::coloron("\e[m\e[1;31m");
    std::string TaskBrowser::underline("\e[4m");
    std::string TaskBrowser::coloroff("\e[m");

    void TaskBrowser::switchBack()
    {
        if ( taskHistory.size() == 0)
            return;
        if ( !taskcontext->engine()->commands()->isProcessed( lastc ) ) {
            Logger::log()<<Logger::Warning
                         << "Previous command was not yet processed by previous Processor." <<Logger::nl
                         << " Can not track command status across tasks."<< Logger::endl;
            // memleak command...
            command = 0;
            delete condition;
            condition = 0;
        } else {
            delete command;
            delete condition;
            command = 0;
            condition = 0;
        }
        taskcontext = taskHistory.front();
        lastc = 0;
        taskHistory.pop_front();
    }

    void TaskBrowser::switchTaskContext(std::string& c) {
        // if nothing new found, return.
        if ( this->findPeer( c + "." ) == 0  || peer == taskcontext )
            return;
            
        // findPeer has set 'peer' :
        this->switchTaskContext( peer );
    }

    void TaskBrowser::switchTaskContext(TaskContext* tc) {
        // put current on the stack :
        if (taskHistory.size() == 20 )
            taskHistory.pop_back();
        taskHistory.push_front( taskcontext );

        // We need to release the comms, since taskcontext is changing,
        // and we do not keep track of in which processor the comm was dropped.
        if ( !taskcontext->engine()->commands()->isProcessed( lastc ) ) {
            Logger::log()<<Logger::Warning
                         << "Previous command was not yet processed by previous Processor." <<Logger::nl
                         << " Can not track command status across tasks."<< Logger::endl;
            // memleak it...
            command = 0;
            delete condition;
            condition = 0;
        } else {
            delete command;
            delete condition;
            command = 0;
            condition = 0;
        }

        // now switch to new one :
        taskcontext = tc; // peer is the new taskcontext.
        lastc = 0;

        cerr << "   Switched to : " << taskcontext->getName() <<endl;

    }

    TaskContext* TaskBrowser::findPeer(std::string c) {
        // returns the one but last peer, which is the one we want.
        std::string s( c );
            
        our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
        our_pos_iter_t parseend;
            
        PeerParser pp( taskcontext );
        try {
            parse( parsebegin, parseend, pp.parser(), SKIP_PARSER );
        }
        catch( ... )
            {
                Logger::log()<<Logger::Debug<<"No such peer : "<< c <<Logger::endl;
                return 0;
            }
        object = pp.object(); // store object.
        peer = pp.peer();
        return pp.peer();
    }

#if 0
    PropertyBase* TaskBrowser::findProperty(std::string c) {
        // returns the one but last peer, which is the one we want.
        std::string s( c );
            
        our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
        our_pos_iter_t parseend;
            
        PropertyParser pp();
        pp.setPropertyBag( taskcontext.attributeRepository.properties() );
        try {
            parse( parsebegin, parseend, pp.locator(), SKIP_PARSER );
        }
        catch( ... )
            {
                Logger::log()<<Logger::Debug<<"No such property : "<< c <<Logger::endl;
                return 0;
            }
        return pp.property();
    }
#endif

    void TaskBrowser::browserAction(std::string& act)
    {
        std::stringstream ss(act);
        std::string instr;
        ss >> instr;

        if ( instr == "list" ) {
            int line;
            ss >> line;
            if (ss) {
                this->printProgram(line);
                return;
            }
            ss.clear();
            string arg;
            ss >> arg;
            if (ss) {
                storedname = arg;
                ss.clear();
                ss >> line;
                if (ss) {
                    // progname and line given
                    this->printProgram(arg, line);
                    return;
                }
                // only progname given.
                this->printProgram( arg );
                return;
            }
            // just 'list' :
            this->printProgram();
            return;
        }

        std::string arg;
        ss >> arg;

        if ( taskcontext->getName() == "programs" || taskcontext->getName() == "states") {
            Logger::log() << Logger::Error << "Refuse to take action in special TaskContext "<< taskcontext->getName() <<Logger::endl;
            return;
        }
        ProgramLoader loader;
        if ( instr == "loadProgram") {
            if ( loader.loadProgram( arg, taskcontext ) )
                cout << "Done."<<endl;
            else
                cout << "Failed."<<endl;
            return;
        }
        if ( instr == "unloadProgram") {
            if ( loader.unloadProgram( arg, taskcontext ) )
                cout << "Done."<<endl;
            else
                cout << "Failed."<<endl;
            return;
        }

        if ( instr == "loadStateMachine") {
            if ( loader.loadStateMachine( arg, taskcontext ) )
                cout << "Done."<<endl;
            else
                cout << "Failed."<<endl;
            return;
        }
        if ( instr == "unloadStateMachine") {
            if ( loader.unloadStateMachine( arg, taskcontext ) )
                cout << "Done."<<endl;
            else
                cout << "Failed."<<endl;
            return;
        }
        cerr << "Unknown Browser Action : "<< act <<endl;
        cerr << "See 'help' for valid syntax."<<endl;
    }

    void TaskBrowser::evaluate(std::string& comm) {
        this->evalCommand(comm);
    }

    void TaskBrowser::evalCommand(std::string& comm )
    {
        cout << "      Got :"<< comm <<nl;

        command_fact = taskcontext->commandFactory.getObjectFactory( comm );
        datasource_fact = taskcontext->dataFactory.getObjectFactory( comm );
        method_fact = taskcontext->methodFactory.getObjectFactory( comm );
        if ( command_fact ) // only commandobject name was typed
            {
                std::vector<std::string> methods = command_fact->getCommandList();
                std::for_each( methods.begin(), methods.end(), boost::bind(&TaskBrowser::printCommand, this, _1) );
            }
                    
        if ( datasource_fact ) // only datasource_fact name was typed
            {
                std::vector<std::string> methods = datasource_fact->getNames();
                std::for_each( methods.begin(), methods.end(), boost::bind(&TaskBrowser::printSource, this, _1) );
            }
        if ( method_fact ) // only method_fact name was typed
            {
                std::vector<std::string> methods = method_fact->getNames();
                std::for_each( methods.begin(), methods.end(), boost::bind(&TaskBrowser::printMethod, this, _1) );
            }
        // Minor hack : also check if it was an attribute of current TC, for example, 
        // if both the object and attribute with that name exist. the if
        // statement after this one would return and not give the expr parser
        // time to evaluate 'comm'. 
        if ( taskcontext->attributeRepository.getValue( comm ) ) {
                this->printResult( taskcontext->attributeRepository.getValue( comm )->toDataSource(), true );
                return;
        }
            
        if ( command_fact || datasource_fact || method_fact )
            return;
                    
        Parser _parser;
        std::pair< CommandInterface*, ConditionInterface*> comcon;

        if (debug)
            cerr << "Trying ValueChange..."<<nl;
        try {
            // Check if it was a method or datasource :
            DataSourceBase::shared_ptr ds = _parser.parseValueChange( comm, taskcontext );
            // methods and DS'es are processed immediately.
            if ( ds.get() != 0 ) {
                this->printResult( ds.get(), false );
                cout << nl;
                return; // done here
            } else if (debug)
                cerr << "returned zero !"<<nl;
        } catch ( fatal_semantic_parse_exception& pe ) { // incorr args, ...
            // way to fatal,  must be reported immediately
            if (debug)
                cerr << "fatal_semantic_parse_exception: ";
            cerr << pe.what() <<nl;
            return;
        } catch ( syntactic_parse_exception& pe ) { // wrong content after = sign etc..
            // syntactic errors must be reported immediately
            if (debug)
                cerr << "syntactic_parse_exception: ";
            cerr << pe.what() <<nl;
            return;
        } catch ( parse_exception_parser_fail &pe )
            {
                // ignore, try next parser
                if (debug) {
                    cerr << "Ignoring ValueChange exception :"<<nl;
                    cerr << pe.what() <<nl;
                }
        } catch ( parse_exception& pe ) { 
            // syntactic errors must be reported immediately
            if (debug)
                cerr << "parse_exception :";
            cerr << pe.what() <<nl;
            return;
        }
        if (debug)
            cerr << "Trying Expression..."<<nl;
        try {
            // Check if it was a method or datasource :
            DataSourceBase::shared_ptr ds = _parser.parseExpression( comm, taskcontext );
            // methods and DS'es are processed immediately.
            if ( ds.get() != 0 ) {
                this->printResult( ds.get(), true );
                cout << nl;
                return; // done here
            } else if (debug)
                cerr << "returned zero !"<<nl;
        } catch ( syntactic_parse_exception& pe ) { // missing brace etc
            // syntactic errors must be reported immediately
            if (debug)
                cerr << "syntactic_parse_exception :";
            cerr << pe.what() <<nl;
            return;
        } catch ( fatal_semantic_parse_exception& pe ) { // incorr args, ...
            // way to fatal,  must be reported immediately
            if (debug)
                cerr << "fatal_semantic_parse_exception :";
            cerr << pe.what() <<nl;
            return;
        } catch ( parse_exception_parser_fail &pe )
            {
                // ignore, try next parser
                if (debug) {
                    cerr << "Ignoring Expression exception :"<<nl;
                    cerr << pe.what() <<nl;
                }
        } catch ( parse_exception& pe ) { 
                // ignore, try next parser
                if (debug) {
                    cerr << "Ignoring Expression parse_exception :"<<nl;
                    cerr << pe.what() <<nl;
                }
        }
        if (debug)
            cerr << "Trying Command..."<<nl;
        try {
            comcon = _parser.parseCommand( comm, taskcontext );
            if ( !taskcontext->engine()->commands()->isProcessed( lastc ) ) {
                cerr << "Warning : previous command is not yet processed by Processor." <<nl;
                // memleak the command, dispose condition...
                delete condition;
            } else {
                delete command;
                delete condition;
            }
            command = comcon.first;
            condition = comcon.second;
        } catch ( parse_exception& pe ) {
            if (debug)
                cerr << "CommandParser parse_exception :"<<nl;
            cerr << pe.what() <<nl;
            return;
        } catch (...) {
            cerr << "Illegal Input."<<nl;
            return;
        }
                
        if ( command == 0 ) { // this should not be reached
            cerr << "Uncaught : Illegal command."<<nl;
            return;
        }
        // It is for sure a real command, dispatch to target processor :
        // to keep track of accepted/rejected status, we wrap it ourselves.
        TryCommand *tcom = new TryCommand( command );
        accepted = tcom->result();
        command = tcom;
        lastc = taskcontext->engine()->commands()->process( command );
        // returns null if Processor not running or not accepting.
        if ( lastc == 0 ) {
            cerr << "Command not accepted by"<<taskcontext->getName()<<"'s Processor !" << nl;
            delete command;
            delete condition;
            command = 0;
            condition = 0;
            accepted = 0;
        }
    }

    void TaskBrowser::printResult( DataSourceBase* ds, bool recurse) { 
        std::string prompt(" = ");
        // setup prompt :
        cout <<prompt<< setw(20)<<left;
        doPrint( ds, recurse );
        cout << right;
    }

    void TaskBrowser::doPrint( DataSourceBase* ds, bool recurse) {
        // this is needed for ds's that rely on initialision.
        // e.g. eval true once or time measurements.
        // becomes only really handy for 'watches' (todo).
        ds->reset();
        /**
         * If this list of types gets to long, we can still add a virtual
         * printOut( std::ostream& ) = 0 to DataSourceBase.
         */
        // this method can print some primitive DataSource<>'s.
        DataSource<bool>* dsb = dynamic_cast<DataSource<bool>*>(ds);
        if (dsb) {
            cout<< boolalpha << dsb->get() << noboolalpha ;
            return;
        }
        DataSource<int>* dsi = dynamic_cast<DataSource<int>*>(ds);
        if (dsi) {
            cout<< dsi->get() ;
            return;
        }
        DataSource<unsigned int>* dsui = dynamic_cast<DataSource<unsigned int>*>(ds);
        if (dsui) {
            cout<< dsui->get() ;
            return;
        }
        DataSource<std::string>* dss = dynamic_cast<DataSource<std::string>*>(ds);
        if (dss) {
            int wdth = 19 - dss->get().length();
            if (wdth < 0)
                wdth = 0;
            cout<<setw(0)<<'"'<< dss->get() << setw(wdth)<<'"' ;
            return;
        }
        DataSource<const std::string&>* dscs = dynamic_cast<DataSource<const std::string&>*>(ds);
        if (dscs) {
            int wdth = 19 - dscs->get().length();
            if (wdth < 0)
                wdth = 0;
            cout<<setw(0)<<'"'<< dscs->get() << setw(wdth)<<'"' ;
            return;
        }
        DataSource<std::vector<double> >* dsvval = dynamic_cast<DataSource< std::vector<double> >* >(ds);
        if (dsvval) {
            cout<< setw(0) << dsvval->get() ;
            return;
        }
        DataSource<const std::vector<double>& >* dsv = dynamic_cast<DataSource<const std::vector<double>&>* >(ds);
        if (dsv) {
            cout<< setw(0) << dsv->get() ;
            return;
        }
        DataSource< Double6D >* ds6d = dynamic_cast<DataSource< Double6D >* >(ds);
        if (ds6d) {
            cout<< setw(0) << ds6d->get() ;
            return;
        }
        DataSource<double>* dsd = dynamic_cast<DataSource<double>*>(ds);
        if (dsd) {
            cout<< dsd->get() ;
            return;
        }
        DataSource<char>* dsc = dynamic_cast<DataSource<char>*>(ds);
        if (dsc) {
            cout<<'\''<< dsc->get()<<'\'' ;
            return;
        }

        DataSource<void>* dsvd = dynamic_cast<DataSource<void>*>(ds);
        if (dsvd) {
            dsvd->get();
            cout<< "(void)" ;
            return;
        }

        DataSource<PropertyBag>* dspbag = dynamic_cast<DataSource<PropertyBag>*>(ds);
        if (dspbag) {
            PropertyBag bag( dspbag->get() );
            if (!recurse) {
                int siz = bag.getProperties().size();
                int wdth = siz ? (20 - (siz / 10 + 1)) : 20;
                cout <<setw(0)<< siz <<setw( wdth )<< " Properties";
            } else {
            if ( ! bag.empty() ) {
                cout <<setw(0)<<nl;
                for( PropertyBag::iterator it= bag.getProperties().begin(); it!=bag.getProperties().end(); ++it) {
                    cout <<setw(14)<<right<<(*it)->getType()<<" "<<coloron<<setw(14)<< (*it)->getName()<<coloroff;
                    DataSourceBase::shared_ptr propds = (*it)->createDataSource();
                    this->printResult( propds.get(), false );
                    cout <<" ("<<(*it)->getDescription()<<')' << nl;
                }
            } else {
                cout <<prompt<<"(empty PropertyBag)";
            }
            }
            return;
        }
#ifdef OROPKG_GEOMETRY
        DataSource<Vector>* dsgv = dynamic_cast<DataSource<Vector>*>(ds);
        if (dsgv) {
            cout <<nl<<setw(0)<< dsgv->get() ;
            return;
        }
        DataSource<Twist>* dsgt = dynamic_cast<DataSource<Twist>*>(ds);
        if (dsgt) {
            cout <<nl<<setw(0)<< dsgt->get() ;
            return;
        }
        DataSource<Wrench>* dsgw = dynamic_cast<DataSource<Wrench>*>(ds);
        if (dsgw) {
            cout <<nl<<setw(0)<< dsgw->get() ;
            return;
        }
        DataSource<Frame>* dsgf = dynamic_cast<DataSource<Frame>*>(ds);
        if (dsgf) {
            cout <<nl<<setw(0)<< dsgf->get() ;
            return;
        }
        DataSource<Rotation>* dsgr = dynamic_cast<DataSource<Rotation>*>(ds);
        if (dsgr) {
            cout <<nl<<setw(0)<< dsgr->get() ;
            return;
        }
#endif
        if (ds) {
            ds->evaluate();
            cout << "( result type '"+ds->getType()+"' not known to TaskBrowser )" ;
        }
	
    }

    struct comcol
    {
        const char* command;
        comcol(const char* c) :command(c) {}
        std::ostream& operator()( std::ostream& os ) const {
            os<<"'"<< TaskBrowser::coloron<< TaskBrowser::underline << command << TaskBrowser::coloroff<<"'";
            return os;
        }
    };

    struct keycol
    {
        const char* command;
        keycol(const char* c) :command(c) {}
        std::ostream& operator()( std::ostream& os )const {
            os<<"<"<< TaskBrowser::coloron<< TaskBrowser::underline << command << TaskBrowser::coloroff<<">";
            return os;
        }
    };

    struct titlecol
    {
        const char* command;
        titlecol(const char* c) :command(c) {}
        std::ostream& operator()( std::ostream& os ) const {
            os<<endl<<"["<< TaskBrowser::coloron<< TaskBrowser::underline << command << TaskBrowser::coloroff<<"]";
            return os;
        }
    };

    std::ostream& operator<<(std::ostream& os, comcol f ){
        return f(os);
    }

    std::ostream& operator<<(std::ostream& os, keycol f ){
        return f(os);
    }

    std::ostream& operator<<(std::ostream& os, titlecol f ){
        return f(os);
    }

    void TaskBrowser::printHelp()
    {
        cout << coloroff;
        cout <<titlecol("Task Browsing")<<nl;
        cout << "  To switch to another task, type "<<comcol("cd <path-to-taskname>")<<nl;
        cout << "  and type "<<comcol("cd ..")<<" to go back to the previous task (History size is 20)."<<nl;
        cout << "  Pressing "<<keycol("tab")<<" multiple times helps you to complete your command."<<nl;
        cout << "  It is not mandatory to switch to a task to interact with it, you can type the"<<nl;;
        cout << "  peer-path to the task (dot-separated) and then type command or expression :"<<nl;
        cout << "     PeerTask.OtherTask.FinalTask.countTo(3) [enter] "<<nl;
        cout << "  Where 'countTo' is a method of 'FinalTask'."<<nl;

        cout << "  "<<titlecol("Task Context Info")<<nl;
        cout << "  To see the contents of a task, type "<<comcol("ls")<<nl;
        cout << "  For a detailed argument list (and helpful info) of the object's methods, "<<nl;
        cout <<"   type the name of one of the listed task objects : " <<nl;
        cout <<"      this [enter]" <<nl<<nl;
        cout <<"  Command    : bool factor( int number )" <<nl;
        cout <<"   Factor a value into its primes." <<nl;
        cout <<"   number : The number to factor in primes." <<nl;
        cout <<"  DataSource : bool isRunning( )" <<nl;
        cout <<"   Is this GenericTaskContext started ?" <<nl;
        cout <<"  Method     : bool loadProgram( const& std::string Filename )" <<nl;
        cout <<"   Load an Orocos Program Script from a file." <<nl;
        cout <<"   Filename : An ops file." <<nl;
        cout <<"   ..."<<nl;

        cout <<titlecol("Expressions")<<nl;
        cout << "  You can evaluate any script expression by merely typing it :"<<nl;
        cout << "     1+1 [enter]" <<nl;
        cout << "   = 2" <<nl;
        cout << "  or inspect the status of a program :"<<nl;
        cout << "     programs.myProgram.isRunning [enter]" <<nl;
        cout << "   = false" <<nl;
        cout << "  and display the contents of complex data types (vector, array,...) :"<<nl;
        cout << "     array(6)" <<nl;
        cout << "   = {0, 0, 0, 0, 0, 0}" <<nl;

        cout <<titlecol("Changing Attributes")<<nl;
        cout << "  To change the value of a Task's attribute, type "<<comcol("varname = <newvalue>")<<nl;
        cout << "  If you provided a correct assignment, the browser will inform you of the success"<<nl;
        cout <<"   with '= true'." <<nl;

        cout <<titlecol("Commands")<<nl;
        cout << "  A Command is 'sent' to a task, which will process it in its own context (thread)."<<nl;
        cout << "  A command consists of an object, followed by a dot ('.'), the command "<<nl;
        cout << "  name, followed by the parameters. An example could be :"<<nl;
        cout << "     otherTask.bar.orderBeers(\"Palm\", 5) [enter] "<<nl;
        cout << "  The prompt will inform you about the status of the last command you entered. "<<nl;
        cout << "  It is allowed to enter a new command while the previous is still busy. "<<nl;

        cout <<titlecol("Methods and DataSources")<<nl;
        cout << "  Methods and DataSources 'look' the same as commands, but they are evaluated"<<nl;
        cout << "  immediately and print the result. An example could be :"<<nl;
        cout << "     someTask.bar.getNumberOfBeers(\"Palm\") [enter] "<<nl;
        cout << "   = 99" <<nl;

        cout <<titlecol("Program and StateMachine Scripts")<<nl;
        cout << "  To load a program script from local disc, type "<<comcol(".loadProgram <filename>")<<nl;
        cout << "  To load a state machine script from local disc, type "<<comcol(".loadStateMachine <filename>")<<nl;
        cout << "   ( notice the starting dot '.' )"<<nl;
        cout << "  Likewise, "<<comcol(".loadProgram <ProgramName>")<<" and "<<comcol(".unloadStateMachine <StateMachineName>")<<nl;
        cout << "   are available (notice it is the program's name, not the filename)."<<nl;
        cout << "  You can "<<comcol("cd programs.progname")<<" to your program and type "<<comcol("ls")<<nl;
        cout << "   to see the programs commands, methods and variables. You can manipulate each one of these,."<<nl;
        cout << "   as if the program is a Task itself (see all items above)."<<nl;
        cout << "  To print a program or state machine listing, use "<<comcol("list progname [linenumber]")<<nl;
        cout << "   to list the contents of the current program lines begin executed,"<<nl;
        cout << "   or 10 lines before or after <linenumber>. When only "<<comcol("list [n]")<<nl;
        cout << "   is typed, 20 lines of the last listed program are printed from line <n> on "<<nl;
        cout << "   ( default : list next 20 lines after previous list )."<<nl;
        cout << "   A status character shows which line is being executed."<<nl;
        cout << "   For programs : 'E':Error, 'S':Stopped, 'R':Running, 'P':Paused"<<nl;
        cout << "   For state machines : <the same as programs> + 'A':Active, 'I':Inactive"<<nl;

    }

    void TaskBrowser::printProgram(const std::string& progname, int cl /*= -1*/) {
        string ps;
        char s;
        stringstream txtss;
        int ln;
        int start;
        int end;
        const ProgramInterfacePtr pr = taskcontext->getExecutionEngine()->getProgramProcessor()->getProgram( progname );
        if ( pr ) {
            ps = taskcontext->getExecutionEngine()->getProgramProcessor()->getProgramStatusStr(progname);
            s = toupper(ps[0]);
            txtss.str( pr->getText() );
            ln = pr->getLineNumber();
            if ( cl < 0 ) cl = ln;
            start = cl < 10 ? 1 : cl - 10;
            end   = cl + 10;
            this->listText( txtss, start, end, ln, s);
        }
        const StateMachinePtr sm = taskcontext->getExecutionEngine()->getStateMachineProcessor()->getStateMachine( progname );
        if ( sm ) {
            ps = taskcontext->getExecutionEngine()->getStateMachineProcessor()->getStateMachineStatusStr(progname);
            s = toupper(ps[0]);
            txtss.str( sm->getText() );
            ln = sm->getLineNumber();
            if ( cl < 0 ) cl = ln;
            start = cl <= 10 ? 1 : cl - 10;
            end   = cl + 10;
            this->listText( txtss, start, end, ln, s);
        }
        if ( sm == 0 && pr == 0)
            cerr << "Error : No such program or state machine found : "<<progname<<endl;
    }

    void TaskBrowser::printProgram(int cl) {
        string ps;
        char s;
        stringstream txtss;
        int ln;
        int start;
        int end;
        const ProgramInterfacePtr pr = taskcontext->getExecutionEngine()->getProgramProcessor()->getProgram( storedname );
        if ( pr ) {
            ps = taskcontext->getExecutionEngine()->getProgramProcessor()->getProgramStatusStr(storedname);
            s = toupper(ps[0]);
            txtss.str( pr->getText() );
            ln = pr->getLineNumber();
            if ( cl < 0 ) cl = storedline;
            if (storedline < 0 ) cl = ln -10;
            start = cl;
            end   = cl + 20;
            this->listText( txtss, start, end, ln, s);
        }
        const StateMachinePtr sm = taskcontext->getExecutionEngine()->getStateMachineProcessor()->getStateMachine( storedname );
        if ( sm ) {
            ps = taskcontext->getExecutionEngine()->getStateMachineProcessor()->getStateMachineStatusStr(storedname);
            s = toupper(ps[0]);
            txtss.str( sm->getText() );
            ln = sm->getLineNumber();
            if ( cl < 0 ) cl = storedline;
            if (storedline < 0 ) cl = ln -10;
            start = cl;
            end   = cl+20;
            this->listText( txtss, start, end, ln, s);
        }
        if ( sm == 0 && pr == 0)
            cerr << "Error : No such program or state machine found : "<<storedname<<endl;
    }

    void TaskBrowser::listText(stringstream& txtss,int start, int end, int ln, char s) {
        int curln = 1;
        string line;
        while ( start > 1 && curln != start ) { // consume lines
            getline( txtss, line, '\n' );
            if ( ! txtss )
                break; // no more lines, break.
            ++curln;
        }
        while ( end > start && curln != end ) { // print lines
            getline( txtss, line, '\n' );
            if ( ! txtss )
                break; // no more lines, break.
            if ( curln == ln ) {
                cout << s<<'>';
            }
            else
                cout << "  ";
            cout<< setw(int(log(double(end)))) <<right << curln<< left;
            cout << ' ' << line <<endl;
            ++curln;
        }
        storedline = curln;
        // done !
    }
        
    void TaskBrowser::printInfo(const std::string& peerp)
    {
        // this sets this->peer to the peer given
        if ( this->findPeer( peerp+"." ) == 0 )
            return;

        cout <<nl<<" In "<<peer->getName()<< " - Attributes :"<<nl;
        PropertyBag* bag = peer->attributeRepository.properties();
        std::vector<std::string> objlist = peer->attributeRepository.attributes();
        // Print Attributes:
        for( std::vector<std::string>::iterator it = objlist.begin(); it != objlist.end(); ++it) {
            DataSourceBase::shared_ptr pds = peer->attributeRepository.getValue(*it)->toDataSource();
            cout << ((bag && bag->find(*it)) ? " (Property ) " : " (Attribute) ")
                 << setw(11)<< pds->getType()<< " "
                 << coloron <<setw(14)<<left<< *it << coloroff;
            this->printResult( pds.get(), false ); // do not recurse
            if (bag && bag->find(*it)) {
                cout<<" ("<< bag->find(*it)->getDescription() <<')'<< nl;
            } else
                cout <<nl;
        }
        // Print Properties:
        if (bag) {
            for( PropertyBag::iterator it = bag->begin(); it != bag->end(); ++it) {
                if (peer->attributeRepository.getValue( (*it)->getName() ) )
                    continue; // atributes were already printed above
                DataSourceBase::shared_ptr pds = (*it)->createDataSource();
                cout << " (Property ) "
                     << setw(11)<< pds->getType()<< " "
                     << coloron <<setw(14)<<left<< (*it)->getName() << coloroff;
                this->printResult( pds.get(), false ); // do not recurse
                cout<<" ("<< (*it)->getDescription() <<')'<< nl;
            }
        }

        cout <<nl<< " Objects      : "<<coloron;
        objlist = peer->commandFactory.getObjectList();
        std::vector<std::string> objlist2 = peer->dataFactory.getObjectList();
        objlist.insert(objlist.end(), objlist2.begin(), objlist2.end() );
        objlist2 = peer->methodFactory.getObjectList();
        objlist.insert(objlist.end(), objlist2.begin(), objlist2.end() );
        sort(objlist.begin(), objlist.end() );
        std::vector<std::string>::iterator new_end = unique(objlist.begin(), objlist.end());
        copy(objlist.begin(), new_end, std::ostream_iterator<std::string>(cout, " "));

        objlist = peer->getExecutionEngine()->getProgramProcessor()->getProgramList();
        if ( !objlist.empty() ) {
            cout <<coloroff<<nl<< " Programs     : "<<coloron;
            copy(objlist.begin(), objlist.end(), std::ostream_iterator<std::string>(cout, " "));
        }
        objlist = peer->getExecutionEngine()->getStateMachineProcessor()->getStateMachineList();
        if ( !objlist.empty() ) {
            cout <<coloroff<<nl<< " StateMachines: "<<coloron;
            copy(objlist.begin(), objlist.end(), std::ostream_iterator<std::string>(cout, " "));
        }

        cout <<coloroff<<nl<< " Peers        : "<<coloron;
        objlist = peer->getPeerList();
        if ( !objlist.empty() )
            copy(objlist.begin(), objlist.end(), std::ostream_iterator<std::string>(cout, " "));
        else
            cout << "(none)";
        cout <<coloroff<<nl;
    }
        
    void TaskBrowser::printCommand( const std::string m )
    {
        using boost::lambda::_1;
        std::vector<ArgumentDescription> args;
        args = command_fact->getArgumentList( m );
        cout << "  Command    : bool " << coloron << m << coloroff<< "( ";
        for (std::vector<ArgumentDescription>::iterator it = args.begin(); it != args.end(); ++it) {
            cout << it->type <<" ";
            cout << coloron << it->name << coloroff;
            if ( it+1 != args.end() )
                cout << ", ";
            else
                cout << " ";
        }
        cout << ")"<<nl;
        cout << "   " << command_fact->getDescription( m )<<nl;
        for (std::vector<ArgumentDescription>::iterator it = args.begin(); it != args.end(); ++it)
            cout <<"   "<< it->name <<" : " << it->description << nl;
    }
                
    void TaskBrowser::printSource( const std::string m )
    {
        std::vector<ArgumentDescription> args;
        args = datasource_fact->getArgumentList( m );
        cout << "  DataSource : "<< datasource_fact->getResultType(m)<<" " << coloron << m << coloroff<< "( ";
        for (std::vector<ArgumentDescription>::iterator it = args.begin(); it != args.end(); ++it) {
            cout << it->type <<" ";
            cout << coloron << it->name << coloroff;
            if ( it+1 != args.end() )
                cout << ", ";
            else
                cout << " ";
        }
        cout << ")"<<nl;
        cout << "   " << datasource_fact->getDescription( m )<<nl;
        for (std::vector<ArgumentDescription>::iterator it = args.begin(); it != args.end(); ++it)
            cout <<"   "<< it->name <<" : " << it->description << nl;
    }
                
    void TaskBrowser::printMethod( const std::string m )
    {
        std::vector<ArgumentDescription> args;
        args = method_fact->getArgumentList( m );
        cout << "  Method     : "<< method_fact->getResultType(m)<<" " << coloron << m << coloroff<< "( ";
        for (std::vector<ArgumentDescription>::iterator it = args.begin(); it != args.end(); ++it) {
            cout << it->type <<" ";
            cout << coloron << it->name << coloroff;
            if ( it+1 != args.end() )
                cout << ", ";
            else
                cout << " ";
        }
        cout << ")"<<nl;
        cout << "   " << method_fact->getDescription( m )<<nl;
        for (std::vector<ArgumentDescription>::iterator it = args.begin(); it != args.end(); ++it)
            cout <<"   "<< it->name <<" : " << it->description << nl;
    }

}
