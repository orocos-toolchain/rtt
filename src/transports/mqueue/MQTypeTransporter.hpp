#ifndef MQ_TYPE_TRANSPORTER_H
#define MQ_TYPE_TRANSPORTER_H

#include "../../types/TypeTransporter.hpp"

namespace RTT
{

    namespace mqueue
    {
        /**
         * Extends the TypeTransporter in order to get more information
         * about the size of blobs.
         */
        class MQTypeTransporter: public RTT::types::TypeTransporter
        {
        public:
            /**
             * Returns the maximum blobsize of a returned blob.
             * @return the size.
             */
            virtual unsigned int blobSize() const = 0;

        };
    }
}

#endif

