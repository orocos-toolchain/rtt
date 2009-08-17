/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  Marshaller.hpp

                        Marshaller.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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

#ifndef PI_PROPERTIES_MARSHALLER
#define PI_PROPERTIES_MARSHALLER

#include <string>
#include "rtt-config.h"
#include "../rtt-fwd.hpp"

namespace RTT
{ namespace marsh {

	/**
     * @brief A Marshaller converts Property objects to a (file/text) format.
	 *
	 * @see Demarshaller
	 * @see Property
	 * @see PropertyBag
     * @ingroup CoreLibProperties
	 */
    class RTT_API Marshaller
    {
        public:
        virtual ~Marshaller() {}

        /**
         * Serialize a property.
         * @param v The property to be serialized.
         */
        virtual void serialize(base::PropertyBase* v) = 0;

        /**
         * Serialize the contents of a property bag with headers and footers. Use this method
         * on your 'root' PropertyBag, such that headers and footers
         * are written.
         * @param v The property bag to be serialized.
         */
        virtual void serialize(const PropertyBag &v) = 0;

        /**
         * Flush all buffers, write footers.
         * Instructs the Marshaller to flush any remaining buffered
         * data and write a footer to indicate that serialization is
         * done. A marshaller may choose to write nothing until this
         * function is called, or print a summary, or anything else.
         */
        virtual void flush() = 0;
	};

	/**
     * @brief An interface for extracting properties from a format.
	 *
	 * @see Marshaller
	 * @see Property
	 * @see PropertyBag
     * @ingroup CoreLibProperties
	 */
	class RTT_API Demarshaller
	{
		public:
        virtual ~Demarshaller() {}
        /**
         * Deserialize data to a property bag.
         * @param v The property bag which will contain the results
         *          of the deserialization.
         * @see PropertyBag
         */
        virtual bool deserialize(PropertyBag &v) = 0;
    };
}} // Namespace RTT
#endif
