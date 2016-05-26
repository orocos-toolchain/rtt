/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:20 CET 2008  CPFMarshaller.cpp

                        CPFMarshaller.cpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "CPFMarshaller.hpp"
#include "../rtt-config.h"

#include <iostream>
using namespace std;
namespace RTT {
    using namespace detail;
    template<class T>
    void CPFMarshaller<std::ostream>::doWrite( const Property<T> &v, const std::string& type )
    {
        *(this->s) <<indent << "<simple ";
        if ( !v.getName().empty() )
            *(this->s) <<"name=\"" << this->escape( v.getName() ) <<"\" ";
        *(this->s) << "type=\""<< type <<"\">";
        if ( !v.getDescription().empty() )
            *(this->s) << "<description>"<< this->escape( v.getDescription() ) << "</description>";
        *(this->s) << "<value>" << v.get() << "</value></simple>\n";
    }

    void CPFMarshaller<std::ostream>::doWrite( const Property<std::string> &v, const std::string& type )
    {
        *(this->s) <<indent << "<simple ";
        if ( !v.getName().empty() )
            *(this->s) <<"name=\"" << this->escape( v.getName() ) <<"\" ";
        *(this->s) << "type=\""<< type <<"\">";
        if ( !v.getDescription().empty() )
            *(this->s) << "<description>"<< this->escape( v.getDescription() ) << "</description>";
        *(this->s) << "<value>" << this->escape( v.get() ) << "</value></simple>\n";
    }


    void CPFMarshaller<std::ostream>::doWrite( const Property<char> &v, const std::string& type )
    {
        *(this->s) <<indent << "<simple ";
        if ( !v.getName().empty() )
            *(this->s) <<"name=\"" << this->escape( v.getName() ) <<"\" ";
        *(this->s) << "type=\""<< type <<"\">";
        if ( !v.getDescription().empty() )
            *(this->s) << "<description>"<< this->escape( v.getDescription() ) << "</description>";
        if ( v.get() == '\0' )
            *(this->s)<< "<value></value></simple>\n";
        else {
            std::string toescape(1, v.get());
            *(this->s) << "<value>" << this->escape( toescape ) << "</value></simple>\n";
        }
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
        if (dynamic_cast<Property<unsigned char>* >(pb) )
        	return introspect( *static_cast<Property<unsigned char>* >(pb) );
        if (dynamic_cast<Property<float>* >(pb) )
        	return introspect( *static_cast<Property<float>* >(pb) );
        // Since the CPFDemarshaller maps 'short' and 'ushort' to int, we don't write out shorts as it would break
        // lots of existing files, where users use 'short' and 'long' interchangingly.
        // This could be finally resolved by using a conversion constructor, but the RTT typekit does not support
        // shorts...
        // if (dynamic_cast<Property<unsigned short>* >(pb) )
        // 	return introspect( *static_cast<Property<unsigned short>* >(pb) );
        // if (dynamic_cast<Property<short>* >(pb) )
        // 	return introspect( *static_cast<Property<>* >(pb) );
        log(Error) << "Couldn't write "<< pb->getName() << " to XML file because the " << pb->getType() << " type is not supported by the CPF format." <<endlog();
        log(Error) << "If your type is a C++ struct or sequence, you can register it with a type info object." <<endlog();
        log(Error) << "We only support these primitive types: boolean|char|double|float|long|octet|string|ulong." <<endlog();
    }


    void CPFMarshaller<std::ostream>::introspect(Property<bool> &v)
    {
        doWrite( v, "boolean");
    }


    void CPFMarshaller<std::ostream>::introspect(Property<char> &v)
    {
        doWrite( v, "char");
    }

    void CPFMarshaller<std::ostream>::introspect(Property<unsigned char> &v)
    {
        doWrite( v, "octet");
    }


    void CPFMarshaller<std::ostream>::introspect(Property<int> &v)
    {
        doWrite( v, "long");
    }


    void CPFMarshaller<std::ostream>::introspect(Property<unsigned int> &v)
    {
        doWrite( v, "ulong");
    }

    void CPFMarshaller<std::ostream>::introspect(Property<short> &v)
    {
        doWrite( v, "short");
    }


    void CPFMarshaller<std::ostream>::introspect(Property<unsigned short> &v)
    {
        doWrite( v, "ushort");
    }

    void CPFMarshaller<std::ostream>::introspect(Property<long long> &v)
    {
        doWrite( v, "llong");
    }

    void CPFMarshaller<std::ostream>::introspect(Property<unsigned long long> &v)
    {
        doWrite( v, "ullong");
    }

    void CPFMarshaller<std::ostream>::introspect(Property<float> &v)
    {
        (this->s)->precision(15);
        doWrite( v, "float");
    }

    void CPFMarshaller<std::ostream>::introspect(Property<double> &v)
    {
        (this->s)->precision(25);
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
          mfile(filename.c_str(), std::fstream::out),
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

