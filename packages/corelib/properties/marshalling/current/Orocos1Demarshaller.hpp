#ifndef PI_PROPERTIES_OROCOS1_DEMARSHALLER
#define PI_PROPERTIES_OROCOS1_DEMARSHALLER

#include "Property.hpp"


namespace ORO_CoreLib
{
    using namespace std;

	/**
	 * A demarshaller which parses data compatible with the previous Orocos property system.
	 * @bug Not working yet :-)
	 */
	template<typename input_stream>
    class Orocos1Demarshaller : public Demarshaller
    {
        public:
            Orocos1Demarshaller(input_stream &is) :
                    _is(is)
            {}
	
            virtual void flush()
            {}
            
			virtual bool deserialize(Property<bool> &v) 
			{ }
			
			virtual bool deserialize(Property<char> &v) 
			{ }
			
			virtual bool deserialize(Property<int> &v) 
			{ }
			
			virtual bool deserialize(Property<double> &v) 
			{}
			
			virtual bool deserialize(Property<string> &v) 
			{}

            virtual bool deserialize(Property<PropertyBag> &v) 
			{}
			
            virtual bool deserialize(PropertyBag &v) 
			{
				string token;
		    	_is >> token;

				cerr <<"sdf:"<<endl;
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
