#include "CPFMarshaller.hpp"
#include "../rtt-config.h"

namespace RTT
{
    template<class T>
    void CPFMarshaller<std::ostream>::doWrite( const Property<T> &v, const std::string& type )
    { 
        *(this->s)<<indent << "<simple name=\"" << this->escape( v.getName() ) << "\" type=\""<< type <<"\">";
        if ( !v.getDescription().empty() )
            *(this->s) << "<description>"<< this->escape( v.getDescription() ) << "</description>";
        *(this->s) << "<value>" << v.get() << "</value></simple>\n";
    }

    void CPFMarshaller<std::ostream>::doWrite( const Property<std::string> &v, const std::string& type )
    { 
        *(this->s)<<indent << "<simple name=\"" << this->escape( v.getName() ) << "\" type=\""<< type <<"\">";
        if ( !v.getDescription().empty() )
            *(this->s) << "<description>"<< this->escape( v.getDescription() ) << "</description>";
        *(this->s) << "<value>" << this->escape( v.get() ) << "</value></simple>\n";
    }

    
    std::string CPFMarshaller<std::ostream>::escape(std::string s)
    {
        std::string::size_type n=0;
        // replace amps first.
        while ((n = s.find("&",n)) != s.npos) {
            s.replace(n, 1, std::string("&amp;"));
            n += 5;
        }

        n=0;
        while ((n = s.find("<",n)) != s.npos) {
            s.replace(n, 1, std::string("&lt;"));
            n += 4;
        }

        n=0;
        while ((n = s.find(">",n)) != s.npos) {
            s.replace(n, 1, std::string("&gt;"));
            n += 4;
        }

        // TODO: Added escapes for other XML entities
        return s;
    }

    
    void CPFMarshaller<std::ostream>::introspect(PropertyBase* pb)
    {
        PropertyIntrospection::introspect( pb );
    }

    
    void CPFMarshaller<std::ostream>::introspect(Property<bool> &v) 
    { 
        doWrite( v, "boolean");
    }

    
    void CPFMarshaller<std::ostream>::introspect(Property<char> &v) 
    { 
        doWrite( v, "char");
    }

    
    void CPFMarshaller<std::ostream>::introspect(Property<int> &v) 
    { 
        doWrite( v, "long");
    }
			
    
    void CPFMarshaller<std::ostream>::introspect(Property<unsigned int> &v) 
    { 
        doWrite( v, "ulong");
    }
			
    
    void CPFMarshaller<std::ostream>::introspect(Property<double> &v) 
    {
        doWrite( v, "double");
    }

    
    void CPFMarshaller<std::ostream>::introspect(Property<std::string> &v) 
    {
        doWrite( v, "string");
    }
			
    
    void CPFMarshaller<std::ostream>::introspect(Property<PropertyBag> &b) 
    {
        PropertyBag v = b.get();
        *(this->s) <<indent<<"<struct name=\""<<escape(b.getName())<<"\" type=\""<< escape(v.getType())<< "\">\n";
        indent +="   ";
        if ( !b.getDescription().empty() )
            *(this->s) <<indent<<"<description>"  <<escape(b.getDescription()) << "</description>\n";

        b.value().identify(this);

        indent = indent.substr(0, indent.length()-3); 
        *(this->s) <<indent<<"</struct>\n";
    }

    CPFMarshaller<std::ostream>::CPFMarshaller(std::ostream &os) 
        : StreamProcessor<std::ostream>(os), indent("  ")
    {
    }

    CPFMarshaller<std::ostream>::CPFMarshaller(const std::string& filename)
        : StreamProcessor<std::ostream>(mfile), 
          mfile(filename.c_str()),
          indent("  ")
    {
        if ( !mfile ) {
            s = 0;
            log(Error) << "Could not open file for writing: "<<filename <<endlog();
        }
    }

    
    void CPFMarshaller<std::ostream>::serialize(PropertyBase* v)
    {
        if (s)
            v->identify( this );
    }

    
    void CPFMarshaller<std::ostream>::serialize(const PropertyBag &v) 
    {
        if ( !s )
            return;
        *(this->s) <<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                   <<"<!DOCTYPE properties SYSTEM \"cpf.dtd\">\n";
        *(this->s) <<"<properties>\n";

        v.identify(this);

        *(this->s) << "</properties>\n";
    }

    
    void CPFMarshaller<std::ostream>::flush()
    {
        if (s)
            this->s->flush();
    }
}

