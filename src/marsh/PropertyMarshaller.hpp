#ifndef PROPERTY_MARSHALLER_HPP
#define PROPERTY_MARSHALLER_HPP

#include <string>
#include "../Marshaller.hpp"

namespace RTT
{
    /**
     * A class for writing a property or propertybag into file.
     * The file format used by Orocos is the 'Component Property Format'.
     * @see PropertyDemarshaller for reading the result back in.
     */
    class PropertyMarshaller 
        : public Marshaller
    {
        Marshaller* m;
        PropertyMarshaller(const PropertyMarshaller&);
    public:
        /** 
         * Construct a PropertyMarshaller which writes to a file.
         * 
         * @param filename 
         */
        PropertyMarshaller(const std::string& filename);
        ~PropertyMarshaller();

        virtual void serialize(PropertyBase* v);

        virtual void serialize(const PropertyBag &v);

        virtual void flush();
	};
}
#endif
