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
