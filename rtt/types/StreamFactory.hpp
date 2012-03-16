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
