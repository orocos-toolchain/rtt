#ifndef PI_PROPERTIES_OROCOS1_MARSHALLER
#define PI_PROPERTIES_OROCOS1_MARSHALLER

#include <corelib/Property.hpp>

#include <sstream>

namespace ORO_CoreLib
{

	/**
	 * A marshaller which generates data compatible with the previous Orocos property system.
	 * @bug Not working yet :-)
	 */
    template<typename output_stream>
    class Orocos1Marshaller : public Marshaller, public PropertyIntrospection
    {
        public:
            Orocos1Marshaller(output_stream &os) :
                    _os(os)
            {}

            virtual void flush() {}

			virtual void serialize(const Property<bool> &v) 
			{ 
				
                _os << v.getName() 
					<<":1>"
					<< v.get() << ";";
			}

			virtual void serialize(const Property<char> &v) 
			{ 
                _os << v.getName() 
					<<":1>"
					<< v.get() << ";";
			}

			virtual void serialize(const Property<int> &v) 
			{ 
				std::stringstream buffer;
				std::string s;
				buffer << v.get();
				buffer >> s;	
                _os << v.getName() 
					<<":"<<s.size() << ">"
					<< s << ";";
			}
			
			virtual void serialize(const Property<double> &v) 
			{
   				std::stringstream buffer;
				std::string s;
				buffer << v.get();
				buffer >> s;
                _os << v.getName() 
					<<":" << s.size() << ">"
					<< s << ";";
			}
			virtual void serialize(const Property<std::string> &v) 
			{
                _os << v.getName() 
					<<":"<<v.get().size() << ">"
					<< v.get() << ";";
			}
			
            virtual void serialize(const PropertyBag &v) 
			{
                _os <<"{";
#if 1
                for (
                    PropertyBag::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                    (*i)->identify(this);
                }
#endif
                _os <<"}";
			}
            virtual void serialize(const Property<PropertyBag> &v) 
			{
                //   cout << "double: " << v;
				_os << v.getName();
				serialize(v.get());
#if 0
                _os <<"<bag>";
                for (
                    vector<PropertyBase*>::iterator i = v._properties.begin();
                    i != v._properties.end();
                    i++ )
                {
                    (*i)->serialize(*this);
                }
                _os <<"</bag>";
#endif
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
				serialize(v.get());
            }

            output_stream &_os;
	};
}
#endif
