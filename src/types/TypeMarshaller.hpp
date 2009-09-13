/*
 * TypeMarshaller.hpp
 *
 *  Created on: Sep 17, 2009
 *      Author: kaltan
 */

#ifndef TYPEMARSHALLER_HPP_
#define TYPEMARSHALLER_HPP_

#include "TypeTransporter.hpp"

namespace RTT
{

    namespace types
    {

        template<class T>
        class TypeMarshaller: public RTT::types::TypeTransporter
        {
        public:
            /**
             * Returns the size in bytes of a marshalled data element.
             * @return the size.
             */
            virtual unsigned int getSampleSize(const T& sample) const = 0;

        };

    }

}

#endif /* TYPEMARSHALLER_HPP_ */
