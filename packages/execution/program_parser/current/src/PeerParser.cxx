
#include "execution/PeerParser.hpp"
#include "execution/parser-debug.hpp"
#include "execution/parse_exception.hpp"
#include "execution/TaskContext.hpp"
#include "execution/parser-types.hpp"

#include <boost/bind.hpp>

namespace ORO_Execution
{
  using boost::bind;

        void PeerParser::done()
        {
            //std::cerr<<"Peerparser operating in "<<  context->getName()<<std::endl;
            // now first find the task / object which matches mcurobject :
            _peer = context;
            // if size() > 1, it must be a peer 
            while ( callqueue.size() > 0 && _peer->hasPeer( callqueue.front() ) ) {
                //std::cerr<< _peer->getName() <<" has peer " << callqueue.front()<<std::endl;
                _peer = _peer->getPeer( callqueue.front() );
                callqueue.pop();
            }

            // Something went wrong, a subtask was not found
            if ( callqueue.size() > 1 ) {
                // print to user the mismatch :
                std::string object = callqueue.front();
                while ( !callqueue.empty() )
                    callqueue.pop();
                iter_t begin;
                throw_(begin, "From TaskContext '"+context->getName()+"': Task '"+_peer->getName()+"' : has no task or object '"+object+"'." );
            }
            if ( callqueue.empty() )
                callqueue.push("this");
            // from here on : callqueue contains "this" or "objectname".
            // check if it is an object of the peer, or a method of the peer itself.
            // Warning : We can not throw because it is allowed to parse nothing.
            // in that case, just assign "this" to mcurobj.
            if ( _peer->commandFactory.getObjectFactory( callqueue.front() )
                 ||
                 _peer->methodFactory.getObjectFactory( callqueue.front() )
                 ||
                 _peer->dataFactory.getObjectFactory( callqueue.front() ) )
                mcurobject = callqueue.front(); // it is an objectname or this
            else {
                // we should only get here if we parsed nothing.
                // put "this" in mcurobject.
                mcurobject = callqueue.front();
            }
            callqueue.pop();
        }

        PeerParser::PeerParser(TaskContext* c)
            : mcurobject("this"),context(c), _peer(context)
        {
            BOOST_SPIRIT_DEBUG_RULE( peerpath );
            peerpath = 
                (lexeme_d[ *(commonparser.lexeme_notassertingidentifier
                            >> ".")[bind( &PeerParser::seenobjectname, this, _1, _2 ) ]
                 ])[bind(&PeerParser::done, this)];
        }

    TaskContext* PeerParser::setContext( TaskContext* tc )
    {
        //std::cerr<< "Peers: context: "<< tc->getName()<<std::endl;
        TaskContext* ret = context;
        context = tc;
        return ret;
    }

    void PeerParser::reset()
    {
        _peer = context;
        mcurobject = "this";
    }

    void PeerParser::seenobjectname( iter_t begin, iter_t end )
    {
        std::string name( begin, --end ); // compensate for extra "."
        callqueue.push( name );
        //std::cerr << "seen " << name <<std::endl;
        ++end;
    }

    rule_t& PeerParser::parser()
    {
        return peerpath;
    }

    TaskContext* PeerParser::peer()
    {
        return _peer;
    }

    std::string PeerParser::object()
    {
        return mcurobject;
    }

}
            
