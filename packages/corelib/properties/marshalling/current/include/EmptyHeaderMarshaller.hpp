/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  EmptyHeaderMarshaller.hpp 

                        EmptyHeaderMarshaller.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 
#ifndef PI_PROPERTIES_EMPTYHEADER_SERIALIZER
#define PI_PROPERTIES_EMPTYHEADER_SERIALIZER

#include <rtt/Property.hpp>
#include <rtt/PropertyIntrospection.hpp>
#include "StreamProcessor.hpp"

namespace RTT
{
    /**
     * @brief A Dummy Empty Header Marshaller.
     */
    template<typename o_stream>
    class EmptyHeaderMarshaller 
        : public Marshaller,
          public StreamProcessor<o_stream>
    {
    public:
        typedef o_stream output_stream;
        typedef o_stream OutputStream;
            
        EmptyHeaderMarshaller(output_stream &os) :
            StreamProcessor<o_stream>(os)
        {
        }

        virtual ~EmptyHeaderMarshaller() {}
            
        virtual void flush() {}
            
        virtual void serialize(PropertyBase* v) 
        { 
        }

        virtual void serialize(const PropertyBag &v) 
        {
        }
	};
}
#endif
