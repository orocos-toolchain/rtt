/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  TableMarshaller.hpp 

                        TableMarshaller.hpp -  description
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
 
#ifndef PI_PROPERTIES_TABLESERIALIZER
#define PI_PROPERTIES_TABLESERIALIZER

#include "../Property.hpp"
#include "../PropertyIntrospection.hpp"
#include "StreamProcessor.hpp"

namespace RTT
{

    /**
     * A Marshaller for generating a stream of numbers, ordered in
     * columns. A new row is created on each flush() command. The
     * TableHeaderMarshaller can create the appropriate heading for
     * the columns.
     */
    template<typename o_stream>
    class TableMarshaller 
        : public Marshaller, public StreamProcessor<o_stream>
    {
        std::string msep;
        public:
        typedef o_stream output_stream;
        typedef o_stream OutputStream;
            
        /**
         * Create a new marshaller, streaming the data to a stream.
         * @param os The stream to write the data to (i.e. cerr)
         * @param sep The separater to place between each column and at
         * the end of the line.
         */
        TableMarshaller(output_stream &os, std::string sep=" ") :
            StreamProcessor<o_stream>(os), msep(sep)
        {}

            virtual ~TableMarshaller() {}

			virtual void serialize(PropertyBase* v) 
			{ 
                *this->s << msep;
                Property<PropertyBag>* bag = dynamic_cast< Property<PropertyBag>* >( v );
                if ( bag )
                    this->serialize( bag->value() );
                else {
                    this->s->width( v->getName().length() );
                    *this->s << v->getDataSource();
                }
			}
			
            virtual void serialize(const PropertyBag &v) 
			{
                for (
                    PropertyBag::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                    this->serialize( *i );
                }
			}

            virtual void flush() 
            {
                // TODO : buffer for formatting and flush here.
                *this->s << msep <<std::endl;
            }
	};
}
#endif
