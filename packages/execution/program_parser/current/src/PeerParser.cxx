
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
            // now first find the task / object which matches mcurobject :
            _peer = context;
            // if size() > 1, it must be a peer 
            while ( callqueue.size() > 0 && _peer->hasPeer( callqueue.front() ) ) {
                _peer = _peer->getPeer( callqueue.front() );
                callqueue.pop();
            }
            // this-wrapping
            if ( callqueue.empty() )
                callqueue.push("this");

            // Something went wrong, a subtask was not found
            if ( callqueue.size() > 1 ) {
                // print to user the mismatch :
                std::string object = callqueue.front();
                callqueue.pop();
                std::string path = callqueue.front();
                while ( !callqueue.empty() )
                    callqueue.pop();
                throw parse_exception_semantic_error( "Task or object '"+path+"' is not known to '" + object + "'" );
            }
            // from here on : callqueue contains "this", "objectname" or "peername"
            // and peer contains the peer context containing it.
            // check if it is an object of the peer, or a method of the peer itself.
            if ( _peer->commandFactory.getObjectFactory( callqueue.front() )
                 ||
                 _peer->methodFactory.getObjectFactory( callqueue.front() )
                 ||
                 _peer->dataFactory.getObjectFactory( callqueue.front() ) )
                mcurobject = callqueue.front(); // it is an objectname or this
            else
                mcurobject = "this"; // it is a peername
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
        std::string name( begin, end - 1 ); // compensate for extra "."
        callqueue.push( name );
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
            
