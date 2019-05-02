/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  CPFMarshallInterface.hpp

                        CPFMarshallInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#ifndef PI_PROPERTIES_CPFSERIALIZER
#define PI_PROPERTIES_CPFSERIALIZER

#include <ostream>
#include <fstream>
#include <string>
#include "MarshallInterface.hpp"
#include "../Property.hpp"
#include "../base/PropertyIntrospection.hpp"
#include "StreamProcessor.hpp"

namespace RTT
{ namespace marsh {


    template<class T>
    class CPFMarshaller;

    /**
     * A class for marshalling a property or propertybag into a
     * component property description, following the CORBA 3 standard.
     * @see CPFDemarshaller for reading the result back in.
     */
    template<>
    class CPFMarshaller<std::ostream>
        : public MarshallInterface,
          protected base::PropertyIntrospection,
          public StreamProcessor<std::ostream>
    {
        std::fstream mfile;
        /**
         * Write-out formatting of a property.
         */
        template<class T>
        void doWrite( const Property<T> &v, const std::string& type );

        /**
         * Specialisation in case of a string.
         * Escapes the string sequence and should check for UTF-8
         */
        void doWrite( const Property<std::string> &v, const std::string& type );

        /**
         * Specialisation in case of a char.
         * Escapes the char and should check for UTF-8. A null character is
         * translated into <value></value>
         */
        void doWrite( const Property<char> &v, const std::string& type );

        std::string indent;

        std::string escape(std::string s);

        virtual void introspect(base::PropertyBase* pb);

        virtual void introspect(Property<bool> &v);

        virtual void introspect(Property<char> &v);

        virtual void introspect(Property<unsigned char> &v);

        virtual void introspect(Property<int> &v);

        virtual void introspect(Property<unsigned int> &v);

        virtual void introspect(Property<short> &v);

        virtual void introspect(Property<unsigned short> &v);

        virtual void introspect(Property<long long> &v);

        virtual void introspect(Property<unsigned long long> &v);

        virtual void introspect(Property<float> &v);

        virtual void introspect(Property<double> &v);

        virtual void introspect(Property<std::string> &v);

        virtual void introspect(Property<PropertyBag> &b);

    public:
        /**
         * Construct a CPFMarshaller from a stream.
         */
        CPFMarshaller(std::ostream &os);

        /**
         * Construct a CPFMarshaller from a file.
         */
        CPFMarshaller(const std::string& filename);

        virtual void serialize(base::PropertyBase* v);

        virtual void serialize(const PropertyBag &v);

        virtual void flush();
	};
}}
#endif
