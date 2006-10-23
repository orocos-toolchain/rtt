/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  XMLDemarshaller.hpp 

                        XMLDemarshaller.hpp -  description
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
 
#ifndef PI_PROPERTIES_XMLDESERIALIZER
#define PI_PROPERTIES_XMLDESERIALIZER

#include <rtt/Property.hpp>
#include <rtt/Marshaller.hpp>

namespace RTT
{

    /**
     * Unimplemented counterpart of XMLMarshaller.
     */
	template<typename input_stream>
    class XMLDemarshaller
        : public Demarshaller
    {
    public:
        XMLDemarshaller(input_stream &is) :
            _is(is)
        {}
			
        virtual bool deserialize(PropertyBag &v) 
        {
            return false;
        }
			
        input_stream &_is;
    };
}
#endif
