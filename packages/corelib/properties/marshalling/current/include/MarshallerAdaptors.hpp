/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  MarshallerAdaptors.hpp 

                        MarshallerAdaptors.hpp -  description
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
 
#ifndef MARSHALLERADAPTOR_HPP
#define MARSHALLERADAPTOR_HPP


namespace ORO_CoreLib
{

    /**
     * Converts one marshaller type to another marshaller.
     */
    template< class InputStream, class DeMarshaller, class Marshaller, class OutputStream>
    class MarshallerAdaptor
    {
    public:
    };

    
    template< class Marshaller, class OutputStream>
    class PropertyAdaptor
    {
        public:
    };


    /**
     * Contains the required classes for a full marshalling operation.
     */
    template< class HeaderMarshaller,
              class BodyMarshaller >
    class MarshallConfiguration
    {
        typedef typename BodyMarshaller::OutputStream OutputStream;
        public:
        /**
         * Setup a new marshalling configuration to a specific stream.
         */
        MarshallConfiguration( OutputStream& o_stream ) 
            : header_marshaller(o_stream), body_marshaller(o_stream), output(o_stream)
            {}

        HeaderMarshaller& header() { return header_marshaller; }
        BodyMarshaller&   body()   { return body_marshaller;   }
        OutputStream&     stream() { return output;            }
        
        protected:
        HeaderMarshaller header_marshaller;
        BodyMarshaller   body_marshaller;

        OutputStream& output;
    };

    /**
     * Contains the required classes for a full demarshalling operation.
     */
    template< class InputStream, class HeaderDemarshaller, class BodyDemarshaller>
    class DemarshallConfiguration
    {
        public:
        DemarshallConfiguration( InputStream& i_stream ) 
            : header_demarshaller(i_stream), body_demarshaller(i_stream), input(i_stream)
            {}

        HeaderDemarshaller& header() { return header_demarshaller; }
        BodyDemarshaller&   body()   { return body_demarshaller;   }
        InputStream&        stream() { return input;               }
        
        protected:
        HeaderDemarshaller header_demarshaller;
        BodyDemarshaller   body_demarshaller;

        InputStream& input;
    };


}

#endif
