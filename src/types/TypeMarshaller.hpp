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

        /**
         * Returns the size in bytes of a data sample if it would be transported
         * using this object. This allows other objects to allocate enough memory
         * to hold the resulting object.
         */
        template<class T>
        class TypeMarshaller: public RTT::types::TypeTransporter
        {
        public:

            /**
             * Create an transportable object for a \a protocol which contains the value of \a source.
             * This is a real-time function which does not allocate memory and which requires source
             * to be an AssignableDataSource.
             */
            virtual std::pair<void*,int> createBlob( base::DataSourceBase::shared_ptr source) const = 0;

            virtual std::pair<void*,int> fillBlob( base::DataSourceBase::shared_ptr source, void* blob, int size) const = 0;

            /**
             * Update \a target with the contents of \a blob which is an object of a \a protocol.
             */
            virtual bool updateBlob(const void* blob, base::DataSourceBase::shared_ptr target) const = 0;

            /**
             * Returns the size in bytes of a marshalled data element.
             * @return the size.
             */
            virtual unsigned int getSampleSize(const T& sample) const = 0;

        };

    }

}

#endif /* TYPEMARSHALLER_HPP_ */
