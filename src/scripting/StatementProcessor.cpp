/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  StatementProcessor.cxx

                        StatementProcessor.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#include "StatementProcessor.hpp"
#include "Parser.hpp"
#include "parse_exception.hpp"

#include "../TaskContext.hpp"
#include "../base/DispatchInterface.hpp"
#include "ConditionInterface.hpp"
#include "../extras/MultiVector.hpp"
#include "../types/TypeStream.hpp"
#include "../Logger.hpp"

#include <vector>
#include <boost/tuple/tuple.hpp>
#include <iostream>


using namespace boost;

namespace RTT
{
    using namespace detail;
    struct StatementProcessor::D
    {
        TaskContext* tc;
        std::vector<boost::tuple<int, DispatchInterface::shared_ptr, ConditionInterface*, std::string> > comms;
        int seq;
        D() : seq(0) {}

        void checkFinished()
        {
            std::vector<boost::tuple<int, DispatchInterface::shared_ptr, ConditionInterface*, std::string> >::iterator it = comms.begin();
            while( it != comms.end() )
                {
                    if ( it->get<1>()->valid() && it->get<2>()->evaluate() == true ) {
                        Logger::log() <<Logger::Info<<"Command ("<<it->get<0>()<<") '"<< it->get<3>() << "' is done." <<Logger::endl;
                        comms.erase(it);
                        it = comms.begin(); // reset !
                        continue;
                    }
                    if ( it->get<1>()->accepted() == false ) {
                        Logger::log() <<Logger::Error<<"Command ("<<it->get<0>()<<") '"<< it->get<3>() << "' was not accepted." <<Logger::endl;
                        comms.erase(it);
                        it = comms.begin(); // reset !
                        continue;
                    }
                    if ( it->get<1>()->executed() == true ) {
                        if ( it->get<1>()->valid() == false ) {
                            Logger::log() <<Logger::Error<<"Command ("<<it->get<0>()<<") '"<< it->get<3>() << "' was executed but rejected." <<Logger::endl;
                            comms.erase(it);
                            it = comms.begin(); // reset !
                            continue;
                        }
                        Logger::log() <<Logger::Info<<"Command ("<<it->get<0>()<<") '"<< it->get<3>() << "' is busy." <<Logger::endl;
                    }
                    ++it;
                }
        }

        DispatchInterface::shared_ptr getCommand(int cnr)
        {
            std::vector<boost::tuple<int, DispatchInterface::shared_ptr, ConditionInterface*, std::string> >::iterator it = comms.begin();
            while( it != comms.end() )
                if ( it->get<0>() == cnr )
                    return it->get<1>();
            return DispatchInterface::shared_ptr();
        }

        int add(DispatchInterface* command, ConditionInterface* cond, std::string code)
        {
            comms.push_back( boost::tuple<int, DispatchInterface::shared_ptr, ConditionInterface*,std::string>(seq, DispatchInterface::shared_ptr(command), cond, code) );
            Logger::log() <<Logger::Info<<"Executing Command ("<<seq<<") '"<< code << "'..." <<Logger::endl;
            command->dispatch();
            ++seq;
            return seq -1;
        }

        void printResult( DataSourceBase* ds, bool recurse) {
            std::string prompt(" = ");
            // setup prompt :
            Logger::log() << Logger::Info <<prompt;
            doPrint( ds, recurse );
            Logger::log() << Logger::endl;
        }

