/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  EmptyHeaderMarshaller.hpp 

                        EmptyHeaderMarshaller.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
 
#ifndef PI_PROPERTIES_EMPTYHEADER_SERIALIZER
#define PI_PROPERTIES_EMPTYHEADER_SERIALIZER

#include <corelib/Property.hpp>
#include "StreamProcessor.hpp"

namespace ORO_CoreLib
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
            
        virtual void serialize(const Property<bool> &v) 
        { 
        }

        virtual void serialize(const Property<char> &v) 
        { 
        }

        virtual void serialize(const Property<int> &v) 
        { 
        }
			
        virtual void serialize(const Property<unsigned int> &v) 
        { 
        }
			
        virtual void serialize(const Property<double> &v) 
        {
        }

        virtual void serialize(const Property<std::string> &v) 
        {
        }
			
        virtual void serialize(const Property<PropertyBag> &v) 
        {
        }

        virtual void serialize(const PropertyBag &v) 
        {
        }
	};
}
#endif
