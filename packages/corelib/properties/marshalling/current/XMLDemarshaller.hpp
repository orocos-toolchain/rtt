/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  XMLDemarshaller.hpp 

                        XMLDemarshaller.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
 
#ifndef PI_PROPERTIES_XMLDESERIALIZER
#define PI_PROPERTIES_XMLDESERIALIZER

#include "Property.hpp"

namespace ORO_CoreLib
{

	template<typename input_stream>
    class XMLDemarshaller : public Demarshaller
    {
        public:
            XMLDemarshaller(input_stream &is) :
                    _is(is)
            {}
			
            virtual bool deserialize(PropertyBag &v) 
			{
				string token;
		    	_is >> token;
#if 0
                for (
                    vector<PropertyBase*>::iterator i = v._properties.begin();
                    i != v._properties.end();
                    i++ )
                {
                    (*i)->serialize(*this);
                }
#endif
//                _os <<"</Bag>\n";
                return true;
			}
			
            input_stream &_is;
    };
}
#endif
