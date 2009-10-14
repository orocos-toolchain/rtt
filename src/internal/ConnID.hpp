/*
 * ConnID.hpp
 *
 *  Created on: Oct 14, 2009
 *      Author: kaltan
 */

#ifndef ORO_CONNID_HPP_
#define ORO_CONNID_HPP_

#include "../rtt-config.h"

namespace RTT
{
    namespace internal {
        /** This class is used in places where a permanent representation of a
         * reference to a connection is needed.
         *
         */
        class RTT_API ConnID
        {
        public:
            virtual bool isSameID(ConnID const& id) const = 0;
            virtual ConnID* clone() const = 0;
            virtual ~ConnID() {}
        };

        /**
         * A simplistic id that is only same with its own clones (and clones of clones).
         */
        class RTT_API SimpleConnID : public ConnID
        {
            const ConnID* cid;
        public:
            SimpleConnID(const ConnID* orig = 0);
            virtual bool isSameID(ConnID const& id) const;
            virtual ConnID* clone() const;
        };
    }
}
#endif /* ORO_CONNID_HPP_ */
