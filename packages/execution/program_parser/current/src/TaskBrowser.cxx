
#pragma implementation
#include "execution/TaskBrowser.hpp"

#include <execution/TemplateFactories.hpp>
#include <execution/TaskContext.hpp>
#include <execution/Parser.hpp>
#include <execution/parse_exception.hpp>
#include <execution/PeerParser.hpp>

#include <iostream>
#include <deque>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

namespace ORO_Execution
{
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

    using std::cout;
    using std::cerr;
    using std::cin;
    using std::endl;
    using boost::bind;

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
        return  dupstr( complete_iter->c_str() ); // malloc !
    }

    void TaskBrowser::find_completes() {
        std::string::size_type pos;
        std::string::size_type startpos;
        std::string line( rl_line_buffer );

        if ( line.find(std::string("switch ")) == 0 ) { // complete on switch
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

        // check if the user is tabbing on an empty command, then add the console commands :
        if (  line.empty() ) {
            completes.push_back("switch "); 
            completes.push_back("back");
            completes.push_back("peers");
            completes.push_back("help");
            completes.push_back("quit");
            // go on below to add components and tasks as well.
        }

        startpos = text.find_last_of(",( ");
        if ( startpos == std::string::npos )
            startpos = 0;      // if none is found, start at beginning

        // complete on peers, and find the peer the user wants completion for
        find_peers(startpos);
        // now proceed with peer as TC and component as object :

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

        std::vector<std::string> comps = peer->commandFactory.getObjectList();
        for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( comp ) == 0 && *i != "this" )
                completes.push_back( peerpath+*i+"." );
        }
        comps = peer->dataFactory.getObjectList();
        for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( comp ) == 0 && *i != "this"  )
                completes.push_back( peerpath+*i+"." );
        }
        comps = peer->methodFactory.getObjectList();
        for (std::vector<std::string>::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( comp ) == 0 && *i != "this"  )
                completes.push_back( peerpath+*i+"." );
        }
        comps = peer->getPeerList();
        for (TaskContext::PeerList::iterator i = comps.begin(); i!= comps.end(); ++i ) {
            if ( i->find( comp ) == 0 && *i != "this" )
                completes.push_back( peerpath+ *i+"." );
        }

        // only try this if text is not empty.
        if ( !text.empty() ) {
            if ( std::string( "switch " ).find(text) == 0 )
                completes.push_back("switch "); 
            if ( std::string( "back" ).find(text) == 0 )
                completes.push_back("back");
            if ( std::string( "peers" ).find(text) == 0 )
                completes.push_back("peers");
            if ( std::string( "help" ).find(text) == 0 )
                completes.push_back("help");
            if ( std::string( "quit" ).find(text) == 0 )
                completes.push_back("quit");
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
           prompt(" (type 'quit' for exit) :"),
           coloron("\033[1;34m"), coloroff("\033[0m"),
           line_read(0),
           lastc(0)
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

        cout << endl<< coloron <<
            "  This console reader allows you to browse TaskContexts. \n\
  You can type in a command (type 'help' for info),\n\
  an datasource, method or expression. \n\n";
        cout << "    To switch to another task, type 'switch <path-to-taskname>'"<<endl;
        cout << "    TAB completion and HISTORY is available for commands" <<coloroff<<endl<<endl;
        while (1)
            {
                cout << " In Task "<< taskcontext->getName() << ". Enter a command. (Status of previous command : ";
                if ( !taskcontext->getProcessor()->isProcessed( lastc ) )
                    cout << "queued )";
                else
                    cout << (condition == 0 ? "none )" : condition->evaluate() == true ? "done )" : "busy )" );
                cout << endl;

                // Call readline wrapper :
                std::string command( rl_gets() ); // copy over to string
                if ( command == "quit" ) {
                    return;
                } else if ( command == "help") {
                    printHelp();
                } else if ( command == "" ) { // nop
                } else if ( command == "peers" ) {
                    std::vector<std::string> objlist;
                    cout <<endl<< "  This task's peer tasks are :"<<endl;
                    objlist = taskcontext->getPeerList();
                    std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
                } else if ( command.find("switch") == 0  ) {
                    int pos = command.find("switch");
                    command = std::string(command, pos+6, command.length());
                    switchTask( command );
                } else if ( command.find("back") == 0  ) {
                    switchBack( );
                } else
                    evalCommand( command );
                cout <<endl;
            }
    }

    std::deque<TaskContext*> taskHistory;

    void TaskBrowser::switchBack()
    {
        if ( taskHistory.size() == 0)
            return;
        taskcontext = taskHistory.front();
        taskHistory.pop_front();
    }

    void TaskBrowser::switchTask(std::string& c) {
        std::string s( c );
        s +=".dummy";
            
        our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
        our_pos_iter_t parseend;
            
        PeerParser pp( taskcontext );
        try {
            parse( parsebegin, parseend, pp.parser(), SKIP_PARSER );
        }
        catch( const parse_exception& e )
            {
                cerr<<e.what() <<endl;
            }
        // put current on the stack :
        if (taskHistory.size() == 20 )
            taskHistory.pop_back();
        taskHistory.push_front( taskcontext );
        // now switch to new one :
        taskcontext = pp.peer(); // peer is the new taskcontext.

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
        catch( const parse_exception& e )
            {
                cerr<<e.what() <<endl;
            }
        object = pp.object(); // store object.
        peer = pp.peer();
        return pp.peer();
    }

    void TaskBrowser::evalCommand(std::string& comm )
    {
        //             if ( condition != 0 && condition->evaluate() == false )
        //                 {
        //                     cout << "Previous command not done yet !"<<endl;
        //                     return;
        //                 }
        cout << "      Got :"<< comm <<endl;

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
        if ( command_fact || datasource_fact || method_fact )
            return;
                    

        Parser _parser;
        std::pair< CommandInterface*, ConditionInterface*> comcon;
        try {
            DataSourceBase::shared_ptr ds = _parser.parseExpression( comm, taskcontext );
            if ( ds.get() != 0 )
                this->printResult( ds.get() );
            return; // done here
        } catch ( syntactic_parse_exception& pe ) { // missing brace etc
            // syntactic errors must be reported immediately
            cerr << "Syntax Error : Invalid Expression."<<endl;
            cerr << pe.what() <<endl;
            return;
        } catch ( fatal_semantic_parse_exception& pe ) { // incorr args, ...
            // way to fatal,  must be reported immediately
            cerr << "Parse Error : Invalid Expression."<<endl;
            cerr << pe.what() <<endl;
            return;
        } catch ( parse_exception &pe ) // Got not a clue exception, so try other parser
            {
                // ignore it, try to parse it as a command :
                //cerr << "Ignoring : "<< pe.what() << endl;
                try {
                    comcon = _parser.parseCommand(comm, taskcontext );
                    if ( !taskcontext->getProcessor()->isProcessed( lastc ) )
                        cerr << "Warning : previous command is not yet processed by Processor." <<endl;
                    delete command;
                    delete condition;
                    command = comcon.first;
                    condition = comcon.second;
                } catch ( parse_exception& pe ) {
                    cerr << "Parse Error : Illegal command."<<endl;
                    cerr << pe.what() <<endl;
                    return;
                }
            }
        catch (...) {
            cerr << "Illegal Command."<<endl;
            return;
        }
                
        if ( command == 0 ) { // this should not be reached
            cerr << "Uncaught : Illegal command."<<endl;
            return;
        }
        lastc = taskcontext->getProcessor()->process( command );
        if ( lastc == 0 && taskcontext->executeCommand(command) == false ) {
            cerr << "Command not accepted by Processor ! " << endl;
            delete command;
            delete condition;
            command = 0;
            condition = 0;
            return;
        }
    }

    void TaskBrowser::printResult( DataSourceBase* ds) {
        std::string prompt("   = ");
        // this method can print some primitive DataSource<>'s.
        DataSource<bool>* dsb = dynamic_cast<DataSource<bool>*>(ds);
        if (dsb)
            cout <<prompt<< (dsb->get() ? "true" : "false") <<endl;
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

    void TaskBrowser::printHelp()
    {
        using boost::lambda::_1;

        cout << endl<< coloron;
        cout << "  To switch to another task, type 'switch <path-to-taskname>'"<<endl;
        cout << "  and type 'back' to go back to the previous task (History size is 20)."<<endl<<endl;
        cout << "  A command consists of an object, followed by a dot ('.'), the method "<<endl;
        cout << "  name, followed by the parameters. An example could be :"<<endl;
        cout << "  myTask.orderBeers(\"Palm\", 5) [then press enter] "<<endl;
        cout << "  1+1 [enter]" <<endl << coloroff;
        cout << endl<<"  The available Command objects are :"<<endl;
        std::vector<std::string> objlist = taskcontext->commandFactory.getObjectList();
        std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
        cout << "  The available DataSource objects are :"<<endl;
        objlist = taskcontext->dataFactory.getObjectList();
        std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
        cout << "  The available Method objects are :"<<endl;
        objlist = taskcontext->methodFactory.getObjectList();
        std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
        cout << "  This task's peers are (also : type 'peers') :"<<endl;
        objlist = taskcontext->getPeerList();
        std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
        cout << coloron <<"  For a detailed argument list of the object's methods, "<<endl;
        cout <<"   just type the object name (eg 'myTask')" <<endl<< coloroff;
        cout <<endl;
    }
        
    void TaskBrowser::printCommand( const std::string m )
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
                
    void TaskBrowser::printSource( const std::string m )
    {
        using boost::lambda::_1;
        std::vector<ArgumentDescription> args;
        args = datasource_fact->getArgumentList( m );
        cout <<coloron << "  DataSource : " << coloroff << m << coloron<< " - ";
        cout << datasource_fact->getDescription( m )<<coloroff<<endl;
        if (args.begin() != args.end() ){
            for (std::vector<ArgumentDescription>::iterator it = args.begin(); it != args.end(); ++it) {
                cout <<coloron<< "  Argument "<< (it - args.begin()) + 1<<" : " <<coloroff;
                cout << it->name << coloron << " - " << it->description <<coloroff<< endl;
            }
        }
        cout <<endl;
    }
                
    void TaskBrowser::printMethod( const std::string m )
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

}
