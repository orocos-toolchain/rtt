/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  TypeMarshaller.hpp

                        TypeMarshaller.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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
