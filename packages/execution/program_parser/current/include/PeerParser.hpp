/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  PeerParser.hpp 

                        PeerParser.hpp -  description
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
