#ifndef PI_PROPERTIES_INISERIALIZER
#define PI_PROPERTIES_INISERIALIZER

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "Property.hpp"

namespace ORO_CoreLib
{
    using namespace std;


    template<typename output_stream>
    class INIMarshaller : public Marshaller, public PropertyIntrospection
    {
        public:
            INIMarshaller(output_stream &os) :
                    _os(os)
            {}

			virtual void serialize(const Property<bool> &v) 
			{ 
                _os << v.getName() << " = " << v.get() << "\n";
			}

			virtual void serialize(const Property<char> &v) 
			{ 
                _os << v.getName() << " = " << v.get() << "\n";
			}

			virtual void serialize(const Property<int> &v) 
			{ 
                _os << v.getName() << " = " << v.get() << "\n";
			}
			
			virtual void serialize(const Property<double> &v) 
			{
                _os << v.getName() << " = " << v.get() << "\n";
			}
			
			virtual void serialize(const Property<std::string> &v) 
			{
                _os << v.getName() << " = " << v.get() << "\n";
			}
			
            virtual void serialize(const PropertyBag &v) 
			{
                _os << "[properties]\n";

                for (
                    std::vector<PropertyBase*>::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                    (*i)->identify(this);
                }
                _os << "\n";
			}
	
			virtual void serialize(const Property<PropertyBag> &b) 
			{
				PropertyBag v = b.get();
                _os <<"[" << b.getName()<< "]\n";
                for (
                    PropertyBag::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    ++i )
                {
                    (*i)->identify(this);
                }
                _os << "\n";
			}

			virtual void introspect(const Property<bool> &v) 
			{ 
                serialize(v);
			}

			virtual void introspect(const Property<char> &v) 
			{ 
                serialize(v);
			}

			virtual void introspect(const Property<int> &v) 
			{ 
                serialize(v);
			}
			
			virtual void introspect(const Property<double> &v) 
			{
                serialize(v);
			}

			virtual void introspect(const Property<std::string> &v) 
			{
                serialize(v);
			}
			
            virtual void introspect(const Property<PropertyBag> &v) 
			{
				serialize(v);
            }
		virtual void flush()
			{}
            output_stream &_os;
	};
}
#endif
