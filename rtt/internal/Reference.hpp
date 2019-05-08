/***************************************************************************
  tag: Peter Soetens Tue Jul 5 17:24:03 2011 +0200 Reference.hpp

                        Reference.hpp -  description
                           -------------------
    begin                : Tue Jul 5 2011
    copyright            : (C) 2011 Peter Soetens
    email                : peter@thesourceworks.com

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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef RTT_INTERNAL_REFERENCE_HPP
#define RTT_INTERNAL_REFERENCE_HPP

#include "../base/DataSourceBase.hpp"

namespace RTT 
{
    namespace internal {
        /**
         * Object that may receive a reference to some data by means of a
         * pointer or data source.
         * Used by ReferenceDataSource in order to allow to modify the reference
         * after creation of the data source.
         */
        class Reference 
        {
        public:
            virtual ~Reference() {};

            /**
             * Sets the reference to a given pointer. The pointer must have the
             * same type as the data referenced. No type checking is done, so
             * make sure you get this right.
             */
            virtual void setReference(void* ref) = 0;
            /**
             * Sets the reference to the data contained in the data source.
             * @param dsb Must be an assignable data source of the same type
             * of the data. The shared pointer is not necessarily stored, so
             * keep track of the lifetime of \a dsb, that it lives longer
             * than this object.
	     * @return false if setting failed. More precisely
	     * this might fail if dynamic_cast'ing the argument to the
	     * Reference DataSource fails.
	     * @note This method will also call evaluate on the dsb to
	     * assure the correct value is used.
             */
            virtual bool setReference(base::DataSourceBase::shared_ptr dsb) = 0;
        };
    }
}

#endif
