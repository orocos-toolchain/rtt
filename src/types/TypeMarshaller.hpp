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
         * Objects implementing this interface have the capability to convert data sources
         * to and from a binary representation.
         */
        template<class T>
        class TypeMarshaller: public RTT::types::TypeTransporter
        {
        public:
            /**
             * Create an transportable object for a \a protocol which contains the value of \a source.
             * This must be a real-time function which does not allocate memory and which requires source
             * to be an AssignableDataSource.
             *
             * @param source The data to be read
             * @param blob Suggested target memory area to write to. In case the type marshaller does not need
             * this, it will return an alternative as a first element in the returned std::pair.
             * @param size The size of the memory area pointed by blob
             * @return Returns (0,0) if the filling failed, otherwise, points to the filled memory area and the effectively
             * written size. The returned pointer may differ from \a blob, in case \a blob was not used.
             */
            virtual std::pair<void*,int> fillBlob( base::DataSourceBase::shared_ptr source, void* blob, int size) const = 0;

            /**
             * Update \a target with the contents of \a blob which is an object of a \a protocol.
             */
            virtual bool updateFromBlob(const void* blob, int size, base::DataSourceBase::shared_ptr target) const = 0;

            /**
             * Returns the size in bytes of a marshalled data element.
             * @return the size.
             */
            virtual unsigned int getSampleSize(const T& sample) const = 0;

        };

    }

}

#endif /* TYPEMARSHALLER_HPP_ */
