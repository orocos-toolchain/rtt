
#ifndef ORO_PEERPARSER_HPP
#define ORO_PEERPARSER_HPP

#include "parser-types.hpp"
#include "CommonParser.hpp"
#include <queue>
#include <string>

namespace ORO_Execution
{
    class TaskContext;

    /**
     * Get the peer and object from an invocation path like
     *  a.b.c.d() .
     */
    class PeerParser
    {
        CommonParser commonparser;
        rule_t peerpath;
        std::queue<std::string> callqueue;
        std::string mcurobject;
        TaskContext* context;
        TaskContext* _peer;
        void done();
    public:
        PeerParser(TaskContext* c);

        void seenobjectname( iter_t begin, iter_t end )
        {
            std::string name( begin, end - 1 ); // compensate for extra "."
            callqueue.push( name );
        }

        rule_t& parser()
        {
            return peerpath;
        }

        TaskContext* peer()
        {
            assert( _peer );
            return _peer;
        }

        std::string object()
        {
            assert( !mcurobject.empty() );
            return mcurobject;
        }
    };
}
            
#endif
