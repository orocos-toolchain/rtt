/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  PeerParser.hpp

                        PeerParser.hpp -  description
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



#ifndef ORO_PEERPARSER_HPP
#define ORO_PEERPARSER_HPP

#include "rtt/scripting/parser-types.hpp"
#include "rtt/scripting/CommonParser.hpp"
#include <queue>
#include <string>
#include "rtt/scripting/rtt-scripting-config.h"
#include "rtt/rtt-fwd.hpp"

namespace RTT
{

    namespace scripting {
    /**
     * Get the peer and object from an invocation path like
     *  a.b.c.d() .
     */
    class RTT_SCRIPTING_API PeerParser
    {
        CommonParser& commonparser;
        rule_t peerpath, peerlocator;
        std::queue<std::string> callqueue;
        ServicePtr mcurobject;
        std::string mlastobject;
        TaskContext* context;
        TaskContext* _peer;
        bool mfullpath, mfoundpath;
        RTT_HIDE void done();

        RTT_HIDE void seenobjectname( iter_t begin, iter_t end );
        RTT_HIDE void locatepeer( iter_t begin, iter_t end );

        enum PeerErrors { peer_not_found };
        guard<PeerErrors> my_guard;

        /**
         * set by locatepeer, read by handle_no_peer
         */
        boost::iterator_difference<iter_t>::type advance_on_error;
        RTT_HIDE error_status<> handle_no_peer(scanner_t const& scan, parser_error<PeerErrors, iter_t>&e );
    public:
        /**
         * Create a PeerParser which starts looking for peers from
         * a task.
         * @param c The task to start searching from.
         * @param fullpath Set to true if the parser() must
         * resolve the full path.
         */
        PeerParser(TaskContext* c, CommonParser& cp, bool fullpath=false);

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

        /**
         * Returns the last matching peer.
         */
        TaskContext* peer();

        /**
         * Returns the last matching object name.
         */
        std::string object();

        /**
         * Returns the last matching Service or zero if not found.
         */
        ServicePtr taskObject();

        /**
         * Returns true if the PeerParser found a valid path.
         * Will also return true if \a fullpath was set to false in the
         * constructor and a partial match was made.
         */
        bool foundPath();
    };
}}

#endif
