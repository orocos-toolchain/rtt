
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

        /**
         * Change the context in which peers are 
         * looked up.
         * @return the previous TaskContext.
         */
        TaskContext* setContext( TaskContext* tc);

        void seenobjectname( iter_t begin, iter_t end );

        /**
         * After reset, peer() == current context and
         * object() == "this".
         */
        void reset();

        rule_t& parser();

        TaskContext* peer();

        std::string object();
    };
}
            
#endif
