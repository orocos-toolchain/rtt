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

    namespace detail {
    /**
     * Get the peer and object from an invocation path like
     *  a.b.c.d() .
     */
    class PeerParser
    {
        CommonParser commonparser;
        rule_t peerpath, peerlocator;
        std::queue<std::string> callqueue;
        std::string mcurobject;
        TaskContext* context;
        TaskContext* _peer;
        void done();

        void seenobjectname( iter_t begin, iter_t end );
        void locatepeer( iter_t begin, iter_t end );

    public:
        PeerParser(TaskContext* c);

        /**
         * Change the context in which peers are 
         * looked up.
         * @return the previous TaskContext.
         */
        TaskContext* setContext( TaskContext* tc);
        /**
         * After reset, peer() == current context and
         * object() == "this".
         */
        void reset();

        /**
         * The parser tries to traverse a full peer-to-object
         * path and throws if it got stuck in the middle. \a peer()
         * will return the target peer and \a object() is \a this
         * or the supposed object of the peer. The parser does not
         * check if this object exists.
         */
        rule_t& parser();

        /**
         * The locator tries to go as far as possible in the peer-to-object
         * path and will never throw. \a peer() and \a object() will contain the last
         * valid peer found and its supposed object, attribute or value.
         */
        rule_t& locator();

        TaskContext* peer();

        std::string object();
    };
}}
            
#endif
