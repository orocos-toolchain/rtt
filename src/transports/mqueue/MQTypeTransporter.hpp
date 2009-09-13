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
             * Returns the size in bytes of a blob.
             * @return the size.
             */
            virtual unsigned int blobSize(void* blob) const = 0;

        };
    }
}

#endif

