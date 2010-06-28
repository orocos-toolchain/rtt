#ifndef ORO_CONNPOLICYTYPE_HPP_
#define ORO_CONNPOLICYTYPE_HPP_

#include <boost/serialization/serialization.hpp>
#include "../ConnPolicy.hpp"

namespace boost {
    namespace serialization {
        /**
         * Serializes RTT::ConnPolicy objects.
         * @param a Any boost::serialization compatible archive
         * @param c A ConnPolicy that will be read or written.
         */
        template<class Archive>
        void serialize(Archive& a, RTT::ConnPolicy& c, unsigned int) {
            a & boost::serialization::make_nvp("type", c.type);
            a & boost::serialization::make_nvp("init", c.init );
            a & boost::serialization::make_nvp("lock_policy", c.lock_policy );
            a & boost::serialization::make_nvp("pull", c.pull );
            a & boost::serialization::make_nvp("size", c.size );
            a & boost::serialization::make_nvp("transport", c.transport );
            a & boost::serialization::make_nvp("data_size", c.data_size );
            a & boost::serialization::make_nvp("name_id", c.name_id );
        }
    }
}


#endif /* ORO_CONNPOLICYTYPE_HPP_ */
