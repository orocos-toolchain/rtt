/***************************************************************************
  tag: Peter Soetens Fri Mar 16 21:32:02 2012 +0100 StreamFactory.hpp

                        StreamFactory.hpp -  description
                           -------------------
    begin                : Fri Mar 16 2012
    copyright            : (C) 2012 Peter Soetens
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


#ifndef ORO_STREAM_FACTORY_HPP
#define ORO_STREAM_FACTORY_HPP

#include <ostream>
#include <istream>
#include "../base/DataSourceBase.hpp"
#include <boost/shared_ptr.hpp>
#include <string>
#include "../rtt-config.h"

namespace RTT
{
    namespace types 
    {

        class RTT_API StreamFactory
        {
        public:
        virtual ~StreamFactory() {};

        /**
         * @name Conversion to/from text
         * Used to convert data to human readable text and vice versa.
         * @{
         */
        /**
         * Output this datasource as a human readable string.
         * The default just writes the type name in parentheses to \a os.
         */
        virtual std::ostream& write(std::ostream& os, base::DataSourceBase::shared_ptr in ) const = 0;

        /**
         * Read a new value for this datasource from a human readable string.
         * The default does nothing.
         */
        virtual std::istream& read(std::istream& os, base::DataSourceBase::shared_ptr out ) const = 0;

        /**
         * Usability function which converts data to a string.
         */
        virtual std::string toString( base::DataSourceBase::shared_ptr in ) const;

        /**
         * Usability function which converts a string to data.
         */
        virtual bool fromString( const std::string& value, base::DataSourceBase::shared_ptr out ) const;

        /**
         * Returns true if this type is directly streamable
         * using read()/write() or toString()/fromString().
         */
        virtual bool isStreamable() const = 0;
        /** @} */
        };
        typedef boost::shared_ptr<StreamFactory> StreamFactoryPtr;
    }
}
#endif
