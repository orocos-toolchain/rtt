/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  TableMarshaller.hpp 

                        TableMarshaller.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
#ifndef PI_PROPERTIES_TABLESERIALIZER
#define PI_PROPERTIES_TABLESERIALIZER

#include <corelib/Property.hpp>
#include <corelib/PropertyIntrospection.hpp>
#include "StreamProcessor.hpp"

namespace ORO_CoreLib
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
        std::string sep;
        public:
            typedef o_stream output_stream;
            typedef o_stream OutputStream;
            
            TableMarshaller(output_stream &os, std::string _sep=" ") :
                    StreamProcessor<o_stream>(os), sep(_sep)
            {}

            virtual ~TableMarshaller() {}

			virtual void serialize(PropertyBase* v) 
			{ 
                *this->s << sep;
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
                *this->s << sep <<std::endl;
            }
	};
}
#endif
