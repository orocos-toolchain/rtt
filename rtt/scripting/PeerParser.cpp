/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  PeerParser.cxx

                        PeerParser.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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



#include "rtt/scripting/PeerParser.hpp"
#include "rtt/scripting/parser-debug.hpp"
#include "rtt/scripting/parse_exception.hpp"
#include "rtt/TaskContext.hpp"
#include "rtt/scripting/parser-types.hpp"
#include "rtt/internal/GlobalService.hpp"

#include <boost/bind.hpp>
#include <boost/iterator/iterator_traits.hpp>

namespace RTT
{
    using boost::bind;
    using namespace detail;
    using namespace std;
    using namespace boost;

    error_status<> PeerParser::handle_no_peer(scanner_t const& scan, parser_error<PeerErrors, iter_t>&e )
    {
        int length = advance_on_error;
        // before, this was not necessary !
        while (advance_on_error != 0) {
            ++scan;
            --advance_on_error;
        }
        // ok, got as far as possible, _peer contains the furthest we got.
        //std::cerr<<"Returning accept, advance "<< length <<std::endl;
        return error_status<>( error_status<>::accept, length );
    }

        void PeerParser::done()
        {
            //std::cerr<<"Peerparser operating in "<<  context->getName()<<std::endl;
            mlastobject = "this";

            // if size() > 1, it must be a peer or service
            // first browse the peers
            while ( callqueue.size() > 0 && _peer->hasPeer( callqueue.front() ) ) {
                //std::cerr<< _peer->getName() <<" has peer " << callqueue.front()<<std::endl;
                _peer = _peer->getPeer( callqueue.front() );

                if ( _peer->ready() == false ) {
                    throw parse_exception_semantic_error
                                    ("Attempt to use TaskContext "+ callqueue.front() +" which is not ready to use." );
                }

                callqueue.pop();
            }

            // BC: user uses 'states.' or 'programs'.
            if ( !callqueue.empty() ) {
                std::string name = callqueue.front();
                if ( (name == "states" || name == "programs") && _peer->provides()->hasService(name) == 0) {
                    log(Warning) << "'"<<name<<"' peer not found. The use of '"<<name<<"' has been deprecated."<<endlog();
                    log(Warning) << "Modify your script to use the program's or state machine's name directly."<<endlog();
                    callqueue.pop();
                }
            }

            mcurobject = _peer->provides();

            // all peers done, now traverse services:
            while ( callqueue.size() > 0 && mcurobject->hasService( callqueue.front() ) ) {
                //std::cerr<< mcurobject->getName() <<" has object " << callqueue.front()<<std::endl;
                mcurobject = mcurobject->provides( callqueue.front() );
                mlastobject = callqueue.front();
                callqueue.pop();
            }

            // last resort: browse the global service if not a single match and items in queue
            if (mcurobject == context->provides() && callqueue.size() != 0 ) {
                mcurobject = GlobalService::Instance();
                while ( callqueue.size() && mcurobject->hasService( callqueue.front() ) ) {
                    mcurobject = mcurobject->provides( callqueue.front() );
                    mlastobject = callqueue.front();
                    callqueue.pop();
                }
            }

            // Something went wrong, a peer or object was not found:
            if ( mfullpath && callqueue.size() != 0 ) {
                // print to user the mismatch :
                string object = callqueue.front();
                while ( !callqueue.empty() )
                    callqueue.pop();
                iter_t begin;
                if ( _peer->provides() == mcurobject )
                    throw_(begin, "From TaskContext '"+context->getName()+"': Task '"+ _peer->getName()+"' has no child Service '"+object+"'." );
                else
                    throw_(begin, "From TaskContext '"+context->getName()+"': Service '"+ mcurobject->getName()+"' has no child Service '"+object+"'." );
            }
            mfoundpath = true;
        }

        PeerParser::PeerParser(TaskContext* c, CommonParser& cp, bool fullpath)
            : commonparser(cp), mcurobject(c->provides()), mlastobject("this"), context(c), _peer(context), mfullpath(fullpath), mfoundpath(false), advance_on_error(0)
        {
            BOOST_SPIRIT_DEBUG_RULE( my_guard );
            BOOST_SPIRIT_DEBUG_RULE( peerpath );
            BOOST_SPIRIT_DEBUG_RULE( peerlocator );
            peerpath =
                ( +(commonparser.notassertingidentifier >> ".")[boost::bind( &PeerParser::seenobjectname, this, _1, _2 ) ] )[boost::bind(&PeerParser::done, this)];

            // find as far as possible a peer without throwing an exception
            // outside our interface
            peerlocator =
                my_guard( *((commonparser.notassertingidentifier >> ".")[boost::bind( &PeerParser::locatepeer, this, _1, _2 ) ]))
                        [ boost::bind(&PeerParser::handle_no_peer, this, _1, _2) ]
                ;
        }

    void PeerParser::reset()
    {
        _peer = context;
        mcurobject = context->provides();
        mlastobject = "this";
        advance_on_error = 0;
        mfoundpath = false;
        while( !callqueue.empty() )
            callqueue.pop();
    }

    void PeerParser::seenobjectname( iter_t begin, iter_t end )
    {
        std::string name( begin, end );
        name.erase( name.length() -1  ); // compensate for extra "."
        callqueue.push( name );
//         std::cerr << "seen " << name <<std::endl;
    }

    void PeerParser::locatepeer( iter_t begin, iter_t end )
    {
        std::string name( begin, end );
        name.erase( name.length() -1  ); // compensate for extra "."

        if ( mcurobject == _peer->provides() && _peer->hasPeer( name ) ) {
            _peer = _peer->getPeer( name );
            if ( _peer->ready() == false ) {
                throw parse_exception_semantic_error
                                ("Attempt to use TaskContext "+name+" which is not ready to use." );
            }
            mcurobject = _peer->provides();
            advance_on_error += end.base() - begin.base();

//            cout << "PP located "<<name <<endl;
        }
        else if ( mcurobject->hasService(name) ) {
            mcurobject = mcurobject->provides(name);
            advance_on_error += end.base() - begin.base();
        }
        // check global service if we're still on the top-level:
        else if (mcurobject == _peer->provides() && GlobalService::Instance()->hasService(name) ) {
            mcurobject = GlobalService::Instance()->provides(name);
            advance_on_error += end.base() - begin.base();
            mfoundpath = true;
        } else {
            if ( name == "states" || name == "programs") {
                log(Warning) << "'"<<name<<"' peer not found. The use of '"<<name<<"' has been deprecated."<<endlog();
                log(Warning) << "Modify your script to use the program's or state machine's name directly."<<endlog();
                advance_on_error += end.base() - begin.base();
                return;
            }
//            cout << "PP failed "<<name <<endl;
            // store object name for higher level access.
            // do not consume it though.
//            cout << std::string(begin, end)<<endl;
            mlastobject = name;
            if (mfullpath) {
                mcurobject.reset(); //when partial paths are OK, leave curobject pointing to last valid object.
                mfoundpath = false;
            }
            throw_(begin, peer_not_found );
        }
    }

    rule_t& PeerParser::parser()
    {
        return peerpath;
    }

    rule_t& PeerParser::locator()
    {
        return peerlocator;
    }

    TaskContext* PeerParser::peer()
    {
        return _peer;
    }

    Service::shared_ptr PeerParser::taskObject()
    {
        return mcurobject;
    }

    std::string PeerParser::object()
    {
        return mlastobject;
    }

    bool PeerParser::foundPath() 
    {
        return mfoundpath;
    }
}