        void doPrint( DataSourceBase* ds, bool recurse) {
            // this is needed for ds's that rely on initialision.
            // e.g. eval true once or time measurements.
            // becomes only really handy for 'watches' (todo).
            ds->reset();
            /**
             * If this list of types gets to long, we can still add a virtual
             * printOut( std::ostream& ) = 0 to DataSourceBase.
             */
            // this method can print some primitive DataSource<>'s.
            DataSource<bool>* dsb = DataSource<bool>::narrow(ds);
            if (dsb) {
                Logger::log() << dsb->get();
                return;
            }
            DataSource<int>* dsi = DataSource<int>::narrow(ds);
            if (dsi) {
                Logger::log() << dsi->get() ;
                return;
            }
#if 0
            // does not work yet with CORBA layer.
            DataSource<long>* dsl = DataSource<long>::narrow(ds);
            if (dsl) {
                Logger::log() << dsl->get() ;
                return;
            }
#endif
            DataSource<unsigned int>* dsui = DataSource<unsigned int>::narrow(ds);
            if (dsui) {
                Logger::log() << dsui->get() ;
                return;
            }
            DataSource<std::string>* dss = DataSource<std::string>::narrow(ds);
            if (dss) {
                Logger::log() <<'"'<< dss->get() << '"' ;
                return;
            }
            DataSource<const std::string&>* dscs = DataSource<const std::string&>::narrow(ds);
            if (dscs) {
                Logger::log() <<'"'<< dscs->get() <<'"' ;
                return;
            }
            DataSource<std::vector<double> >* dsvval = DataSource< std::vector<double> >::narrow(ds);
            if (dsvval) {
                Logger::log()  << dsvval->get() ;
                return;
            }
            DataSource<const std::vector<double>& >* dsv = DataSource<const std::vector<double>& >::narrow(ds);
            if (dsv) {
                Logger::log()  << dsv->get() ;
                return;
            }
            DataSource< Double6D >* ds6d = DataSource<Double6D>::narrow(ds);
            if (ds6d) {
                Logger::log()  << ds6d->get() ;
                return;
            }
            DataSource<double>* dsd = DataSource<double>::narrow(ds);
            if (dsd) {
                Logger::log() << dsd->get() ;
                return;
            }
            DataSource<char>* dsc = DataSource<char>::narrow(ds);
            if (dsc) {
                Logger::log() <<'\''<< dsc->get()<<'\'' ;
                return;
            }

            DataSource<PropertyBag>* dspbag = DataSource<PropertyBag>::narrow(ds);
            if (dspbag) {
                PropertyBag bag( dspbag->get() );
                if (!recurse) {
                    int siz = bag.getProperties().size();
                    Logger::log()  << siz <<" Properties";
                } else {
                    if ( ! bag.empty() ) {
                        Logger::log()  <<Logger::nl;
                        for( PropertyBag::iterator it= bag.getProperties().begin(); it!=bag.getProperties().end(); ++it) {
                            Logger::log()  <<(*it)->getType()<<" "<< (*it)->getName();
                            DataSourceBase::shared_ptr propds = (*it)->getDataSource();
                            this->printResult( propds.get(), false );
                            Logger::log()  <<" ("<<(*it)->getDescription()<<')' << Logger::nl;
                        }
                    } else {
                        Logger::log()  <<"(empty PropertyBag)";
                    }
                }
                return;
            }

            // Leave void  as last since any DS is convertible to void !
            DataSource<void>* dsvd = DataSource<void>::narrow(ds);
            if (dsvd) {
                dsvd->get();
                Logger::log() << "(void)" ;
                return;
            }

            if (ds) {
                ds->evaluate();
                Logger::log() << "( result type '"+ds->getType()+"' not known to TaskBrowser )" ;
            }

        }

    };


    StatementProcessor::StatementProcessor(TaskContext* tc)
        : d ( new D() )
    {
        d->tc = tc;
    }

    StatementProcessor::~StatementProcessor() {
        delete d;
    }

    void StatementProcessor::checkFinished() {
        Logger::In in("StatementProcessor");
        d->checkFinished();
    }

    DispatchInterface::shared_ptr StatementProcessor::getCommand(int cnr) {
        Logger::In in("StatementProcessor");
        return d->getCommand(cnr);
    }

    int StatementProcessor::execute(const std::string& comm)
    {
        Logger::In in("StatementProcessor");

        d->checkFinished();

        TaskContext* taskcontext = d->tc;

        // Minor hack : also check if it was an attribute of current TC, for example,
        // if both the object and attribute with that name exist. the if
        // statement after this one would return and not give the expr parser
        // time to evaluate 'comm'.
        if ( taskcontext->attributes()->getValue( comm ) ) {
                d->printResult( taskcontext->attributes()->getValue( comm )->getDataSource().get(), true );
                return 0;
        }

        Parser _parser;
        std::pair< CommandInterface*, ConditionInterface*> comcon;
        DispatchInterface* command;
        ConditionInterface* condition;

        Logger::log() <<Logger::Debug << "Trying ValueChange...";
        try {
            // Check if it was a method or datasource :
            DataSourceBase::shared_ptr ds = _parser.parseValueChange( comm, taskcontext );
            // methods and DS'es are processed immediately.
            if ( ds.get() != 0 ) {
                Logger::log() << "ok" << Logger::endl;
                d->printResult( ds.get(), false );
                return 0; // done here
            } else
                Logger::log() <<Logger::Debug << "no"<<Logger::endl;
        } catch ( fatal_semantic_parse_exception& pe ) { // incorr args, ...
            // way to fatal,  must be reported immediately
            Logger::log() << Logger::Debug << "fatal_semantic_parse_exception: ";
            Logger::log() << Logger::Error << pe.what() <<Logger::nl;
            return -1;
        } catch ( syntactic_parse_exception& pe ) { // wrong content after = sign etc..
            // syntactic errors must be reported immediately
            Logger::log() << Logger::Error << "syntactic_parse_exception: ";
            Logger::log() << Logger::Error << pe.what() <<Logger::nl;
            return -1;
        } catch ( parse_exception_parser_fail &pe )
            {
                // ignore, try next parser
                Logger::log() << Logger::Debug << "Ignoring ValueChange exception :"<<Logger::nl;
                Logger::log() << Logger::Debug << pe.what() <<Logger::nl;
        } catch ( parse_exception& pe ) {
            // syntactic errors must be reported immediately
            Logger::log() << Logger::Error << "parse_exception :";
            Logger::log() << Logger::Error << pe.what() <<Logger::nl;
            return -1;
        }
        Logger::log() << Logger::Debug << "Trying Expression..."<<Logger::nl;
        try {
            // Check if it was a method or datasource :
            DataSourceBase::shared_ptr ds = _parser.parseExpression( comm, taskcontext );
            // methods and DS'es are processed immediately.
            if ( ds.get() != 0 ) {
                d->printResult( ds.get(), true );
                return 0; // done here
            } else
                Logger::log() << Logger::Error << "returned zero !"<<Logger::nl;
        } catch ( syntactic_parse_exception& pe ) { // missing brace etc
            // syntactic errors must be reported immediately
            Logger::log() << Logger::Error << "syntactic_parse_exception :";
            Logger::log() << Logger::Error << pe.what() <<Logger::nl;
            return -1;
        } catch ( fatal_semantic_parse_exception& pe ) { // incorr args, ...
            // way to fatal,  must be reported immediately
            Logger::log() << Logger::Error << "fatal_semantic_parse_exception :";
            Logger::log() << Logger::Error << pe.what() <<Logger::nl;
            return -1;
        } catch ( parse_exception_parser_fail &pe ) {
                // ignore, try next parser
                Logger::log() << Logger::Debug << "Ignoring Expression exception :"<<Logger::nl;
                Logger::log() << Logger::Debug << pe.what() <<Logger::nl;
        } catch ( parse_exception& pe ) {
            // ignore, try next parser
            Logger::log() << Logger::Debug << "Ignoring Expression parse_exception :"<<Logger::nl;
            Logger::log() << Logger::Debug << pe.what() <<Logger::nl;
        }
        Logger::log() << Logger::Debug << "Trying Command...";
        try {
            comcon = _parser.parseCommand( comm, taskcontext, true ); // create a dispatch command.
            assert( dynamic_cast<DispatchInterface*>(comcon.first) );
            command = dynamic_cast<DispatchInterface*>(comcon.first);
            condition = comcon.second;
        } catch ( parse_exception& pe ) {
            Logger::log() << Logger::Debug << "CommandParser parse_exception :"<<Logger::nl;
            Logger::log() << Logger::Error << pe.what() <<Logger::nl;
            return -1;
        } catch (...) {
            Logger::log() << Logger::Error << "Illegal Input."<<Logger::nl;
            return -1;
        }

        if ( command == 0 ) { // this should not be reached
            Logger::log() << Logger::Error << "Uncaught : Illegal command."<<Logger::nl;
            return -1;
        }

        return d->add( command, condition, comm);
    }

}

